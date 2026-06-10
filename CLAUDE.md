# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 限制

你被禁止更改initial commit 中就有的函数，变量名和接口

## Project Overview

**RMDB** — a relational database management system built for the 2026 National College Student Computer System Capability Competition (database track). It implements a full SQL execution pipeline from parser through query optimization, execution, storage, indexing, transaction management (2PL), and WAL-based crash recovery (ARIES). The target is running TPC-C workloads.

Originally developed by Renmin University of China's database teaching team.

## Build & Run

```bash
# Build (from rmdb/ directory)
mkdir -p build && cd build
cmake .. && make -j$(nproc)

# Run the server
./bin/rmdb <database_name>

# Run the client (separate terminal)
cd build && ./bin/rmdb_client
```

### Compiler flags
- Debug mode (default): `-Wall -O0 -g -ggdb3` includes full debug symbols, no optimization
- Release mode: uncomment `-O3` line in `CMakeLists.txt` and comment out the debug flag line

### Tests

```bash
# Build and run unit tests (from build/)
make unit_test
./bin/unit_test

# Run a single test (registered via gtest)
./bin/unit_test --gtest_filter="BufferPoolManagerTest.SampleTest"
./bin/unit_test --gtest_filter="LRUReplacerTest.*"
./bin/unit_test --gtest_filter="StorageTest.*"
./bin/unit_test --gtest_filter="RecordManagerTest.*"
```

### Dependencies
- GCC 7.1+ (C++17 support required)
- CMake 3.16+
- flex, bison, readline
- Google Test (bundled in `deps/googletest/`)

## Architecture Overview

The server (`rmdb.cpp`) starts as a TCP server (port 8765, `MAX_CONN_LIMIT = 8`) that spawns a thread per client connection. All manager objects (`DiskManager`, `BufferPoolManager`, `RmManager`, `IxManager`, `SmManager`, `LockManager`, `TransactionManager`, `Planner`, `Optimizer`, `QlManager`, `LogManager`, `RecoveryManager`, `Portal`, `Analyze`) are constructed once as globals in `rmdb.cpp` and shared across connection threads. Each client request passes through the full pipeline:

```
SQL text → Parser (flex/bison) → AST → Analyzer → Query
  → Planner (logical opt → physical opt) → Plan → Portal → Executor tree
  → QlManager → results back to client
```

### Module Layout (`src/`)

| Module | Description |
|---|---|
| `parser/` | SQL parser using flex (`lex.l`) + bison (`yacc.y`), produces AST in `ast.h` |
| `analyze/` | Semantic analysis: resolves column/table references, converts AST → `Query` |
| `optimizer/` | Query optimization: `Planner` does logical optimization (predicate pushdown) then physical optimization (scan type, join order) |
| `execution/` | Volcano-style iterator model. `AbstractExecutor` base class with concrete executors for seq scan, index scan, nested-loop join, projection, insert, delete, update, sort |
| `portal.h` | Bridge layer: converts `Plan` tree into `AbstractExecutor` tree via `convert_plan_executor()` |
| `storage/` | `DiskManager` (file/disk I/O), `BufferPoolManager` (page cache with LRU replacer), `Page` (4KB pages) |
| `record/` | Record (tuple) manager: `RmFileHandle` per table file, bitmap-based slot management |
| `index/` | B+ tree index: `IxManager`, `IxIndexHandle`, `IxScan`. Supports multi-column keys. |
| `system/` | `SmManager`: metadata management (database/table/index creation/deletion), DDL execution |
| `transaction/` | Lock Manager (multi-granularity: table + record locks, 2PL), `TransactionManager` (begin/commit/abort, write set, undo logs), MVCC infrastructure |
| `recovery/` | WAL-based crash recovery: `LogManager`, `RecoveryManager` (analyze/redo/undo phases, ARIES-style) |
| `replacer/` | `LRUReplacer` for buffer pool page eviction |
| `common/` | Shared types (ColType, Rid, CompOp, Value, Condition, TabCol, SetClause, Context) and config constants |
| `test/` | Performance test data (TPC-C table CSVs) |
| `rmdb_client/` | TCP client with readline-based REPL |

### Data Flow for a SELECT query

1. **Parser**: `yy_scan_string()` + `yyparse()` → `ast::parse_tree` (AST root)
2. **Analyzer**: `Analyze::do_analyze()` resolves column types, validates tables/columns, produces `Query` with `conds`, `cols`, `tables`
3. **Planner**: `Planner::plan_query()` → logical optimization (predicate pushdown) → `physical_optimization()` (choose SeqScan/IndexScan, build join tree, add projection/sort)
4. **Plan**: Output is a tree of `Plan` objects (`ScanPlan`, `JoinPlan`, `ProjectionPlan`, `SortPlan`, `DMLPlan`, etc.)
5. **Portal**: `Portal::start()` converts Plan → Executor tree via `convert_plan_executor()` (recursive `dynamic_pointer_cast` dispatch)
6. **Executor**: `Portal::run()` → `QlManager::select_from()` pulls tuples from executor tree root
7. **Results**: Serialized to `data_send` buffer and written back to client socket

### Key Design Decisions

- **Page size**: 4KB (`PAGE_SIZE = 4096`)
- **Buffer pool**: 65536 frames = 256MB (`BUFFER_POOL_SIZE = 65536`)
- **Record limit**: Max record size 512 bytes (`RM_MAX_RECORD_SIZE = 512`)
- **Index**: B+ tree, multi-column, order based on key size
- **Concurrency control**: Two-Phase Locking (2PL) with multi-granularity locks (table IS/IX/S/X/SIX, record S/X). Lock upgrade and deadlock prevention via transaction abort.
- **Recovery**: WAL with INSERT/UPDATE/DELETE/BEGIN/COMMIT/ABORT log record types. Redo + undo phases at startup.
- **C++17 features used extensively**: `std::shared_ptr`, `std::unique_ptr`, `std::optional`, `std::variant`-like enum dispatch, `dynamic_pointer_cast` for plan type dispatch, `std::atomic`, `std::thread`
- **Error handling**: All custom exceptions inherit from `RMDBError` (defined in `errors.h`)
- **Config**: Global tuning knobs in `common/config.h` (buffer pool size, page size, log timeout, etc.)

### Notable Patterns

- **Plan type dispatch**: Uses `std::dynamic_pointer_cast<>()` checks on `Plan` shared pointers rather than a visitor pattern — every type case is an `if(auto x = std::dynamic_pointer_cast<...>(plan))` chain
- **Executor tree**: Standard volcano iterator model — each executor implements `beginTuple()`, `nextTuple()`, `is_end()`, `Next()`. The `Next()` method returns `std::unique_ptr<RmRecord>` (null for end).
- **Record storage**: Per-page bitmap tracks slot occupancy (`bitmap.h`). Pages chained via `next_free_page_no` for free space management.
- **Context object**: `Context` wraps per-request state (transaction, lock manager, log manager, output buffer) passed through the execution pipeline
- **KV config**: Runtime knobs set via SQL `set enable_nestloop = true/false` → `SetKnobPlan` → `SetKnobExecutor`
- **Recovery**: WAL log file (`db.log`) with analyze → redo → undo phases at server startup
