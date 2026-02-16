# CTR-in-C

This is a game environment for [psx-modding-toolchain](https://github.com/mateusfavarin/psx-modding-toolchain/) that houses an ongoing decompilation project for the PSX game Crash Team Racing. This is a non byte matching decompilation, aiming for code quality while achieving the same functionality as the original game. This project adopts the [Ship of Theseus](https://en.wikipedia.org/wiki/Ship_of_Theseus) strategy. By taking advantage of [PCSX-Redux](https://github.com/grumpycoders/pcsx-redux/) 8MB memory expansion, we can re-write game functions and load them in memory, while the original game stays intact. Then, we can force the game to call each function we re-wrote, and compare the output of the decompiled function with the original game function.

# Repository Structure

## Rewrite-specific

`rewrite/src/hooks/dll/`: boot loader for the decompile.

`rewrite/src/exe/`: main executable decomp.

`rewrite/src/tests/`: tests for each decompiled function.

`rewrite/scripts/`: helpful misc scripts.

`include/ctr/`: decompile headers.

`symbols/gcc-syms-rewrite.txt`: original function addresses.

`symbols/gcc-extern-rewrite.txt`: extern global variable addresses.

## Miscellaneous

`build/`: paste your original CTR rom here in order to compile CTR builds.

`ghidra/`: commented disassembler output of the game's source code; ancient, data structures are ghidra stock.

`plugins/`: third-party programs and extensions added to the modding toolchain; see psx-modding-toolchain's documentation for more info.

# Tests
Each decompiled function must have an entry in `s_functions` table at `src/tests/test.c`. During boot, this entry is used to patch each original function so that the decompile version can be called. At the end of each decompiled function, a call to a `TEST` function should be made, which restores the state before the decompiled function was called, then patches the game to call the original function, and then compares the output of the original function with the decompiled function. For simple test functions that don't use any global variables, see `src/tests/test_math.c` and `src/exe/math.c`. For functions that change global variables, a call to a `BACKUP` function should be made to store the state upon function call. For examples, see `str/tests/test_rng.c` and `src/exe/rng.c`.

Note: the original function name must have the same name of the decompiled function + `ND_` prefix.

# Note
This portion of the project uses gcc 15.2, if you've installed your deps following the instructions in the psx-modding-toolchain repo, run the `mips` or `mips use <version>` command to change versions. (Note that the decomp folder uses gcc 13.1/2)
