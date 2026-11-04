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

bool evolves_to(const char* name, std::vector<std::string> cells,
                std::vector<std::string> expected_cells) {
    const life::World world(std::move(cells));
    const auto original = world;
    const life::World expected(std::move(expected_cells));
    const auto next = world.next_generation();
    if (next != expected) {
        std::cerr << name << ": incorrect next generation\n";
        return false;
    }
    if (world != original) {
        std::cerr << name << ": the original world was modified\n";
        return false;
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
    if (!world.has_living_cells() || dead_world.has_living_cells()) {
        std::cerr << "Incorrect detection of living cells\n";
        return 1;
    }
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
    const std::vector<std::string> vertical{".....", "..*..", "..*..", "..*..", "....."};
    const std::vector<std::string> horizontal{".....", ".....", ".***.", ".....", "....."};
    const std::vector<std::string> block{"....", ".**.", ".**.", "...."};
    if (!evolves_to("Blinker first step", vertical, horizontal) ||
        !evolves_to("Blinker second step", horizontal, vertical) ||
        !evolves_to("Stable block", block, block) ||
        !evolves_to("Loneliness", {"*"}, {"."}) ||
        !evolves_to("Underpopulation", {"**"}, {".."}) ||
        !evolves_to("Empty world", {"...", "..."}, {"...", "..."}) ||
        !evolves_to("Overpopulation", {"***", "***", "***"}, {"*.*", "...", "*.*"}) ||
        !evolves_to("Single row", {"***"}, {".*."}) ||
        !evolves_to("Corner birth", {"**", "*."}, {"**", "**"})) {
        return 1;
    }
    const life::World blinker(vertical);
    if (blinker.next_generation().next_generation() != blinker) {
        std::cerr << "Blinker did not return to its original state after two steps\n";
        return 1;
    }
    return 0;
}
