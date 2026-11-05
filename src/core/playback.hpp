#pragma once

#include <chrono>
#include <cstddef>

namespace life {

class Playback {
public:
    using Clock = std::chrono::steady_clock;

    explicit Playback(std::size_t fps, Clock::time_point now = Clock::now());
    [[nodiscard]] bool playing() const noexcept;
    [[nodiscard]] std::size_t fps() const noexcept;
    void pause() noexcept;
    void toggle(Clock::time_point now = Clock::now());
    void increase_speed(Clock::time_point now = Clock::now());
    void decrease_speed(Clock::time_point now = Clock::now());
    [[nodiscard]] bool tick(Clock::time_point now = Clock::now());

private:
    void schedule_next_frame(Clock::time_point now);

    std::size_t fps_;
    bool playing_ = true;
    Clock::time_point next_frame_;
};

} // namespace life
