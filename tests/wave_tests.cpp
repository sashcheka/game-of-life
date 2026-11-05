#include "wave.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

int main() {
    try {
        const life::World field({".....", ".....", ".....", ".....", "....."});
        const life::World seed({".....", ".....", "..*..", ".....", "....."});
        require(life::wave_frame(field, 0) == seed, "Wave did not start at the center");
        const life::World first_ring({
            ".....",
            ".***.",
            ".*.*.",
            ".***.",
            ".....",
        });
        require(life::wave_frame(field, 4) == first_ring, "The first ring is incorrect");
        const life::World outer_ring({
            ".***.",
            "*...*",
            "*...*",
            "*...*",
            ".***.",
        });
        require(life::wave_frame(field, 8) == outer_ring, "The ring left an interior trail");
        require(!life::wave_frame(field, 20).has_living_cells(),
                "The wave did not disappear beyond the field");
        const life::World rectangle({"........", "........", "........", "........"});
        require(life::wave_frame(rectangle, 0).is_alive(4, 2),
                "An even rectangular field lost its center");
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
