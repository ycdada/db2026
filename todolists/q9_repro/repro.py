#!/usr/bin/env python3
"""Throwaway multi-connection repro harness for 题目9 SI/SER schedules.

Drives interleaved transaction schedules over several TCP connections to the
rmdb server (127.0.0.1:8765), exactly in the step order given, then diffs the
database directory's output.txt against the spec's expected block.

Not part of the engine. Lives under todolists/q9_repro/ and is removed after use.
"""
import socket
import subprocess
import sys
import time
import os
import signal
import shutil
import threading

HOST = "127.0.0.1"
PORT = 8765
RMDB = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../build/bin/rmdb"))
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))


class Conn:
    def __init__(self, name):
        self.name = name
        self.s = socket.create_connection((HOST, PORT))
        self.s.settimeout(10)

    def exec(self, sql):
        self.s.sendall(sql.encode() + b"\x00")
        buf = b""
        while b"\x00" not in buf:
            chunk = self.s.recv(8192)
            if not chunk:
                break
            buf += chunk
        return buf.split(b"\x00")[0].decode(errors="replace")

    def close(self):
        try:
            self.s.sendall(b"exit\x00")
        except OSError:
            pass
        self.s.close()


def start_server(dbname):
    dbpath = os.path.join(ROOT, dbname)
    if os.path.isdir(dbpath):
        shutil.rmtree(dbpath)
    proc = subprocess.Popen(
        [RMDB, dbname],
        cwd=ROOT,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        preexec_fn=os.setsid,
    )
    for _ in range(100):
        try:
            socket.create_connection((HOST, PORT), timeout=0.2).close()
            break
        except OSError:
            time.sleep(0.1)
    else:
        raise RuntimeError("server did not start")
    return proc, dbpath


def stop_server(proc):
    try:
        os.killpg(os.getpgid(proc.pid), signal.SIGINT)
    except ProcessLookupError:
        return
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        os.killpg(os.getpgid(proc.pid), signal.SIGKILL)


def read_output(dbpath):
    p = os.path.join(dbpath, "output.txt")
    if not os.path.isfile(p):
        return ""
    with open(p) as f:
        return f.read()


def run_schedule(dbname, setup, steps):
    proc, dbpath = start_server(dbname)
    responses = []
    try:
        c0 = Conn("setup")
        for sql in setup:
            c0.exec(sql)
        c0.close()
        conns = {}
        for cname, sql in steps:
            if cname not in conns:
                conns[cname] = Conn(cname)
            resp = conns[cname].exec(sql)
            responses.append((cname, sql, resp))
        for c in conns.values():
            c.close()
        time.sleep(0.2)
        out = read_output(dbpath)
    finally:
        stop_server(proc)
    return responses, out


def banner(title):
    print("\n" + "=" * 70)
    print(title)
    print("=" * 70)


def show(responses, out, expected=None):
    for cname, sql, resp in responses:
        print(f"[{cname}] {sql}")
        if resp.strip():
            for line in resp.rstrip("\n").split("\n"):
                print(f"      | {line}")
    print("\n--- output.txt (got) ---")
    print(out if out else "(empty)")
    if expected is not None:
        if out == expected:
            print("=== MATCH ===")
        else:
            print("=== MISMATCH ===")
            print("--- expected ---")
            print(expected)


def table(headers, rows):
    line = "+" + "+".join(["-" * 18 for _ in headers]) + "+\n"
    body = line
    body += "|" + "|".join(f"{h:>17} " for h in headers) + "|\n"
    body += line
    for row in rows:
        body += "|" + "|".join(f"{str(v):>17} " for v in row) + "|\n"
    body += line
    body += f"Total record(s): {len(rows)}\n"
    return body


def ex1(level="SNAPSHOT ISOLATION"):
    setup = [
        "create table account (id int, balance int);",
        "insert into account values (1, 100);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "update account set balance = 120 where id = 1;"),
        ("T2", "begin;"),
        ("T2", "update account set balance = 90 where id = 1;"),
        ("T1", "commit;"),
        ("T2", "commit;"),
        ("T3", "select * from account where id = 1;"),
    ]
    expected = "abort\n" + table(["id", "balance"], [(1, 120)])
    return setup, steps, expected


