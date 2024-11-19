cmake_minimum_required(VERSION 3.29)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VkCkBk VERSION 0.1 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

include(FetchContent)

# =============CREATE COMPILE FLAGS INTERFACE ================== #
add_library(vkckbk_compiler_flags INTERFACE)
target_compile_features(vkckbk_compiler_flags INTERFACE cxx_std_20)

# =============DECLARE DEPENDENCIES============= #
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
)

FetchContent_Declare(
    gli
    GIT_REPOSITORY https://github.com/g-truc/gli.git
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)

# =============POPULATE DEPENDENCIES============== #
FetchContent_MakeAvailable(glm gli glfw stb)

# ==============INCLUDE DEPENDENCY DIRECTORIES================= #
include_directories(
    vulkancore
    enginecore
    ${glm_SOURCE_DIR}
    ${gli_SOURCE_DIR}
    ${GLFW_SOURCE_DIR}/include
    ${stb_SOURCE_DIR}
)

# ==============PROCESS SUBDIRECTORIES=================#
add_subdirectory(vulkancore)

# add_subdirectory(enginecore)
add_subdirectory(chapter1)
