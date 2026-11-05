#include "world.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

int main() {
    try {
        require(life::Rules{}.notation() == "B3/S23", "Incorrect default rules");
        require(life::Rules("63", "32").notation() == "B36/S23",
                "Rule notation is not canonical");
        require(life::Rules("2", "none").notation() == "B2/S", "Empty survival was lost");
        const std::vector<std::pair<std::string, std::string>> cases{
            {"3", "23"}, {"36", "23"}, {"4", "34"}, {"4", "23"},
            {"2", "none"}, {"3678", "34678"}, {"0", "none"},
            {"none", "none"}, {"012345678", "012345678"},
        };
        for (const auto& [birth, survival] : cases) {
            const life::Rules rules(birth, survival);
            for (std::size_t neighbors = 0; neighbors <= 8; ++neighbors) {
                for (const bool alive : {false, true}) {
                    std::vector<std::string> cells(3, "...");
                    cells[1][1] = alive ? '*' : '.';
                    std::size_t placed = 0;
                    for (std::size_t y = 0; y < 3; ++y) {
                        for (std::size_t x = 0; x < 3; ++x) {
                            if ((x != 1 || y != 1) && placed < neighbors) {
                                cells[y][x] = '*';
                                ++placed;
                            }
                        }
                    }
                    const auto& counts = alive ? survival : birth;
                    const bool expected = counts.find(static_cast<char>('0' + neighbors)) !=
                                          std::string::npos;
                    const life::World world(cells);
                    require(world.next_generation(rules).is_alive(1, 1) == expected,
                            "Evolution did not use the selected birth or survival rule");
                }
            }
        }
        for (const auto* invalid : {"9", "-1", "3,6", "x", "3.0", "none3"}) {
            for (const bool birth : {false, true}) {
                bool rejected = false;
                try {
                    const life::Rules rules(birth ? invalid : "3", birth ? "23" : invalid);
                } catch (const std::invalid_argument&) {
                    rejected = true;
                }
                require(rejected, "Invalid neighbor counts were accepted");
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
