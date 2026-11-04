#include "json_storage.hpp"
#include "terminal_renderer.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

namespace {

std::atomic_flag stop_requested{};
using Clock = std::chrono::steady_clock;

extern "C" void request_stop(int) {
    (void)stop_requested.test_and_set();
}

void wait_for_frame(Clock::time_point deadline) {
    const auto slice = std::chrono::duration_cast<Clock::duration>(std::chrono::milliseconds(10));
    while (!stop_requested.test() && Clock::now() < deadline) {
        std::this_thread::sleep_until(std::min(deadline, Clock::now() + slice));
    }
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

int main(int argc, char* argv[]) {
    try {
        bool batch = false;
        auto style = life::CellStyle::HalfBlocks;
        for (int i = 1; i < argc; ++i) {
            const std::string_view argument(argv[i]);
            if (argument == "--batch") {
                batch = true;
            } else if (argument == "--ascii") {
                style = life::CellStyle::Ascii;
            } else {
                throw std::invalid_argument("Usage: game_of_life [--batch] [--ascii]");
            }
        }
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
        {
            life::TerminalRenderer terminal(!batch, style);
            if (terminal.active()) {
                terminal.draw(world, generation, config.cycles, config.fps);
            }
            const auto period = std::chrono::duration_cast<Clock::duration>(
                std::chrono::duration<double>(1.0 / static_cast<double>(config.fps)));
            auto next_frame = Clock::now();
            while (!stop_requested.test() && generation < config.cycles && world.has_living_cells()) {
                if (terminal.active()) {
                    next_frame += period;
                    wait_for_frame(next_frame);
                    if (stop_requested.test()) {
                        break;
                    }
                }
                world = world.next_generation();
                ++generation;
                if (terminal.active()) {
                    terminal.draw(world, generation, config.cycles, config.fps);
                }
            }
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
