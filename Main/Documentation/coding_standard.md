# Coding Standard

[TOC]

The development of the core of Photon complies to a collection of rules.

## Why

It is benefitial to have a common coding style to follow with, the reasons are:

* code is written by one person, but viewed multiple times by others
* 80% of the lifetime cost of a piece of software goes to maintenance (from UE4)
* sometimes higher level concepts can be implied by the layouts

This guideline is for coding in C++ since most of the code for engine is written in it; nonetheless, some concepts can be applied to other parts of Photon (and is encouraged). You do not need to follow all these rules strictly as long as you have a good reason that doing the other way is better for the context.

## Language and Naming

* Follow C++20 standard, try to use standard features only.
* Class names are in capitalized camel case, e.g., `SomeClassName`.
* Namespaces and function names are in lower case and underscore separated, e,g., `some_name`.
* Member variables should have `m_` prefix.
* Local variables and methods are camel case and uncapitalized, e.g., `worldToLocal`.
* Try not to use abbreviations, except for
  * `num` for number
  * `pos` for position
  * `dir` for direction
  * common abbreviations in graphics
  * common abbreviations in mathematics
  * formulae in referenced papers
* Classes that defines an interfaces should have an `I` prefix, e.g., `IFileSystem`.
* Name a variable in plural if it is a collection.
* Boolean variables and methods should ask a question, e.g., `m_isRunning`, `hasMaterial()`.
* Use `#pragma once`. Do **not** use macro-based header guards.
* Use `PH_ASSERT` for runtime assertions. See [`Engine/Source/Common/assertion.h`](https://github.com/TzuChieh/Photon-v2/blob/master/Engine/Source/Common/assertion.h).

## Filename Extensions and Includes

* `.h` for header files
* `.cpp` for source code
* `.ipp` for template implementations
* Include directives should have the following order (with each category separated by a blank line):
  1. Engine Headers
  2. Library Headers
  3. Third-party Library Headers
  4. Standard Headers
* Include engine headers with quotes (`""`) and absolute path, relative to `Engine/Source/`.
* Include non-engine headers with angle brackets (`<>`).
* Favor forward declaration over inclusion.
  * This can help to reduce compile time over time.
  * Helps to break cyclic dependency.

## Formatting

* Curly braces should have their own line.
* Operators surrounded by spaces.
* No spaces around brackets and parentheses.
* Try not to declare multiple variables in single line.
* Indent with tabs and align with spaces.
* Do not write comments that state obvious things.
* Do not indent namespace body.

## C++ Syntax

* Add `const` to anything that is intended to be a constant.
* Add `const` to methods if it will not alter the state of the object.
* Add `override` when overriding virtual methods, do not repeat `virtual` in this case.
* Add a virtual destructor if the object is intended to be used polymorphically.
* Use `static_assert` if you have assumptions that can be verified in compile time.
* Use `nullptr`, do **not** use `NULL`.
* Use `typename` for template type parameters, do **not** use `class`.
* Use `constexpr` for constants where you can.
* `auto` should not be used, except for
  * range based for loops (only when the iterated target is verbose)
  * variable type is obvious, such as constructing smart pointers via `std::make_shared<T>`
  * TMP where this is necessary, e.g., `decltype(auto)`
* Perform explicit captures for lambda expressions.
* Pass by `const` reference if the parameter is not intended to be modified.
* Pass by non-`const` reference and a `out_` prefix for partially modified parameters.
* Pass by non-`const` pointer and a `out_` prefix for completely modified parameters.
* Strongly-typed enum should always be used, and with a `E` prefix, e.g., `enum class EUnit`.
* Use of anonymous namespaces is encouraged for implementation specific helpers.
* Do not use `final` everywhere; use it only when the target is **designed not to be** inherited.
* Always use a pair of braces for statements (`if`, `for`, `while`, etc.) that contains only a single line.
* Do not put `using` declarations in global scope.
* Pointers and references:
  * Declare like this `ICommand* command`
  * **Not** like this `ICommand *command`
* If needed, define functions/methods as `inline`, not when declaring them.
* Do not use `new` and `delete` directly.
* Make constructor `explicit` if it has single input parameter.

## Primitive Type Aliasing

Photon uses aliased types most of the time for cross platform compatibilities and ease of tuning. They are declared in [`Engine/Source/Common/primitive_type.h`](https://github.com/TzuChieh/Photon-v2/blob/master/Engine/Source/Common/primitive_type.h).

* Use `real` for reals, and a `_r` suffix for real literals.
* Use `integer` for integers.
* If you need specific precision and Photon has it, use it; otherwise use standard types.

## Others

* All engine code should be under `ph` namespace, preferably with an ending comment:

```cpp
namespace ph
{
  // some code
}// end namespace ph
```

* Any function/method with empty body should be formatted like this:

```cpp
void someMeaningfulName()
{}
```

* Uninitialized variable/object should have its initializing routine follows immediately, without any blank lines:

```cpp
TimeStamp someTime;// uninitialized
m_timeMachine.getTime(&someTime);// <someTime> initialized
```