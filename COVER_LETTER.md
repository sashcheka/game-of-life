# Сопроводительное письмо / Cover letter

## Русский

Решение — терминальная «Игра Жизнь» на C++20 с CMake для Windows, macOS и Linux.
Основной подход: простая синхронная симуляция, независимая от JSON и отображения.
Компоненты разделены по ответственности:

- `src/core/`: правила, битовое хранение поля, вычисление поколений, история и таймер воспроизведения.
- `src/storage/`: чтение и проверка JSON, запись результата.
- `src/terminal/`: отображение поля, клавиатурное управление и ускорение перемотки.
- `src/app/`: параметры запуска, выполнение игры и обработка остановки; `main.cpp` — точка входа.

Реализованы фиксированные границы, правила Конвея, нулевое число шагов,
остановка с сохранением, пакетный расчёт и проверки некорректного ввода/ошибок записи.
Дополнительно доступны пауза, скорость, перемотка, выбор правил и примеры узоров.
`--wave` — отдельная геометрическая анимация, а не вариант правил «Жизни».

Используются nlohmann/json 3.12.0 и FTXUI 7.0.3; исходники и лицензии включены.
Игровой движок и внешние сервисы для работы приложения не требуются.
Автоматические тесты проверяют правила, границы, JSON, историю, отображение и CLI;
workflow GitHub Actions настроен на сборку и тесты Windows, macOS и Linux.
OpenAI Codex (AI) использовался для обсуждения архитектуры, написания и проверки кода,
тестов и документации. Материалы LifeWiki и Golly использовались при подборе узоров.

Ограничения и компромиссы:

- Расчёт проверяет всё поле за каждый шаг: O(ширина × высота).
- Интерактивная история хранит полные битовые снимки и растёт с числом поколений;
  пакетный режим историю не хранит. Дедупликация повторяющихся состояний не реализована.
- Diff мог бы хранить индексы изменившихся клеток: переключение этих клеток позволяет
  двигаться вперёд и назад. Это экономит память при редких изменениях, но индексы
  при плотных изменениях могут занимать больше места, чем полный битовый снимок;
  перемотка требует применения изменений. Ради простоты diff не реализован.
- Вид Unicode зависит от шрифта; есть ASCII-режим.
  Небольшое окно терминала обрезает отображаемое поле.
- Результат записывается напрямую, без атомарной замены файла.

## English

This is a C++20 terminal Game of Life built with CMake for Windows, macOS, and Linux.
The approach is a straightforward synchronous simulation independent of JSON and rendering.
Components are separated by responsibility:

- `src/core/`: rules, bit-packed grid, generation calculation, history, and playback timing.
- `src/storage/`: JSON loading, validation, and result writing.
- `src/terminal/`: grid rendering, keyboard controls, and accelerated history navigation.
- `src/app/`: command-line options, execution, and stop handling; `main.cpp` is the entry point.

Implemented features include fixed boundaries, Conway's rules, zero-step runs,
saving on stop, batch execution, and invalid-input/write-error handling.
Extras include pause, speed controls, rewind/replay, configurable rules, and sample patterns.
`--wave` is a separate geometric animation, not a Life rule variant.

The project uses nlohmann/json 3.12.0 and FTXUI 7.0.3, with sources and licenses included.
No game engine or external runtime service is required.
Automated tests cover rules, boundaries, JSON, history, rendering, and the CLI;
a GitHub Actions workflow is configured to build and test on Windows, macOS, and Linux.
OpenAI Codex (AI) assisted with architecture discussions, code implementation and review,
tests, and documentation. LifeWiki and Golly were consulted for sample patterns.

Limitations and tradeoffs:

- Each simulation step scans the entire grid: O(width × height).
- Interactive history stores full bit-packed snapshots and grows with the generation count;
  batch mode stores no history. Repeated states are not deduplicated.
- Diffs could store changed-cell indices; toggling those cells supports forward and backward
  navigation. This saves memory for sparse changes, but dense changes can make the indices
  larger than a full bit-packed snapshot, and navigation requires applying the changes.
  Diffs were left unimplemented to keep the design simple.
- Unicode appearance depends on fonts; an ASCII option is available.
  Small terminals clip the displayed grid.
- The result file is written directly, without atomic replacement.

Sources: [nlohmann/json](https://github.com/nlohmann/json),
[FTXUI](https://github.com/ArthurSonzogni/FTXUI),
[LifeWiki: HighLife](https://conwaylife.com/wiki/OCA:HighLife/Replicator),
[Day & Night](https://mail.conwaylife.com/wiki/OCA%3ADay_%26_Night),
[Kok's galaxy](https://conwaylife.com/wiki/Kok%27s_galaxy),
[Golly rule descriptions](https://golly.sourceforge.io/Help/Algorithms/QuickLife.html).
