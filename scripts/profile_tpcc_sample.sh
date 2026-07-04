#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build-profile"
OUT_DIR="$ROOT_DIR/build/profile-tpcc"
DB_DIR="$OUT_DIR/db"
DURATION=30
CLIENTS=1
FREQ=199

usage() {
    cat <<'USAGE'
Usage:
  scripts/profile_tpcc_sample.sh [options]

Options:
  --build-dir <dir>    Build directory containing bin/rmdb. Default: build-profile
  -o, --output-dir     Output directory for perf/flamegraph files. Default: build/profile-tpcc
  --db-dir <dir>       Temporary database directory. Default: <output-dir>/db
  --duration <sec>     perf sampling duration. Default: 30
  --clients <n>        Concurrent SQL clients. Default: 1
  -F, --freq <hz>      perf sampling frequency. Default: 199

The script creates a temporary TPCC-shaped database from src/test/performance_test/table_data,
runs the SQL patterns from 决赛性能测试SQL示例.txt in a loop, and writes flamegraph.svg under
the output directory.
USAGE
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        -o|--output-dir)
            OUT_DIR="$2"
            shift 2
            ;;
        --db-dir)
            DB_DIR="$2"
            shift 2
            ;;
        --duration)
            DURATION="$2"
            shift 2
            ;;
        --clients)
            CLIENTS="$2"
            shift 2
            ;;
        -F|--freq)
            FREQ="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

RMDB_BIN="$BUILD_DIR/bin/rmdb"
DATA_DIR="$ROOT_DIR/src/test/performance_test/table_data"

if [[ ! -x "$RMDB_BIN" ]]; then
    echo "rmdb binary not found: $RMDB_BIN" >&2
    echo "Build it first, for example: cmake -S . -B build-profile -DCMAKE_BUILD_TYPE=RelWithDebInfo && cmake --build build-profile -j" >&2
    exit 1
fi

if [[ ! -d "$DATA_DIR" ]]; then
    echo "table_data directory not found: $DATA_DIR" >&2
    exit 1
fi

mkdir -p "$OUT_DIR"
rm -rf "$DB_DIR"

SERVER_LOG="$OUT_DIR/rmdb.log"
WORKLOAD_LOG="$OUT_DIR/workload.log"
: > "$SERVER_LOG"
: > "$WORKLOAD_LOG"

SERVER_PID=""
PROFILE_PID=""
WORKER_PIDS=()

cleanup() {
    set +e
    if [[ -n "$PROFILE_PID" ]] && kill -0 "$PROFILE_PID" 2>/dev/null; then
        wait "$PROFILE_PID"
    fi
    if [[ -n "$SERVER_PID" ]] && kill -0 "$SERVER_PID" 2>/dev/null; then
        kill -INT "$SERVER_PID" 2>/dev/null
        wait "$SERVER_PID" 2>/dev/null
    fi
}
trap cleanup EXIT

"$RMDB_BIN" "$DB_DIR" > "$SERVER_LOG" 2>&1 &
SERVER_PID=$!

for _ in {1..50}; do
    if exec 9<>/dev/tcp/127.0.0.1/8765; then
        exec 9>&-
        exec 9<&-
        break
    fi
    sleep 0.1
done

if ! kill -0 "$SERVER_PID" 2>/dev/null; then
    echo "rmdb server failed to start. See $SERVER_LOG" >&2
    exit 1
fi

send_sql_fd() {
    local fd="$1"
    local sql="$2"
    local tolerate_error="${3:-0}"
    local response=""
    printf '%s\0' "$sql" >&"$fd"
    IFS= read -r -d '' response <&"$fd" || true
    if [[ "$response" == *"failure"* || "$response" == *"abort"* || "$response" == Error:* ]]; then
        {
            echo "SQL failed:"
            echo "$sql"
            echo "$response"
        } >> "$WORKLOAD_LOG"
        if [[ "$tolerate_error" == "1" ]]; then
            return 0
        fi
        return 1
    fi
}

send_workload_sql() {
    send_sql_fd "$1" "$2" 1
}

