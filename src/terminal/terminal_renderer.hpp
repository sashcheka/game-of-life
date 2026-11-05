#pragma once

#include "world.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace life {

enum class CellStyle { Ascii, HalfBlocks };

[[nodiscard]] std::vector<std::string> format_cells(const World& world, CellStyle style,
                                                  std::size_t columns, std::size_t rows);

} // namespace life
