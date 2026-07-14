# CEngine Guide

## API Design
- Keep public C contracts self-contained; signatures and documentation must use exposed C API
  types and concepts, not internal C++ owners.
- Use `PhUInt32` for observable layer/statistic counts and indices; index domains are
  `[0, count)`. Do not add negative sentinels without a real caller.

## Output Contract
- If API returns `PhResult`, validate input/output defensively.
- Void query APIs assert documented required outputs; documented name output buffers remain
  optional for size-only queries. Initialize observable counts to zero and return empty names when
  unavailable or out of range.
