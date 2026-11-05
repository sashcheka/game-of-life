#include "playback.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace life {

Playback::Playback(std::size_t fps, Clock::time_point now) : fps_(fps) {
    if (fps_ == 0) {
        throw std::invalid_argument("Playback FPS must be positive");
    }
    reset_timer(now);
}

bool Playback::playing() const noexcept {
    return playing_;
}

std::size_t Playback::fps() const noexcept {
    return fps_;
}

void Playback::pause() noexcept {
    playing_ = false;
}

void Playback::toggle(Clock::time_point now) {
    playing_ = !playing_;
    reset_timer(now);
}

void Playback::increase_speed(Clock::time_point now) {
    if (fps_ < std::numeric_limits<std::size_t>::max()) {
        ++fps_;
        reset_timer(now);
    }
}

void Playback::decrease_speed(Clock::time_point now) {
    if (fps_ > 1) {
        --fps_;
        reset_timer(now);
    }
}

bool Playback::tick(Clock::time_point now) {
    if (!playing_ || now < next_frame_) {
        return false;
    }
    reset_timer(now);
    return true;
}

void Playback::reset_timer(Clock::time_point now) {
    const auto period = std::chrono::duration<double>(1.0 / static_cast<double>(fps_));
    const auto delay = std::chrono::duration_cast<Clock::duration>(period);
    next_frame_ = now + std::max(delay, Clock::duration{1});
}

} // namespace life
