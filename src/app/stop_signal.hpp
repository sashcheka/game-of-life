#pragma once

namespace life {

void install_stop_handlers();
[[nodiscard]] bool stop_requested() noexcept;

} // namespace life
