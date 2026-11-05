#include "simulation.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void check_navigation() {
    const life::World vertical({
        ".....",
        "..*..",
        "..*..",
        "..*..",
        ".....",
    });
    const life::World horizontal({
        ".....",
        ".....",
        ".***.",
        ".....",
        ".....",
    });
    life::Simulation simulation(vertical, 3);
    require(simulation.generation() == 0 && simulation.latest_generation() == 0 &&
            simulation.current_world() == vertical, "Incorrect initial state");
    require(!simulation.step_backward(), "Went before generation zero");
    require(simulation.step_forward() && simulation.generation() == 1 &&
            simulation.current_world() == horizontal, "Incorrect first generation");
    require(simulation.step_forward() && simulation.generation() == 2 &&
            simulation.current_world() == vertical, "Incorrect second generation");
    require(simulation.step_backward() && simulation.current_world() == horizontal,
            "Previous generation was not restored");
    require(simulation.step_backward() && simulation.current_world() == vertical,
            "Initial generation was not restored");
    require(simulation.can_step_forward() && simulation.latest_generation() == 2,
            "Going back discarded future history");
    require(simulation.step_forward() && simulation.current_world() == horizontal &&
            simulation.latest_generation() == 2, "Stored first generation was not reused");
    require(simulation.step_forward() && simulation.current_world() == vertical &&
            simulation.latest_generation() == 2, "Stored second generation was not reused");
    require(simulation.step_forward() && simulation.current_world() == horizontal &&
            simulation.latest_generation() == 3, "Could not extend history");
    require(!simulation.step_forward() && simulation.generation() == 3,
            "Went beyond the cycle limit");
    require(!simulation.can_step_forward(), "Allowed playback beyond the cycle limit");
}

void check_stopping() {
    life::Simulation zero_cycles(life::World({"*"}), 0);
    require(!zero_cycles.can_step_forward() && !zero_cycles.step_forward() &&
            zero_cycles.current_world().is_alive(0, 0),
            "Zero cycles changed the initial state");
    life::Simulation empty(life::World({"...", "..."}), 10);
    require(!empty.can_step_forward() && !empty.step_forward() && empty.generation() == 0,
            "An empty world continued evolving");
    life::Simulation extinction(life::World({"*"}), 10);
    require(extinction.step_forward() && !extinction.current_world().has_living_cells(),
            "Extinction generation was not saved");
    require(!extinction.step_forward(), "Continued after extinction");
    require(!extinction.can_step_forward(), "Allowed playback after extinction");
    require(extinction.step_backward() && extinction.current_world().is_alive(0, 0),
            "Could not rewind after extinction");
    require(extinction.step_forward() && extinction.latest_generation() == 1,
            "Extinction history was not reused");
    const life::World block({"**", "**"});
    life::Simulation stable(block, 3);
    for (int step = 0; step < 3; ++step) {
        require(stable.step_forward() && stable.current_world() == block,
                "A stable world stopped before the cycle limit");
    }
}

void check_custom_rules() {
    const life::World cross({".*.", "*.*", ".*."});
    const life::World filled({".*.", "***", ".*."});
    life::Simulation simulation(cross, 2, life::Rules("4", "23"));
    require(simulation.rules().notation() == "B4/S23", "Simulation lost its rules");
    require(simulation.step_forward() && simulation.current_world() == filled,
            "Simulation did not evolve with custom rules");
    require(simulation.step_backward() && simulation.current_world() == cross,
            "Custom rules broke rewind");
    require(simulation.step_forward() && simulation.current_world() == filled &&
            simulation.latest_generation() == 1, "Custom history was recomputed");

    life::Simulation rebirth(life::World({"."}), 2, life::Rules("0", "none"));
    require(rebirth.can_step_forward() && rebirth.step_forward() &&
            rebirth.current_world().is_alive(0, 0), "B0 stopped at an empty generation");
    require(rebirth.step_forward() && !rebirth.current_world().has_living_cells() &&
            !rebirth.can_step_forward(), "B0 ignored the cycle limit");
    require(rebirth.step_backward() && rebirth.current_world().is_alive(0, 0),
            "B0 history was lost");
}

int main() {
    try {
        check_navigation();
        check_stopping();
        check_custom_rules();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
