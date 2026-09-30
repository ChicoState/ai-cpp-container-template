# C++ CLI Calculator

A small interactive calculator MVP that supports integer addition and
subtraction.

## Quick Start

Build and start the calculator:

```bash
g++ -std=c++17 -Wall -Wextra -Werror main.cpp -o app
./app
```

Enter one command per line:

```text
add 2 3
5
sub 9 4
5
exit
```

## Commands

| Command | Description |
| --- | --- |
| `add <integer> <integer>` | Adds two integers. |
| `sub <integer> <integer>` | Subtracts the second integer from the first. |
| `exit` | Ends the calculator. |

Invalid commands print an error and leave the calculator running so that the
next command can be entered.

## Test

Run the automated CLI behavior checks:

```bash
./test_runner.sh
```

## Getting Started

This repository is compatible with [cpp-container](https://github.com/ChicoState/cpp-container). If not already built on your machine, clone and build it.

Run the container:

```bash
docker run -v "$(pwd)":/usr/src -it cpp-container
```

Run the application interactively in a shell:

```bash
docker run -v "$(pwd)":/usr/src -it cpp-container sh
```

## Structure

* `.agents` - AI agent configurations and skills (in `/skills` subdirectory) for this project
* `.` - The root directory contains the C++ code for the application as well as necessary scripts
* `specs` - Specification documentation
* `tests` - Test code
