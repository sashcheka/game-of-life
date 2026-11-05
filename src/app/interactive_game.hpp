#pragma once

#include "cell_formatter.hpp"
#include "simulation.hpp"

#include <cstddef>

namespace life {

std::size_t run_interactive(World& world, std::size_t cycles, std::size_t fps,
                            CellStyle style, const Rules& rules,
                            SimulationMode mode = SimulationMode::Life);

} // namespace life
