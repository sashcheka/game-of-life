#include "rules.hpp"

#include <stdexcept>

namespace life {
namespace {

std::array<bool, 9> parse_counts(std::string_view text) {
    std::array<bool, 9> counts{};
    if (text == "none") {
        return counts;
    }
    for (const char digit : text) {
        if (digit < '0' || digit > '8') {
            throw std::invalid_argument("Neighbor counts must be digits 0-8, or 'none'");
        }
        counts[static_cast<std::size_t>(digit - '0')] = true;
    }
    return counts;
}

} // namespace

Rules::Rules(std::string_view birth, std::string_view survival)
    : birth_(parse_counts(birth)), survival_(parse_counts(survival)) {}

bool Rules::next_state(bool alive, std::size_t neighbors) const {
    return (alive ? survival_ : birth_).at(neighbors);
}

std::string Rules::notation() const {
    std::string result = "B";
    for (std::size_t count = 0; count < birth_.size(); ++count) {
        if (birth_[count]) {
            result += static_cast<char>('0' + count);
        }
    }
    result += "/S";
    for (std::size_t count = 0; count < survival_.size(); ++count) {
        if (survival_[count]) {
            result += static_cast<char>('0' + count);
        }
    }
    return result;
}

} // namespace life
