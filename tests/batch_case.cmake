foreach(required IN ITEMS PROGRAM CASE TEST_DIR)
    if(NOT DEFINED ${required})
        message(FATAL_ERROR "Missing test parameter: ${required}")
    endif()
endforeach()

if(CASE STREQUAL "blinker" OR CASE STREQUAL "redirected_auto")
    set(width 5)
    set(height 5)
    set(cells
        "....."
        "..*.."
        "..*.."
        "..*.."
        "....."
    )
    set(expected_cells
        "....."
        "....."
        ".***."
        "....."
        "....."
    )
    set(cycles 1)
    set(expected_generation 1)
    set(expected_reason "cycle limit")

elseif(CASE STREQUAL "empty")
    set(width 3)
    set(height 3)
    set(cells
        "..."
        "..."
        "..."
    )
    set(expected_cells ${cells})
    set(cycles 10)
    set(expected_generation 0)
    set(expected_reason "no living cells")

elseif(CASE STREQUAL "block")
    set(width 4)
    set(height 4)
    set(cells
        "...."
        ".**."
        ".**."
        "...."
    )
    set(expected_cells ${cells})
    set(cycles 100)
    set(expected_generation 100)
    set(expected_reason "cycle limit")

elseif(CASE STREQUAL "zero_cycles")
    set(width 5)
    set(height 5)
    set(cells
        "....."
        "..*.."
        "..*.."
        "..*.."
        "....."
    )
    set(cycles 0)
    set(expected_generation 0)
    set(expected_cells ${cells})
    set(expected_reason "cycle limit")

elseif(CASE STREQUAL "extinction")
    set(width 1)
    set(height 1)
    set(cells
        "*"
    )
    set(expected_cells
        "."
    )
    set(cycles 100)
    set(expected_generation 1)
    set(expected_reason "no living cells")

elseif(CASE STREQUAL "rectangle")
    set(width 3)
    set(height 2)
    set(cells
        "*.."
        "..*"
    )
    set(expected_cells
        "..."
        "..."
    )
    set(cycles 100)
    set(expected_generation 1)
    set(expected_reason "no living cells")

elseif(CASE STREQUAL "blinker_two_steps")
    set(width 5)
    set(height 5)
    set(cells
        "....."
        "..*.."
        "..*.."
        "..*.."
        "....."
    )
    set(cycles 2)
    set(expected_generation 2)
    set(expected_cells ${cells})
    set(expected_reason "cycle limit")

elseif(CASE STREQUAL "corner_birth")
    set(width 2)
    set(height 2)
    set(cells
        "**"
        "*."
    )
    set(expected_cells
        "**"
        "**"
    )
    set(cycles 2)
    set(expected_generation 2)
    set(expected_reason "cycle limit")

elseif(CASE STREQUAL "invalid_input")
    set(width 5)
    set(height 5)
    set(cells
        "....."
        "..*.."
        "..*.."
        "..*.."
        "....."
    )
    set(cycles -1)

elseif(CASE STREQUAL "output_error")
    set(width 5)
    set(height 5)
    set(cells
        "....."
        "..*.."
        "..*.."
        "..*.."
        "....."
    )
    set(cycles 1)

else()
    message(FATAL_ERROR "Unknown batch case: ${CASE}")
endif()

file(MAKE_DIRECTORY "${TEST_DIR}")
set(input_file "${TEST_DIR}/game_of_life_input.json")
set(output_file "${TEST_DIR}/game_of_life_output.json")

if(CASE STREQUAL "output_error")
    file(MAKE_DIRECTORY "${output_file}")
else()
    file(REMOVE "${output_file}")
endif()

string(JOIN "\", \"" joined_cells ${cells})
set(input_template [=[
{
    "world": {
        "width": @width@,
        "height": @height@,
        "cells": ["@joined_cells@"]
    },
    "simulation": {
        "fps": 30,
        "cycles": @cycles@
    }
}
]=])
string(CONFIGURE "${input_template}" input_json @ONLY)
file(WRITE "${input_file}" "${input_json}")

set(arguments --batch)
if(CASE STREQUAL "redirected_auto")
    set(arguments)
endif()
execute_process(
    COMMAND "${PROGRAM}" ${arguments}
    WORKING_DIRECTORY "${TEST_DIR}"
    RESULT_VARIABLE status
    OUTPUT_VARIABLE console
    ERROR_VARIABLE error
    TIMEOUT 10
)

string(ASCII 27 escape)
if(console MATCHES "${escape}")
    message(FATAL_ERROR "Batch or redirected output must not contain terminal controls")
endif()

if(CASE STREQUAL "output_error" OR CASE STREQUAL "invalid_input")
    if(NOT status STREQUAL "1")
        message(FATAL_ERROR "Expected exit code 1, got ${status}: ${error}")
    endif()

    if(CASE STREQUAL "output_error" AND NOT error MATCHES "Cannot open output file")
        message(FATAL_ERROR "Missing output error message: ${error}")
    endif()

    if(CASE STREQUAL "invalid_input")
        if(NOT error MATCHES "cycles" OR EXISTS "${output_file}")
            message(FATAL_ERROR "Invalid input must report an error without saving")
        endif()
    endif()

    return()
endif()

if(NOT status STREQUAL "0")
    message(FATAL_ERROR "Application failed: ${status}: ${error}")
endif()

file(READ "${output_file}" saved)
string(JSON root_fields LENGTH "${saved}")
string(JSON result_fields LENGTH "${saved}" result)
string(JSON actual_width GET "${saved}" result width)
string(JSON actual_height GET "${saved}" result height)
string(JSON row_count LENGTH "${saved}" result cells)
if(NOT root_fields EQUAL 1 OR NOT result_fields EQUAL 3 OR
   NOT actual_width EQUAL width OR NOT actual_height EQUAL height OR
   NOT row_count EQUAL height)
    message(FATAL_ERROR "Incorrect output schema or dimensions: ${saved}")
endif()

math(EXPR last_row "${height} - 1")
foreach(index RANGE 0 ${last_row})
    list(GET expected_cells ${index} expected_row)
    string(JSON actual_row GET "${saved}" result cells ${index})
    if(NOT actual_row STREQUAL expected_row)
        message(FATAL_ERROR "Row ${index}: expected ${expected_row}, got ${actual_row}")
    endif()
endforeach()

if(NOT console MATCHES "Stopped: ${expected_reason}\\. Saved generation ${expected_generation} to")
    message(FATAL_ERROR "Incorrect stop reason or generation: ${console}")
endif()
