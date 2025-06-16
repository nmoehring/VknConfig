project(minTest)
add_executable(minTest minTest.cpp)
target_link_libraries(minTest VknConfig_args VknConfig glfw)

# -------Shaders Compilation-----------
if(COMPILE_SHADERS)
    find_program(GLSLANG_VALIDATOR_EXECUTABLE glslangValidator HINTS ENV VULKAN_SDK PATHS "${Vulkan_BIN_DIR}" ENV PATH)

    if(NOT GLSLANG_VALIDATOR_EXECUTABLE)
        message(FATAL_ERROR "glslangValidator not found. Make sure the Vulkan SDK is installed and in your PATH, or set VULKAN_SDK environment variable.")
    endif()

    message(STATUS "Found glslangValidator: ${GLSLANG_VALIDATOR_EXECUTABLE}")

    # Define shader directory and collect them all
    set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders")
    file(GLOB SHADER_SOURCES "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag")
    set(COMPILED_SHADER_FILES_LIST "") # List to hold all compiled shader file paths

    foreach(SHADER_SOURCE_FILE ${SHADER_SOURCES})
        get_filename_component(SHADER_BASENAME ${SHADER_SOURCE_FILE} NAME) # e.g., simple_shader.vert

        # The output filename will be like simple_shader.vert.spv, as expected by your C++ code
        set(SHADER_SPV_FILENAME "${SHADER_BASENAME}.spv")
        set(SHADER_SPV_FILE_FULLPATH "${SHADER_DIR}/${SHADER_SPV_FILENAME}")
        list(APPEND COMPILED_SHADER_FILES_LIST ${SHADER_SPV_FILE_FULLPATH})

        add_custom_command(
            OUTPUT ${SHADER_SPV_FILE_FULLPATH}
            COMMAND ${GLSLANG_VALIDATOR_EXECUTABLE} -V -o ${SHADER_SPV_FILE_FULLPATH} ${SHADER_SOURCE_FILE}
            DEPENDS ${SHADER_SOURCE_FILE}
            COMMENT "Compiling ${SHADER_BASENAME} to ${SHADER_SPV_FILENAME}"
            VERBATIM
        )
    endforeach()

    add_custom_target(CompileShaders ALL
        DEPENDS ${COMPILED_SHADER_FILES_LIST}
        COMMENT "Ensuring all shaders are compiled"
    )

    # Ensure shaders are compiled before resources copied
    add_dependencies(minTest CompileShaders)
endif()

copyResources()