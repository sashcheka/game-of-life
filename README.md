# Game of Life

A C++20 console implementation of Conway's Game of Life, built incrementally.

The current version stores a rectangular world, counts living neighbors, and
prints a built-in example. Evolution rules and JSON input/output will be added
in subsequent steps.

## Requirements

- CMake 3.20 or newer.
- A C++20 compiler (Apple Clang, Clang, GCC, or MSVC).
- The compiler's build tools, such as Make, Ninja, or MSBuild.

The project has no third-party dependencies and needs no downloads during build.

## Build and run

From the repository root on macOS or Linux:

```sh
cmake -S . -B build
cmake --build build
./build/game_of_life
```

On Windows with a Visual Studio generator, use PowerShell:

```powershell
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\game_of_life.exe
```

Expected output:

```text
Game of Life: 5 x 5
.....
..*..
..*..
..*..
.....
Living neighbors at (2, 2): 2
```

Neighbor counting includes horizontal, vertical, and diagonal neighbors, excluding
the cell itself. Cells outside the field are treated as dead; edges do not wrap.
Queries must refer to a cell inside the field; invalid coordinates throw
`std::out_of_range`.

## Tests

After building, run `ctest --test-dir build --output-on-failure` (add `-C Debug`
for a Visual Studio generator). Tests check dimensions, cell access, and rejection
of invalid world data. A world with no living cells is valid; zero dimensions are not.
Neighbor checks cover corners, edges, rectangular and one-dimensional fields,
single-cell worlds, and fields with no living cells.

## Project structure

- `CMakeLists.txt`: executable target, language standard, and compiler warnings.
- `src/main.cpp`: program entry point and temporary example output.
- `src/world.hpp`, `src/world.cpp`: world storage and checked `(x, y)` cell access.
- `tests/world_tests.cpp`: world construction and access checks, run through CTest.
- `build/`: generated build files, excluded from Git.
