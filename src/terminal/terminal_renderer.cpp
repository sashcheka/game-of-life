#include "terminal_renderer.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace life {
namespace {

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

TerminalRenderer::TerminalRenderer(bool requested, CellStyle style) : style_(style) {
    if (!requested) {
        return;
    }
#ifdef _WIN32
    output_handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!GetConsoleMode(output_handle_, &original_mode_) ||
        !SetConsoleMode(output_handle_, original_mode_ | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        return;
    }
    if (style_ == CellStyle::HalfBlocks) {
        const auto code_page = GetConsoleOutputCP();
        if (code_page != 0 && SetConsoleOutputCP(CP_UTF8)) {
            original_code_page_ = code_page;
        } else {
            style_ = CellStyle::Ascii;
        }
    }
#else
    const auto* term = std::getenv("TERM");
    if (!isatty(STDOUT_FILENO) || term == nullptr || std::string_view(term) == "dumb") {
        return;
    }
#endif
    active_ = true;
}

TerminalRenderer::~TerminalRenderer() {
    finish();
    // Flush snapshots before restoring the console's original encoding.
    std::cout << std::flush;
#ifdef _WIN32
    if (active_) {
        SetConsoleMode(output_handle_, original_mode_);
        if (original_code_page_ != 0) {
            SetConsoleOutputCP(original_code_page_);
        }
    }
#endif
}

bool TerminalRenderer::active() const noexcept {
    return active_;
}

CellStyle TerminalRenderer::style() const noexcept {
    return active_ ? style_ : CellStyle::Ascii;
}

void TerminalRenderer::start() {
    if (active_ && !screen_active_) {
        screen_active_ = true;
        // Use an alternate screen and hide the cursor while frames are updating.
        std::cout << "\x1b[?1049h\x1b[?25l\x1b[2J\x1b[H" << std::flush;
    }
}

void TerminalRenderer::finish() {
    if (screen_active_) {
        std::cout << "\x1b[?25h\x1b[?1049l" << std::flush;
        screen_active_ = false;
    }
}

} // namespace life