def ex2():
    # PDF page 23-24: T2 has NO `set transaction isolation level` (only T1 sets SI).
    # Reproduced verbatim from the spec — this is the literal grader example.
    setup = [
        "create table counter_test (id int, val int);",
        "insert into counter_test values (1, 100);",
    ]
    steps = [
        ("T1", "set transaction isolation level snapshot isolation;"),
        ("T1", "begin;"),
        ("T1", "select * from counter_test where id = 1;"),
        ("T2", "begin;"),
        ("T2", "update counter_test set val = 200 where id = 1;"),
        ("T2", "commit;"),
        ("T1", "select * from counter_test where id = 1;"),
        ("T1", "commit;"),
    ]
    box = table(["id", "val"], [(1, 100)])
    return setup, steps, box + box


def ex3(level="SNAPSHOT ISOLATION"):
    setup = [
        "create table duty (doctor_id int, on_call int);",
        "insert into duty values (1, 1);",
        "insert into duty values (2, 1);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T2", "begin;"),
        ("T1", "select * from duty where doctor_id = 2;"),
        ("T2", "select * from duty where doctor_id = 1;"),
        ("T1", "update duty set on_call = 0 where doctor_id = 1;"),
        ("T2", "update duty set on_call = 0 where doctor_id = 2;"),
        ("T1", "commit;"),
        ("T2", "commit;"),
        ("T3", "select * from duty;"),
    ]
    read2 = table(["doctor_id", "on_call"], [(2, 1)])
    read1 = table(["doctor_id", "on_call"], [(1, 1)])
    if level == "SNAPSHOT ISOLATION":
        final = table(["doctor_id", "on_call"], [(1, 0), (2, 0)])
        expected = read2 + read1 + final
    else:
        final = table(["doctor_id", "on_call"], [(1, 0), (2, 1)])
        expected = read2 + read1 + "abort\n" + final
    return setup, steps, expected


def deadlock(level="SNAPSHOT ISOLATION"):
    # Two rows; T1 writes row1 then row2, T2 writes row2 then row1 (crossed).
    # Under SI there is no lock-deadlock; the second cross-write hits an active
    # writer and must abort (first-committer / first-writer wins).
    setup = [
        "create table t (id int, v int);",
        "insert into t values (1, 10);",
        "insert into t values (2, 20);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T2", "begin;"),
        ("T1", "update t set v = 11 where id = 1;"),
        ("T2", "update t set v = 22 where id = 2;"),
        ("T1", "update t set v = 12 where id = 2;"),  # row2 held by active T2 -> abort
        ("T2", "update t set v = 21 where id = 1;"),  # row1 held by aborted T1 -> ok
        ("T1", "commit;"),
        ("T2", "commit;"),
        ("T3", "select * from t;"),
    ]
    expected = "abort\n" + table(["id", "v"], [(1, 21), (2, 22)])
    return setup, steps, expected


def lost_update(level="SNAPSHOT ISOLATION"):
    # T1 reads old snapshot; T2 updates+commits; T1 updates same row -> abort
    # (writes onto a version committed after T1.start). Final value = T2's.
    setup = [
        "create table acct (id int, bal int);",
        "insert into acct values (1, 10);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "select * from acct where id = 1;"),     # sees 10
        ("T2", "begin;"),
        ("T2", "update acct set bal = 20 where id = 1;"),
        ("T2", "commit;"),
        ("T1", "select * from acct where id = 1;"),     # still 10 (snapshot)
        ("T1", "update acct set bal = 30 where id = 1;"),  # -> abort
        ("T1", "commit;"),
        ("T3", "select * from acct where id = 1;"),     # 20
    ]
    old = table(["id", "bal"], [(1, 10)])
    final = table(["id", "bal"], [(1, 20)])
    expected = old + old + "abort\n" + final
    return setup, steps, expected


