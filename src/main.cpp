#include "world.hpp"

#include <cstddef>
#include <iostream>

int main() {
    const life::World world({".....", "..*..", "..*..", "..*..", "....."});

    std::cout << "Game of Life: " << world.width() << " x " << world.height() << '\n';
    for (std::size_t y = 0; y < world.height(); ++y) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            std::cout << (world.is_alive(x, y) ? '*' : '.');
        }
        std::cout << '\n';
    }
    return 0;
}
