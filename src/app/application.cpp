#include "application.hpp"

#include "interactive_game.hpp"
#include "json_storage.hpp"
#include "stop_signal.hpp"
#include "cell_formatter.hpp"

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

struct RunOptions {
    bool batch = false;
    life::CellStyle style = life::CellStyle::HalfBlocks;
    life::Rules rules;
    std::filesystem::path input_path = "game_of_life_input.json";
};

enum class StopReason { CycleLimit, Extinction, Cancelled };

struct RunResult {
    std::size_t generation;
    StopReason reason;
};

RunOptions parse_options(int argc, char* argv[]) {
    RunOptions options;
    std::string_view birth = "3";
    std::string_view survival = "23";
    for (int i = 1; i < argc; ++i) {
        const std::string_view argument(argv[i]);
        if (argument == "--batch") {
            options.batch = true;
        } else if (argument == "--ascii") {
            options.style = life::CellStyle::Ascii;
        } else if (argument == "--birth" || argument == "--survive" || argument == "--input") {
            if (i + 1 == argc) {
                throw std::invalid_argument("Missing value for " + std::string(argument));
            }
            const std::string_view value(argv[++i]);
            if (argument == "--birth") {
                birth = value;
            } else if (argument == "--survive") {
                survival = value;
            } else {
                options.input_path = value;
            }
        } else {
            throw std::invalid_argument("Usage: game_of_life [--batch] [--ascii] "
                                        "[--birth digits|none] [--survive digits|none] [--input file]");
        }
    }
    options.rules = life::Rules(birth, survival);
    return options;
}

StopReason determine_stop_reason(const life::World& world, const life::Rules& rules) {
    if (life::stop_requested()) {
        return StopReason::Cancelled;
    }
    if (!world.has_living_cells() && !rules.next_state(false, 0)) {
        return StopReason::Extinction;
    }
    return StopReason::CycleLimit;
}

RunResult run_batch(life::GameConfig& config, const life::Rules& rules) {
    auto& world = config.world;
    std::size_t generation = 0;
    while (!life::stop_requested() && generation < config.cycles &&
           (world.has_living_cells() || rules.next_state(false, 0))) {
        world = world.next_generation(rules);
        ++generation;
    }
    return {generation, determine_stop_reason(world, rules)};
}

void print_world(const life::World& world, life::CellStyle style) {
    for (const auto& row : life::format_cells(world, style, world.width(), world.height())) {
        std::cout << row << '\n';
    }
}

void print_initial_state(const life::GameConfig& config, life::CellStyle style,
                          const life::Rules& rules) {
    const auto& world = config.world;
    std::cout << "Game of Life: " << world.width() << " x " << world.height() << '\n';
    std::cout << "FPS: " << config.fps << ", requested cycles: " << config.cycles << '\n';
    std::cout << "Rules: " << rules.notation() << '\n';
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
    auto config = load_config(options.input_path);
    if (!options.batch) {
        const auto generation = run_interactive(config.world, config.cycles, config.fps,
                                                options.style, options.rules);
        save_result("game_of_life_output.json", config.world);
        std::cout << "Saved generation " << generation << " to game_of_life_output.json\n";
        return;
    }

    print_initial_state(config, CellStyle::Ascii, options.rules);
    const auto result = run_batch(config, options.rules);
    save_result("game_of_life_output.json", config.world);
    print_final_state(config.world, result, CellStyle::Ascii);
}

} // namespace life
