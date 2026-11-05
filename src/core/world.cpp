#include "world.hpp"

#include <algorithm>
#include <stdexcept>

namespace life {

World::World(const std::vector<std::string>& cells) {
    if (cells.empty() || cells.front().empty()) {
        throw std::invalid_argument("World dimensions must be positive");
    }
    width_ = cells.front().size();
    height_ = cells.size();
    for (const auto& row : cells) {
        if (row.size() != width()) {
            throw std::invalid_argument("World rows must have equal lengths");
        }
        if (row.find_first_not_of(".*") != std::string::npos) {
            throw std::invalid_argument("World cells must be '.' or '*'");
        }
    }
    if (height_ > cells_.max_size() / width_) {
        throw std::length_error("World dimensions exceed supported storage size");
    }
    cells_.resize(width_ * height_);
    for (std::size_t y = 0; y < height_; ++y) {
        for (std::size_t x = 0; x < width_; ++x) {
            cells_[y * width_ + x] = cells[y][x] == '*';
        }
    }
}

std::size_t World::width() const noexcept {
    return width_;
}

std::size_t World::height() const noexcept {
    return height_;
}

bool World::is_alive(std::size_t x, std::size_t y) const {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Cell coordinates are outside the world");
    }
    return cells_.at(y * width_ + x);
}

bool World::has_living_cells() const noexcept {
    return std::find(cells_.begin(), cells_.end(), true) != cells_.end();
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
    World next = *this;

    for (std::size_t y = 0; y < height(); ++y) {
        for (std::size_t x = 0; x < width(); ++x) {
            const auto neighbors = count_live_neighbors(x, y);
            next.cells_[y * width_ + x] = neighbors == 3 || (neighbors == 2 && is_alive(x, y));
        }
    }
    return next;
}

} // namespace life
