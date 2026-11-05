#include "wave.hpp"

#include <cmath>
#include <string>
#include <vector>

namespace life {

World wave_frame(const World& world, std::size_t generation) {
    constexpr double generations_per_cell = 4.0;
    const double radius = static_cast<double>(generation) / generations_per_cell;
    const double center_x = static_cast<double>(world.width() / 2);
    const double center_y = static_cast<double>(world.height() / 2);
    std::vector<std::string> cells(world.height(), std::string(world.width(), '.'));
    for (std::size_t y = 0; y < world.height(); ++y) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            const double distance = std::hypot(static_cast<double>(x) - center_x,
                                                static_cast<double>(y) - center_y);
            if (std::abs(distance - radius) <= 0.5) {
                cells[y][x] = '*';
            }
        }
    }
    return World(cells);
}

} // namespace life
