# Common Guide

## String Utilities
- `string_utils::next_token()` trims returned tokens. Whitespace tokenization skips leading separators; explicit non-whitespace separators preserve empty middle fields and ignore trailing empty fields.
- `CommandLineArguments::retrieveCommaSeparatedStrings()` follows `next_token()` field semantics: empty input yields no entries, `a,,c` keeps the empty middle slot, and trailing comma/whitespace-only fields are ignored.

## Timer Stats
- Timer stat declarations use a short display name plus a dotted path; the report derives hierarchy from the full dotted path, so choose paths that match the intended timing tree rather than relying on runtime parent tracking.
- Timer records accumulate elapsed steady-clock ticks in thread-local storage and merge on thread exit, `PH_FLUSH_TIMER_STATS()`, or report construction; call the flush macro before reporting from persistent worker threads.
- Report columns use `avg = total / calls` and `share = total / parent-total`; `total` is summed scoped wall time, so parallel children can exceed parent wall time.
