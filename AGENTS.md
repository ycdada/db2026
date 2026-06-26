# Repository Guidelines

## Project Structure & Module Organization
`src/` contains the database engine, split by subsystem: `storage/`, `record/`, `index/`, `system/`, `transaction/`, `recovery/`, `execution/`, `optimizer/`, `analyze/`, `parser/`, and shared headers in `common/`. The main server entry point is `src/rmdb.cpp`; the client stub lives in `rmdb_client/`. Unit and parser tests are in `src/unit_test.cpp` and `src/parser/test_parser.cpp`. Third-party code is vendored under `deps/` (notably GoogleTest). Build artifacts should stay in `build/`; temporary test data may appear under `BufferPoolManagerTest_db/`.

## Build, Test, and Development Commands
Configure once with `cmake -S . -B build` and rebuild with `cmake --build build -j`. Run the server with `./build/bin/rmdb`. Run the GoogleTest binary directly with `./build/bin/unit_test`. Run parser coverage with `cd build && ctest --output-on-failure`; today, CTest registers `test_parser`. If Flex/Bison grammar changes are required, edit `src/parser/lex.l` and `src/parser/yacc.y`, then rebuild so generated files are refreshed.

## Coding Style & Naming Conventions
Use C++17 and keep changes consistent with the existing codebase: 4-space indentation, braces on the same line, and headers separated from implementation in `.h` and `.cpp` pairs. Types and classes use `PascalCase` (`TransactionManager`), functions use `snake_case` or existing project conventions, and constants/macros are uppercase (`BUFFER_POOL_SIZE`). Prefer small, localized changes over large rewrites, and avoid manually editing generated parser files such as `yacc.tab.cpp` or `lex.yy.cpp`.

## Testing Guidelines
Add or extend tests when touching storage, concurrency, parser, or recovery logic. Follow GoogleTest naming like `TEST(LRUReplacerTest, SampleTest)`. Keep test data isolated to repository-local directories and clean up files when practical. Before opening a PR, run `./build/bin/unit_test` and `cd build && ctest --output-on-failure`.

## Commit & Pull Request Guidelines
Recent history uses short, task-focused subjects, often tied to contest items (for example `5.2`, `第三题提交`, `第四题修复1`). Keep commit titles brief, imperative, and scoped to one change. Pull requests should include: the problem addressed, the affected subsystem (`transaction`, `parser`, `storage`, etc.), how to reproduce or validate, and any required screenshots or logs for user-visible SQL behavior.

## Configuration & Generated Files
Develop on Linux with GCC, CMake 3.16+, Flex, Bison, and `readline`, matching the repository README. Do not commit `build/` outputs or local database artifacts unless the task explicitly requires fixture updates.

