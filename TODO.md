# TODO

---

- [ ] Have the symbols paths in `config.json` be relative to the root of the repo instead of hardcoded to `symbols/`.

  **Context:** The `psx-modding-toolchain` resolves symbol file paths by joining `DIR_GAME / "symbols"` with the filenames listed in `config.json` (`tools/mod-builder/common.py:66`, consumed in `game_options.py:42`). We moved the symbol files from a top-level `symbols/` folder into `src/*/symbols/` (e.g. `src/decompile/symbols/`, `src/rewrite/symbols/`) to co-locate them with their respective projects. The toolchain needs to be updated so the `symbols` field in `config.json` can specify paths relative to the repo root (or at least not assume a single hardcoded directory).

---

- [ ] Allow per-project include directories instead of requiring all headers in the root `include/` folder.

  **Context:** The toolchain adds a single game-level include path via `-I$(GAMEINCLUDEDIR)` (`games/common.mk:13`), where `GAMEINCLUDEDIR` is hardcoded to `DIR_GAME / "include"` (`tools/mod-builder/common.py:68`, written into the Makefile by `makefile.py:158`). Individual mods also get `-I$(MODDIR)` and `-I$(MODDIR)/src/`, but shared headers across the project must all live in the root `include/` folder. It would be useful to support additional include paths (e.g. per-project or via `config.json`) so headers can be co-located with their respective projects.

---
