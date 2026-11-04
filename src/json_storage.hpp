#pragma once

#include "world.hpp"

#include <cstddef>
#include <filesystem>
#include <iosfwd>

namespace life {

struct GameConfig {
    World world;
    std::size_t fps;
    std::size_t cycles;
};

[[nodiscard]] GameConfig read_config(std::istream& input);
[[nodiscard]] GameConfig load_config(const std::filesystem::path& path);

} // namespace life
