# Game of Life

A C++20 console implementation of Conway's Game of Life, built incrementally.

The current version reads `game_of_life_input.json` from the working directory
and prints the initial world and at most one evolution step. Full cycle execution,
output JSON, and playback timing will be added in subsequent steps.

## Requirements

- CMake 3.20 or newer.
- A C++20 compiler (Apple Clang, Clang, GCC, or MSVC).
- The compiler's build tools, such as Make, Ninja, or MSBuild.

The nlohmann/json 3.12.0 header and its MIT license are included in `third_party/`.
No dependency installation or downloads are needed during build.

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

In VS Code with CMake Tools installed, run **CMake: Configure** from the Command
Palette. Use CMake Tools as the IntelliSense configuration provider so the editor
gets the language standard and include paths from CMake.

Expected output:

```text
Game of Life: 5 x 5
FPS: 1, requested cycles: 1
Generation 0
.....
..*..
..*..
..*..
.....
Generation 1
.....
.....
.***.
.....
.....
```

Neighbor counting includes horizontal, vertical, and diagonal neighbors, excluding
the cell itself. Cells outside the field are treated as dead; edges do not wrap.
Queries must refer to a cell inside the field; invalid coordinates throw
`std::out_of_range`.

Each generation is calculated in a separate grid using only the previous state.
A living cell survives with two or three living neighbors; a dead cell becomes
alive with exactly three. All other cells in the new generation are dead.
`World::next_generation()` returns a new world and leaves the original unchanged.

## Input configuration

The repository includes `game_of_life_input.json` with the blinker shown above.
Run the executable from the directory containing this file.

- `world.width` and `world.height`: positive integers matching the grid dimensions.
- `world.cells`: an array of equally sized strings containing only `.` and `*`.
- `simulation.fps`: a positive integer, currently displayed without affecting timing.
- `simulation.cycles`: a nonnegative integer. Zero displays only the initial world;
  positive values currently display one step as a preview.

Numeric parameters must use integer JSON values and fit `std::size_t`.
Invalid JSON, missing fields, invalid values, and file errors produce an error
message on stderr and a nonzero exit code.

## Tests

After building, run `ctest --test-dir build --output-on-failure` (add `-C Debug`
for a Visual Studio generator). Tests check dimensions, cell access, and rejection
of invalid world data. A world with no living cells is valid; zero dimensions are not.
Neighbor checks cover corners, edges, rectangular and one-dimensional fields,
single-cell worlds, and fields with no living cells.
Evolution checks cover a blinker over two steps, a stable block, loneliness,
underpopulation, overpopulation, empty worlds, and boundaries. Each evolution
case also checks that the original world is unchanged.
`json_storage_tests` checks configuration parsing, numeric limits, required fields,
grid dimensions, cell values, and rejection of malformed or trailing JSON data.

## Project structure

- `CMakeLists.txt`: executable target, language standard, and compiler warnings.
- `src/main.cpp`: program entry point and temporary example output.
- `src/world.hpp`, `src/world.cpp`: world storage, neighbor counting, and evolution.
- `src/json_storage.hpp`, `src/json_storage.cpp`: configuration loading and validation.
- `tests/world_tests.cpp`: world construction, access, and evolution checks via CTest.
- `tests/json_storage_tests.cpp`: configuration parsing and validation checks.
- `third_party/`: the bundled JSON library, license, and version information.
- `build/`: generated build files, excluded from Git.
