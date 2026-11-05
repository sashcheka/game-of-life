#include "cell_formatter.hpp"

#include <algorithm>
#include <string>

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

} // namespace life
