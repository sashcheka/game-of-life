# Game of Life

Game of Life with configurable rules and a wave animation mode in a C++20 terminal application
for macOS, Linux, and Windows.

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
The supplied inputs use 30 FPS and a limit of 6000 generations.

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

## Options and examples

| Option | Purpose |
| --- | --- |
| `--batch` | Calculate without animation, delays, or history, then exit. Use for redirected output. |
| `--ascii` | Use `*`/`.` instead of Unicode in interactive mode. |
| `--input path` | Select another input JSON; output still goes to the working directory. |
| `--birth 36 --survive 23` | Allowed neighbor counts: HighLife example; `none` means an empty set. |
| `--wave` | Separate geometric animation of repeating rings; incompatible with `--birth` / `--survive`. |

Example: `./build/game_of_life --input examples/day-night-waves.json --birth 3678 --survive 34678`.
For rings: `./build/game_of_life --wave --input examples/wave.json`.
On Windows, use `.\build\Debug\game_of_life.exe` with the same arguments.
Small terminals clip the grid, and Unicode appearance depends on the font.
See [the cover letter](COVER_LETTER.md) for architecture, limitations, and development tools.

## Tests

```sh
ctest --test-dir build --output-on-failure
```

For Visual Studio builds, add `-C Debug`.
GitHub Actions builds and tests on macOS, Linux, and Windows on pushes and pull requests.
