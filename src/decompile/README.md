## `decompile` project

### How to rewrite functions:

- Make sure you have cloned this repo and installed the requirements.
- Choose a function from a *.c file from the [ghidra](reference/ghidra) folder. Each file represents a code section or category. All non-numbered *.c files are parts of the main EXE while the numbered files are overlays.
- Choose a function in that section to rewrite. Read the documentation comments on what the function does and what's the address.
- Set up a compile folder for your rewritten function using psx-modding-toolchain (if it doesn't already exist). You can see ones that already exist in [src/decompile/General](src/decompile/General).

\* Build codenames:  
common: All versions;  
926: USA Retail;  
1006: Japan Trial;  
1020: Europe Retail;  
1111: Japan Retail.  

\**  Code region:  
exe: main EXE;  
221-233: Overlays (use the original .c filename number)

- Run `build.bat` and choose "Compile", "Build ISO", then test the game.
- After you've confirmed that it's functional, add your new function .c file to the [decompile directory](src/decompile).
- Feel free to make a pull request after everything is working!

If you have any questions, reach us out in our [Discord server](https://discord.gg/WHkuh2n).