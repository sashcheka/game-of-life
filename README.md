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

A **C++20** terminal Game of Life for **Windows, macOS, and Linux**.
Conway's rules are the default: birth with 3 neighbors, survival with 2 or 3.
Generations update synchronously on a rectangular grid with fixed, non-wrapping edges.

### Requirements and libraries

Requires **CMake 3.20+**, a C++20 compiler, and build tools
(e.g. Visual Studio 2022 with C++ development components on Windows).
CMake and the compiler are development tools and are not bundled.
**nlohmann/json 3.12.0** (JSON) and **FTXUI 7.0.3** (terminal UI) are included as source
in `third_party/`, with their licenses. No library installation or internet access is required.

### Build and run

Run all commands from the project root.

macOS / Linux:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/game_of_life
ctest --test-dir build -C Release --output-on-failure
```

Windows, PowerShell with Visual Studio build tools available:

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\game_of_life.exe
ctest --test-dir build -C Release --output-on-failure
```

### Input, output, and controls

Reads `game_of_life_input.json` from the working directory by default, using the JSON
schema shown above. Grid dimensions must match the cell rows: `*` is alive, `.` is dead.
Dimensions and FPS must be positive integers; `cycles` must be a nonnegative integer
(0 preserves the initial grid).
Writes `game_of_life_output.json` with a `result` object containing `width`, `height`, and `cells`.

**Space** pauses; **Left/Right** steps through history (holding accelerates);
**Up/Down** changes speed; **Q / Ctrl+C** saves the selected generation and exits.
Extinction or the cycle limit saves the result and pauses the interface.

| Option | Purpose |
| --- | --- |
| `--batch` | Calculate without animation, delays, or history, then exit. Use for redirected output. |
| `--ascii` | Use `*`/`.` instead of Unicode in interactive mode. |
| `--input path` | Select another input JSON; output still goes to the working directory. |
| `--birth 36 --survive 23` | Allowed neighbor counts: HighLife example; `none` means an empty set. |
| `--wave` | Separate geometric animation of repeating rings; incompatible with `--birth` / `--survive`. |

Example: `./build/game_of_life --input examples/day-night-waves.json --birth 3678 --survive 34678`.
For rings: `./build/game_of_life --wave --input examples/wave.json`.
On Windows, use `.\build\Release\game_of_life.exe` with the same arguments.
Small terminals clip the grid, and Unicode appearance depends on the font.
See [the cover letter](COVER_LETTER.md) for architecture, limitations, and development tools.
