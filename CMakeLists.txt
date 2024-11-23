cmake_minimum_required(VERSION 3.29)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VkCkBk VERSION 0.1 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

include(FetchContent)
find_package(Vulkan REQUIRED SPIRV-Tools)

# find_package(Python3 REQUIRED COMPONENTS Interpreter)
message("Vulkan header version: ${VulkanHeaderVersion}")

add_library(vkckbk_compiler_flags INTERFACE)
target_compile_features(vkckbk_compiler_flags INTERFACE cxx_std_20)

add_subdirectory(vulkancore)

# add_subdirectory(enginecore)
add_subdirectory(chapter1)
