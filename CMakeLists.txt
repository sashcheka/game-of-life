cmake_minimum_required(VERSION 3.20)

project(GameOfLife VERSION 0.1.0 LANGUAGES CXX)

add_library(life_core src/world.cpp)
target_include_directories(life_core PUBLIC src)
target_compile_features(life_core PUBLIC cxx_std_20)

add_library(life_json src/json_storage.cpp)
target_include_directories(life_json SYSTEM PRIVATE third_party)
target_link_libraries(life_json PUBLIC life_core)

add_executable(game_of_life src/main.cpp)
target_link_libraries(game_of_life PRIVATE life_json)

set(life_targets life_core life_json game_of_life)

include(CTest)
if(BUILD_TESTING)
    add_executable(world_tests tests/world_tests.cpp)
    target_link_libraries(world_tests PRIVATE life_core)
    add_test(NAME world_tests COMMAND world_tests)
    add_executable(json_storage_tests tests/json_storage_tests.cpp)
    target_link_libraries(json_storage_tests PRIVATE life_json)
    target_include_directories(json_storage_tests SYSTEM PRIVATE third_party)
    add_test(NAME json_storage_tests COMMAND json_storage_tests)
    list(APPEND life_targets world_tests json_storage_tests)
endif()

foreach(target IN LISTS life_targets)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
