#include "json_storage.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

life::GameConfig parse(const std::string& text) {
    std::istringstream input(text);
    return life::read_config(input);
}

std::string replace(std::string text, const std::string& from, const std::string& to) {
    const auto position = text.find(from);
    if (position == std::string::npos) {
        throw std::logic_error("Invalid test replacement: " + from);
    }
    text.replace(position, from.size(), to);
    return text;
}

bool rejects(const std::string& text) {
    try {
        (void)parse(text);
    } catch (const std::invalid_argument&) {
        return true;
    }
    std::cerr << "Invalid configuration was accepted: " << text << '\n';
    return false;
}

int main() {
    const std::string valid = R"({
        "world":{"width":3,"height":2,"cells":["*..","..*"]},
        "simulation":{"fps":30,"cycles":0}
    })";
    const life::World expected({"*..", "..*"});
    const auto config = parse(valid);
    if (config.world != expected || config.fps != 30 || config.cycles != 0) {
        std::cerr << "Incorrect world or simulation parameters\n";
        return 1;
    }
    const auto escaped = parse(replace(valid, "*..", R"(\u002a..)"));
    const auto positive_cycles = parse(replace(valid, "\"cycles\":0", "\"cycles\":10"));
    if (escaped.world != expected || positive_cycles.cycles != 10) {
        std::cerr << "Escaped cells or positive cycles were parsed incorrectly\n";
        return 1;
    }
    const auto maximum = std::numeric_limits<std::size_t>::max();
    const auto large = parse(replace(valid, "\"cycles\":0",
                                     "\"cycles\":" + std::to_string(maximum)));
    if (large.cycles != maximum) {
        std::cerr << "A large cycle count lost precision\n";
        return 1;
    }
    const std::vector<std::pair<std::string, std::string>> invalid_values{
        {"\"width\":3", "\"width\":0"},
        {"\"width\":3", "\"width\":-1"},
        {"\"width\":3", "\"width\":3.5"},
        {"\"width\":3", "\"width\":true"},
        {"\"width\":3", "\"width\":null"},
        {"\"width\":3", "\"width\":\"3\""},
        {"\"width\":3", "\"width\":4"},
        {"\"height\":2", "\"height\":0"},
        {"\"height\":2", "\"height\":3"},
        {"\"fps\":30", "\"fps\":0"},
        {"\"cycles\":0", "\"cycles\":-1"},
        {"\"cycles\":0", "\"cycles\":1.5"},
        {"\"cycles\":0", "\"cycles\":18446744073709551616"},
        {"\"world\"", "\"missing_world\""},
        {"\"simulation\"", "\"missing_simulation\""},
        {"\"fps\"", "\"missing_fps\""},
        {"\"cells\"", "\"missing_cells\""},
        {"[\"*..\",\"..*\"]", "[]"},
        {"[\"*..\",\"..*\"]", "\"*..\""},
        {"[\"*..\",\"..*\"]", "[123,456]"},
        {"*..", "x.."},
        {"..*", ".*"},
    };
    for (const auto& [from, to] : invalid_values) {
        if (!rejects(replace(valid, from, to))) {
            return 1;
        }
    }
    if (!rejects("") || !rejects("{") || !rejects("[]") ||
        !rejects(valid + "{}") || !rejects(valid + "garbage")) {
        return 1;
    }
    return 0;
}
