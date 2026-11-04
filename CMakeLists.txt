cmake_minimum_required(VERSION 3.20)

project(GameOfLife VERSION 0.1.0 LANGUAGES CXX)

add_executable(game_of_life src/main.cpp)

target_compile_features(game_of_life PRIVATE cxx_std_20)
set_target_properties(game_of_life PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(game_of_life PRIVATE /W4 /permissive-)
else()
    target_compile_options(game_of_life PRIVATE -Wall -Wextra -Wpedantic)
endif()
