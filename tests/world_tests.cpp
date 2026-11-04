#include "world.hpp"

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

bool rejects(std::vector<std::string> cells) {
    try {
        const life::World world(std::move(cells));
    } catch (const std::invalid_argument&) {
        return true;
    }
    return false;
}

bool has_neighbor_counts(std::vector<std::string> cells,
                         const std::vector<std::vector<std::size_t>>& expected) {
    const life::World world(std::move(cells));
    for (std::size_t y = 0; y < world.height(); ++y) {
        for (std::size_t x = 0; x < world.width(); ++x) {
            const auto actual = world.count_live_neighbors(x, y);
            if (actual != expected.at(y).at(x)) {
                std::cerr << "Neighbor count at (" << x << ", " << y << "): expected "
                          << expected.at(y).at(x) << ", got " << actual << '\n';
                return false;
            }
        }
    }
    return true;
}

int main() {
    const life::World world({"*..", "..*"});
    if (world.width() != 3 || world.height() != 2 ||
        !world.is_alive(0, 0) || !world.is_alive(2, 1) || world.is_alive(1, 0)) {
        std::cerr << "Rectangular world or cell coordinates are incorrect\n";
        return 1;
    }
    const life::World dead_world({"."});
    if (dead_world.is_alive(0, 0)) {
        std::cerr << "A valid world can contain no living cells\n";
        return 1;
    }
    if (!rejects({}) || !rejects({""}) || !rejects({"..", "."}) ||
        !rejects({".x"})) {
        std::cerr << "Invalid world data was accepted\n";
        return 1;
    }
    try {
        (void)world.is_alive(world.width(), 0);
        std::cerr << "An invalid column was accepted\n";
        return 1;
    } catch (const std::out_of_range&) {
    }
    try {
        (void)world.is_alive(0, world.height());
        std::cerr << "An invalid row was accepted\n";
        return 1;
    } catch (const std::out_of_range&) {
    }
    if (!has_neighbor_counts({"***", "***", "***"}, {{3, 5, 3}, {5, 8, 5}, {3, 5, 3}}) ||
        !has_neighbor_counts({"*..", "..*"}, {{0, 2, 1}, {1, 2, 0}}) ||
        !has_neighbor_counts({"*.*"}, {{0, 2, 0}}) ||
        !has_neighbor_counts({"*", ".", "*"}, {{0}, {2}, {0}}) ||
        !has_neighbor_counts({"*"}, {{0}}) ||
        !has_neighbor_counts({"."}, {{0}}) ||
        !has_neighbor_counts({"..", ".."}, {{0, 0}, {0, 0}})) {
        return 1;
    }
    try {
        (void)world.count_live_neighbors(world.width(), 0);
        std::cerr << "Neighbor counting accepted an invalid column\n";
        return 1;
    } catch (const std::out_of_range&) {
    }
    try {
        (void)world.count_live_neighbors(0, world.height());
        std::cerr << "Neighbor counting accepted an invalid row\n";
        return 1;
    } catch (const std::out_of_range&) {
    }
    return 0;
}
