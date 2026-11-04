# Game of Life

Conway's Game of Life in a C++20 terminal application for macOS, Linux, and Windows.

Requires **CMake 3.20+**, a **C++20 compiler**, and its build tools.
`nlohmann/json` and its MIT license are included in `third_party/`; no dependency
installation is needed.

## Build and run

macOS / Linux:

```sh
cmake -S . -B build
cmake --build build
./build/game_of_life
```

Windows with Visual Studio (PowerShell):

```powershell
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\game_of_life.exe
```

Run from the repository root. Generations update in place at the configured FPS.
Add `--batch` to calculate immediately without animation.

## Input and output

Edit `game_of_life_input.json`:

- `world.width`, `world.height`: positive integers matching the grid dimensions.
- `world.cells`: strings using `*` for living cells and `.` for dead cells.
- `simulation.fps`: positive integer controlling live playback speed.
- `simulation.cycles`: nonnegative integer limiting steps; `0` keeps the initial state.

Field edges do not wrap. The game stops at the cycle limit, when all cells die,
or on **Ctrl+C**, and saves `game_of_life_output.json`. Cancellation finishes the
current generation before saving. Resize the terminal to see more of a large field.

## Tests

```sh
ctest --test-dir build --output-on-failure
```

For Visual Studio builds, add `-C Debug`.
GitHub Actions builds and tests on macOS, Linux, and Windows on pushes and pull requests.
