#include "json_storage.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace life {
namespace {

std::size_t read_integer(const nlohmann::json& object, const char* key,
                         std::size_t minimum) {
    const auto& value = object.at(key);
    const auto message = std::string(key) + " must be an integer >= " +
                         std::to_string(minimum) + " within the supported range";
    if (!value.is_number_integer() ||
        (!value.is_number_unsigned() && value.get<std::int64_t>() < 0)) {
        throw std::invalid_argument(message);
    }
    const auto number = value.get<std::uint64_t>();
    if (number < minimum || number > std::numeric_limits<std::size_t>::max()) {
        throw std::invalid_argument(message);
    }
    return static_cast<std::size_t>(number);
}

} // namespace

GameConfig read_config(std::istream& input) {
    try {
        const auto document = nlohmann::json::parse(input);
        if (input.bad()) {
            throw std::runtime_error("Cannot read JSON input");
        }
        const auto& field = document.at("world");
        const auto width = read_integer(field, "width", 1);
        const auto height = read_integer(field, "height", 1);
        World world(field.at("cells").get<std::vector<std::string>>());
        if (world.width() != width || world.height() != height) {
            throw std::invalid_argument("world dimensions must match world.cells");
        }
        const auto& simulation = document.at("simulation");
        const auto fps = read_integer(simulation, "fps", 1);
        const auto cycles = read_integer(simulation, "cycles", 0);
        return GameConfig{std::move(world), fps, cycles};
    } catch (const nlohmann::json::exception& error) {
        throw std::invalid_argument(std::string("Invalid game configuration: ") + error.what());
    }
}

GameConfig load_config(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        throw std::runtime_error("Cannot open input file: " + path.string());
    }
    return read_config(input);
}

} // namespace life
