#include "application.hpp"

#include "interactive_game.hpp"
#include "json_storage.hpp"
#include "stop_signal.hpp"
#include "cell_formatter.hpp"
#include "wave.hpp"

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
    life::SimulationMode mode = life::SimulationMode::Life;
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
    bool custom_rules = false;
    for (int i = 1; i < argc; ++i) {
        const std::string_view argument(argv[i]);
        if (argument == "--batch") {
            options.batch = true;
        } else if (argument == "--ascii") {
            options.style = life::CellStyle::Ascii;
        } else if (argument == "--wave") {
            options.mode = life::SimulationMode::Wave;
        } else if (argument == "--birth" || argument == "--survive" || argument == "--input") {
            if (i + 1 == argc) {
                throw std::invalid_argument("Missing value for " + std::string(argument));
            }
            const std::string_view value(argv[++i]);
            if (argument == "--birth") {
                birth = value;
                custom_rules = true;
            } else if (argument == "--survive") {
                survival = value;
                custom_rules = true;
            } else {
                options.input_path = value;
            }
        } else {
            throw std::invalid_argument("Usage: game_of_life [--batch] [--ascii] [--wave] "
                                        "[--birth digits|none] [--survive digits|none] [--input file]");
        }
    }
    if (options.mode == life::SimulationMode::Wave && custom_rules) {
        throw std::invalid_argument("--wave cannot be combined with --birth or --survive");
    }
    options.rules = life::Rules(birth, survival);
    return options;
}

StopReason determine_stop_reason(const life::World& world, bool can_revive) {
    if (life::stop_requested()) {
        return StopReason::Cancelled;
    }
    if (!world.has_living_cells() && !can_revive) {
        return StopReason::Extinction;
    }
    return StopReason::CycleLimit;
}

RunResult run_batch(life::GameConfig& config, const RunOptions& options) {
    auto& world = config.world;
    std::size_t generation = 0;
    const bool can_revive = options.mode == life::SimulationMode::Wave ||
                            options.rules.next_state(false, 0);
    while (!life::stop_requested() && generation < config.cycles &&
           (world.has_living_cells() || can_revive)) {
        world = options.mode == life::SimulationMode::Wave
                    ? life::wave_frame(world, generation + 1)
                    : world.next_generation(options.rules);
        ++generation;
    }
    return {generation, determine_stop_reason(world, can_revive)};
}

void print_world(const life::World& world, life::CellStyle style) {
    for (const auto& row : life::format_cells(world, style, world.width(), world.height())) {
        std::cout << row << '\n';
    }
}

void print_initial_state(const life::GameConfig& config, life::CellStyle style,
                          const RunOptions& options) {
    const auto& world = config.world;
    std::cout << "Game of Life: " << world.width() << " x " << world.height() << '\n';
    std::cout << "FPS: " << config.fps << ", requested cycles: " << config.cycles << '\n';
    if (options.mode == life::SimulationMode::Wave) {
        std::cout << "Mode: Wave\n";
    } else {
        std::cout << "Rules: " << options.rules.notation() << '\n';
    }
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
    if (options.mode == SimulationMode::Wave) {
        config.world = wave_frame(config.world, 0);
    }
    if (!options.batch) {
        const auto generation = run_interactive(config.world, config.cycles, config.fps,
                                                options.style, options.rules, options.mode);
        save_result("game_of_life_output.json", config.world);
        std::cout << "Saved generation " << generation << " to game_of_life_output.json\n";
        return;
    }

    print_initial_state(config, CellStyle::Ascii, options);
    const auto result = run_batch(config, options);
    save_result("game_of_life_output.json", config.world);
    print_final_state(config.world, result, CellStyle::Ascii);
}

} // namespace life
