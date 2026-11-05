#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

namespace life {

class Rules {
public:
    explicit Rules(std::string_view birth = "3", std::string_view survival = "23");

    [[nodiscard]] bool next_state(bool alive, std::size_t neighbors) const;
    [[nodiscard]] std::string notation() const;

private:
    std::array<bool, 9> birth_;
    std::array<bool, 9> survival_;
};

} // namespace life
