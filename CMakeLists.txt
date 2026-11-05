cmake_minimum_required(VERSION 3.20)

project(GameOfLife VERSION 0.1.0 LANGUAGES CXX)

add_library(life_core src/core/world.cpp)
target_include_directories(life_core PUBLIC src/core)
target_compile_features(life_core PUBLIC cxx_std_20)

add_library(life_json src/storage/json_storage.cpp)
target_include_directories(life_json PUBLIC src/storage)
target_include_directories(life_json SYSTEM PRIVATE third_party)
target_link_libraries(life_json PUBLIC life_core)

add_library(life_terminal src/terminal/terminal_renderer.cpp)
target_include_directories(life_terminal PUBLIC src/terminal)
target_link_libraries(life_terminal PUBLIC life_core)

add_executable(game_of_life src/main.cpp src/app/application.cpp)
target_include_directories(game_of_life PRIVATE src/app)
target_link_libraries(game_of_life PRIVATE life_json life_terminal)

set(life_targets life_core life_json life_terminal game_of_life)

include(CTest)
if(BUILD_TESTING)
    add_executable(world_tests tests/world_tests.cpp)
    target_link_libraries(world_tests PRIVATE life_core)
    add_test(NAME world_tests COMMAND world_tests)
    add_executable(json_storage_tests tests/json_storage_tests.cpp)
    target_link_libraries(json_storage_tests PRIVATE life_json)
    target_include_directories(json_storage_tests SYSTEM PRIVATE third_party)
    add_test(NAME json_storage_tests COMMAND json_storage_tests)
    add_executable(save_result_tests tests/save_result_tests.cpp)
    target_link_libraries(save_result_tests PRIVATE life_json)
    add_test(NAME save_result_tests COMMAND save_result_tests)
    add_executable(terminal_renderer_tests tests/terminal_renderer_tests.cpp)
    target_link_libraries(terminal_renderer_tests PRIVATE life_terminal)
    add_test(NAME terminal_renderer_tests COMMAND terminal_renderer_tests)
    list(APPEND life_targets world_tests json_storage_tests save_result_tests terminal_renderer_tests)
    foreach(case IN ITEMS blinker redirected_auto empty block zero_cycles extinction rectangle
                          blinker_two_steps corner_birth output_error invalid_input)
        add_test(NAME batch_${case} COMMAND ${CMAKE_COMMAND}
            "-DPROGRAM=$<TARGET_FILE:game_of_life>" "-DCASE=${case}"
            "-DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}/batch-tests/${case}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/batch_case.cmake")
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
