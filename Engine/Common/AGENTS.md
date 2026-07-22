# Common Guide

## Config Macros
- For build-facing boolean config, use the same final `PH_*` name as the CMake option and C++ define; avoid parallel aliases or helper lists for simple pass-through definitions.
- Keep low-cost stats enabled by default, high-cost stats opt-in, debug logging tied to `PH_DEBUG`, and hardware capability detection (`PH_HARDWARE_HAS_*`) separate from derived feature-use checks (`PH_USE_*`).

## String Utilities
- Preserve `string_utils::next_token()` field semantics in
  `CommandLineArguments::retrieveCommaSeparatedStrings()`: trim tokens, skip leading whitespace
  separators, preserve empty middle fields for explicit separators, and ignore trailing empty fields.

## Timer Stats
- Timer stat declarations use a short display name plus a dotted path; the report derives hierarchy from the full dotted path, so choose paths that match the intended timing tree rather than relying on runtime parent tracking.
- Timer records accumulate elapsed steady-clock ticks in thread-local storage and merge on thread exit, `PH_FLUSH_TIMER_STATS()`, or report construction; call the flush macro before reporting from persistent worker threads.
- Report columns use `avg = total / calls` and `share = total / parent-total`; `total` is summed scoped wall time, so parallel children can exceed parent wall time.
