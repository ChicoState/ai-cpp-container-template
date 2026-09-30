# Spinning Donut

A dependency-free C++17 terminal animation that renders a shaded ASCII torus.

## Getting Started

This repository is compatible with [cpp-container](https://github.com/ChicoState/cpp-container). If it is not already built on your machine, clone and build it.

Run the container:

```bash
docker run -v "$(pwd)":/usr/src -it cpp-container
```

Or open a shell in the container:

```bash
docker run -v "$(pwd)":/usr/src -it cpp-container sh
```

## Run

Compile and start the animation from an ANSI-capable terminal:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp donut.cpp -o app
./app
```

Press Ctrl-C to stop the animation.

For a bounded, non-interactive run, use `--frames`. `--delay-ms 0` skips the delay between frames and is useful for automation:

```bash
./app --frames 2 --delay-ms 0
```

Available options:

```text
--frames N    Render exactly N frames; N must be non-negative.
--delay-ms N  Wait N milliseconds between frames; N must be non-negative.
--help        Show command usage.
```

## Test

Run the unit and terminal end-to-end tests:

```bash
bash test_runner.sh
```

The test runner creates and removes its build directory automatically, and never starts the unbounded animation.

## Structure

- `.agents` — AI agent configurations and skills.
- `donut.cpp`, `donut.hpp` — ASCII torus renderer.
- `main.cpp` — command-line animation program.
- `specs` — feature plan and specification documentation.
- `tests` — renderer unit tests and terminal end-to-end test.
