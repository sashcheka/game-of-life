#include "playback.hpp"

#include <limits>
#include <stdexcept>

namespace life {

Playback::Playback(std::size_t fps, Clock::time_point now) : fps_(fps) {
    if (fps_ == 0) {
        throw std::invalid_argument("Playback FPS must be positive");
    }
    schedule_next_frame(now);
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
    schedule_next_frame(now);
}

void Playback::increase_speed(Clock::time_point now) {
    if (fps_ < std::numeric_limits<std::size_t>::max()) {
        ++fps_;
        schedule_next_frame(now);
    }
}

void Playback::decrease_speed(Clock::time_point now) {
    if (fps_ > 1) {
        --fps_;
        schedule_next_frame(now);
    }
}

bool Playback::tick(Clock::time_point now) {
    if (!playing_ || now < next_frame_) {
        return false;
    }
    schedule_next_frame(now);
    return true;
}

void Playback::schedule_next_frame(Clock::time_point now) {
    const auto period = std::chrono::duration<double>(1.0 / static_cast<double>(fps_));
    next_frame_ = now + std::chrono::duration_cast<Clock::duration>(period);
}

} // namespace life
