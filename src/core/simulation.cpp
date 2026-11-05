#include "simulation.hpp"

#include <utility>

namespace life {

Simulation::Simulation(World initial, std::size_t cycle_limit) : cycle_limit_(cycle_limit) {
    history_.push_back(std::move(initial));
}

const World& Simulation::current_world() const noexcept {
    return history_[generation_];
}

std::size_t Simulation::generation() const noexcept {
    return generation_;
}

std::size_t Simulation::latest_generation() const noexcept {
    return history_.size() - 1;
}

bool Simulation::can_step_forward() const noexcept {
    return generation_ < latest_generation() ||
           (generation_ < cycle_limit_ && current_world().has_living_cells());
}

bool Simulation::step_forward() {
    if (!can_step_forward()) {
        return false;
    }
    if (generation_ == latest_generation()) {
        history_.push_back(current_world().next_generation());
    }
    ++generation_;
    return true;
}

bool Simulation::step_backward() noexcept {
    if (generation_ == 0) {
        return false;
    }
    --generation_;
    return true;
}

} // namespace life
