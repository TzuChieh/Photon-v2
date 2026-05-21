# SDL Guide

## Parser Utilities
- Keep parser helpers small, allocation-free, and `std::string_view`-centric.
- Low-level trim/consume helpers should be reusable and syntax-local, not context-coupled.

## Error Handling
- Prefer clear fallback semantics at higher-level loaders with precise warnings.
- Keep grammar expectations explicit and deterministic.

## Testing
- Test low-level parser helpers by contract (no-op, trim success, corner cases).
- Keep high-level array/field tests focused on integration behavior, not helper internals.
