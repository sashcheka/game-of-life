cmake_minimum_required(VERSION 3.20)

project(GameOfLife VERSION 0.1.0 LANGUAGES CXX)

find_package(Threads REQUIRED)
add_library(ftxui STATIC third_party/ftxui/ftxui.cpp)
target_include_directories(ftxui SYSTEM PUBLIC third_party/ftxui)
target_compile_features(ftxui PUBLIC cxx_std_20)
target_link_libraries(ftxui PRIVATE Threads::Threads)
set_target_properties(ftxui PROPERTIES CXX_EXTENSIONS OFF)
if(WIN32)
    target_compile_definitions(ftxui PUBLIC UNICODE _UNICODE NOMINMAX)
endif()
if(MSVC)
    target_compile_options(ftxui PRIVATE /utf-8)
endif()

add_library(life_core
    src/core/rules.cpp
    src/core/wave.cpp
    src/core/world.cpp
    src/core/simulation.cpp
    src/core/playback.cpp
)
target_include_directories(life_core PUBLIC src/core)
target_compile_features(life_core PUBLIC cxx_std_20)

add_library(life_json src/storage/json_storage.cpp)
target_include_directories(life_json PUBLIC src/storage)
target_include_directories(life_json SYSTEM PRIVATE third_party)
target_link_libraries(life_json PUBLIC life_core)

add_library(life_terminal
    src/terminal/cell_formatter.cpp
    src/terminal/game_view.cpp
    src/terminal/game_controls.cpp
    src/terminal/key_repeat.cpp
)
target_include_directories(life_terminal PUBLIC src/terminal)
target_link_libraries(life_terminal PUBLIC life_core ftxui)

add_executable(game_of_life
    src/main.cpp
    src/app/application.cpp
    src/app/interactive_game.cpp
    src/app/stop_signal.cpp
)
target_include_directories(game_of_life PRIVATE src/app)
target_link_libraries(game_of_life PRIVATE life_json life_terminal)

set(life_targets life_core life_json life_terminal game_of_life)

include(CTest)

if(BUILD_TESTING)
    add_executable(wave_tests tests/wave_tests.cpp)
    target_link_libraries(wave_tests PRIVATE life_core)
    add_test(NAME wave_tests COMMAND wave_tests)

    add_executable(rules_tests tests/rules_tests.cpp)
    target_link_libraries(rules_tests PRIVATE life_core)
    add_test(NAME rules_tests COMMAND rules_tests)

    add_executable(world_tests tests/world_tests.cpp)
    target_link_libraries(world_tests PRIVATE life_core)
    add_test(NAME world_tests COMMAND world_tests)

    add_executable(simulation_tests tests/simulation_tests.cpp)
    target_link_libraries(simulation_tests PRIVATE life_core)
    add_test(NAME simulation_tests COMMAND simulation_tests)

    add_executable(playback_tests tests/playback_tests.cpp)
    target_link_libraries(playback_tests PRIVATE life_core)
    add_test(NAME playback_tests COMMAND playback_tests)

    add_executable(json_storage_tests tests/json_storage_tests.cpp)
    target_link_libraries(json_storage_tests PRIVATE life_json)
    target_include_directories(json_storage_tests SYSTEM PRIVATE third_party)
    add_test(NAME json_storage_tests COMMAND json_storage_tests)

    add_executable(cell_formatter_tests tests/cell_formatter_tests.cpp)
    target_link_libraries(cell_formatter_tests PRIVATE life_terminal)
    add_test(NAME cell_formatter_tests COMMAND cell_formatter_tests)

    add_executable(game_view_tests tests/game_view_tests.cpp)
    target_link_libraries(game_view_tests PRIVATE life_terminal)
    add_test(NAME game_view_tests COMMAND game_view_tests)

    add_executable(key_repeat_tests tests/key_repeat_tests.cpp)
    target_link_libraries(key_repeat_tests PRIVATE life_terminal)
    add_test(NAME key_repeat_tests COMMAND key_repeat_tests)

    list(APPEND life_targets
        wave_tests
        rules_tests
        world_tests
        simulation_tests
        playback_tests
        json_storage_tests
        cell_formatter_tests
        game_view_tests
        key_repeat_tests
    )

    foreach(case IN ITEMS
        blinker
        ascii
        empty
        block
        zero_cycles
        extinction
        rectangle
        blinker_two_steps
        corner_birth
        highlife
        seeds
        birth_four
        rebirth
        output_error
        invalid_input
    )
        add_test(
            NAME batch_${case}
            COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:game_of_life>"
            "-DCASE=${case}"
            "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/batch-tests/${case}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/batch_case.cmake"
        )
    endforeach()

    foreach(case IN ITEMS missing_birth missing_survive missing_input invalid_birth
                          invalid_survive unknown_option missing_file)
        add_test(NAME cli_${case} COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:game_of_life>" "-DCASE=${case}"
            "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/cli-tests/${case}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/cli_errors.cmake")
    endforeach()
endif()

foreach(target IN LISTS life_targets)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
