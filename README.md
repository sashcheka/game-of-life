# Game of Life

A C++20 console implementation of Conway's Game of Life, built incrementally.

The current version reads `game_of_life_input.json` from the working directory
and runs up to the requested number of evolution steps. It saves the final world
to `game_of_life_output.json`. In a supported terminal, generations update in
place at the configured FPS. History navigation will be added later.

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

The example animates a blinker at 5 FPS for up to 100 generations. Run it in a
terminal, such as VS Code's integrated terminal, and press Ctrl+C to stop early.
The display uses an alternate screen and restores the original screen and cursor
when finished. Large fields show the top-left region that fits the window; the
entire field is simulated and saved. Resizing updates the visible region.

For an immediate calculation without animation or delays, use:

```sh
./build/game_of_life --batch
```

Redirected output and unsupported terminals automatically use batch mode.
On Windows, live display enables virtual terminal processing and restores the
previous console mode on exit. On macOS/Linux, it requires a terminal with `TERM`
set to a value other than `dumb`.

Expected batch output:

```text
Game of Life: 5 x 5
FPS: 5, requested cycles: 100
Generation 0
.....
..*..
..*..
..*..
.....
Press Ctrl+C to stop.
Generation 100
.....
..*..
..*..
..*..
.....
Stopped: cycle limit. Saved generation 100 to game_of_life_output.json
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
- `simulation.fps`: target updates per second in live mode; unused in batch mode.
- `simulation.cycles`: maximum number of evolution steps. Zero saves the initial world.

Numeric parameters must use integer JSON values and fit `std::size_t`.
Invalid JSON, missing fields, invalid values, and file errors produce an error
message on stderr and a nonzero exit code.

## Stopping and output

The simulation stops at the cycle limit, when no living cells remain, or after
Ctrl+C (`SIGINT`) or a termination request (`SIGTERM`). Cancellation finishes the
current generation and then saves it; no further evolution steps are started.
During frame delays, cancellation is checked at intervals of at most 10 ms
(actual response time also depends on system scheduling).
An initially empty world stops at generation zero.

The output contains only `result` with `width`, `height`, and `cells`, preserving
the assignment schema. It is created or overwritten in the working directory.
Normal completion and cancellation return exit code zero after saving; read or
write errors return a nonzero code. Forced termination that cannot be intercepted
(such as `SIGKILL` or power loss) cannot trigger saving.

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
Batch tests run the executable against all three assignment samples, zero cycles,
early extinction, boundaries, and input/output errors. Test files stay in `build/`.

## Project structure

- `CMakeLists.txt`: executable target, language standard, and compiler warnings.
- `src/main.cpp`: simulation loop, cancellation, and console output.
- `src/terminal_renderer.hpp`, `src/terminal_renderer.cpp`: live terminal display and restoration.
- `src/world.hpp`, `src/world.cpp`: world storage, neighbor counting, and evolution.
- `src/json_storage.hpp`, `src/json_storage.cpp`: configuration loading and result saving.
- `tests/world_tests.cpp`: world construction, access, and evolution checks via CTest.
- `tests/json_storage_tests.cpp`: configuration parsing and validation checks.
- `tests/batch_case.cmake`: executable and output-file checks using CMake only.
- `third_party/`: the bundled JSON library, license, and version information.
- `build/`: generated build files, excluded from Git.
