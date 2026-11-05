#include "json_storage.hpp"
#include "simulation.hpp"
#include "terminal_renderer.hpp"

#include <ftxui.hpp>

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <utility>

int main() {
    try {
        const auto config = life::load_config("game_of_life_input.json");
        life::Simulation simulation(config.world, config.cycles);
        auto app = ftxui::App::Fullscreen();
        app.TrackMouse(false);
        app.ForceHandleCtrlC(false);

        auto view = ftxui::Renderer([&] {
            const auto size = ftxui::Terminal::Size();
            const auto columns = static_cast<std::size_t>(std::max(0, size.dimx - 2));
            const auto rows = static_cast<std::size_t>(std::max(0, size.dimy - 5));
            ftxui::Elements content{
                ftxui::text("Generation " + std::to_string(simulation.generation()) +
                            " / " + std::to_string(config.cycles) + " | Computed: " +
                            std::to_string(simulation.latest_generation())),
                ftxui::text("Left/Right: step | Q: save and exit"),
                ftxui::separator(),
            };
            for (const auto& row : life::format_cells(simulation.current_world(),
                                                     life::CellStyle::HalfBlocks, columns, rows)) {
                content.push_back(ftxui::text(row));
            }
            return ftxui::vbox(std::move(content)) | ftxui::border;
        });

        auto controls = ftxui::CatchEvent(view, [&](ftxui::Event event) {
            if (event == ftxui::Event::ArrowLeft) {
                (void)simulation.step_backward();
                return true;
            }
            if (event == ftxui::Event::ArrowRight) {
                (void)simulation.step_forward();
                return true;
            }
            if (event == ftxui::Event::Character('q') ||
                event == ftxui::Event::Character('Q') || event == ftxui::Event::CtrlC) {
                app.Exit();
                return true;
            }
            return false;
        });
        app.Loop(controls);
        life::save_result("game_of_life_output.json", simulation.current_world());
        std::cout << "Saved generation " << simulation.generation()
                  << " to game_of_life_output.json\n";
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
