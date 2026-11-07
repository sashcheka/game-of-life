# Game of Life

## Русский

Терминальная «Игра Жизнь» на **C++20** для **Windows, macOS и Linux**.
По умолчанию используются правила Конвея: рождение при 3 соседях, выживание при 2 или 3.
Поколения обновляются одновременно; прямоугольное поле имеет фиксированные границы без зацикливания.

### Требования и библиотеки

Нужны **CMake 3.20+**, компилятор C++20 и инструменты сборки
(например, Visual Studio 2022 с компонентами C++ на Windows).
CMake и компилятор относятся к среде разработки и не входят в поставку.
**nlohmann/json 3.12.0** (JSON) и **FTXUI 7.0.3** (терминал) включены исходниками
в `third_party/` вместе с лицензиями. Установка библиотек и доступ к интернету не нужны.

### Сборка и запуск

Выполняйте команды из корня проекта.

macOS / Linux:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/game_of_life
ctest --test-dir build -C Release --output-on-failure
```

Windows, PowerShell с доступными инструментами Visual Studio:

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\game_of_life.exe
ctest --test-dir build -C Release --output-on-failure
```

### Вход, выход и управление

По умолчанию читается `game_of_life_input.json` из текущего каталога:

```json
{
  "world": {"width": 3, "height": 3, "cells": [".*.", ".*.", ".*."]},
  "simulation": {"fps": 30, "cycles": 1}
}
```

Размеры должны совпадать со строками поля; `*` — живая клетка, `.` — мёртвая.
Размеры и FPS — положительные целые; `cycles` — целое от 0 (0 сохраняет исходное поле).
Результат записывается в `game_of_life_output.json`: `{"result":{"width":3,"height":3,"cells":["...","***","..."]}}`.

**Space** — пауза; **Left/Right** — перемотка (удержание ускоряет её);
**Up/Down** — скорость; **Q / Ctrl+C** — сохранить выбранное поколение и выйти.
При вымирании или достижении лимита результат сохраняется, интерфейс остаётся на паузе.

| Параметр | Назначение |
| --- | --- |
| `--batch` | Расчёт без анимации, задержек и истории; автоматический выход. Используйте при перенаправлении вывода. |
| `--ascii` | Символы `*`/`.` вместо Unicode в интерактивном режиме. |
| `--input path` | Другой входной JSON; результат по-прежнему сохраняется в текущем каталоге. |
| `--birth 36 --survive 23` | Допустимые количества соседей: пример HighLife; `none` — пустой набор. |
| `--wave` | Отдельная геометрическая анимация повторяющихся колец; несовместима с `--birth` / `--survive`. |

Например: `./build/game_of_life --input examples/day-night-waves.json --birth 3678 --survive 34678`.
Для колец: `./build/game_of_life --wave --input examples/wave.json`.
На Windows используйте `.\build\Release\game_of_life.exe` с теми же аргументами.
Небольшое окно обрезает поле, вид Unicode зависит от шрифта.
Архитектура, ограничения и использованные инструменты описаны в [сопроводительном письме](COVER_LETTER.md).

## English

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
