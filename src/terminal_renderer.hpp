#pragma once

#include "world.hpp"

#include <cstddef>

namespace life {

class TerminalRenderer {
public:
    explicit TerminalRenderer(bool requested = true);
    ~TerminalRenderer();
    TerminalRenderer(const TerminalRenderer&) = delete;
    TerminalRenderer& operator=(const TerminalRenderer&) = delete;

    [[nodiscard]] bool active() const noexcept;
    void draw(const World& world, std::size_t generation, std::size_t limit,
              std::size_t fps) const;

private:
    bool active_ = false;
#ifdef _WIN32
    void* output_handle_ = nullptr;
    unsigned long original_mode_ = 0;
#endif
};

} // namespace life
