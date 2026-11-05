#pragma once

#include "playback.hpp"
#include "simulation.hpp"

#include <ftxui.hpp>

namespace life {

ftxui::Component make_game_controls(ftxui::App& app, ftxui::Component view,
                                    Simulation& simulation, Playback& playback);

} // namespace life
