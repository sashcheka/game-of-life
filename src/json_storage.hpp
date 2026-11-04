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
void write_result(std::ostream& output, const World& world);
void save_result(const std::filesystem::path& path, const World& world);

} // namespace life
