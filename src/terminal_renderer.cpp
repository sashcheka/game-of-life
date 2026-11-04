#include "terminal_renderer.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace life {
namespace {

struct TerminalSize {
    std::size_t columns = 80;
    std::size_t rows = 24;
};

std::vector<std::string> draw_ascii(const World& world, std::size_t columns,
                                   std::size_t rows) {
    std::vector<std::string> lines;
    for (std::size_t y = 0; y < rows; ++y) {
        std::string line;
        for (std::size_t x = 0; x < columns; ++x) {
            line += world.is_alive(x, y) ? '*' : '.';
        }
        lines.push_back(line);
    }
    return lines;
}

std::vector<std::string> draw_half_blocks(const World& world, std::size_t columns,
                                         std::size_t rows) {
    std::vector<std::string> lines;
    for (std::size_t row = 0; row < rows; ++row) {
        const auto y = row * 2;
        std::string line;
        for (std::size_t x = 0; x < columns; ++x) {
            const bool upper = world.is_alive(x, y);
            const bool lower = y + 1 < world.height() && world.is_alive(x, y + 1);
            if (upper && lower) {
                line += "█";
            } else if (upper) {
                line += "▀";
            } else if (lower) {
                line += "▄";
            } else {
                line += ' ';
            }
        }
        lines.push_back(line);
    }
    return lines;
}

TerminalSize terminal_size() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        return {static_cast<std::size_t>(info.srWindow.Right - info.srWindow.Left + 1),
                static_cast<std::size_t>(info.srWindow.Bottom - info.srWindow.Top + 1)};
    }
#else
    winsize size{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 && size.ws_col > 0 && size.ws_row > 0) {
        return {size.ws_col, size.ws_row};
    }
#endif
    return {};
}

} // namespace

std::vector<std::string> format_cells(const World& world, CellStyle style,
                                     std::size_t columns, std::size_t rows) {
    columns = std::min(columns, world.width());
    if (style == CellStyle::Ascii) {
        return draw_ascii(world, columns, std::min(rows, world.height()));
    }
    const auto paired_rows = world.height() / 2 + world.height() % 2;
    return draw_half_blocks(world, columns, std::min(rows, paired_rows));
}

TerminalRenderer::TerminalRenderer(bool requested) {
    if (!requested) {
        return;
    }
#ifdef _WIN32
    output_handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!GetConsoleMode(output_handle_, &original_mode_) ||
        !SetConsoleMode(output_handle_, original_mode_ | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        return;
    }
#else
    const auto* term = std::getenv("TERM");
    if (!isatty(STDOUT_FILENO) || term == nullptr || std::string_view(term) == "dumb") {
        return;
    }
#endif
    active_ = true;
    // Use an alternate screen and hide the cursor while frames are updating.
    std::cout << "\x1b[?1049h\x1b[?25l\x1b[2J\x1b[H" << std::flush;
}

TerminalRenderer::~TerminalRenderer() {
    if (active_) {
        std::cout << "\x1b[?25h\x1b[?1049l" << std::flush;
#ifdef _WIN32
        SetConsoleMode(output_handle_, original_mode_);
#endif
    }
}

bool TerminalRenderer::active() const noexcept {
    return active_;
}

void TerminalRenderer::draw(const World& world, std::size_t generation,
                            std::size_t limit, std::size_t fps) const {
    if (!active_) {
        return;
    }
    const auto size = terminal_size();
    const auto columns = size.columns - 1;
    const auto rows = size.rows - 1;
    const auto visible_columns = std::min(world.width(), columns);
    const auto visible_rows = rows > 4 ? std::min(world.height(), rows - 4) : 0;
    std::ostringstream content;
    content << "Game of Life | " << world.width() << " x " << world.height() << '\n'
            << "Generation " << generation << " / " << limit << " | " << fps << " fps\n"
            << "Ctrl+C to stop\n";
    if (visible_columns < world.width() || visible_rows < world.height()) {
        content << "Showing top-left " << visible_columns << " x " << visible_rows;
    }
    content << '\n';
    for (std::size_t y = 0; y < visible_rows; ++y) {
        for (std::size_t x = 0; x < visible_columns; ++x) {
            content << (world.is_alive(x, y) ? '*' : '.');
        }
        content << '\n';
    }
    std::istringstream lines(content.str());
    std::ostringstream frame;
    frame << "\x1b[H";
    std::string line;
    for (std::size_t y = 0; y < rows && std::getline(lines, line); ++y) {
        frame << line.substr(0, columns) << "\x1b[K\n";
    }
    // Erase any remainder from the previous frame, for example after resizing.
    frame << "\x1b[J";
    std::cout << frame.str() << std::flush;
}

} // namespace life
