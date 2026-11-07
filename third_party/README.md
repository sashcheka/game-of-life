# Third-party dependencies

## nlohmann/json

- Version: 3.12.0.
- Upstream: https://github.com/nlohmann/json/tree/v3.12.0
- Header: `nlohmann/json.hpp`, copied unchanged from
  `single_include/nlohmann/json.hpp` in the upstream release.
- License: MIT, included unchanged as `nlohmann/LICENSE.MIT`.
- Header SHA-256:
  `aaf127c04cb31c406e5b04a63f1ae89369fccde6d8fa7cdda1ed4f32dfc5de63`.

## FTXUI

- Version: 7.0.3.
- Terminal rendering and keyboard input on Windows, macOS, and Linux.
- Amalgamated sources: `ftxui/ftxui.hpp` and `ftxui/ftxui.cpp`.
- License and release provenance: see [ftxui/README.md](ftxui/README.md).

Both libraries are included as source in the submission. No package manager,
network download, or separately installed library is needed to build or run.
CMake's `Threads::Threads` uses the platform's threading support; it is not an
additional third-party dependency. CMake, a C++20 compiler, and build tools are
required development tools and are not bundled.
