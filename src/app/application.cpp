#include "application.hpp"

#include "interactive_game.hpp"
#include "json_storage.hpp"
#include "stop_signal.hpp"
#include "terminal_renderer.hpp"

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace {

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

StopReason determine_stop_reason(const life::World& world) {
    if (life::stop_requested()) {
        return StopReason::Cancelled;
    }
    if (!world.has_living_cells()) {
        return StopReason::Extinction;
    }
    return StopReason::CycleLimit;
}

RunResult run_batch(life::GameConfig& config) {
    auto& world = config.world;
    std::size_t generation = 0;
    while (!life::stop_requested() && generation < config.cycles && world.has_living_cells()) {
        world = world.next_generation();
        ++generation;
    }
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

namespace life {

void run_application(int argc, char* argv[]) {
    const auto options = parse_options(argc, argv);
    install_stop_handlers();
    auto config = load_config("game_of_life_input.json");
    if (!options.batch) {
        const auto generation = run_interactive(config.world, config.cycles, config.fps,
                                                options.style);
        save_result("game_of_life_output.json", config.world);
        std::cout << "Saved generation " << generation << " to game_of_life_output.json\n";
        return;
    }

    print_initial_state(config, CellStyle::Ascii);
    const auto result = run_batch(config);
    save_result("game_of_life_output.json", config.world);
    print_final_state(config.world, result, CellStyle::Ascii);
}

} // namespace life
