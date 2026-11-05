#pragma once

#include "world.hpp"

#include <cstddef>
#include <vector>

namespace life {

enum class SimulationMode { Life, Wave };

class Simulation {
public:
    Simulation(World initial, std::size_t cycle_limit, Rules rules = Rules{},
               SimulationMode mode = SimulationMode::Life);

    [[nodiscard]] const World& current_world() const noexcept;
    [[nodiscard]] std::size_t generation() const noexcept;
    [[nodiscard]] std::size_t latest_generation() const noexcept;
    [[nodiscard]] const Rules& rules() const noexcept;
    [[nodiscard]] SimulationMode mode() const noexcept;
    [[nodiscard]] bool can_step_forward() const noexcept;
    [[nodiscard]] bool step_forward();
    [[nodiscard]] bool step_backward() noexcept;

private:
    std::vector<World> history_;
    std::size_t generation_ = 0;
    std::size_t cycle_limit_;
    Rules rules_;
    SimulationMode mode_;
};

} // namespace life
