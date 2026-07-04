# Repository Guidelines

## Project Structure & Module Organization

This is a C++ RMDB database system based on GCC7.5. Core source lives in `src/`, split by subsystem: `storage/`, `record/`, `index/`, `system/`, `execution/`, `optimizer/`, `parser/`, `transaction/`, `recovery/`, `replacer/`, `analyze/`, and `common/`. The server entry point is `src/rmdb.cpp`; the standalone client is in `rmdb_client/`. Tests are in `src/unit_test.cpp` and `src/parser/test_parser.cpp`. Performance CSV fixtures are under `src/test/performance_test/table_data/`. Documentation is in `docs/`; vendored GoogleTest is in `deps/`.

## Build, Test, and Development Commands

- `cmake -S . -B build`: configure the main C++17 project.
- `cmake --build build -j`: build libraries and binaries into `build/bin/`.
- `ctest --test-dir build --output-on-failure`: run CTest-registered tests such as `test_parser`.
- `./build/bin/unit_test`: run the GoogleTest unit test binary, which is not currently registered with CTest.
- `./build/bin/rmdb testdb`: start the RMDB server on TCP port `8765`, creating `testdb` if needed.
- `cmake -S rmdb_client -B build/rmdb_client && cmake --build build/rmdb_client -j`: build the C++20 client separately when needed.

Install prerequisites from the README first: GCC with C++17 support, CMake 3.16+, Flex, Bison, and Readline.

## Coding Style & Naming Conventions

Follow the existing C++ style: 4-space indentation, K&R-style braces, and comments only for non-obvious behavior. Use `PascalCase` for classes and major types, `snake_case` for functions and locals, trailing underscores for private members such as `txn_id_`, and `UPPER_SNAKE_CASE` or `static constexpr` for constants. Keep filenames lowercase with underscores. Edit `src/parser/yacc.y` and `src/parser/lex.l` for grammar changes; generated `yacc.tab.*` and `lex.yy.*` files may change after regeneration.

## Testing Guidelines

Use GoogleTest for C++ unit coverage. Add targeted tests near related cases, with names like `TEST(BufferPoolManagerTest, EvictsUnpinnedPage)`. Run both `ctest --test-dir build --output-on-failure` and `./build/bin/unit_test` before submitting parser, storage, execution, transaction, or recovery changes. Clean up temporary database directories and output files.

## Commit & Pull Request Guidelines

Recent history uses short messages, often with `feat:` or `fix:` prefixes. Commit messages must be Chinese; keep the first line specific, for example `fix: 正确回滚恢复中的脏页`. Pull requests should describe behavior changes, list tests run, link related tasks, and call out parser regeneration, fixture changes, or compatibility concerns.

## Agent-Specific Instructions

Do not modify any `CMakeLists.txt`, `build/`, vendored `deps/googletest/`, or unrelated dirty worktree files unless explicitly required. Keep changes scoped and prefer existing helper APIs. After feedback confirms a fix is successful, summarize what changed and why so later edits do not repeat the mistake.