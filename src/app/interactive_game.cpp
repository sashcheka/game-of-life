#include "interactive_game.hpp"

#include "game_controls.hpp"
#include "game_view.hpp"
#include "json_storage.hpp"
#include "stop_signal.hpp"

#include <chrono>
#include <thread>

namespace life {
namespace {

void run_playback(ftxui::App& app, const ftxui::Component& controls,
                  Simulation& simulation, Playback& playback) {
    ftxui::Loop loop(&app, controls);
    playback.reset_timer();
    bool saved_end = false;
    while (!loop.HasQuitted()) {
        loop.RunOnce();
        if (loop.HasQuitted() || stop_requested()) {
            break;
        }
        if (playback.tick()) {
            (void)simulation.step_forward();
            if (!simulation.can_step_forward()) {
                playback.pause();
            }
            app.PostEvent(ftxui::Event::Custom);
        }
        if (!simulation.can_step_forward() && !saved_end) {
            save_result("game_of_life_output.json", simulation.current_world());
            saved_end = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

} // namespace

std::size_t run_interactive(World& world, std::size_t cycles, std::size_t fps,
                            CellStyle style) {
    Simulation simulation(world, cycles);
    Playback playback(fps);
    if (!simulation.can_step_forward()) {
        playback.pause();
    }
    auto app = ftxui::App::Fullscreen();
    app.TrackMouse(false);
    app.ForceHandleCtrlC(false);
    auto quirks = ftxui::Terminal::GetQuirks();
    quirks.SetComponentAscii(style == CellStyle::Ascii);
    ftxui::Terminal::SetQuirks(quirks);
    auto view = make_game_view(simulation, playback, cycles, style);
    auto controls = make_game_controls(app, view, simulation, playback);
    run_playback(app, controls, simulation, playback);
    world = simulation.current_world();
    return simulation.generation();
}

} // namespace life
