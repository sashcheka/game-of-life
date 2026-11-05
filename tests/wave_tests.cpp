#include "wave.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

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
                "The first wave did not leave the small field");
        require(!life::wave_frame(field, 31).has_living_cells(),
                "A new ring appeared before its spawn time");
        require(life::wave_frame(field, 32) == seed, "The source did not emit a second ring");
        const life::World line({std::string(35, '.')});
        const auto rings = life::wave_frame(line, 64);
        for (std::size_t x = 0; x < line.width(); ++x) {
            require(rings.is_alive(x, 0) == (x == 1 || x == 9 || x == 17 || x == 25 || x == 33),
                    "New rings replaced older rings or changed their spacing");
        }
        const life::World rectangle({"........", "........", "........", "........"});
        require(life::wave_frame(rectangle, 0).is_alive(4, 2),
                "An even rectangular field lost its center");
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
