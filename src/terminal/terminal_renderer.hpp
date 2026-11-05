#pragma once

#include "world.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace life {

enum class CellStyle { Ascii, HalfBlocks };

[[nodiscard]] std::vector<std::string> format_cells(const World& world, CellStyle style,
                                                  std::size_t columns, std::size_t rows);

class TerminalRenderer {
public:
    explicit TerminalRenderer(bool requested = true, CellStyle style = CellStyle::HalfBlocks);
    ~TerminalRenderer();
    TerminalRenderer(const TerminalRenderer&) = delete;
    TerminalRenderer& operator=(const TerminalRenderer&) = delete;

    [[nodiscard]] bool active() const noexcept;
    [[nodiscard]] CellStyle style() const noexcept;
    void start();
    void finish();

private:
    bool active_ = false;
    bool screen_active_ = false;
    CellStyle style_;
#ifdef _WIN32
    void* output_handle_ = nullptr;
    unsigned long original_mode_ = 0;
    unsigned int original_code_page_ = 0;
#endif
};

} // namespace life
