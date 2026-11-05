#include "wave.hpp"

#include <cmath>
#include <string>
#include <vector>

namespace life {

World wave_frame(const World& world, std::size_t generation) {
    constexpr double generations_per_cell = 4.0;
    constexpr std::size_t spawn_interval = 32;
    constexpr double ring_spacing = spawn_interval / generations_per_cell;
    const double inner_radius = static_cast<double>(generation % spawn_interval) /
                                generations_per_cell;
    const double oldest_ring_index = static_cast<double>(generation / spawn_interval);
    const double center_x = static_cast<double>(world.width() / 2);
    const double center_y = static_cast<double>(world.height() / 2);
    std::vector<std::string> cells(world.height(), std::string(world.width(), '.'));
    for (std::size_t y = 0; y < world.height(); ++y) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            const double distance = std::hypot(static_cast<double>(x) - center_x,
                                                static_cast<double>(y) - center_y);
            const double ring_index = std::round((distance - inner_radius) / ring_spacing);
            const double radius = inner_radius + ring_index * ring_spacing;
            if (ring_index >= 0 && ring_index <= oldest_ring_index &&
                std::abs(distance - radius) <= 0.5) {
                cells[y][x] = '*';
            }
        }
    }
    return World(cells);
}

} // namespace life
