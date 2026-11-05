#include "game_view.hpp"

#include <string>
#include <utility>

namespace life {

ftxui::Element make_game_board(const World& world, CellStyle style) {
    ftxui::Elements rows;
    for (const auto& row : format_cells(world, style, world.width(), world.height())) {
        rows.push_back(ftxui::text(row));
    }
    auto board = ftxui::vbox(std::move(rows));
    if (style == CellStyle::Ascii) {
        ftxui::Cell edge;
        edge.character = "#";
        board = board | ftxui::borderWith(edge);
    } else {
        board = board | ftxui::borderLight;
    }
    auto aligned = ftxui::vbox({ftxui::hbox({board, ftxui::filler()}), ftxui::filler()});
    return aligned | ftxui::frame | ftxui::flex;
}

ftxui::Component make_game_view(const Simulation& simulation, const Playback& playback,
                                std::size_t cycles, CellStyle style) {
    return ftxui::Renderer([&simulation, &playback, cycles, style] {
        const bool ascii = style == CellStyle::Ascii;
        ftxui::Elements content{
            ftxui::text("Generation " + std::to_string(simulation.generation()) +
                        " / " + std::to_string(cycles) + " | Computed: " +
                        std::to_string(simulation.latest_generation()) + " | " +
                        simulation.rules().notation()),
            ftxui::text(std::string(playback.playing() ? "Playing" : "Paused") +
                        " | " + std::to_string(playback.fps()) + " fps"),
            ftxui::text("Space: play/pause | Left/Right: step/hold | Up/Down: speed | Q: save/exit"),
            ascii ? ftxui::separatorCharacter("-") : ftxui::separator(),
        };
        content.push_back(make_game_board(simulation.current_world(), style));
        return ftxui::vbox(std::move(content));
    });
}

} // namespace life
