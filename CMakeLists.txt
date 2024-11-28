# Uses vcpkg to install all the dependencies. vcpkg is included via CMakePresets.json
# Output files are written to a neighboring VkCkBk_build folder. This directory is
# meant to stay unchanged, as it typically has version control through git.
# cmake minimum version is set to 3.29 because it ships with Visual Studio 2022

# ######## INITIALIZE ###########
cmake_minimum_required(VERSION 3.29)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VkCkBk VERSION 0.3 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

# ##### FIND, CONFIGURE, AND BUILD DEPENDENCIES #####(What's left anyway after vcpkg ran)
include(FetchContent)
FetchContent_Declare(
    bs-thread-pool
    GIT_REPOSITORY https://github.com/bshoshany/thread-pool.git
    GIT_TAG v4.1.0
)
FetchContent_MakeAvailable(bs-thread-pool)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# imgui surely exists by now, and the backends need to be linked below
if(NOT EXISTS imguiBackends)
    file(
        COPY ${VCPKG_INSTALLED_DIR}/vcpkg/blds/imgui/src/v1.91.0-51846a2755.clean/backends
        DESTINATION ${CMAKE_SOURCE_DIR}
    )
    file(RENAME backends imguiBackends)
    file(COPY_FILE cmakelists.txt.imguiBackends imguiBackends/CMakeLists.txt)
endif()

# ########## LIB AND INCLUDE ############
message("Vulkan header version: ${VulkanHeaderVersion}")
message("Vulkan include directory: ${Vulkan_INCLUDE_DIRS}")
message("If this points to some other installed Vulkan header files and not your SDK install,
 maybe delete them and the CMakeCache and try again.")
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
    ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
    ${CMAKE_BINARY_DIR}/_deps/bs-thread-pool-src/include # MakeAvailable fails me again
)

# Some debug libraries are suffixed with the letter 'd'
# which is nice because we need to link to the release builds eventually, anyway
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Adding debug libraries.")
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib
        ${CMAKE_BINARY_DIR}/imguiBackends/Debug
        ${Vulkan_INCLUDE_DIR}/../lib
    )
else()
    message("Adding release libraries.")
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
        ${CMAKE_BINARY_DIR}/imguiBackends
        ${Vulkan_INCLUDE_DIR}/../lib
    )
endif()

# Optimizations (referenced in bs::thread-pool)
# For Clang or GCC, use the -O3 flag.
# For MSVC, use /O2.

# CMake docs recommended this style of modifying compiler/linker flags and
# specifying the C++ version
add_library(vkckbk_compiler_flags INTERFACE)
target_compile_features(vkckbk_compiler_flags INTERFACE cxx_std_20)

# target_compile_options(vkckbk_compiler_flags INTERFACE /O2)

# Single-header libraries
# Decided not to include stb here because it's more work than it's worth
add_library(vma INTERFACE) # this is a special thing (no sources after INTERFACE)
target_sources(vma INTERFACE single-headers/vma.cpp) # this is how to add sources to an interface lib

# ########### UTILITY FUNCTIONS ##############
# Copy directories to build folders keeping in mind debug configs
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

# ############ PROCESS SUBDIRECTORIES #################
add_subdirectory(vulkancore)
add_subdirectory(enginecore)
add_subdirectory(imguiBackends)

# add_subdirectory(VkCore)
buildSubDir(VkExtCheck)

# buildSubDir(minTest)
buildSubDir(Chapter1)
buildSubDir(Chapter2)
