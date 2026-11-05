foreach(required IN ITEMS PROGRAM CASE TEST_DIR)
    if(NOT DEFINED ${required})
        message(FATAL_ERROR "Missing test parameter: ${required}")
    endif()
endforeach()

if(CASE STREQUAL "missing_birth")
    set(arguments --birth)
    set(expected_error "Missing value for --birth")
elseif(CASE STREQUAL "missing_survive")
    set(arguments --survive)
    set(expected_error "Missing value for --survive")
elseif(CASE STREQUAL "missing_input")
    set(arguments --input)
    set(expected_error "Missing value for --input")
elseif(CASE STREQUAL "invalid_birth")
    set(arguments --birth 9)
    set(expected_error "Neighbor counts")
elseif(CASE STREQUAL "invalid_survive")
    set(arguments --survive -1)
    set(expected_error "Neighbor counts")
elseif(CASE STREQUAL "unknown_option")
    set(arguments --unknown)
    set(expected_error "Usage:")
elseif(CASE STREQUAL "missing_file")
    set(arguments --input missing.json)
    set(expected_error "Cannot open input file")
else()
    message(FATAL_ERROR "Unknown CLI case: ${CASE}")
endif()

file(MAKE_DIRECTORY "${TEST_DIR}")
file(WRITE "${TEST_DIR}/game_of_life_input.json" [=[
{"world":{"width":1,"height":1,"cells":["."]},"simulation":{"fps":1,"cycles":0}}
]=])
set(output_file "${TEST_DIR}/game_of_life_output.json")
file(WRITE "${output_file}" "previous result")
execute_process(
    COMMAND "${PROGRAM}" ${arguments}
    WORKING_DIRECTORY "${TEST_DIR}"
    RESULT_VARIABLE status
    ERROR_VARIABLE error
    OUTPUT_VARIABLE console
    TIMEOUT 10
)
if(NOT status STREQUAL "1" OR NOT error MATCHES "${expected_error}")
    message(FATAL_ERROR "Expected ${expected_error}, got status ${status}: ${error}")
endif()
file(READ "${output_file}" saved)
if(NOT saved STREQUAL "previous result")
    message(FATAL_ERROR "Invalid arguments overwrote the previous result")
endif()
