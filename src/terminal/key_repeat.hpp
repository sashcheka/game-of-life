#pragma once

#include <chrono>
#include <cstddef>

namespace life {

class KeyRepeat {
public:
    using Clock = std::chrono::steady_clock;
    enum class Direction { Backward, Forward };

    [[nodiscard]] std::size_t steps(Direction direction, Clock::time_point now = Clock::now());
    void reset() noexcept;

private:
    bool active_ = false;
    Direction direction_ = Direction::Backward;
    Clock::time_point first_press_{};
    Clock::time_point last_press_{};
};

} // namespace life
