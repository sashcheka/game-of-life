#include "simulation.hpp"
#include "wave.hpp"

#include <utility>

namespace life {

Simulation::Simulation(World initial, std::size_t cycle_limit, Rules rules, SimulationMode mode)
    : cycle_limit_(cycle_limit), rules_(rules), mode_(mode) {
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

const Rules& Simulation::rules() const noexcept {
    return rules_;
}

SimulationMode Simulation::mode() const noexcept {
    return mode_;
}

bool Simulation::can_step_forward() const noexcept {
    return generation_ < latest_generation() ||
           (generation_ < cycle_limit_ &&
            (current_world().has_living_cells() ||
             (mode_ == SimulationMode::Life && rules_.next_state(false, 0))));
}

bool Simulation::step_forward() {
    if (!can_step_forward()) {
        return false;
    }
    if (generation_ == latest_generation()) {
        history_.push_back(mode_ == SimulationMode::Wave
                               ? wave_frame(current_world(), generation_ + 1)
                               : current_world().next_generation(rules_));
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
