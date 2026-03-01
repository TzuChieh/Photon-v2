# Code Review Checklist

Use this checklist during Step 1 of the `commit-expert` workflow.

## Style & Naming
- [ ] Member variables have `m_` prefix.
- [ ] Class names are `CapitalizedCamelCase`.
- [ ] Function names are `snake_case`.
- [ ] All `ph` namespace blocks have an ending comment: `}// end namespace ph`.

## Correctness & Safety
- [ ] `const` is applied to all non-modified parameters, local variables, and methods.
- [ ] No raw `new` or `delete` (use smart pointers or `std::make_shared`).
- [ ] `nullptr` is used instead of `NULL`.
- [ ] `override` is present for all overridden virtual methods (and `virtual` is removed).
- [ ] `explicit` is used for single-parameter constructors.

## Formatting
- [ ] Curly braces have their own line.
- [ ] Operators are surrounded by spaces.
- [ ] Tab indentation with space alignment.
- [ ] Braces are used for all `if`, `for`, `while` statements, even for single lines.

## Minimalism
- [ ] No "just-in-case" alternatives or redundant logic.
- [ ] No comments stating the obvious.
- [ ] No `auto` except for verbose range-based loops or smart pointer creation.
