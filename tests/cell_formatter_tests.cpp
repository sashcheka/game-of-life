#include "cell_formatter.hpp"

#include <iostream>
#include <string>
#include <vector>

int main() {
    using life::CellStyle;
    const life::World combinations({"**..", "*.*."});
    if (life::format_cells(combinations, CellStyle::HalfBlocks, 4, 1) !=
        std::vector<std::string>{"█▀▄ "}) {
        std::cerr << "Incorrect half-block mapping for the four cell combinations\n";
        return 1;
    }
    if (life::format_cells(combinations, CellStyle::Ascii, 4, 2) !=
        std::vector<std::string>{"**..", "*.*."}) {
        std::cerr << "ASCII cell output changed\n";
        return 1;
    }
    const life::World odd_height({"*.", ".*", "**"});
    if (life::format_cells(odd_height, CellStyle::HalfBlocks, 10, 10) !=
        std::vector<std::string>{"▀▄", "▀▀"}) {
        std::cerr << "An unpaired last row must have a dead lower half\n";
        return 1;
    }
    if (life::format_cells(odd_height, CellStyle::HalfBlocks, 1, 1) !=
        std::vector<std::string>{"▀"} ||
        life::format_cells(odd_height, CellStyle::Ascii, 1, 2) !=
        std::vector<std::string>{"*", "."}) {
        std::cerr << "Viewport clipping is incorrect\n";
        return 1;
    }
    if (!life::format_cells(odd_height, CellStyle::HalfBlocks, 2, 0).empty()) {
        std::cerr << "A zero-height viewport must contain no rows\n";
        return 1;
    }
    const life::World empty({"..", ".."});
    if (life::format_cells(empty, CellStyle::HalfBlocks, 2, 1) !=
        std::vector<std::string>{"  "}) {
        std::cerr << "Dead cells must be rendered as spaces\n";
        return 1;
    }
    return 0;
}