def update_test(level="SNAPSHOT ISOLATION"):
    # Indexed table; SI update visibility + commit, plus a concurrent reader
    # that began before the update committed (must see old value).
    setup = [
        "create table idx_t (id int, v int);",
        "create index idx_t (id);",
        "insert into idx_t values (1, 10);",
        "insert into idx_t values (2, 20);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "select * from idx_t where id = 1;"),    # 10
        ("T2", "begin;"),
        ("T2", "update idx_t set v = 100 where id = 1;"),
        ("T1", "select * from idx_t where id = 1;"),    # still 10 (T2 uncommitted)
        ("T2", "commit;"),
        ("T1", "select * from idx_t where id = 1;"),    # still 10 (snapshot)
        ("T1", "commit;"),
        ("T3", "select * from idx_t where id = 1;"),    # 100
        ("T3", "select * from idx_t;"),
    ]
    old = table(["id", "v"], [(1, 10)])
    new = table(["id", "v"], [(1, 100)])
    final = table(["id", "v"], [(1, 100), (2, 20)])
    expected = old + old + old + new + final
    return setup, steps, expected


def idx_key_change(level="SNAPSHOT ISOLATION"):
    # Indexed column itself changes (id 1 -> 3). An SI reader whose snapshot
    # predates the change must still see the OLD row by its old key, even
    # though the physical index entry has moved.
    setup = [
        "create table idx_dirty (id int, v int);",
        "create index idx_dirty (id);",
        "insert into idx_dirty values (1, 10);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "select * from idx_dirty where id = 1;"),   # 10
        ("T2", "begin;"),
        ("T2", "update idx_dirty set id = 3 where id = 1;"),
        ("T2", "commit;"),
        ("T1", "select * from idx_dirty where id = 1;"),   # SI: still (1,10)
        ("T1", "commit;"),
        ("T3", "select * from idx_dirty;"),                # (3,10)
    ]
    old = table(["id", "v"], [(1, 10)])
    final = table(["id", "v"], [(3, 10)])
    return setup, steps, old + old + final


def del_idx_vis(level="SNAPSHOT ISOLATION"):
    # T2 deletes a row (index entry removed immediately during the statement).
    # An older SI snapshot T1 must still see the row, even via an index scan.
    setup = [
        "create table di (id int, v int);",
        "create index di (id);",
        "insert into di values (1, 10);",
        "insert into di values (2, 20);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "select * from di where id = 1;"),    # 10 (snapshot established)
        ("T2", "begin;"),
        ("T2", "delete from di where id = 1;"),
        ("T2", "commit;"),
        ("T1", "select * from di where id = 1;"),    # SI: STILL sees (1,10)
        ("T1", "commit;"),
        ("T3", "select * from di;"),                 # only (2,20)
    ]
    old = table(["id", "v"], [(1, 10)])
    final = table(["id", "v"], [(2, 20)])
    return setup, steps, old + old + final


def del_reinsert(level="SNAPSHOT ISOLATION"):
    # T2 deletes id=1 then re-inserts id=1 with a new value, commits.
    # Older snapshot T1 must still see the original (1,10).
    setup = [
        "create table dr (id int, v int);",
        "create index dr (id);",
        "insert into dr values (1, 10);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "select * from dr where id = 1;"),    # 10
        ("T2", "begin;"),
        ("T2", "delete from dr where id = 1;"),
        ("T2", "insert into dr values (1, 99);"),
        ("T2", "commit;"),
        ("T1", "select * from dr where id = 1;"),    # SI: still (1,10)
        ("T1", "commit;"),
        ("T3", "select * from dr;"),                 # (1,99)
    ]
    old = table(["id", "v"], [(1, 10)])
    final = table(["id", "v"], [(1, 99)])
    return setup, steps, old + old + final


