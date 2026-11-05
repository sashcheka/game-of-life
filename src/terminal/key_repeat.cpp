#include "key_repeat.hpp"

namespace life {

std::size_t KeyRepeat::steps(Direction direction, Clock::time_point now) {
    constexpr auto repeat_gap = std::chrono::milliseconds(250);
    if (!active_ || direction != direction_ || now - last_press_ > repeat_gap) {
        active_ = true;
        direction_ = direction;
        first_press_ = now;
    }
    last_press_ = now;
    const auto held = now - first_press_;
    if (held >= std::chrono::seconds(2)) {
        return 20;
    }
    if (held >= std::chrono::seconds(1)) {
        return 5;
    }
    return 1;
}

void KeyRepeat::reset() noexcept {
    active_ = false;
}

} // namespace life
