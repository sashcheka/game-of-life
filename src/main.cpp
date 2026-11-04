#include "json_storage.hpp"

#include <cstddef>
#include <exception>
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
    try {
        const auto config = life::load_config("game_of_life_input.json");
        const auto& world = config.world;

        std::cout << "Game of Life: " << world.width() << " x " << world.height() << '\n';
        std::cout << "FPS: " << config.fps << ", requested cycles: " << config.cycles << '\n';
        std::cout << "Generation 0\n";
        print_world(world);
        if (config.cycles > 0) {
            const auto next = world.next_generation();
            std::cout << "Generation 1\n";
            print_world(next);
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
