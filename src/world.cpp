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

} // namespace life
