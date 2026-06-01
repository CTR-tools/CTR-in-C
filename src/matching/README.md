# Matching project

The goal of this project is to find C source that compiles, byte-for-byte, to
the original NTSC-U (9/26/99 build, id `926`) game executable. See
[../README.md](../README.md) for how `matching` fits next to `decompile` and
`rewrite`.

This README covers **how to set up and run the matching scorer** locally — i.e.
how to take a `.c` file under `General/*/` and compare its compiled output
against the original `.o`.

---

## Layout

```
src/matching/
├── README.md                ← you are here
├── General/                       ← TRACKED deliverables (cleaned matching C)
│   ├── COLL/
│   │   ├── COLL.h           ← shared typedefs + struct shapes
│   │   ├── COLL_*.c         ← one function per file
│   │   └── ...
│   ├── INSTANCE/
│   ├── UI/
│   └── Vehicle/
├── staging/                       ← GITIGNORED scratch (nothing here is committed)
│   ├── .gitignore           ← ignores everything but itself + README.md
│   ├── README.md
│   ├── SCUS-94426.s         ← user-placed copyrighted disassembly (gitignored)
│   └── <FuncName>/          ← per-function permuter work dirs + output-*/
└── tools/                         ← the toolchain
    ├── permuter/                  ← submodule: TheUbMunster/decomp-permuter@windows-support
    ├── maspsx/                    ← submodule: mkst/maspsx
    ├── windows-gcc-psx/           ← submodule: TheUbMunster/windows-gcc-psx
    ├── patch_gprel.py             ← GP-relative relocation fixer
    ├── compile.sh.template        ← Git Bash compile script template
    ├── compile.ps1.template       ← PowerShell compile script template
    └── gcc/                       ← user-placed Linux gcc tree (gitignored)
```

`tools/permuter`, `tools/maspsx` and `tools/windows-gcc-psx` come down via git
submodules. The contributor supplies two external artifacts by hand:
`tools/gcc/` (Linux gcc only) and **`staging/SCUS-94426.s`** (the copyrighted
NTSC-U disassembly). All work-in-progress — `SCUS-94426.s`, extracted
`target.o`/`target.s`, and per-function permuter dirs — stays under `staging/`,
which is gitignored wholesale so none of it can be committed. See
[`staging/README.md`](staging/README.md) and the setup steps below.

---

## Prerequisites

- **OS:** Windows-native is the primary, verified path — the `windows-gcc-psx`
  submodule ships `gcc.exe`/`cc1.exe`/`cpp.exe`, so no WSL is required. Run from
  Git Bash (`compile.sh`) or PowerShell (`compile.ps1`). Linux/WSL also works
  (use a Linux gcc tree at `tools/gcc/`).
- **Python 3.8+** with `pip`
- **Binutils for MIPS little-endian** (any modern version, no pin):
  - **Windows:** install via the pcsx-redux `mips.ps1` helper; it lands
    `mipsel-none-elf-as.exe` under `%APPDATA%\mips\mips\bin\`.
  - **Linux/WSL:** `sudo apt install binutils-mipsel-linux-gnu` (provides
    `mipsel-linux-gnu-as`).

---

## One-time setup

### 1. Clone with submodules

```bash
git clone --recurse-submodules <repo-url>
# or, if you already cloned:
git submodule update --init \
    src/matching/tools/permuter \
    src/matching/tools/maspsx \
    src/matching/tools/windows-gcc-psx
