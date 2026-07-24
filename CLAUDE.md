# Crash Team Racing (PS1) Decompilation Agent

## Mission

Use the configured Ghidra MCP connection to decompile Crash Team Racing (CTR). Recover accurate names, types, structures, globals, functions, variables, and behavior. Perform the decomplation inside ghidra, not in the project directory.

## Reference Source

The working directory (`.`) contains the CTR decompilation project. Use it as a **reference** for structures, symbols, architecture, patterns, naming, and behavior — but **never as a source of truth**. Always cross-check and validate any usage against the CTR binary in Ghidra. The decomp may differ from the binary you are analyzing in offsets, globals, structures, names, control flow, and implementation, and it may itself be incomplete or wrong.

### IWYU PSYQ SDK Headers

IWYU (include-what-you-use) PSYQ PSX SDK headers are available at `.\reference\psyq-include\include` (relative to this project root). These are the **IWYU** PSYQ headers, not the official PSY-Q SDK headers — a trimmed include-what-you-use set. They are, however, **sufficient for use as a reference**: use them to verify SDK types, constants, structs, and function prototypes when decompiling PSX library/API interactions (e.g. `libgpu`, `libgte`, `libgs`, `libcd`). Treat the headers as SDK reference material, not proof of CTR-specific game behavior.

### Ghidra Project State

The Ghidra project is an old work-in-progress. It already contains some symbols and structures, but they may be **incomplete, partial, or inaccurate**. Treat any existing names, types, and structures as starting hints that must be double-checked against actual usage before you rely on them.

## Verification Rules

Do not guess. Before committing a name, type, or conclusion:

* Verify it in the CTR decompiled code.
* Check cross-references, callers, callees, and nearby functions.
* Confirm structure offsets from actual usage.
* Confirm globals through reads, writes, and references.
* Validate behavior from CTR context, not only from reference-source similarity.
* Re-validate any pre-existing Ghidra symbols/structures rather than trusting them outright.

When unclear, gather more evidence from related code paths, strings, data, vtables, type usage, sibling functions, and call sites.

## Naming Guidelines

Prefer accurate names over confident guesses.

Use reference-source (CTR decomp) names only when the CTR binary behavior and usage are verified to match. For partial understanding, use conservative temporary names that describe known facts, such as:

* `Actor_UpdatePosition`
* `MaybePlayerState`

Avoid specific names for unverified behavior.

## Workflow

1. Start from known entry points, strings, imports, globals, and high-reference functions.
2. Compare with the CTR decomp reference where useful.
3. Verify every match in the CTR binary.
4. Rename functions, globals, locals, fields, and types only with evidence.
5. Build structures incrementally from observed offset usage.
6. Before creating a new structure, check existing structures (including the pre-existing WIP Ghidra structures) for the same intent, size, fields, and usage. Reuse or refine the existing structure when it matches and is verified; create a new one only when verified behavior requires a distinct layout.
7. Revisit names when new evidence contradicts them.
8. Leave concise notes or conservative names when evidence is incomplete.

## Function Decompilation

A function is `Decompiled` only when its behavior is understood and all of these are true:

* Every global it uses is named and well typed.
* Every function it calls is named and properly typed.
* Every temporary/local variable is named and typed.
* A top function comment documents the function.
* Relevant parameters, fields, structures, and memory layout are accurately named and typed.

When a function reaches this bar:

* Update its Status to `Decompiled` in `FUNCTIONS.MD`.
* Update `FUNCTIONS.MD` with the current function name.

Do not mark functions as decompiled if unknowns remain. Instead, add a concise Status note describing unresolved globals, fields, types, control flow, behavior, or undocumented temporary state.

Ghidra may reuse one local variable across multiple lifetimes, especially in large loader functions with moving data cursors. Reused locals still block `Decompiled` status until each temporary/local lifetime is understood well enough to be named and typed, or the blocker is documented in `FUNCTIONS.MD`.

When a flag, mode, status, state, type, command, or other finite-value field/global/local/parameter is understood, replace the primitive integer type in Ghidra with a fixed-size enum matching the original storage width. Use bitmask enums for flags and value enums for mutually exclusive states. Add entries only for verified bits/values, prefer reference-source names only when CTR usage matches, and otherwise use conservative CTR-specific names. Avoid leaving hardcoded magic numbers in decompiled functions when an enum can accurately express the meaning. If an enum cannot yet be applied because a struct field or global cannot be edited safely, document that blocker in `FUNCTIONS.MD`.

Linter audit feedback can be ignored. Treat it as advisory only; do not block `Decompiled` status on audit-prefix, interior-alias, or other linter-only complaints when the decompiler output is readable and the underlying behavior/types are understood.

Do not name constants that Ghidra mistakenly treats as symbols or addresses, such as `0x0`, `0x1000`, or `0x8000`, unless the name gives a specific verified insight in the current code path. Prefer leaving generic immediates as literals over creating misleading global symbols.

## Quality Bar

Accuracy matters more than speed. Do not commit speculative names. If evidence is insufficient, inspect references and nearby code, or keep a cautious temporary name until verified.
