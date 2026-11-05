#include "stop_signal.hpp"

#include <atomic>
#include <csignal>
#include <stdexcept>

namespace {

std::atomic_flag requested{};

extern "C" void request_stop(int) {
    (void)requested.test_and_set();
}

} // namespace

namespace life {

void install_stop_handlers() {
    requested.clear();
    if (std::signal(SIGINT, request_stop) == SIG_ERR ||
        std::signal(SIGTERM, request_stop) == SIG_ERR) {
        throw std::runtime_error("Cannot install stop handlers");
    }
}

bool stop_requested() noexcept {
    return requested.test();
}

} // namespace life
