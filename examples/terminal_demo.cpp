#include "json_storage.hpp"
#include "terminal_renderer.hpp"

#include <ftxui.hpp>

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <utility>

int main() {
    try {
        const auto config = life::load_config("game_of_life_input.json");
        auto app = ftxui::App::Fullscreen();
        app.TrackMouse(false);
        app.ForceHandleCtrlC(false);

        auto view = ftxui::Renderer([&] {
            const auto size = ftxui::Terminal::Size();
            const auto columns = static_cast<std::size_t>(std::max(0, size.dimx - 2));
            const auto rows = static_cast<std::size_t>(std::max(0, size.dimy - 4));
            ftxui::Elements content{
                ftxui::text("FTXUI prototype | Q to exit"),
                ftxui::separator(),
            };
            for (const auto& row : life::format_cells(config.world, life::CellStyle::HalfBlocks,
                                                     columns, rows)) {
                content.push_back(ftxui::text(row));
            }
            return ftxui::vbox(std::move(content)) | ftxui::border;
        });

        auto controls = ftxui::CatchEvent(view, [&](ftxui::Event event) {
            if (event == ftxui::Event::Character('q') ||
                event == ftxui::Event::Character('Q') || event == ftxui::Event::CtrlC) {
                app.Exit();
                return true;
            }
            return false;
        });
        app.Loop(controls);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
