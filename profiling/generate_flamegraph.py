#!/usr/bin/env python3
import argparse
import html
import os
import re
import shutil
import signal
import socket
import subprocess
import sys
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PROFILE_BUILD = ROOT / "build-profile"
PROFILE_BIN = PROFILE_BUILD / "bin" / "rmdb"
OUT_DIR = ROOT / "profiling"
PORT = 8765


TABLE_DDL = [
    "create table warehouse (w_id int, w_name char(10), w_street_1 char(20),w_street_2 char(20), w_city char(20), w_state char(2), w_zip char(9), w_tax float, w_ytd float);",
    "create table district (d_id int, d_w_id int, d_name char(10), d_street_1 char(20), d_street_2 char(20), d_city char(20), d_state char(2), d_zip char(9),d_tax float, d_ytd float, d_next_o_id int);",
    "create table customer (c_id int, c_d_id int, c_w_id int, c_first char(16),c_middle char(2), c_last char(16), c_street_1 char(20), c_street_2 char(20),c_city char(20), c_state char(2), c_zip char(9), c_phone char(16), c_since char(30), c_credit char(2), c_credit_lim int, c_discount float, c_balance float,c_ytd_payment float, c_payment_cnt int, c_delivery_cnt int, c_data char(50));",
    "create table history (h_c_id int, h_c_d_id int, h_c_w_id int, h_d_id int, h_w_id int, h_date char(19), h_amount float, h_data char(24));",
    "create table new_orders (no_o_id int, no_d_id int, no_w_id int);",
    "create table orders (o_id int, o_d_id int, o_w_id int, o_c_id int, o_entry_d char(19), o_carrier_id int, o_ol_cnt int, o_all_local int);",
    "create table order_line (ol_o_id int, ol_d_id int, ol_w_id int, ol_number int, ol_i_id int, ol_supply_w_id int, ol_delivery_d char(30), ol_quantity int, ol_amount float, ol_dist_info char(24));",
    "create table item (i_id int, i_im_id int, i_name char(24), i_price float, i_data char(50));",
    "create table stock (s_i_id int, s_w_id int, s_quantity int, s_dist_01 char(24),s_dist_02 char(24), s_dist_03 char(24), s_dist_04 char(24), s_dist_05 char(24),s_dist_06 char(24), s_dist_07 char(24), s_dist_08 char(24), s_dist_09 char(24),s_dist_10 char(24), s_ytd float, s_order_cnt int, s_remote_cnt int, s_data char(50));",
]


INDEX_DDL = [
    "create index warehouse(w_id);",
    "create index district(d_w_id, d_id);",
    "create index customer(c_w_id, c_d_id, c_id);",
    "create index new_orders(no_w_id, no_d_id, no_o_id);",
    "create index orders(o_w_id, o_d_id, o_id);",
    "create index order_line(ol_w_id, ol_d_id, ol_o_id, ol_number);",
    "create index item(i_id);",
    "create index stock(s_w_id, s_i_id);",
]


LOAD_SQL = [
    "load ../../src/test/performance_test/table_data/warehouse.csv into warehouse;",
    "load ../../src/test/performance_test/table_data/district.csv into district;",
    "load ../../src/test/performance_test/table_data/customer.csv into customer;",
    "load ../../src/test/performance_test/table_data/history.csv into history;",
    "load ../../src/test/performance_test/table_data/new_orders.csv into new_orders;",
    "load ../../src/test/performance_test/table_data/orders.csv into orders;",
    "load ../../src/test/performance_test/table_data/order_line.csv into order_line;",
    "load ../../src/test/performance_test/table_data/item.csv into item;",
    "load ../../src/test/performance_test/table_data/stock.csv into stock;",
]


def connect_with_retry(timeout_sec: float = 10.0) -> socket.socket:
    deadline = time.time() + timeout_sec
    last_error = None
    while time.time() < deadline:
        try:
            sock = socket.create_connection(("127.0.0.1", PORT), timeout=1.0)
            sock.settimeout(5.0)
            return sock
        except OSError as exc:
            last_error = exc
            time.sleep(0.05)
    raise RuntimeError(f"server did not accept connections on port {PORT}: {last_error}")