def read_own_write(level="SNAPSHOT ISOLATION"):
    # One SI txn: UPDATE then SELECT must see own write; multi-row UPDATE;
    # then a concurrent older snapshot must not see any of it until commit.
    setup = [
        "create table row (id int, v int);",
        "insert into row values (1, 10);",
        "insert into row values (2, 20);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "update row set v = 11 where id = 1;"),
        ("T1", "select * from row where id = 1;"),     # own write: 11
        ("T1", "update row set v = 120 where id = 2;"),
        ("T1", "select * from row;"),                  # (1,11),(2,120)
        ("T1", "commit;"),
        ("T2", f"set transaction isolation level {level};"),
        ("T2", "begin;"),
        ("T2", "select * from row;"),                  # (1,11),(2,120)
        ("T2", "commit;"),
    ]
    one = table(["id", "v"], [(1, 11)])
    final = table(["id", "v"], [(1, 11), (2, 120)])
    return setup, steps, one + final + final


def si_init_data(level="SNAPSHOT ISOLATION"):
    # Initialize the row INSIDE an SI transaction (not a plain statement),
    # so last_commit_ts advances via MVCC commit. Then run lost-update.
    setup = []
    steps = [
        ("S", f"set transaction isolation level {level};"),
        ("S", "begin;"),
        ("S", "create table z (id int, v int);"),
        ("S", "commit;"),
        ("S", "begin;"),
        ("S", "insert into z values (1, 10);"),
        ("S", "commit;"),
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "select * from z where id = 1;"),       # 10
        ("T2", "begin;"),
        ("T2", "update z set v = 20 where id = 1;"),
        ("T2", "commit;"),
        ("T1", "select * from z where id = 1;"),       # SI: still 10
        ("T1", "update z set v = 30 where id = 1;"),    # abort
        ("T1", "commit;"),
        ("T3", "select * from z;"),                     # 20
    ]
    old = table(["id", "v"], [(1, 10)])
    final = table(["id", "v"], [(1, 20)])
    return setup, steps, old + old + "abort\n" + final


def range_update_conflict(level="SNAPSHOT ISOLATION"):
    setup = [
        "create table ru (id int, v int);",
        "insert into ru values (1, 10);",
        "insert into ru values (2, 20);",
        "insert into ru values (3, 30);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "select * from ru where id >= 2;"),
        ("T2", "begin;"),
        ("T2", "update ru set v = 200 where id >= 2;"),
        ("T2", "commit;"),
        ("T1", "select * from ru where id >= 2;"),
        ("T1", "update ru set v = 300 where id >= 2;"),
        ("T1", "commit;"),
        ("T3", "select * from ru;"),
    ]
    old = table(["id", "v"], [(2, 20), (3, 30)])
    final = table(["id", "v"], [(1, 10), (2, 200), (3, 200)])
    return setup, steps, old + old + "abort\n" + final


def range_delete_conflict(level="SNAPSHOT ISOLATION"):
    setup = [
        "create table rd (id int, v int);",
        "insert into rd values (1, 10);",
        "insert into rd values (2, 20);",
        "insert into rd values (3, 30);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "select * from rd where id >= 2;"),
        ("T2", "begin;"),
        ("T2", "delete from rd where id >= 2;"),
        ("T2", "commit;"),
        ("T1", "select * from rd where id >= 2;"),
        ("T1", "delete from rd where id >= 2;"),
        ("T1", "commit;"),
        ("T3", "select * from rd;"),
    ]
    old = table(["id", "v"], [(2, 20), (3, 30)])
    final = table(["id", "v"], [(1, 10)])
    return setup, steps, old + old + "abort\n" + final


def range_reconstruct_after_delete_insert(level="SNAPSHOT ISOLATION"):
    setup = [
        "create table rr (id int, v int);",
        "insert into rr values (1, 10);",
        "insert into rr values (2, 20);",
        "insert into rr values (3, 30);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T2", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "select * from rr where id >= 2;"),
        ("T2", "begin;"),
        ("T2", "delete from rr where id >= 2;"),
        ("T2", "insert into rr values (2, 200);"),
        ("T2", "insert into rr values (3, 300);"),
        ("T2", "commit;"),
        ("T1", "select * from rr where id >= 2;"),
        ("T1", "commit;"),
        ("T3", "select * from rr;"),
    ]
    old = table(["id", "v"], [(2, 20), (3, 30)])
    final = table(["id", "v"], [(1, 10), (2, 200), (3, 300)])
    return setup, steps, old + old + final


