# Code Review Checklist

Use with the project [coding standard](../../../Documentation/coding_standard.md) and nearby code style.

## Correctness

- [ ] Behavior changes are intentional, scoped, and covered where risk warrants.
- [ ] Errors, ownership, lifetimes, and edge cases are handled explicitly.
- [ ] New test data is committed to the separate `Photon-v2-Resource` repo, not only the ignored `build/Photon-v2-Resource` setup copy. Ask the user for the source repo location when needed.
- [ ] No unrelated files or generated artifacts are included accidentally.

## C++ Style

- [ ] Naming and formatting match surrounding code; members use `m_`, classes use `PascalCase`, and namespace comments follow local style.
- [ ] Apply `const`, `constexpr`, `override`, `explicit`, `nullptr`, RAII, and strongly typed enums where appropriate.
- [ ] Avoid direct `new` or `delete`; use `auto` only in the cases allowed by the coding standard.
- [ ] Keep comments useful and logic minimal.

## Final Diff

- [ ] Inspect both staged and unstaged diffs.
- [ ] Run `git diff --check`.
- [ ] Report tests not run according to repo rules.
