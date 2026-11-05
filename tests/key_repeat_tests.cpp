#include "key_repeat.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <stdexcept>

using namespace std::chrono_literals;
using Direction = life::KeyRepeat::Direction;

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void check_acceleration() {
    const life::KeyRepeat::Clock::time_point start{};
    life::KeyRepeat repeat;
    for (int elapsed = 0; elapsed < 1000; elapsed += 100) {
        require(repeat.steps(Direction::Forward, start + std::chrono::milliseconds(elapsed)) == 1,
                "A short press accelerated too early");
    }
    for (int elapsed = 1000; elapsed < 2000; elapsed += 100) {
        require(repeat.steps(Direction::Forward, start + std::chrono::milliseconds(elapsed)) == 5,
                "A held arrow did not reach medium speed");
    }
    require(repeat.steps(Direction::Forward, start + 2s) == 20,
            "A long hold did not reach maximum speed");
    require(repeat.steps(Direction::Forward, start + 2100ms) == 20,
            "Maximum speed was not retained");
    repeat.reset();
    require(repeat.steps(Direction::Forward, start + 2200ms) == 1,
            "Another key did not reset acceleration");
}

void check_direction_and_gaps() {
    const life::KeyRepeat::Clock::time_point start{};
    life::KeyRepeat repeat;
    std::size_t steps = 0;
    for (int elapsed = 0; elapsed <= 1000; elapsed += 250) {
        steps = repeat.steps(Direction::Backward, start + std::chrono::milliseconds(elapsed));
    }
    require(steps == 5, "Regular repeats at the gap boundary did not accelerate");
    require(repeat.steps(Direction::Forward, start + 1100ms) == 1,
            "Changing direction retained acceleration");
    for (int elapsed = 1200; elapsed <= 2100; elapsed += 100) {
        steps = repeat.steps(Direction::Forward, start + std::chrono::milliseconds(elapsed));
    }
    require(steps == 5, "The new direction did not accelerate");
    require(repeat.steps(Direction::Forward, start + 2351ms) == 1,
            "A released arrow retained acceleration");
}

int main() {
    try {
        check_acceleration();
        check_direction_and_gaps();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
