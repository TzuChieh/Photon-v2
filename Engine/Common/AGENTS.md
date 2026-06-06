# Common Guide

## String Utilities
- `string_utils::next_token()` trims returned tokens. Whitespace tokenization skips leading separators; explicit non-whitespace separators preserve empty middle fields and ignore trailing empty fields.
- `CommandLineArguments::retrieveCommaSeparatedStrings()` follows `next_token()` field semantics: empty input yields no entries, `a,,c` keeps the empty middle slot, and trailing comma/whitespace-only fields are ignored.
