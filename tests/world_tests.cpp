#include "world.hpp"

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
    return 0;
}
