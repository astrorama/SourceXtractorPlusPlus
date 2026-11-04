# Repository instructions

## Building and testing

This project is CMake-based but uses the Elements build system. Never run CMake
commands directly; use the project's Makefile through Elements instead.

Run the following commands from the repository root as needed:

- `make` — build the project.
- `make install` — install the build products.
- `make tests` — build and run the tests.

Always run `make install` before integration runs such as executing `sourcextractor++`
in `tests/sim12`, and use the installed executable and environment.

## Coding guidelines

- Follow `.editorconfig`: UTF-8, LF line endings, spaces, two-space indentation,
  a maximum line length of 120 characters, no trailing whitespace, and a final
  newline. Python uses four-space indentation; Makefiles use tabs.
- Match the surrounding code when conventions vary. Keep changes focused and
  avoid reformatting unrelated code.
- In C++, use `PascalCase` for classes, `camelCase` for methods and functions,
  `snake_case` for local variables and parameters, and `m_snake_case` for data
  members. Match existing public API names.
- Put opening braces on the same line as class, function, and control-flow
  declarations. Use spaces after control-flow keywords (`if (...)`, `for (...)`).
  Keep namespace contents and class access labels unindented, as in nearby code.
- Always use braces `{}` around C++ `if`, `else if`, `else`, and `for` bodies,
  even for a single statement. Do not use unbraced conditionals or `for` loops,
  even when nearby code does so.
- Follow the module layout: public headers in `<Module>/<Module>/`, implementation
  files in `<Module>/src/lib/`, and tests in `<Module>/tests/src/`. Use `.h` and
  `.cpp` files named after the class where applicable.
- Use header guards and module-qualified project includes such as
  `"SEFramework/Task/SourceTask.h"`. Follow nearby include grouping and guard names.
  Keep code in its existing namespace (`SourceXtractor`, `ModelFitting`, etc.).
- Use constructor initializer lists for members, `explicit` for single-argument
  constructors where implicit conversion is not intended, and `override` for
  overridden virtual methods. Mark read-only member functions `const` and use
  const references for read-only access to nontrivial objects.
- Follow existing ownership patterns with `std::unique_ptr` and `std::shared_ptr`;
  use RAII for resource management. Use `auto` where the type is clear from context.
- Preserve numerical types and precision choices in the surrounding code,
  including the aliases in `SEUtils/Types.h` where used.
- Preserve existing license headers. Use Doxygen comments for public C++ API
  documentation and explain non-obvious algorithms, assumptions, and units.
- Follow the existing Boost.Test style for C++ tests: `*_test.cpp` files,
  `BOOST_AUTO_TEST_SUITE`, and test cases or fixtures matching nearby tests.
- In Python, use `snake_case` functions and variables, `PascalCase` classes, and
  docstrings consistent with the surrounding module.