def send_sql(sock: socket.socket, sql: str) -> str:
    sock.sendall(sql.encode("utf-8") + b"\0")
    chunks = []
    while True:
        chunk = sock.recv(65536)
        if not chunk:
            raise RuntimeError(f"connection closed while waiting for response to: {sql}")
        chunks.append(chunk)
        if b"\0" in chunk:
            break
    raw = b"".join(chunks).split(b"\0", 1)[0]
    return raw.decode("utf-8", errors="replace")


def checked_sql(sock: socket.socket, sql: str) -> str:
    response = send_sql(sock, sql)
    lowered = response.lower()
    if "failure" in lowered or "abort" in lowered:
        raise RuntimeError(f"SQL failed: {sql}\nresponse:\n{response}")
    return response


def send_exit(sock: socket.socket) -> None:
    sock.sendall(b"exit\0")


def workload_sql(iterations: int) -> list[str]:
    stmts = []
    for i in range(iterations):
        district_id = (i % 3) + 1
        customer_id = (i % 10) + 1
        item_id = (i % 10) + 1
        order_id = 1000 + i
        next_order_id = 2000 + i
        quantity = 5 + (i % 7)
        stock_quantity = 10 + (i % 70)
        amount = 10.0 + item_id * 3.125
        order_time = f"2026-07-02 16:{(i // 60) % 60:02d}:{i % 60:02d}"
        stmts.extend(
            [
                "begin;",
                (
                    "select c_discount, c_last, c_credit, w_tax "
                    "from customer, warehouse "
                    f"where w_id=1 and c_w_id=w_id and c_d_id={district_id} and c_id={customer_id};"
                ),
                f"select d_next_o_id, d_tax from district where d_id={district_id} and d_w_id=1;",
                f"update district set d_next_o_id={next_order_id} where d_id={district_id} and d_w_id=1;",
                f"insert into orders values ({order_id}, {district_id}, 1, {customer_id}, '{order_time}', 0, 5, 1);",
                f"insert into new_orders values ({order_id}, {district_id}, 1);",
                f"select i_price, i_name, i_data from item where i_id={item_id};",
                (
                    "select s_quantity, s_data, s_dist_01, s_dist_02, s_dist_03, "
                    "s_dist_04, s_dist_05,s_dist_06, s_dist_07, s_dist_08, "
                    f"s_dist_09, s_dist_10 from stock where s_i_id={item_id} and s_w_id=1;"
                ),
                f"update stock set s_quantity={stock_quantity} where s_i_id={item_id} and s_w_id=1;",
                (
                    "insert into order_line values "
                    f"({order_id}, {district_id}, 1, 1, {item_id}, 1, '{order_time}', "
                    f"{quantity}, {amount:.3f}, 'profile-workload');"
                ),
                f"select i_price, i_name, i_data from item where i_id={item_id};",
                "commit;",
            ]
        )
    return stmts


