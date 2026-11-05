#include "json_storage.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>

#ifndef _WIN32
#include <csignal>
#include <cstdlib>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

struct TestDirectory {
    fs::path path = "save-result-tests";

    TestDirectory() {
        require(fs::create_directory(path), "Test directory already exists");
    }
    ~TestDirectory() {
        std::error_code ignored;
        fs::remove_all(path, ignored);
    }
};

std::string read_file(const fs::path& path) {
    std::ifstream input(path);
    require(input.is_open(), "Cannot read saved result");
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

void require_no_temporary_files(const fs::path& directory, const fs::path& output) {
    for (const auto& entry : fs::directory_iterator(directory)) {
        require(entry.path() == output, "Temporary output files were not removed");
    }
}

#ifndef _WIN32
void check_failed_write(const fs::path& output, const life::World& world) {
    const auto before = read_file(output);
    const auto child = fork();
    require(child >= 0, "Cannot start write-failure test");
    if (child == 0) {
        std::signal(SIGXFSZ, SIG_IGN);
        const rlimit limit{32, 32};
        if (setrlimit(RLIMIT_FSIZE, &limit) != 0) {
            std::_Exit(2);
        }
        try {
            life::save_result(output, world);
        } catch (const std::runtime_error&) {
            std::_Exit(0);
        }
        std::_Exit(1);
    }
    int status = 0;
    require(waitpid(child, &status, 0) == child && WIFEXITED(status) &&
                WEXITSTATUS(status) == 0, "A failed write was not reported");
    require(read_file(output) == before, "A failed write changed the previous result");
}
#endif

int main() {
    try {
        const TestDirectory directory;
        const auto output = directory.path / "result.json";
        const life::World original({"*..", "..*"});
        const life::World replacement({"**", "**"});
        life::save_result(output, original);
        std::ostringstream expected;
        life::write_result(expected, original);
        require(read_file(output) == expected.str(), "Incorrect initial saved result");
#ifndef _WIN32
        check_failed_write(output, replacement);
#endif
        require_no_temporary_files(directory.path, output);
        life::save_result(output, replacement);
        std::ostringstream updated;
        life::write_result(updated, replacement);
        require(read_file(output) == updated.str(), "Existing result was not replaced");
        require_no_temporary_files(directory.path, output);

        fs::remove(output);
        fs::create_directory(output);
        std::ofstream(output / "keep.txt") << "previous data";
        bool rejected = false;
        try {
            life::save_result(output, replacement);
        } catch (const std::runtime_error&) {
            rejected = true;
        }
        require(rejected, "A failed replacement was not reported");
        require(read_file(output / "keep.txt") == "previous data",
                "A failed replacement changed the destination");
        require_no_temporary_files(directory.path, output);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
