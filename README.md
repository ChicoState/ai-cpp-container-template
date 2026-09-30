# cpp-container-template

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

## Rock–Paper–Scissors

Run the complete build and test suite from the repository root:

```bash
./test_runner.sh
```

Then play interactively:

```bash
./build/app
```

Enter `r`, `p`, or `s` to choose a move. Inputs are case-insensitive and may
include surrounding whitespace. The computer and player moves appear as simple
ASCII cards side-by-side; narrow terminals may wrap the output. The first side
to two decisive wins takes the match, while ties replay the round without
changing the score. At the end of a match, enter `y` to play again or any other
input to quit.

To run the suite in the container:

```bash
docker run --rm -v "$(pwd)":/usr/src -w /usr/src cpp-container ./test_runner.sh
```
