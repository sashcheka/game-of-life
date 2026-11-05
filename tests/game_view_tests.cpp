#include "game_view.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

ftxui::Screen draw_board(const life::World& world, life::CellStyle style, int width, int height) {
    auto screen = ftxui::Screen::Create(ftxui::Dimension::Fixed(width),
                                       ftxui::Dimension::Fixed(height));
    ftxui::Render(screen, life::make_game_board(world, style));
    return screen;
}

int main() {
    try {
        const life::World world({"*..*", "....", "..*.", "*..."});
        const auto full = draw_board(world, life::CellStyle::HalfBlocks, 16, 10);
        require(full.CellAt(0, 0).character == "┌" && full.CellAt(5, 0).character == "┐" &&
                full.CellAt(0, 3).character == "└" && full.CellAt(5, 3).character == "┘",
                "The border does not match the world dimensions");
        require(full.CellAt(1, 1).character == "▀" && full.CellAt(1, 2).character == "▄",
                "The border shifted the game cells");
        require(full.CellAt(6, 0).character.empty() && full.CellAt(0, 4).character.empty(),
                "The border stretched to fill the terminal");

        const auto clipped = draw_board(world, life::CellStyle::HalfBlocks, 4, 3);
        require(clipped.CellAt(3, 0).character == "─" &&
                clipped.CellAt(0, 2).character == "│" && clipped.CellAt(3, 2).character == "▀",
                "Clipping created a false world boundary");
        const auto tiny = draw_board(world, life::CellStyle::HalfBlocks, 1, 1);
        require(tiny.CellAt(0, 0).character == "┌", "A tiny terminal lost the board corner");

        const auto ascii = draw_board(world, life::CellStyle::Ascii, 16, 10);
        require(ascii.CellAt(0, 0).character == "#" && ascii.CellAt(5, 5).character == "#" &&
                ascii.CellAt(1, 1).character == "*" && ascii.CellAt(2, 1).character == ".",
                "The ASCII border or cells are incorrect");
        const life::World odd({"*.", ".*", "**"});
        const auto unpaired = draw_board(odd, life::CellStyle::HalfBlocks, 16, 10);
        require(unpaired.CellAt(3, 3).character == "┘", "Odd height changed the border");
        const life::World empty({"...", "..."});
        const auto dead = draw_board(empty, life::CellStyle::HalfBlocks, 16, 10);
        require(dead.CellAt(4, 2).character == "┘", "An empty world lost its border");
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
