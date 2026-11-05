#pragma once

#include "playback.hpp"
#include "simulation.hpp"
#include "cell_formatter.hpp"

#include <ftxui.hpp>

namespace life {

ftxui::Element make_game_board(const World& world, CellStyle style);

ftxui::Component make_game_view(const Simulation& simulation, const Playback& playback,
                                std::size_t cycles, CellStyle style);

} // namespace life
