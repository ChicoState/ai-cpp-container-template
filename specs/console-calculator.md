# Spec: C++ Console Calculator

## Objective

Build a small C++ console program that performs one arithmetic calculation per
run. A user supplies a first operand, one operator, and a second operand; the
program prints the result or a clear error. The supported operations are
addition (`+`), subtraction (`-`), multiplication (`*`), and division (`/`).

Success means valid integer, negative, and decimal calculations return the
correct value, while invalid input never produces a calculation or uses
uninitialized data.

### Assumptions

1. The program is interactive and reads one calculation from standard input.
2. Operands are finite `double` values.
3. A malformed operand includes a partially numeric token such as `12abc`.
4. Invalid input and division by zero end the program with a non-zero status.
5. No history, repeat-calculation loop, advanced operators, or external
   dependencies are in scope.

## Tech Stack

- C++ compiled with `g++`.
- Standard library only; no third-party dependencies.
- Source: `main.cpp`.

## Commands

Build:

```bash
g++ *.cpp -o app
```

Run interactively:

```bash
./app
```

Build and run using the repository helper:

```bash
./test_runner.sh
```

## Project Structure

```text
main.cpp                    Interactive calculator implementation
specs/console-calculator.md Requirements and acceptance criteria
tests/                      Optional future automated test sources/scripts
test_runner.sh              Existing compile-and-run helper
```

## Code Style

Use clear, conventional C++ with narrowly scoped variables and descriptive
names. Keep this application in one source file; do not introduce abstractions
that are disproportionate to four operations. Check every input operation
before using its result.

```cpp
double firstOperand{};
if (!(std::cin >> firstOperand)) {
  std::cerr << "Invalid first operand.\n";
  return 1;
}
```

- Use `double` for operands and results.
- Use a `switch` on the operator for the four calculation paths.
- Write diagnostics to `std::cerr` and successful results to `std::cout`.
- Return `0` only after a successful calculation; return non-zero for errors.

## Input and Error Requirements

- Accept the calculation in the order: first operand, operator, second operand.
- Validate that each operand is a complete, finite numeric value; reject text,
  partial numeric tokens, and end-of-file before all fields are read.
- Accept only `+`, `-`, `*`, and `/`; reject every other operator.
- Before division, reject a second operand equal to `0.0`, including `-0.0`.
- On any error, display a concise reason, do not print a result, and exit
  non-zero.

## Testing Strategy

Initially, compile with `g++ *.cpp -o app` and exercise the program through
standard input. Automated tests may be added under `tests/` later, but no test
framework or dependency is required for this feature.

Required manual cases:

```text
8 + 2       => 10
8 - 2       => 6
8 * 2       => 16
8 / 2       => 4
5 / 2       => 2.5
-5 * 2      => -10
8 / 0       => division-by-zero error and non-zero exit
8 % 2       => unsupported-operator error and non-zero exit
12abc + 2   => invalid-number error and non-zero exit
EOF         => clean error/termination and non-zero exit
```

## Boundaries

- Always: validate all input before calculation, test every supported operator
  plus every defined failure mode, and build successfully before review.
- Ask first: add dependencies, change the compiler/tooling, change the input
  format, or add looping/history/advanced operations.
- Never: divide by zero, silently coerce malformed input, output a result after
  an error, commit generated `app` binaries, or expand the scope beyond the
  four requested operators.

## Success Criteria

- All four supported operations correctly calculate finite `double` operands.
- Decimal and negative operands are supported.
- Division by `0.0` or `-0.0` is rejected without calculating.
- Unsupported operators, invalid/partial numeric values, and incomplete input
  produce clear errors and non-zero exits.
- `g++ *.cpp -o app` completes successfully.

## Open Questions

- Should successful whole-number results be formatted as `4` or `4.0`? The
  implementation may use the standard stream representation unless a display
  convention is requested.
