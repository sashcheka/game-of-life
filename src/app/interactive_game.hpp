#pragma once

#include "terminal_renderer.hpp"

#include <cstddef>

namespace life {

std::size_t run_interactive(World& world, std::size_t cycles, std::size_t fps,
                            CellStyle style);

} // namespace life
