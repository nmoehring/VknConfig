#  Uses vcpkg to install all the dependencies. vcpkg is included via CMakePresets.json
#  Output files are written to a neighboring VkCkBk_build folder. This directory is 
# meant to stay unchanged, as it typically has version control through git.

cmake_minimum_required(VERSION 3.29)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VkCkBk VERSION 0.2 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

find_package(Vulkan REQUIRED)
#not sure why these still need to be found but other vcpkg's dont'
#might try running without them and figure it out
find_package(SPIRV-Tools CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Stb REQUIRED)

message("Vulkan header version: ${VulkanHeaderVersion}")
message("Vulkan include directory: ${Vulkan_INCLUDE_DIRS}")
message("If this points to some other installed Vulkan header files and not your SDK install,
 maybe delete them and the CMakeCache and try again.")
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
)

#Some debug libraries are suffixed with the letter 'd'
# which is nice because we need to link to the release builds eventually, anyway
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Adding debug libraries.")
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib
    )
else()
    message("Adding release libraries.")
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
    )
endif()

#CMake docs recommended this style of modifying compiler/linker flags and
# specifying the C++ version
add_library(vkckbk_compiler_flags INTERFACE)
target_compile_features(vkckbk_compiler_flags INTERFACE cxx_std_20)

#Single-header libraries
#Decided not to include stb here because it's more work than it's worth
add_library(vma INTERFACE) #this is a special thing (no sources after INTERFACE)
target_sources(vma INTERFACE vma/vma.cpp) #this is how to add sources to an interface lib

add_subdirectory(vulkancore)
# add_subdirectory(enginecore)
add_subdirectory(chapter1)
