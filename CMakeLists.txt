cmake_minimum_required(VERSION 3.29)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VkCkBk VERSION 0.1 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

# ===============INCLUDE MODULES & PACKAGES===================== #
message("================ Finding modules and packages. ======================")
include(FetchContent)
find_package(Vulkan REQUIRED SPIRV-Tools)
message("Vulkan header version: ${VulkanHeaderVersion}")

# =============CREATE COMPILE FLAGS INTERFACE ================== #
add_library(vkckbk_compiler_flags INTERFACE)
target_compile_features(vkckbk_compiler_flags INTERFACE cxx_std_20)

# =============DECLARE DEPENDENCIES============= #
message("================ Declaring dependencies ======================")
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git)
FetchContent_Declare(gli GIT_REPOSITORY https://github.com/g-truc/gli.git)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git)
FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git)
FetchContent_Declare(volk GIT_REPOSITORY https://github.com/zeux/volk.git)

FetchContent_Declare(spirv-headers
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
    GIT_TAG origin/main)
FetchContent_Declare(spirv-tools
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
    GIT_TAG origin/main)
FetchContent_Declare(glslang
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang
    GIT_TAG origin/main)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG origin/main)

# =============POPULATE DEPENDENCIES============== #
message("================ Populating dependencies ======================")
FetchContent_MakeAvailable(googletest spirv-headers spirv-tools glslang glm gli glfw stb volk)

# I just keep getting "no such file or directory". It doesn't come from cmd, so I imagine it's CMake.
# Python gives a "No such file or directory" which starts with a capitalized 'No', so it probably isn't it.
execute_process(
    COMMAND ${CMAKE_COMMAND} -E env python3 ${CMAKE_SOURCE_DIR}/scripts/patch_gli_cmake_version.py 3.29
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/_deps/gli-source/
    RESULT_VARIABLE gli_patch_result)
message(${gli_patch_result})

# ==============INCLUDE DEPENDENCY DIRECTORIES================= #
include_directories(
    ${CMAKE_SOURCE_DIR}
    vulkancore
    enginecore
    ${glm_SOURCE_DIR}
    ${gli_SOURCE_DIR}
    ${volk_SOURCE_DIR}
    ${GLFW_SOURCE_DIR}/include
    ${stb_SOURCE_DIR})

# ==============PROCESS SUBDIRECTORIES=================#
message("================ Processing subdirectories ======================")
add_subdirectory(vulkancore)

# add_subdirectory(enginecore)
add_subdirectory(chapter1)
