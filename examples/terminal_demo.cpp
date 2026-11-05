#include "game_controls.hpp"
#include "game_view.hpp"
#include "json_storage.hpp"
#include "playback.hpp"
#include "simulation.hpp"
#include "cell_formatter.hpp"

#include <ftxui.hpp>

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

void run_playback(ftxui::App& app, const ftxui::Component& controls,
                  life::Simulation& simulation, life::Playback& playback) {
    ftxui::Loop loop(&app, controls);
    playback.reset_timer();
    while (!loop.HasQuitted()) {
        loop.RunOnce();
        if (loop.HasQuitted()) {
            break;
        }
        if (playback.tick()) {
            (void)simulation.step_forward();
            if (!simulation.can_step_forward()) {
                playback.pause();
            }
            app.PostEvent(ftxui::Event::Custom);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    try {
        const auto config = life::load_config("game_of_life_input.json");
        life::Simulation simulation(config.world, config.cycles);
        life::Playback playback(config.fps);
        if (!simulation.can_step_forward()) {
            playback.pause();
        }
        auto app = ftxui::App::Fullscreen();
        app.TrackMouse(false);
        app.ForceHandleCtrlC(false);

        auto view = life::make_game_view(simulation, playback, config.cycles,
                                         life::CellStyle::HalfBlocks);

        auto controls = life::make_game_controls(app, view, simulation, playback);
        run_playback(app, controls, simulation, playback);
        life::save_result("game_of_life_output.json", simulation.current_world());
        std::cout << "Saved generation " << simulation.generation()
                  << " to game_of_life_output.json\n";
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