```

Then enable the matching pre-commit guard once for this clone (it refuses to
commit copyrighted `.s` disassembly or anything in `staging/`):

```bash
git config core.hooksPath .githooks
```

### 2. Install permuter Python deps

```bash
cd src/matching/tools/permuter
pip install -r requirements.txt    # pycparser, attrs, toml, ...
```

### 3. Place the gcc 2.8.1 toolchain

The matching score is meaningful only when the C is compiled by the *same*
compiler Naughty Dog used — modern gcc emits different register choices and
instruction orderings. Place a working gcc 2.8.1 tree at:

```
src/matching/tools/gcc/
├── gcc        ← driver
├── cc1        ← C frontend
├── cpp        ← preprocessor
└── ...
```

The exact file set depends on which distribution you use; all four matching
test sweeps pass with each of these.

**Option A — Windows native binaries (verified, default):** provided by the
`tools/windows-gcc-psx` submodule (the [TheUbMunster/windows-gcc-psx](https://github.com/TheUbMunster/windows-gcc-psx)
fork), which ships `gcc.exe`, `cc1.exe`, `cpp.exe`, `cccp.exe`. **Nothing to
place by hand on Windows** — the compile templates resolve
`tools/windows-gcc-psx/gcc-2.8.1-psx/gcc.exe` directly, so the `tools/gcc/`
step is Linux-only. Works on Windows natively and on WSL via binfmt interop.

**Option B — Linux build:** the gcc-2.8.1 binary distributed with
[psx-modding-toolchain](https://github.com/mateusfavarin/psx-modding-toolchain),
or extracted from the `decompals/old-gcc` Docker image. Copy/symlink that
tree into `src/matching/tools/gcc/`.

This directory is `.gitignore`d.

### 4. Place the NTSC-U disassembly

The scorer extracts the original game's `target.o` for any function on demand,
by reading the full disassembly. Place the file at the root of the staging dir:

```
src/matching/staging/SCUS-94426.s
```

Common sources:
- The output of `splat` against the NTSC-U executable (build id 926)
- The copy bundled in your `psx-modding-toolchain` setup

**This is copyrighted game code — never commit it.** It lives in `staging/`,
which is gitignored wholesale; the root `.gitignore` also blocks
`src/matching/**/*.s`, and the `.githooks/pre-commit` hook rejects it if it is
ever force-staged. Do all permuter work in `staging/<FuncName>/` and move only
the cleaned `General/<Subdir>/<Name>.c` out to commit.

---

## Scoring a function

Two compile script templates are provided under `tools/`:

- [`tools/compile.sh.template`](tools/compile.sh.template) — Git Bash, works
  on Windows (no WSL needed) and on Linux.
- [`tools/compile.ps1.template`](tools/compile.ps1.template) — PowerShell,
  pure Windows.

Both drive the same pipeline (windows-gcc-psx → maspsx → mipsel-none-elf-as)
and both honour an optional `GP_VALUE` env var that, when set, runs
`patch_gprel.py` on the produced `.o` (required for the handful of functions
that touch GP-relative globals — currently `VehPhysCrash_AI` and
`VehPhysCrash_Attack`, both `gp=0x8008CF6C`).

### Setup a per-function test directory

1. Make a dir under `staging/`, e.g. `staging/VehPhysProc_SpinStop_Update/`.
   (Everything in `staging/` is gitignored — see [staging/README.md](staging/README.md).)
2. Drop in `base.c` (the matching source), the relevant `<Subdir>.h` next to
   it, and `target.o` (produced separately from `SCUS-94426.s` — see [Manual
   flow](#manual-flow-works-today) below).
3. Write a one-line `settings.toml`:
   ```toml
   func_name = "VehPhysProc_SpinStop_Update"
   compiler_type = "gcc"
   ```
4. Copy one of the templates in as `compile.sh` or `compile.ps1`. They walk up
   to find `tools/`, so they run unchanged from inside `staging/`.

Then run the scorer (Windows examples):

```
# from Git Bash (compile.sh)
python src/matching/tools/permuter/permuter.py src/matching/staging/<dir-name> --debug

# from PowerShell (compile.ps1)
python src\matching\tools\permuter\permuter.py src\matching\staging\<dir-name> --debug
```

A `base score = 0` line means a byte-perfect match. The scripts themselves are
self-contained — running one of them manually (`./compile.sh base.c -o
candidate.o`) gives you a `.o` you can objdump-compare without going through
the permuter at all.

### Manual flow (works today)

From this directory (Git Bash, PowerShell, or WSL):

```bash
# extract target.o for one address from staging/SCUS-94426.s into a work dir
bash tools/permuter/path-to/extract_func.sh <ADDRESS> staging/<NAME>

