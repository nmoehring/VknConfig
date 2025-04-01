cmake_minimum_required(VERSION 3.29)
message("RUNNING BUILD.CMAKE")

# --- Parse Arguments ---
set(options LOG TRACE_LOG)
set(oneValueArgs CONFIG)
set(multiValueArgs)
cmake_parse_arguments(BUILD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

# --- Set Default Values ---
if(NOT CONFIG)
    set(CONFIG "Debug")
endif()

# --- Build Configurations ---
set(BUILD_CONFIGURATIONS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# --- Check for Valid Configuration ---
list(FIND BUILD_CONFIGURATIONS "${CONFIG}" configIndex)

if(configIndex EQUAL -1)
    message(FATAL_ERROR "Invalid build configuration: ${CONFIG}. Valid configurations are: ${BUILD_CONFIGURATIONS}")
endif()

# --- Configure ---
message(STATUS "Configuring project with configuration: ${CONFIG}")

if(LOG)
    execute_process(
        COMMAND ${CMAKE_COMMAND} . --preset debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/config.log
        ERROR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/config.log
    )
elseif(BUILD_TRACE_LOG)
    execute_process(
        COMMAND ${CMAKE_COMMAND} . --preset debug --trace
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/config.log
        ERROR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/config.log
    )
else()
    execute_process(
        COMMAND ${CMAKE_COMMAND} . --preset debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# --- Build ---
message(STATUS "Building project with configuration: ${CONFIG}")

if(LOG)
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --preset debug --config ${CONFIG}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/build.log
        ERROR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/build_error.log
    )
elseif(BUILD_TRACE_LOG)
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --preset debug --trace
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/build.log
        ERROR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/build_error.log
    )
else()
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --preset debug --config ${CONFIG}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
