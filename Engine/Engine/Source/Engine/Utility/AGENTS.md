# Engine Utility Guide

## Fixed-Capacity Containers
- For containers that track active size/index and overwrite slots before reads, keep backing storage value-initialization debug-only; optimized builds should avoid clearing unused fixed storage unless correctness requires it.

## Callable Introspection
- Use `std::invoke_result_t` or invocable concepts when a template depends on a concrete call
  expression; reserve `TCallableTraits` for fixed, unambiguous signatures whose functors have a
  unique non-template call operator.
