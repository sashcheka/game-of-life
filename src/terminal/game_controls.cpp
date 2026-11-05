#include "game_controls.hpp"

#include <utility>

namespace life {

ftxui::Component make_game_controls(ftxui::App& app, ftxui::Component view,
                                    Simulation& simulation, Playback& playback) {
    return ftxui::CatchEvent(std::move(view), [&app, &simulation, &playback](ftxui::Event event) {
        if (event == ftxui::Event::Character(' ')) {
            if (playback.playing() || simulation.can_step_forward()) {
                playback.toggle();
            }
            return true;
        }
        if (event == ftxui::Event::ArrowUp) {
            playback.increase_speed();
            return true;
        }
        if (event == ftxui::Event::ArrowDown) {
            playback.decrease_speed();
            return true;
        }
        if (event == ftxui::Event::ArrowLeft) {
            playback.pause();
            (void)simulation.step_backward();
            return true;
        }
        if (event == ftxui::Event::ArrowRight) {
            playback.pause();
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
}

} // namespace life
