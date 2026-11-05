#include "playback.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std::chrono_literals;

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void check_timing() {
    const life::Playback::Clock::time_point start{};
    life::Playback playback(2, start);
    require(playback.playing() && playback.fps() == 2, "Incorrect initial playback");
    require(!playback.tick(start + 499ms), "Advanced before the frame deadline");
    require(playback.tick(start + 500ms), "Missed the frame deadline");
    require(!playback.tick(start + 500ms), "Advanced twice at the same deadline");
    playback.pause();
    require(!playback.playing() && !playback.tick(start + 10s), "Pause did not stop time");
    playback.toggle(start + 10s);
    require(playback.playing() && !playback.tick(start + 10s), "Resume used an old deadline");
    require(!playback.tick(start + 10499ms) && playback.tick(start + 10500ms),
            "Resume did not wait for a full frame");
    require(playback.tick(start + 20s) && !playback.tick(start + 20s),
            "A delayed frame caused a catch-up burst");
    playback.toggle(start + 20s);
    require(!playback.playing(), "Space-style toggle did not pause");
}

void check_speed() {
    const life::Playback::Clock::time_point start{};
    life::Playback playback(1, start);
    playback.increase_speed(start);
    require(playback.fps() == 2 && !playback.tick(start + 499ms) &&
            playback.tick(start + 500ms), "Increasing speed did not change the deadline");
    playback.decrease_speed(start + 1s);
    require(playback.fps() == 1 && !playback.tick(start + 1999ms) &&
            playback.tick(start + 2s), "Decreasing speed did not change the deadline");
    playback.decrease_speed(start + 2500ms);
    require(playback.fps() == 1 && playback.tick(start + 3s),
            "Minimum speed changed or postponed the next frame");
    playback.pause();
    playback.increase_speed(start + 3s);
    require(!playback.playing() && playback.fps() == 2, "Speed change resumed playback");
    const auto maximum = std::numeric_limits<std::size_t>::max();
    life::Playback fastest(maximum, start);
    fastest.increase_speed(start);
    require(fastest.fps() == maximum, "Speed overflowed");
}

int main() {
    try {
        check_timing();
        check_speed();
        try {
            const life::Playback invalid(0);
            throw std::runtime_error("Zero FPS was accepted");
        } catch (const std::invalid_argument&) {
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
