#include "game_view.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>

namespace life {

ftxui::Component make_game_view(const Simulation& simulation, const Playback& playback,
                                std::size_t cycles, CellStyle style) {
    return ftxui::Renderer([&simulation, &playback, cycles, style] {
        const auto size = ftxui::Terminal::Size();
        const auto columns = static_cast<std::size_t>(std::max(0, size.dimx - 2));
        const auto rows = static_cast<std::size_t>(std::max(0, size.dimy - 6));
        ftxui::Elements content{
            ftxui::text("Generation " + std::to_string(simulation.generation()) +
                        " / " + std::to_string(cycles) + " | Computed: " +
                        std::to_string(simulation.latest_generation())),
            ftxui::text(std::string(playback.playing() ? "Playing" : "Paused") +
                        " | " + std::to_string(playback.fps()) + " fps"),
            ftxui::text("Space: play/pause | Left/Right: step | Up/Down: speed | Q: save and exit"),
            ftxui::separator(),
        };
        for (const auto& row : format_cells(simulation.current_world(), style, columns, rows)) {
            content.push_back(ftxui::text(row));
        }
        return ftxui::vbox(std::move(content)) | ftxui::border;
    });
}

} // namespace life
