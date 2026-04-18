# CTR-in-C

This is a game environment for [psx-modding-toolchain](https://github.com/mateusfavarin/psx-modding-toolchain/) that houses an ongoing decompilation project for the PSX game Crash Team Racing. This is a non byte matching decompilation, aiming for code quality while achieving the same functionality as the original game. This project adopts the [Ship of Theseus](https://en.wikipedia.org/wiki/Ship_of_Theseus) strategy. By taking advantage of [PCSX-Redux](https://github.com/grumpycoders/pcsx-redux/) 8MB memory expansion, we can re-write game functions and load them in memory, while the original game stays intact. Then, we can force the game to call each function we re-wrote, and compare the output of the decompiled function with the original game function.

# Contributing

Are you interested in contributing? Have any experience in C programming language? You're welcome to join!

## Requirements

This SDK requires the installation of [mateusfavarin](https://github.com/mateusfavarin)'s [psx-modding-toolchain](https://github.com/mateusfavarin/psx-modding-toolchain). You can check the repo's readme for instructions on the installation and additional documentation on its usage. All python and pip steps are mandatory.

After setting up psx-modding-toolchain, clone this repository
into the local directory psx-modding-toolchain/games:

```
$ git clone https://github.com/CTR-tools/CTR-in-C.git
```

## `rewrite` project

The `rewrite` folder houses the primary effort at decompiling CTR, with higher code standards and more rigorous testing than the original `decompile` project (mentioned below). It targets only the NTSC-U build (9/26/99 build date) — there are no considerations for other regional builds.

Like the `decompile`, the `rewrite` adopts the [Ship of Theseus](https://en.wikipedia.org/wiki/Ship_of_Theseus) strategy: using 8MB memory expansion (supported by many PS1 emulators), rewritten functions are loaded alongside the original game, allowing direct comparison between the original and rewritten implementations. The focus is on matching both behavior and API.

### Testing

Pure functions (no global side effects) must have unit tests that call both the original ASM and the rewritten C at runtime and compare outputs. Impure functions should be verified through rigorous testing, integration tests, and/or matching ASM (from the `matching` project) with only trivial restructuring. See the [src/rewrite/README.md](src/rewrite/README.md) for details on the test framework.

### How to contribute

- Make sure you have cloned this repo and installed the requirements (including psx-modding-toolchain).
- The rewrite source lives in [src/rewrite/src/exe/](src/rewrite/src/exe/). Pick a function to rewrite — you can use the [ghidra reference](reference/ghidra) and the `matching` project as guides.
- Write a corresponding test in [src/rewrite/src/tests/](src/rewrite/src/tests/). See the [rewrite README](src/rewrite/README.md) for the testing conventions.
- Note: the `rewrite` uses gcc 15.2. If you've installed deps via psx-modding-toolchain, run `mips` or `mips use <version>` to switch.
- Run `build.bat` and choose "Compile", "Build ISO", then test in PCSX-Redux.
- Once everything is working, feel free to open a pull request!

If you have any questions, reach us out in our [Discord server](https://discord.gg/WHkuh2n).

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

# Modding CTR

If you are looking to use mods for CTR, please take a look at [this repo](https://github.com/CTR-tools/CTR-ModSDK) instead.