#pragma once

#include "playback.hpp"
#include "simulation.hpp"
#include "cell_formatter.hpp"

#include <ftxui.hpp>

namespace life {

ftxui::Component make_game_view(const Simulation& simulation, const Playback& playback,
                                std::size_t cycles, CellStyle style);

} // namespace life
