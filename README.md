# Game of Life

Game of Life with configurable rules in a C++20 terminal application for macOS, Linux, and Windows.

Requires **CMake 3.20+**, a **C++20 compiler**, and its build tools.
`nlohmann/json`, FTXUI, and their licenses are included in `third_party/`;
no dependency installation or network access is needed during the build.

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
Live mode uses Unicode half-blocks: two cell rows per terminal line.
The board border follows the world edges; smaller terminals clip the view.
Add `--ascii` for the original `*`/`.` display, or `--batch` for a fast calculation
without animation. Both flags can be combined. Use `--batch` for redirected output.

Playback starts at the configured FPS. **Space** toggles pause; **Up/Down** changes FPS
by one (minimum 1). **Left/Right** pauses and steps through history; holding an arrow
accelerates from 1 to 5 to 20 generations per repeat. A gap or another key resets it.
The cycle limit or extinction pauses playback so you can rewind.
**Q** or **Ctrl+C** saves the selected state and exits.

## Input and output

Edit `game_of_life_input.json`, or select another file with `--input path`:

- `world.width`, `world.height`: positive integers matching the grid dimensions.
- `world.cells`: strings using `*` for living cells and `.` for dead cells.
- `simulation.fps`: positive integer controlling live playback speed.
- `simulation.cycles`: nonnegative integer limiting steps; `0` keeps the initial state.

Field edges do not wrap. Reaching the cycle limit or extinction saves
`game_of_life_output.json`: interactive mode pauses for rewinding; batch mode exits.
Quitting saves the currently selected generation. Ctrl+C also stops batch runs
after the current generation. Resize the terminal to see more of a large field.

## Try other rules

Rules default to `--birth 3 --survive 23` (Conway). Each digit is an allowed neighbor
count from 0 to 8: `36` means 3 or 6. Use `none` for an empty set.
Rules stay fixed during a run, including rewind and replay.

Run these commands from the repository root:

| Example | Command (macOS / Linux) |
| --- | --- |
| HighLife: a replicating seed | `./build/game_of_life --input examples/highlife.json --birth 36 --survive 23` |
| Seeds: expanding patterns | `./build/game_of_life --input examples/seeds.json --birth 2 --survive none` |
| Day & Night: changing dense regions | `./build/game_of_life --input examples/day-night.json --birth 3678 --survive 34678` |

On Windows, use `.\build\Debug\game_of_life.exe` with the same arguments.
All three examples use a 78 x 32 field and the original JSON schema.
HighLife seed: [LifeWiki replicator](https://conwaylife.com/wiki/OCA:HighLife/Replicator).

## Tests

```sh
ctest --test-dir build --output-on-failure
```

For Visual Studio builds, add `-C Debug`.
GitHub Actions builds and tests on macOS, Linux, and Windows on pushes and pull requests.