run_setup() {
    exec 3<>/dev/tcp/127.0.0.1/8765
    send_sql_fd 3 "set output_file off"
    send_sql_fd 3 "create table warehouse (w_id int, w_name char(16), w_street_1 char(32), w_street_2 char(32), w_city char(32), w_state char(4), w_zip char(16), w_tax float, w_ytd float);"
    send_sql_fd 3 "create table district (d_id int, d_w_id int, d_name char(16), d_street_1 char(32), d_street_2 char(32), d_city char(32), d_state char(4), d_zip char(16), d_tax float, d_ytd float, d_next_o_id int);"
    send_sql_fd 3 "create table customer (c_id int, c_d_id int, c_w_id int, c_first char(24), c_middle char(4), c_last char(24), c_street_1 char(32), c_street_2 char(32), c_city char(32), c_state char(4), c_zip char(16), c_phone char(20), c_since char(24), c_credit char(4), c_credit_lim float, c_discount float, c_balance float, c_ytd_payment float, c_payment_cnt int, c_delivery_cnt int, c_data char(500));"
    send_sql_fd 3 "create table history (h_c_id int, h_c_d_id int, h_c_w_id int, h_d_id int, h_w_id int, h_date char(24), h_amount float, h_data char(64));"
    send_sql_fd 3 "create table item (i_id int, i_im_id int, i_name char(32), i_price float, i_data char(64));"
    send_sql_fd 3 "create table stock (s_i_id int, s_w_id int, s_quantity int, s_dist_01 char(32), s_dist_02 char(32), s_dist_03 char(32), s_dist_04 char(32), s_dist_05 char(32), s_dist_06 char(32), s_dist_07 char(32), s_dist_08 char(32), s_dist_09 char(32), s_dist_10 char(32), s_ytd float, s_order_cnt int, s_remote_cnt int, s_data char(64));"
    send_sql_fd 3 "create table orders (o_id int, o_d_id int, o_w_id int, o_c_id int, o_entry_d char(24), o_carrier_id int, o_ol_cnt int, o_all_local int);"
    send_sql_fd 3 "create table new_orders (no_o_id int, no_d_id int, no_w_id int);"
    send_sql_fd 3 "create table order_line (ol_o_id int, ol_d_id int, ol_w_id int, ol_number int, ol_i_id int, ol_supply_w_id int, ol_delivery_d char(24), ol_quantity int, ol_amount float, ol_dist_info char(32));"

    send_sql_fd 3 "load '$DATA_DIR/warehouse.csv' into warehouse;"
    send_sql_fd 3 "load '$DATA_DIR/district.csv' into district;"
    send_sql_fd 3 "load '$DATA_DIR/customer.csv' into customer;"
    send_sql_fd 3 "load '$DATA_DIR/history.csv' into history;"
    send_sql_fd 3 "load '$DATA_DIR/item.csv' into item;"
    send_sql_fd 3 "load '$DATA_DIR/stock.csv' into stock;"
    send_sql_fd 3 "load '$DATA_DIR/orders.csv' into orders;"
    send_sql_fd 3 "load '$DATA_DIR/new_orders.csv' into new_orders;"
    send_sql_fd 3 "load '$DATA_DIR/order_line.csv' into order_line;"

    send_sql_fd 3 "create index warehouse(w_id);"
    send_sql_fd 3 "create index district(d_w_id, d_id);"
    send_sql_fd 3 "create index customer(c_w_id, c_d_id, c_id);"
    send_sql_fd 3 "create index item(i_id);"
    send_sql_fd 3 "create index stock(s_w_id, s_i_id);"
    send_sql_fd 3 "create index orders(o_w_id, o_d_id, o_id);"
    send_sql_fd 3 "create index new_orders(no_w_id, no_d_id, no_o_id);"
    send_sql_fd 3 "create index order_line(ol_w_id, ol_d_id, ol_o_id, ol_number);"
    send_sql_fd 3 "exit"
    exec 3>&-
    exec 3<&-
}

