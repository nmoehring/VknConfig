# Uses vcpkg to install all the dependencies. vcpkg is included via CMakePresets.json
# Output files are written to a neighboring vknconfig_build folder. This directory is
# meant to stay unchanged, as it typically has version control through git.
# cmake minimum version is set to 3.29 because it ships with Visual Studio 2022

# ######## INITIALIZE ###########
cmake_minimum_required(VERSION 3.29)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VknConfig VERSION 0.3 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

# ########### UTILITY FUNCTIONS ##############
# Copy directories to build folders keeping in mind debug configs
function(createBuildDir)
    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR})
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()
endfunction()

function(copyResources)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/Debug)
            file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Debug)
        endif()

        file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
    else()
        file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif()
endfunction()

function(buildSubDir subdir)
    if(NOT TARGET_SUBDIR OR TARGET_SUBDIR MATCHES ALL OR TARGET_SUBDIR MATCHES ${subdir})
        message(" BUILDING ${TARGET_SUBDIR} ")
        add_subdirectory(${subdir})
    endif()
endfunction()

createBuildDir()

# ##### FIND, CONFIGURE, AND BUILD DEPENDENCIES #####(What's left anyway after vcpkg ran)
include(FetchContent)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# ########## LIB AND INCLUDE ############
message("Vulkan header version: ${VulkanHeaderVersion}")
message("Vulkan include directory: ${Vulkan_INCLUDE_DIRS}")
message("^If this points to some other installed Vulkan header files and not your SDK install,
 maybe delete them and the CMakeCache and try again.")
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
    ${glfw3_DIR}/include
)

# Optimizations (referenced in bs::thread-pool)
# TODO: Other system Cxx compiler optimizations
# TODO: SPIR-V optimizations
# TODO: Translate SPIR-V to LLVM and use LLVM optimizations, translate back to SPIR-V.
# For Clang or GCC, use the -O3 flag.
# For MSVC, use /O2.

# CMake docs recommended this style of modifying compiler/linker flags and
# specifying the C++ version
add_library(vknconfig_compiler_args INTERFACE)
target_compile_features(vknconfig_compiler_args INTERFACE cxx_std_20)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Adding debug library directories.")
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib
        ${Vulkan_INCLUDE_DIR}/../lib
    )
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(vknconfig_compiler_args INTERFACE /O2)
    message("Adding release library directories.")
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
        ${Vulkan_INCLUDE_DIR}/../lib
    )
else()
    message("Unknown build type.")
endif()

# Single-header libraries
add_library(vma INTERFACE)
target_sources(vma INTERFACE single-headers/vma.cpp)

# ############ PROCESS SUBDIRECTORIES #################
add_subdirectory(VknConfig)

# add_subdirectory(VkCore)
buildSubDir(VkExtCheck)
buildSubDir(minTest)
