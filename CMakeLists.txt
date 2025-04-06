cmake_minimum_required(VERSION 3.29)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VknConfig VERSION 0.3 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

# --- Utility functions ---
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

# --- Create build dir ---
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

# --- DEPENDENCIES ---
include(FetchContent)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

message(STATUS "Vulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan_LIBRARIES: ${Vulkan_LIBRARIES}")

# --- Fetch gtest ---
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
    GIT_SHALLOW
)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

# ########## LIB AND INCLUDE ########### #
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

# --- Compiler Arguments (Cross-Platform) ---
add_library(VknConfig_args INTERFACE)
target_compile_features(VknConfig_args INTERFACE cxx_std_20)

if(CMAKE_BUILD_TYPE MATCHES Release)
    if(MSVC)
        target_compile_options(VknConfig_args INTERFACE /O2) # MSVC Release optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(VknConfig_args INTERFACE -O3) # GCC Release optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(VknConfig_args INTERFACE -O3) # Clang Release optimization
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    if(MSVC)
        target_compile_options(VknConfig_args INTERFACE /Od /Zi) # MSVC Debug optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(VknConfig_args INTERFACE -Og -g) # GCC Debug optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(VknConfig_args INTERFACE -Og -g) # Clang Debug optimization
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    if(MSVC)
        target_compile_options(VknConfig_args INTERFACE /O2 /Zi) # MSVC RelWithDebInfo optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(VknConfig_args INTERFACE -O3 -g) # GCC RelWithDebInfo optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(VknConfig_args INTERFACE -O3 -g) # Clang RelWithDebInfo optimization
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    if(MSVC)
        target_compile_options(VknConfig_args INTERFACE /O1) # MSVC MinSizeRel optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(VknConfig_args INTERFACE -Os) # GCC MinSizeRel optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(VknConfig_args INTERFACE -Os) # Clang MinSizeRel optimization
    endif()
endif()

# --- Lib Dirs ---
if(CMAKE_BUILD_TYPE MATCHES Debug)
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib
    )
elseif(CMAKE_BUILD_TYPE MATCHES Release OR
    CMAKE_BUILD_TYPE MATCHES RelWithDebInfo OR
    CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    link_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
    )
else()
    message("Unknown build type.")
endif()

# --- Single-header libraries
add_library(vma INTERFACE)
target_sources(vma INTERFACE single-headers/vma.cpp)

# --- subdir ---
add_subdirectory(VknConfig)

# add_subdirectory(tests)

# add_subdirectory(VkCore)
buildSubDir(VkExtCheck)
buildSubDir(minTest)