run_worker() {
    local client_id="$1"
    local end_epoch="$2"
    local i=0
    exec 4<>/dev/tcp/127.0.0.1/8765
    send_workload_sql 4 "set output_file off"
    while [[ "$(date +%s)" -lt "$end_epoch" ]]; do
        i=$((i + 1))
        local d=$((i % 3 + 1))
        local c=$((i % 10 + 1))
        local item=$((i % 10 + 1))
        local qty=$(((i * 7) % 90 + 10))
        local order_id=$((1000000 + client_id * 100000 + i))
        local next_order_id=$((order_id + 1))
        local recent_order_floor=$((next_order_id - 20))

        send_workload_sql 4 "begin;"
        send_workload_sql 4 "select c_discount, c_last, c_credit, w_tax from customer, warehouse where w_id=1 and c_w_id=w_id and c_d_id=$d and c_id=$c;"
        send_workload_sql 4 "select d_next_o_id, d_tax from district where d_id=$d and d_w_id=1;"
        send_workload_sql 4 "update district set d_next_o_id=$next_order_id where d_id=$d and d_w_id=1;"
        send_workload_sql 4 "insert into orders values($order_id, $d, 1, $c, '2026-07-04 20:00:00', 0, 1, 1);"
        send_workload_sql 4 "insert into new_orders values($order_id, $d, 1);"
        send_workload_sql 4 "select i_price, i_name, i_data from item where i_id=$item;"
        send_workload_sql 4 "select s_quantity, s_data, s_dist_01, s_dist_02, s_dist_03, s_dist_04, s_dist_05, s_dist_06, s_dist_07, s_dist_08, s_dist_09, s_dist_10 from stock where s_i_id=$item and s_w_id=1;"
        send_workload_sql 4 "update stock set s_quantity=$qty where s_i_id=$item and s_w_id=1;"
        send_workload_sql 4 "insert into order_line values($order_id, $d, 1, 1, $item, 1, '2026-07-04 20:00:00', 5, 10.5, 'profile-dist-info');"
        send_workload_sql 4 "commit;"

        send_workload_sql 4 "begin;"
        send_workload_sql 4 "update warehouse set w_ytd=w_ytd+10.5 where w_id=1;"
        send_workload_sql 4 "select w_street_1, w_street_2, w_city, w_state, w_zip, w_name from warehouse where w_id=1;"
        send_workload_sql 4 "update district set d_ytd=d_ytd+10.5 where d_w_id=1 and d_id=$d;"
        send_workload_sql 4 "select d_street_1, d_street_2, d_city, d_state, d_zip, d_name from district where d_w_id=1 and d_id=$d;"
        send_workload_sql 4 "select c_first, c_middle, c_last, c_street_1, c_street_2, c_city, c_state, c_zip, c_phone, c_credit, c_credit_lim, c_discount, c_balance, c_since from customer where c_w_id=1 and c_d_id=$d and c_id=$c;"
        send_workload_sql 4 "update customer set c_balance=10.5 where c_w_id=1 and c_d_id=$d and c_id=$c;"
        send_workload_sql 4 "insert into history values($c, $d, 1, $d, 1, '2026-07-04 20:00:00', 10.5, 'profile-history');"
        send_workload_sql 4 "commit;"

        send_workload_sql 4 "begin;"
        send_workload_sql 4 "select min(no_o_id) as min_o_id from new_orders where no_d_id=$d and no_w_id=1;"
        send_workload_sql 4 "delete from new_orders where no_o_id=$order_id and no_d_id=$d and no_w_id=1;"
        send_workload_sql 4 "select o_c_id from orders where o_id=$order_id and o_d_id=$d and o_w_id=1;"
        send_workload_sql 4 "update orders set o_carrier_id=1 where o_id=$order_id and o_d_id=$d and o_w_id=1;"
        send_workload_sql 4 "update order_line set ol_delivery_d='2026-07-04 20:00:01' where ol_o_id=$order_id and ol_d_id=$d and ol_w_id=1;"
        send_workload_sql 4 "select sum(ol_amount) as sum_amount from order_line where ol_o_id=$order_id and ol_d_id=$d;"
        send_workload_sql 4 "update customer set c_balance=21.0, c_delivery_cnt=c_delivery_cnt+1 where c_id=$c and c_d_id=$d and c_w_id=1;"
        send_workload_sql 4 "commit;"

        send_workload_sql 4 "select count(c_id) as count_c_id from customer where c_w_id=1 and c_d_id=$d and c_last='BARRBARRBARR';"
        send_workload_sql 4 "select c_balance, c_first, c_middle, c_last from customer where c_w_id=1 and c_d_id=$d and c_last='BARRBARRBARR' order by c_first;"
        send_workload_sql 4 "select c_balance, c_first, c_middle, c_last from customer where c_w_id=1 and c_d_id=$d and c_id=$c;"
        send_workload_sql 4 "select o_id, o_entry_d, o_carrier_id from orders where o_w_id=1 and o_d_id=$d and o_c_id=$c and o_id=$order_id;"
        send_workload_sql 4 "select ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_delivery_d from order_line where ol_w_id=1 and ol_d_id=$d and ol_o_id=$order_id;"

        send_workload_sql 4 "select d_next_o_id from district where d_id=$d and d_w_id=1;"
        send_workload_sql 4 "select ol_i_id from order_line where ol_w_id=1 and ol_d_id=$d and ol_o_id<$next_order_id and ol_o_id>=$recent_order_floor;"
        send_workload_sql 4 "select count(*) as count_stock from stock where s_w_id=1 and s_i_id=$item and s_quantity<50;"
    done
    send_workload_sql 4 "exit"
    exec 4>&-
    exec 4<&-
}

echo "Setting up TPCC-shaped sample database..."
run_setup

END_EPOCH=$(($(date +%s) + DURATION))
"$ROOT_DIR/scripts/profile_flamegraph.sh" -o "$OUT_DIR" -F "$FREQ" --pid "$SERVER_PID" --duration "$DURATION" &
PROFILE_PID=$!

for client_id in $(seq 1 "$CLIENTS"); do
    run_worker "$client_id" "$END_EPOCH" &
    WORKER_PIDS+=("$!")
done

wait "$PROFILE_PID"
PROFILE_PID=""

for worker_pid in "${WORKER_PIDS[@]}"; do
    if kill -0 "$worker_pid" 2>/dev/null; then
        kill "$worker_pid" 2>/dev/null || true
    fi
done
for worker_pid in "${WORKER_PIDS[@]}"; do
    wait "$worker_pid" 2>/dev/null || true
done

echo "Wrote $OUT_DIR/flamegraph.svg"
echo "Raw perf data: $OUT_DIR/perf.data"
echo "Workload log: $WORKLOAD_LOG"
