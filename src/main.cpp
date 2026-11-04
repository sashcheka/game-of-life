#include "world.hpp"

#include <cstddef>
#include <iostream>

void print_world(const life::World& world) {
    for (std::size_t y = 0; y < world.height(); ++y) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            std::cout << (world.is_alive(x, y) ? '*' : '.');
        }
        std::cout << '\n';
    }
}

int main() {
    const life::World world({".....", "..*..", "..*..", "..*..", "....."});
    const auto next = world.next_generation();

    std::cout << "Game of Life: " << world.width() << " x " << world.height() << '\n';
    std::cout << "Generation 0\n";
    print_world(world);
    std::cout << "Generation 1\n";
    print_world(next);
    return 0;
}