def run_workload(iterations: int) -> tuple[Path, Path]:
    if not PROFILE_BIN.exists():
        raise RuntimeError(f"{PROFILE_BIN} does not exist; build with profiling first")

    db_name = f"profile_flame_db_{int(time.time())}_{os.getpid()}"
    db_path = PROFILE_BUILD / db_name
    gmon_path = PROFILE_BUILD / "gmon.out"
    server_log = OUT_DIR / "rmdb_flamegraph_server.log"
    client_log = OUT_DIR / "rmdb_flamegraph_workload.log"
    gmon_path.unlink(missing_ok=True)
    server_log.unlink(missing_ok=True)
    client_log.unlink(missing_ok=True)

    with server_log.open("w", encoding="utf-8") as log:
        proc = subprocess.Popen(
            [str(PROFILE_BIN), db_name],
            cwd=PROFILE_BUILD,
            stdout=log,
            stderr=subprocess.STDOUT,
            text=True,
        )

    sock = None
    try:
        sock = connect_with_retry()
        with client_log.open("w", encoding="utf-8") as log:
            for sql in TABLE_DDL:
                log.write(f"SQL> {sql}\n{checked_sql(sock, sql)}\n")
            for sql in INDEX_DDL:
                log.write(f"SQL> {sql}\n{checked_sql(sock, sql)}\n")
            for sql in LOAD_SQL:
                log.write(f"SQL> {sql}\n{checked_sql(sock, sql)}\n")
            log.write(f"SQL> set output_file off\n{checked_sql(sock, 'set output_file off')}\n")
            for sql in workload_sql(iterations):
                log.write(f"SQL> {sql}\n{checked_sql(sock, sql)}\n")
            send_exit(sock)
    finally:
        if sock is not None:
            sock.close()
        if proc.poll() is None:
            proc.send_signal(signal.SIGINT)
            try:
                proc.wait(timeout=10)
            except subprocess.TimeoutExpired:
                proc.terminate()
                try:
                    proc.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    proc.kill()
                    proc.wait(timeout=5)

    if proc.returncode not in (0, -signal.SIGINT):
        raise RuntimeError(f"server exited with code {proc.returncode}; see {server_log}")
    if not gmon_path.exists():
        raise RuntimeError(f"{gmon_path} was not produced; ensure rmdb was built with -pg")

    saved_gmon = OUT_DIR / "rmdb_gmon.out"
    shutil.copy2(gmon_path, saved_gmon)
    return saved_gmon, db_path


def run_gprof(gmon_path: Path) -> Path:
    out_path = OUT_DIR / "rmdb_gprof.txt"
    with out_path.open("w", encoding="utf-8") as out:
        subprocess.run(
            ["gprof", "-b", str(PROFILE_BIN), str(gmon_path)],
            cwd=PROFILE_BUILD,
            stdout=out,
            stderr=subprocess.STDOUT,
            check=True,
            text=True,
        )
    return out_path


def parse_flat_profile(gprof_path: Path) -> list[tuple[str, float, float]]:
    entries = []
    in_flat = False
    line_re = re.compile(r"^\s*(\d+(?:\.\d+)?)\s+(\d+(?:\.\d+)?)\s+(\d+(?:\.\d+)?)\s+(.*?)\s*$")
    numeric_re = re.compile(r"^[+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:e[+-]?\d+)?$", re.IGNORECASE)
    with gprof_path.open(encoding="utf-8", errors="replace") as f:
        for line in f:
            if line.startswith("Flat profile:"):
                in_flat = True
                continue
            if in_flat and line.startswith("Call graph"):
                break
            if not in_flat:
                continue
            match = line_re.match(line)
            if not match:
                continue
            pct = float(match.group(1))
            self_sec = float(match.group(3))
            rest = match.group(4).strip()
            tokens = rest.split()
            if len(tokens) >= 4 and tokens[0].isdigit() and numeric_re.match(tokens[1]) and numeric_re.match(tokens[2]):
                name = " ".join(tokens[3:])
            else:
                name = rest
            if not name:
                continue
            if pct <= 0.0 and self_sec <= 0.0:
                continue
            entries.append((name, pct, self_sec))
    if not entries:
        raise RuntimeError(f"could not parse flat profile from {gprof_path}")
    return entries


def color_for(name: str) -> str:
    h = 0
    for ch in name:
        h = (h * 131 + ord(ch)) & 0xFFFFFFFF
    r = 205 + (h % 45)
    g = 70 + ((h >> 8) % 95)
    b = 40 + ((h >> 16) % 55)
    return f"rgb({r},{g},{b})"


def shorten(name: str, max_chars: int) -> str:
    if max_chars <= 0:
        return ""
    if len(name) <= max_chars:
        return name
    if max_chars <= 3:
        return name[:max_chars]
    return name[: max_chars - 2] + ".."


