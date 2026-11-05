#include "game_controls.hpp"
#include "key_repeat.hpp"

#include <cstddef>
#include <utility>

namespace life {

ftxui::Component make_game_controls(ftxui::App& app, ftxui::Component view,
                                    Simulation& simulation, Playback& playback) {
    auto on_event = [&app, &simulation, &playback, repeat = KeyRepeat{}](ftxui::Event event) mutable {
        const bool forward = event == ftxui::Event::ArrowRight;
        if (forward || event == ftxui::Event::ArrowLeft) {
            playback.pause();
            const auto direction = forward ? KeyRepeat::Direction::Forward
                                           : KeyRepeat::Direction::Backward;
            const auto steps = repeat.steps(direction);
            for (std::size_t step = 0; step < steps; ++step) {
                const bool moved = forward ? simulation.step_forward() : simulation.step_backward();
                if (!moved) {
                    break;
                }
            }
            return true;
        }
        if (event != ftxui::Event::Custom) {
            repeat.reset();
        }
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
        if (event == ftxui::Event::Character('q') ||
            event == ftxui::Event::Character('Q') || event == ftxui::Event::CtrlC) {
            app.Exit();
            return true;
        }
        return false;
    };
    return ftxui::CatchEvent(std::move(view), std::move(on_event));
}

} // namespace life
