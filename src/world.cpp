#include "world.hpp"

#include <stdexcept>
#include <utility>

namespace life {

World::World(std::vector<std::string> cells) : cells_(std::move(cells)) {
    if (cells_.empty() || cells_.front().empty()) {
        throw std::invalid_argument("World dimensions must be positive");
    }
    for (const auto& row : cells_) {
        if (row.size() != width()) {
            throw std::invalid_argument("World rows must have equal lengths");
        }
        if (row.find_first_not_of(".*") != std::string::npos) {
            throw std::invalid_argument("World cells must be '.' or '*'");
        }
    }
}

std::size_t World::width() const noexcept {
    return cells_.front().size();
}

std::size_t World::height() const noexcept {
    return cells_.size();
}

bool World::is_alive(std::size_t x, std::size_t y) const {
    return cells_.at(y).at(x) == '*';
}

bool World::has_living_cells() const noexcept {
    for (const auto& row : cells_) {
        if (row.find('*') != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::size_t World::count_live_neighbors(std::size_t x, std::size_t y) const {
    if (x >= width() || y >= height()) {
        throw std::out_of_range("Cell coordinates are outside the world");
    }

    const auto first_x = x == 0 ? 0 : x - 1;
    const auto last_x = x == width() - 1 ? x : x + 1;
    const auto first_y = y == 0 ? 0 : y - 1;
    const auto last_y = y == height() - 1 ? y : y + 1;

    std::size_t count = 0;
    for (std::size_t neighbor_y = first_y; neighbor_y <= last_y; ++neighbor_y) {
        for (std::size_t neighbor_x = first_x; neighbor_x <= last_x; ++neighbor_x) {
            if ((neighbor_x != x || neighbor_y != y) && is_alive(neighbor_x, neighbor_y)) {
                ++count;
            }
        }
    }
    return count;
}

World World::next_generation() const {
    std::vector<std::string> next_cells(height(), std::string(width(), '.'));

    for (std::size_t y = 0; y < height(); ++y) {
        for (std::size_t x = 0; x < width(); ++x) {
            const auto neighbors = count_live_neighbors(x, y);
            if (neighbors == 3 || (neighbors == 2 && is_alive(x, y))) {
                next_cells[y][x] = '*';
            }
        }
    }
    return World(std::move(next_cells));
}

} // namespace life
