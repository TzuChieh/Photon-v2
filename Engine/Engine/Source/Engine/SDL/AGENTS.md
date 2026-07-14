# SDL Guide

## Parser Utilities
- Keep parser helpers small, allocation-free, and `std::string_view`-centric.
- Low-level trim/consume helpers should be reusable and syntax-local, not context-coupled.

## Error Handling
- Prefer clear fallback semantics at higher-level loaders with precise warnings.
- Keep grammar expectations explicit and deterministic.

## SDL Fields
- For legacy aliases that reuse a field name with a different SDL type, keep lookup typed and keep default/fallback behavior deterministic; prefer separate alias storage plus owner-level precedence over sharing one member that an untouched optional alias can reset.

## Template Boundaries
- Keep low-level SDL headers dependency-light and place introspection-dependent template definitions behind `.ipp` boundaries; when constrained partial specializations expose identical member signatures, use a structure that does not require MSVC to distinguish their out-of-class definitions.

## Imports
- Write import paths as double-quoted tokens: `#import "path.p2";`. `SdlSceneFileReader::loadImported()` resolves relative paths from the root scene working directory, including nested imports.
- Recursive imports use `SdlCommandParser::m_parseStateStack`: index 0 is the root incremental state; imported text pushes an isolated state. `parseImported()` flushes at imported EOF and `pushParseState()` limits nesting to 32.

## Testing
- Test low-level parser helpers by contract (no-op, trim success, corner cases).
- Keep high-level array/field tests focused on integration behavior, not helper internals.
- For tokenization and inline-clause regressions such as quoted PRI values, braced payloads, and specifier names, prefer `SdlInlinePacketInterface` or `sdl_parser` tests; use `SdlSceneFileReader` only when command, resource, import, or file-loading behavior is part of the contract.
