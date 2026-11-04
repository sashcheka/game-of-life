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

struct RunOptions {
    bool batch = false;
    life::CellStyle style = life::CellStyle::HalfBlocks;
};

enum class StopReason { CycleLimit, Extinction, Cancelled };

struct RunResult {
    std::size_t generation;
    StopReason reason;
};

RunOptions parse_options(int argc, char* argv[]) {
    RunOptions options;
    for (int i = 1; i < argc; ++i) {
        const std::string_view argument(argv[i]);
        if (argument == "--batch") {
            options.batch = true;
        } else if (argument == "--ascii") {
            options.style = life::CellStyle::Ascii;
        } else {
            throw std::invalid_argument("Usage: game_of_life [--batch] [--ascii]");
        }
    }
    return options;
}

extern "C" void request_stop(int) {
    (void)stop_requested.test_and_set();
}

void install_stop_handlers() {
    if (std::signal(SIGINT, request_stop) == SIG_ERR ||
        std::signal(SIGTERM, request_stop) == SIG_ERR) {
        throw std::runtime_error("Cannot install stop handlers");
    }
}

void wait_for_frame(Clock::time_point deadline) {
    const auto slice = std::chrono::duration_cast<Clock::duration>(std::chrono::milliseconds(10));
    while (!stop_requested.test() && Clock::now() < deadline) {
        std::this_thread::sleep_until(std::min(deadline, Clock::now() + slice));
    }
}

StopReason determine_stop_reason(const life::World& world) {
    if (stop_requested.test()) {
        return StopReason::Cancelled;
    }
    if (!world.has_living_cells()) {
        return StopReason::Extinction;
    }
    return StopReason::CycleLimit;
}

RunResult run_generations(life::GameConfig& config, life::TerminalRenderer& terminal) {
    auto& world = config.world;
    std::size_t generation = 0;
    terminal.start();
    terminal.draw(world, generation, config.cycles, config.fps);

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
        terminal.draw(world, generation, config.cycles, config.fps);
    }
    terminal.finish();
    return {generation, determine_stop_reason(world)};
}

void print_world(const life::World& world, life::CellStyle style) {
    for (const auto& row : life::format_cells(world, style, world.width(), world.height())) {
        std::cout << row << '\n';
    }
}

void print_initial_state(const life::GameConfig& config, life::CellStyle style) {
    const auto& world = config.world;
    std::cout << "Game of Life: " << world.width() << " x " << world.height() << '\n';
    std::cout << "FPS: " << config.fps << ", requested cycles: " << config.cycles << '\n';
    std::cout << "Generation 0\n";
    print_world(world, style);
    std::cout << "Press Ctrl+C to stop.\n" << std::flush;
}

void print_final_state(const life::World& world, const RunResult& result,
                       life::CellStyle style) {
    if (result.generation > 0) {
        std::cout << "Generation " << result.generation << '\n';
        print_world(world, style);
    }
    const char* reason = "cycle limit";
    if (result.reason == StopReason::Cancelled) {
        reason = "cancelled";
    } else if (result.reason == StopReason::Extinction) {
        reason = "no living cells";
    }
    std::cout << "Stopped: " << reason << ". Saved generation " << result.generation
              << " to game_of_life_output.json\n";
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const auto options = parse_options(argc, argv);
        install_stop_handlers();
        auto config = life::load_config("game_of_life_input.json");
        life::TerminalRenderer terminal(!options.batch, options.style);

        print_initial_state(config, terminal.style());
        const auto result = run_generations(config, terminal);
        life::save_result("game_of_life_output.json", config.world);
        print_final_state(config.world, result, terminal.style());
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