def write_svg(entries: list[tuple[str, float, float]], svg_path: Path, title: str) -> None:
    width = 1400
    frame_h = 18
    margin_l = 10
    margin_r = 10
    top = 72
    root_y = top
    leaf_y = top + frame_h + 8
    height = leaf_y + frame_h + 48
    plot_w = width - margin_l - margin_r
    total_pct = sum(pct for _, pct, _ in entries)
    total_self = sum(self_sec for _, _, self_sec in entries)
    scale = plot_w / total_pct

    parts = [
        '<?xml version="1.0" standalone="no"?>',
        f'<svg version="1.1" width="{width}" height="{height}" '
        'onload="init(evt)" viewBox="0 0 {0} {1}" '
        'xmlns="http://www.w3.org/2000/svg" '
        'xmlns:xlink="http://www.w3.org/1999/xlink">'.format(width, height),
        "<style>",
        "text { font-family: Verdana, sans-serif; font-size: 12px; fill: #222; }",
        ".title { font-size: 18px; font-weight: 600; }",
        ".subtitle { font-size: 12px; fill: #555; }",
        ".frame:hover rect { stroke: #111; stroke-width: 1; }",
        "</style>",
        "<script><![CDATA[",
        "function init(evt){}",
        "]]></script>",
        f'<text class="title" x="{margin_l}" y="28">{html.escape(title)}</text>',
        (
            f'<text class="subtitle" x="{margin_l}" y="50">'
            f"Generated from gprof flat samples; total self time {total_self:.2f}s. "
            "Each top-level block is one sampled function."
            "</text>"
        ),
        f'<g class="frame"><title>root ({total_pct:.2f}% sampled self time)</title>'
        f'<rect x="{margin_l}" y="{root_y}" width="{plot_w}" height="{frame_h}" '
        'fill="rgb(245,245,245)" stroke="rgb(220,220,220)" />'
        f'<text x="{margin_l + 3}" y="{root_y + 13}">rmdb</text></g>',
    ]

    x = margin_l
    for name, pct, self_sec in entries:
        w = pct * scale
        if w < 0.5:
            continue
        esc = html.escape(name)
        label = html.escape(shorten(name, int(w / 7)))
        parts.append(
            f'<g class="frame"><title>{esc} - {pct:.2f}%, self {self_sec:.4f}s</title>'
            f'<rect x="{x:.3f}" y="{leaf_y}" width="{w:.3f}" height="{frame_h}" '
            f'fill="{color_for(name)}" stroke="rgb(160,60,30)" />'
            f'<text x="{x + 3:.3f}" y="{leaf_y + 13}">{label}</text></g>'
        )
        x += w

    parts.append("</svg>")
    svg_path.write_text("\n".join(parts), encoding="utf-8")


def write_folded(entries: list[tuple[str, float, float]], folded_path: Path) -> None:
    total_pct = sum(pct for _, pct, _ in entries)
    scale = 10000.0 / total_pct
    with folded_path.open("w", encoding="utf-8") as f:
        for name, pct, _ in entries:
            samples = max(1, round(pct * scale))
            f.write(f"rmdb;{name} {samples}\n")


def main() -> int:
    global PROFILE_BUILD, PROFILE_BIN, OUT_DIR
    parser = argparse.ArgumentParser(description="Generate an RMDB gprof-based SVG flame graph.")
    parser.add_argument("--iterations", type=int, default=250, help="number of OLTP-style transactions to run")
    parser.add_argument("--build-dir", type=Path, default=PROFILE_BUILD, help="CMake build directory containing bin/rmdb")
    parser.add_argument("--out-dir", type=Path, default=OUT_DIR, help="directory for profiling outputs")
    args = parser.parse_args()

    PROFILE_BUILD = args.build_dir.resolve()
    PROFILE_BIN = PROFILE_BUILD / "bin" / "rmdb"
    OUT_DIR = args.out_dir.resolve()
    OUT_DIR.mkdir(exist_ok=True)
    gmon_path, db_path = run_workload(args.iterations)
    gprof_path = run_gprof(gmon_path)
    entries = parse_flat_profile(gprof_path)
    svg_path = OUT_DIR / "rmdb_flamegraph.svg"
    folded_path = OUT_DIR / "rmdb_flamegraph.folded"
    write_svg(entries, svg_path, f"RMDB Flame Graph ({args.iterations} OLTP-style transactions)")
    write_folded(entries, folded_path)
    print(f"SVG: {svg_path}")
    print(f"gprof: {gprof_path}")
    print(f"folded: {folded_path}")
    print(f"gmon: {gmon_path}")
    print(f"database: {db_path}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
