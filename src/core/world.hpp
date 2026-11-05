#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace life {

class World {
public:
    explicit World(const std::vector<std::string>& cells);

    [[nodiscard]] std::size_t width() const noexcept;
    [[nodiscard]] std::size_t height() const noexcept;
    [[nodiscard]] bool is_alive(std::size_t x, std::size_t y) const;
    [[nodiscard]] bool has_living_cells() const noexcept;
    [[nodiscard]] std::size_t count_live_neighbors(std::size_t x, std::size_t y) const;
    [[nodiscard]] World next_generation() const;

    [[nodiscard]] bool operator==(const World&) const = default;

private:
    std::size_t width_ = 0;
    std::size_t height_ = 0;
    std::vector<bool> cells_;
};

} // namespace life
