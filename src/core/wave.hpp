#pragma once

#include "world.hpp"

#include <cstddef>

namespace life {

[[nodiscard]] World wave_frame(const World& world, std::size_t generation);

} // namespace life
