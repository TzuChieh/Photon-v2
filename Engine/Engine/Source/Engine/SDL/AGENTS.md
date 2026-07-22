# SDL Guide

## Parser Utilities
- Keep low-level parser helpers allocation-free, `std::string_view`-centric, and syntax-local;
  place fallback policy and precise warnings in higher-level loaders.

## SDL Fields
- For legacy aliases that reuse a field name with a different SDL type, keep lookup typed and keep default/fallback behavior deterministic; prefer separate alias storage plus owner-level precedence over sharing one member that an untouched optional alias can reset.

## Template Boundaries
- Keep low-level SDL headers dependency-light and place introspection-dependent template definitions behind `.ipp` boundaries; when constrained partial specializations expose identical member signatures, use a structure that does not require MSVC to distinguish their out-of-class definitions.

## Imports
- Write import paths as double-quoted tokens: `#import "path.p2";`. `SdlSceneFileReader::loadImported()` resolves relative paths from the root scene working directory, including nested imports.
- Isolate parser state for each recursive import, flush imported content at EOF, reject excessive
  nesting, and never let nested imports mutate the root incremental state.

## Testing
- Test low-level helpers by no-op, trim, and corner-case contracts; keep high-level array/field
  tests focused on integration behavior rather than helper internals.
- For tokenization and inline-clause regressions such as quoted PRI values, braced payloads, and specifier names, prefer `SdlInlinePacketInterface` or `sdl_parser` tests; use `SdlSceneFileReader` only when command, resource, import, or file-loading behavior is part of the contract.
