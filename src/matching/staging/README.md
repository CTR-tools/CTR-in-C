# Matching staging area

**Nothing in this directory is committed** (see [`.gitignore`](.gitignore)). It
is the scratch space for the CTR matching workflow.

## What lives here

- **`SCUS-94426.s`** and any extracted target `.s` — this is **copyrighted game
  disassembly**. Committing it would be redistributing copyrighted material, so
  it must never leave this gitignored directory.
- **Per-function permuter work dirs**, e.g. `staging/<FuncName>/` containing
  `base.c`, `compile.sh` / `compile.ps1`, `settings.toml`, and the permuter's
  junky `output-*/` directories.

## Workflow

1. Do all permuter work in `src/matching/staging/<FuncName>/`.
2. When a function reaches its best score and is cleaned up (Phase 5), copy
   **only** the finished C file out to the tracked location:
   `src/matching/General/<Subdir>/<Name>.c`.
3. That `General/` file is what gets committed — never the `.s`, never the work
   dir.

The per-function `compile.sh` / `compile.ps1` (copied from
`src/matching/tools/compile.*.template`) locate the toolchain by walking up to
`src/matching/tools/`, so they work from here without any path edits.

## Guards (defense in depth)

- This directory's `.gitignore` (`*`, keeping only `.gitignore` + this README).
- Root `.gitignore`: `src/matching/**/*.s` and `**/*.S`.
- `.githooks/pre-commit` rejects staged `.s`/`.S` under `src/matching/` and any
  staged file inside this directory. Enable it once per clone:
  ```
  git config core.hooksPath .githooks
  ```
