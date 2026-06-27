# Record Manager Regression Notes

## Shared record cache lifecycle

When record files use shared in-memory state, the cache must be tied to the table file lifecycle, not only to the filename.

Regression scenario:
- Open a record file and populate its shared header/record cache.
- Close and destroy the file while the old `RmFileHandle` object is still alive.
- Recreate a file at the same path, possibly with a different record size.
- Open the recreated file.

Risk:
- If the filename still maps to the old shared state, the new handle can reuse stale `RmFileHdr` metadata and cached records.
- This can make a recreated empty file appear to contain old rows, or use the old `record_size` and page metadata.

Guardrail:
- Closing, destroying, or recreating a table file at the same path must invalidate the global filename-to-cache mapping so future opens cannot inherit stale shared headers or cached records.
- Do not clear the shared cache object itself while another live `RmFileHandle` may still own it; closing one handle must not make another already-open handle lose cache-backed reads or scans.
- `DiskManager::open_file` returns the same fd for repeated opens of a path, so record-file close logic must track active handles per shared file. Only the last active handle may close the shared fd or detach the filename-to-cache mapping.
- A closed `RmFileHandle` must reject public operations even when another handle keeps the shared fd/cache alive.
- Do not fix this class of bug by removing the shared-cache optimization unless explicitly requested; keep the optimization and repair the lifecycle boundary.
- Keep a regression test that closes, destroys, recreates, and reopens the same filename before destroying the old handle object.

Current regression test:
- `RecordManagerTest.ClosingOneHandleKeepsOtherHandleCache` in `src/unit_test.cpp`.
- `RecordManagerTest.ReopenRecreatedFileDoesNotReuseClosedCache` in `src/unit_test.cpp`.