def computed_set_diag(level="SNAPSHOT ISOLATION"):
    setup = [
        "create table cs (id int, v int);",
        "insert into cs values (1, 10);",
    ]
    steps = [
        ("T1", f"set transaction isolation level {level};"),
        ("T1", "begin;"),
        ("T1", "update cs set v = v + 1 where id = 1;"),
        ("T1", "commit;"),
        ("T2", "select * from cs;"),
    ]
    expected = "failure\n" + table(["id", "v"], [(1, 10)])
    return setup, steps, expected


def noset_dirty_read():
    # T1 SI reads; T2 (NO SET) updates uncommitted -> T1 must NOT see it.
    setup = [
        "create table dr2 (id int, v int);",
        "insert into dr2 values (1, 10);",
    ]
    steps = [
        ("T1", "set transaction isolation level snapshot isolation;"),
        ("T1", "begin;"),
        ("T1", "select * from dr2 where id = 1;"),
        ("T2", "begin;"),
        ("T2", "update dr2 set v = 99 where id = 1;"),
        ("T1", "select * from dr2 where id = 1;"),
        ("T2", "commit;"),
        ("T1", "select * from dr2 where id = 1;"),
        ("T1", "commit;"),
    ]
    # SI: T1's snapshot is stable at 10 across all three reads.
    expected = table(["id", "v"], [(1, 10)]) * 3
    return setup, steps, expected


def noset_insert_vis():
    # T1 SI snapshot; T2 (NO SET) inserts a new row + commits -> T1 must NOT see it.
    setup = [
        "create table iv (id int, v int);",
        "insert into iv values (1, 10);",
    ]
    steps = [
        ("T1", "set transaction isolation level snapshot isolation;"),
        ("T1", "begin;"),
        ("T1", "select * from iv;"),
        ("T2", "begin;"),
        ("T2", "insert into iv values (2, 20);"),
        ("T2", "commit;"),
        ("T1", "select * from iv;"),
        ("T1", "commit;"),
        ("T3", "select * from iv;"),
    ]
    # SI: T1 snapshot shows only (1,10) both times; T3 (new txn) sees both.
    expected = table(["id", "v"], [(1, 10)]) * 2 + table(["id", "v"], [(1, 10), (2, 20)])
    return setup, steps, expected


def noset_delete_vis():
    # T1 SI snapshot; T2 (NO SET) deletes the row + commits -> T1 must still see it.
    setup = [
        "create table dv2 (id int, v int);",
        "insert into dv2 values (1, 10);",
        "insert into dv2 values (2, 20);",
    ]
    steps = [
        ("T1", "set transaction isolation level snapshot isolation;"),
        ("T1", "begin;"),
        ("T1", "select * from dv2;"),
        ("T2", "begin;"),
        ("T2", "delete from dv2 where id = 1;"),
        ("T2", "commit;"),
        ("T1", "select * from dv2;"),
        ("T1", "commit;"),
        ("T3", "select * from dv2;"),
    ]
    # SI: T1 snapshot still sees both rows after T2's delete; T3 sees only (2,20).
    expected = table(["id", "v"], [(1, 10), (2, 20)]) * 2 + table(["id", "v"], [(2, 20)])
    return setup, steps, expected


def noset_ww_conflict():
    # T1 SI writes; T2 (NO SET) writes same row. WW-conflict handling.
    setup = [
        "create table ww (id int, v int);",
        "insert into ww values (1, 10);",
    ]
    steps = [
        ("T1", "set transaction isolation level snapshot isolation;"),
        ("T1", "begin;"),
        ("T1", "update ww set v = 11 where id = 1;"),
        ("T2", "begin;"),
        ("T2", "update ww set v = 22 where id = 1;"),
        ("T1", "commit;"),
        ("T2", "commit;"),
        ("T3", "select * from ww where id = 1;"),
    ]
    # T1(SI) commits 11 first; T2(RC) is last-writer-wins -> final 22.
    expected = table(["id", "v"], [(1, 22)])
    return setup, steps, expected


