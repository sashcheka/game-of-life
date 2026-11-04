#include "json_storage.hpp"

#include <atomic>
#include <csignal>
#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>

namespace {

std::atomic_flag stop_requested{};

extern "C" void request_stop(int) {
    (void)stop_requested.test_and_set();
}

void print_world(const life::World& world) {
    for (std::size_t y = 0; y < world.height(); ++y) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            std::cout << (world.is_alive(x, y) ? '*' : '.');
        }
        std::cout << '\n';
    }
}

} // namespace

int main() {
    try {
        if (std::signal(SIGINT, request_stop) == SIG_ERR ||
            std::signal(SIGTERM, request_stop) == SIG_ERR) {
            throw std::runtime_error("Cannot install stop handlers");
        }
        auto config = life::load_config("game_of_life_input.json");
        auto& world = config.world;

        std::cout << "Game of Life: " << world.width() << " x " << world.height() << '\n';
        std::cout << "FPS: " << config.fps << ", requested cycles: " << config.cycles << '\n';
        std::cout << "Generation 0\n";
        print_world(world);
        std::cout << "Press Ctrl+C to stop.\n" << std::flush;

        std::size_t generation = 0;
        while (!stop_requested.test() && generation < config.cycles && world.has_living_cells()) {
            world = world.next_generation();
            ++generation;
        }
        const bool cancelled = stop_requested.test();
        life::save_result("game_of_life_output.json", world);
        if (generation > 0) {
            std::cout << "Generation " << generation << '\n';
            print_world(world);
        }
        const char* reason = "cycle limit";
        if (cancelled) {
            reason = "cancelled";
        } else if (!world.has_living_cells()) {
            reason = "no living cells";
        }
        std::cout << "Stopped: " << reason << ". Saved generation " << generation
                  << " to game_of_life_output.json\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
