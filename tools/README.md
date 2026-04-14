## What is this folder for?

* budget_calc - the old bytebudget calculator for the decompile project. This tool was used to ensure that newly written functions, when compiled down to machine code, could be placed "on top" of the old location. This way, calls wouldn't need to be patched. This tool was buggy and didn't catch all locations.
* byte_budget - the newer bytebudget calculator for the decompile project. This tool uses special naming conventions in the decompile project to create a more accurate mapping of function -> location in EXE.
  - The purpose of these bytebudget tools was to ensure that newly written functions, when placed in their original location in the EXE, didn't overwrite the "beginning part" of the function that follows it.