CASES = {
    "ex1_si": lambda: ex1("SNAPSHOT ISOLATION"),
    "ex1_ser": lambda: ex1("SERIALIZABLE"),
    "ex2": ex2,
    "ex3_si": lambda: ex3("SNAPSHOT ISOLATION"),
    "ex3_ser": lambda: ex3("SERIALIZABLE"),
    "deadlock": deadlock,
    "lost_update": lost_update,
    "update_test": update_test,
    "idx_key_change": idx_key_change,
    "del_idx_vis": del_idx_vis,
    "del_reinsert": del_reinsert,
    "read_own_write": read_own_write,
    "si_init_data": si_init_data,
    "range_update_conflict": range_update_conflict,
    "range_delete_conflict": range_delete_conflict,
    "range_reconstruct_after_delete_insert": range_reconstruct_after_delete_insert,
    "computed_set_diag": computed_set_diag,
    "noset_dirty_read": noset_dirty_read,
    "noset_insert_vis": noset_insert_vis,
    "noset_delete_vis": noset_delete_vis,
    "noset_ww_conflict": noset_ww_conflict,
}


def race_test(rounds=200):
    """Fire two conflicting UPDATEs to the same row as simultaneously as
    possible across many rounds, to expose a TOCTOU window between
    CheckMvccWriteConflict and MvccUpdate. Under SI exactly one of the two
    concurrent updates may commit; the other's UPDATE must abort. A round
    where BOTH commit (final value from a 'loser') indicates a lost update."""
    proc, dbpath = start_server("repro9_race")
    bad = 0
    try:
        setup = Conn("setup")
        setup.exec("create table r (id int, v int);")
        setup.exec("insert into r values (1, 0);")
        setup.close()
        for i in range(rounds):
            a = Conn("A")
            b = Conn("B")
            a.exec("set transaction isolation level snapshot isolation;")
            b.exec("set transaction isolation level snapshot isolation;")
            a.exec("begin;")
            b.exec("begin;")
            res = {}
            def fire(conn, val, key):
                res[key] = conn.exec(f"update r set v = {val} where id = 1;")
            ta = threading.Thread(target=fire, args=(a, i * 2 + 1, "a"))
            tb = threading.Thread(target=fire, args=(b, i * 2 + 2, "b"))
            ta.start(); tb.start(); ta.join(); tb.join()
            a.exec("commit;")
            b.exec("commit;")
            a_abort = "abort" in res["a"]
            b_abort = "abort" in res["b"]
            chk = Conn("chk")
            chk.exec("set transaction isolation level snapshot isolation;")
            final = chk.exec("select * from r where id = 1;")
            chk.close()
            a.close(); b.close()
            # exactly one of the two concurrent writers should have aborted
            if a_abort == b_abort:
                bad += 1
                print(f"round {i}: BOTH-{'ABORT' if a_abort else 'COMMIT'} "
                      f"a={res['a']!r} b={res['b']!r}\n{final}")
            if i % 50 == 0:
                print(f"  round {i}: a_abort={a_abort} b_abort={b_abort}")
        print(f"\nrace: {bad} bad rounds out of {rounds}")
    finally:
        stop_server(proc)
    sys.exit(1 if bad else 0)


def main():
    which = sys.argv[1] if len(sys.argv) > 1 else "all"
    if which == "race":
        return race_test(int(sys.argv[2]) if len(sys.argv) > 2 else 200)
    names = list(CASES) if which == "all" else [which]
    results = {}
    for name in names:
        setup, steps, expected = CASES[name]()
        banner(name)
        responses, out = run_schedule(f"repro9_{name}", setup, steps)
        show(responses, out, expected)
        results[name] = (out == expected)
    banner("SUMMARY")
    for name, ok in results.items():
        print(f"  {name}: {'MATCH' if ok else 'MISMATCH'}")
    sys.exit(0 if all(results.values()) else 1)


if __name__ == "__main__":
    main()
