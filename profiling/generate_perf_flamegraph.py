#!/usr/bin/env python3
import argparse
import os
import signal
import socket
import subprocess
import sys
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
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


def connect_with_retry(timeout_sec):
    deadline = time.time() + timeout_sec
    last_error = None
    while time.time() < deadline:
        try:
            sock = socket.create_connection(("127.0.0.1", PORT), timeout=1.0)
            sock.settimeout(10.0)
            return sock
        except OSError as exc:
            last_error = exc
            time.sleep(0.05)
    raise RuntimeError("server did not accept connections on port {}: {}".format(PORT, last_error))


def send_sql(sock, sql):
    sock.sendall(sql.encode("utf-8") + b"\0")
    chunks = []
    while True:
        chunk = sock.recv(65536)
        if not chunk:
            raise RuntimeError("connection closed while waiting for response to: {}".format(sql))
        chunks.append(chunk)
        if b"\0" in chunk:
            break
    raw = b"".join(chunks).split(b"\0", 1)[0]
    return raw.decode("utf-8", errors="replace")


def checked_sql(sock, sql):
    response = send_sql(sock, sql)
    lowered = response.lower()
    if "failure" in lowered or "abort" in lowered:
        raise RuntimeError("SQL failed: {}\nresponse:\n{}".format(sql, response))
    return response


def send_exit(sock):
    sock.sendall(b"exit\0")