# drop the cleaned source + per-subdir header alongside it
cp General/<SUBDIR>/<NAME>.c staging/<NAME>/base.c
cp General/<SUBDIR>/<SUBDIR>.h staging/<NAME>/

# write a compile.sh that gives gcc -I for the header dir,
# defines USE_REAL_GTE=1, and pipes through maspsx; then:
python3 tools/permuter/permuter.py staging/<NAME> --debug
```

The last line prints `base score = N`. **0 = byte-perfect match.** Higher
scores indicate progressive divergence (register diffs, instruction inserts/
deletes, etc.).

### Things to watch for

- **`USE_REAL_GTE=1`** must reach the preprocessor (not just gcc) — the
  permuter pre-processes `base.c` through `cpp` before invoking `compile.sh`,
  so flags passed to gcc come too late for `#if` conditionals. `Vehicle.h`
  works around this by `#define`ing `USE_REAL_GTE 1` itself when undefined.
- **`patch_gprel.py`** — a couple of functions (currently
  `VehPhysCrash_AI` / `VehPhysCrash_Attack`, both at `gp=0x8008CF6C`) need a
  post-assembly GP-relative relocation fix. Without it the score is non-zero
  due to `lo(D_XXX)(gp)` mismatches.

### Windows-specific gotchas when using Option A (Windows gcc)

If you're driving `gcc.exe` from a WSL bash compile script:

- **No UNC paths.** `gcc.exe` can't open files at `\\wsl.localhost\...`.
  Stage `input.c` (and any neighbour `.h`) into a directory under
  `C:\Users\<you>\AppData\Local\Temp\` (i.e. `/mnt/c/Users/<you>/AppData/Local/Temp/`
  from WSL) before invoking gcc, and pass `-I <that-temp-dir>` so includes
  resolve there too.
- **`cd` to a Windows filesystem cwd before launching `gcc.exe`.** Otherwise
  WSL binfmt interop occasionally fails with `Invalid argument` when the
  parent's cwd is in the WSL filesystem and the subprocess inherits it. A
  `cd "$TMPDIR"` where `$TMPDIR` is on `C:` is enough.
- **Strip CR from `.s` output.** Windows gcc writes CRLF line endings; pipe
  through `sed -i 's/\r$//'` before handing the file to `maspsx`.

See the workflow notes in
[scripts/check_scores.sh](../tools/permuter/scripts/check_scores.sh) and the
`anthropic-skills:decomp-matching-workflow` skill for the full iterative
permuter loop.

---

## Adding a new function to match

1. Find the function's address (e.g. via Ghidra or the gcc-syms file in
   `../decompile/symbols/`).
2. Create `General/<SUBDIR>/<NAME>.c` with the cleaned C source. Use
   `#include "<SUBDIR>.h"` for shared typedefs.
3. If the function reads or writes struct fields the header doesn't expose
   yet, add them to `<SUBDIR>.h` — see the existing headers for layout style
   (offsets in comments, partial structs with `char padXX[..]` filler).
4. Score it (see above). Iterate.

The headers exist for *organisation*, not isolation — each `.c` is still
compiled standalone by the permuter, so the header must be self-contained
(no nested includes).

---

## Why the matching project exists

`matching` is a reference, not a build target. Mods should pull from
`decompile/` or `rewrite/`. The point here is to *prove* a given C source
faithfully reproduces ND's original `.o`, so the `rewrite` project can lift
matched logic with confidence that no behavior was lost in translation.

If you're contributing, focus on getting a function to score 0 and don't worry
about whether the resulting C is "pretty" — readability cleanup is a Phase 5
pass once the match is locked in.
