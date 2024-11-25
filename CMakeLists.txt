cmake_minimum_required(VERSION 3.29)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VkCkBk VERSION 0.2 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

find_package(Vulkan REQUIRED)
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
    ${CMAKE_SOURCE_DIR}/enginecore
    ${CMAKE_SOURCE_DIR}/vulkancore
    ${Vulkan_INCLUDE_DIRS}
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/vma
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/spirv-reflect
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Adding debug library.")
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib
    )
else()
    message("Adding release library.")
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
    )
endif()

add_library(vkckbk_compiler_flags INTERFACE)
target_compile_features(vkckbk_compiler_flags INTERFACE cxx_std_20)
#add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)

add_subdirectory(vulkancore)

# add_subdirectory(enginecore)
add_subdirectory(chapter1)