def transaction_sql(batch_id):
    district_id = batch_id % 3 + 1
    customer_id = batch_id % 10 + 1
    item_id = batch_id % 10 + 1
    order_id = 100000 + batch_id
    next_order_id = 200000 + batch_id
    quantity = 5 + batch_id % 7
    stock_quantity = 10 + batch_id % 70
    amount = 10.0 + item_id * 3.125
    order_time = "2026-07-07 12:{:02d}:{:02d}".format((batch_id // 60) % 60, batch_id % 60)

    new_order = [
        "begin;",
        "select c_discount, c_last, c_credit, w_tax from customer, warehouse where w_id=1 and c_w_id=w_id and c_d_id={} and c_id={};".format(district_id, customer_id),
        "select d_next_o_id, d_tax from district where d_id={} and d_w_id=1;".format(district_id),
        "update district set d_next_o_id={} where d_id={} and d_w_id=1;".format(next_order_id, district_id),
        "insert into orders values ({}, {}, 1, {}, '{}', 0, 5, 1);".format(order_id, district_id, customer_id, order_time),
        "insert into new_orders values ({}, {}, 1);".format(order_id, district_id),
        "select i_price, i_name, i_data from item where i_id={};".format(item_id),
        "select s_quantity, s_data, s_dist_01 from stock where s_i_id={} and s_w_id=1;".format(item_id),
        "update stock set s_quantity={} where s_i_id={} and s_w_id=1;".format(stock_quantity, item_id),
        "insert into order_line values ({}, {}, 1, 1, {}, 1, '{}', {}, {:.3f}, 'perf-workload');".format(order_id, district_id, item_id, order_time, quantity, amount),
        "commit;",
    ]
    payment = [
        "begin;",
        "select c_balance, c_ytd_payment, c_payment_cnt from customer where c_w_id=1 and c_d_id={} and c_id={};".format(district_id, customer_id),
        "update warehouse set w_ytd=w_ytd+{} where w_id=1;".format(amount),
        "update district set d_ytd=d_ytd+{} where d_w_id=1 and d_id={};".format(amount, district_id),
        "update customer set c_balance=c_balance-{} where c_w_id=1 and c_d_id={} and c_id={};".format(amount, district_id, customer_id),
        "insert into history values ({}, {}, 1, {}, 1, '{}', {:.3f}, 'perf-payment');".format(customer_id, district_id, district_id, order_time, amount),
        "commit;",
    ]
    order_status = [
        "begin;",
        "select c_balance, c_first, c_middle, c_last from customer where c_w_id=1 and c_d_id={} and c_id={};".format(district_id, customer_id),
        "select o_id, o_carrier_id, o_entry_d from orders where o_w_id=1 and o_d_id={} and o_c_id={};".format(district_id, customer_id),
        "commit;",
    ]
    delivery = [
        "begin;",
        "select no_o_id from new_orders where no_w_id=1 and no_d_id={};".format(district_id),
        "update orders set o_carrier_id=1 where o_w_id=1 and o_d_id={} and o_id={};".format(district_id, order_id),
        "delete from new_orders where no_w_id=1 and no_d_id={} and no_o_id={};".format(district_id, order_id),
        "commit;",
    ]
    stock_level = [
        "begin;",
        "select d_next_o_id from district where d_w_id=1 and d_id={};".format(district_id),
        "select s_quantity from stock where s_w_id=1 and s_quantity<30;",
        "commit;",
    ]
    return new_order + payment + order_status + delivery + stock_level


def run_sql_batch(sock, stmts, log):
    for sql in stmts:
        response = checked_sql(sock, sql)
        if log:
            log.write("SQL> {}\n{}\n".format(sql, response))


def run_workload_until(sock, deadline, log, start_batch):
    batch = start_batch
    while time.time() < deadline:
        run_sql_batch(sock, transaction_sql(batch), log)
        batch += 1
    return batch


def run_command(argv, cwd, stdout_path=None):
    if stdout_path:
        with stdout_path.open("w", encoding="utf-8") as out:
            subprocess.run(argv, cwd=str(cwd), stdout=out, stderr=subprocess.STDOUT, check=True, text=True)
    else:
        subprocess.run(argv, cwd=str(cwd), check=True, text=True)


def main():
    parser = argparse.ArgumentParser(description="Generate an RMDB perf-based flame graph.")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build-profile", help="CMake build directory containing bin/rmdb")
    parser.add_argument("--out-dir", type=Path, default=ROOT / "profiling", help="directory for profiling outputs")
    parser.add_argument("--perf", type=Path, default=ROOT / "WSL2-Linux-Kernel/tools/perf/perf", help="perf executable")
    parser.add_argument("--flamegraph-dir", type=Path, default=ROOT / "FlameGraph", help="Brendan Gregg FlameGraph directory")
    parser.add_argument("--warmup-seconds", type=float, default=30.0, help="TPC-C-style warmup duration before sampling")
    parser.add_argument("--record-seconds", type=float, default=30.0, help="perf sampling duration")
    parser.add_argument("--frequency", type=int, default=99, help="perf sampling frequency")
    parser.add_argument("--event", default="cpu-clock", help="perf event")
    args = parser.parse_args()

    build_dir = args.build_dir.resolve()
    rmdb_bin = build_dir / "bin" / "rmdb"
    out_dir = args.out_dir.resolve()
    out_dir.mkdir(exist_ok=True)

    if not rmdb_bin.exists():
        raise RuntimeError("{} does not exist; build RMDB first".format(rmdb_bin))
    if not args.perf.exists():
        raise RuntimeError("{} does not exist; build perf first".format(args.perf))
    stackcollapse = args.flamegraph_dir / "stackcollapse-perf.pl"
    flamegraph = args.flamegraph_dir / "flamegraph.pl"
    if not stackcollapse.exists() or not flamegraph.exists():
        raise RuntimeError("FlameGraph scripts were not found in {}".format(args.flamegraph_dir))

    db_name = "perf_flame_db_{}_{}".format(int(time.time()), os.getpid())
    server_log = out_dir / "rmdb_perf_server.log"
    workload_log = out_dir / "rmdb_perf_workload.log"
    perf_data = out_dir / "rmdb_perf.data"
    perf_unfold = out_dir / "rmdb_perf.unfold"
    perf_folded = out_dir / "rmdb_perf.folded"
    perf_svg = out_dir / "rmdb_perf_flamegraph.svg"

    for path in [server_log, workload_log, perf_data, perf_unfold, perf_folded, perf_svg]:
        path.unlink(missing_ok=True)

    with server_log.open("w", encoding="utf-8") as log:
        server = subprocess.Popen([str(rmdb_bin), db_name], cwd=str(build_dir), stdout=log, stderr=subprocess.STDOUT, text=True)

    sock = None
    perf_proc = None
    try:
        sock = connect_with_retry(10.0)
        with workload_log.open("w", encoding="utf-8") as log:
            run_sql_batch(sock, TABLE_DDL, log)
            run_sql_batch(sock, INDEX_DDL, log)
            run_sql_batch(sock, LOAD_SQL, log)
            log.write("SQL> set output_file off\n{}\n".format(checked_sql(sock, "set output_file off")))

            next_batch = run_workload_until(sock, time.time() + args.warmup_seconds, log, 0)

            perf_cmd = [
                str(args.perf),
                "record",
                "-F",
                str(args.frequency),
                "-e",
                args.event,
                "--call-graph",
                "fp",
                "-p",
                str(server.pid),
                "-o",
                str(perf_data),
                "--",
                "sleep",
                str(args.record_seconds),
            ]
            perf_proc = subprocess.Popen(perf_cmd, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
            next_batch = run_workload_until(sock, time.time() + args.record_seconds, log, next_batch)
            perf_output, _ = perf_proc.communicate(timeout=max(10.0, args.record_seconds + 10.0))
            log.write("perf output:\n{}\n".format(perf_output))
            if perf_proc.returncode != 0:
                raise RuntimeError("perf record failed with code {}; see {}".format(perf_proc.returncode, workload_log))
            send_exit(sock)
    finally:
        if perf_proc is not None and perf_proc.poll() is None:
            perf_proc.terminate()
            perf_proc.wait(timeout=5)
        if sock is not None:
            sock.close()
        if server.poll() is None:
            server.send_signal(signal.SIGINT)
            try:
                server.wait(timeout=10)
            except subprocess.TimeoutExpired:
                server.terminate()
                server.wait(timeout=5)

    if server.returncode not in (0, -signal.SIGINT):
        raise RuntimeError("server exited with code {}; see {}".format(server.returncode, server_log))

    run_command([str(args.perf), "script", "-i", str(perf_data)], ROOT, perf_unfold)
    run_command([str(stackcollapse), str(perf_unfold)], ROOT, perf_folded)
    run_command([str(flamegraph), "--title", "RMDB perf flame graph", str(perf_folded)], ROOT, perf_svg)

    print("SVG: {}".format(perf_svg))
    print("perf data: {}".format(perf_data))
    print("unfold: {}".format(perf_unfold))
    print("folded: {}".format(perf_folded))
    print("server log: {}".format(server_log))
    print("workload log: {}".format(workload_log))
    print("database: {}".format(build_dir / db_name))


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        print("error: {}".format(exc), file=sys.stderr)
        raise SystemExit(1)
