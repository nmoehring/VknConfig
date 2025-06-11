cmake_minimum_required(VERSION 3.22)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VknConfig VERSION 0.5 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

# Option to enable/disable tests, can be configured from Android Studio's build.gradle
option(ENABLE_TESTING "Enable GTest for the project" ON)

# --- Platform detection ---
if(ANDROID)
    message("Android build detected")

    if(NOT DEFINED VKNCONFIG_ANDROID_NATIVE_ACTIVITY)
        set(VKNCONFIG_ANDROID_NATIVE_ACTIVITY FALSE)
    endif()
elseif(WIN32)
    # WIN32 is defined by CMake for both 32-bit and 64-bit Windows.
    # If you need to distinguish, you can use CMAKE_SIZEOF_VOID_P.
    # For example: if(CMAKE_SIZEOF_VOID_P EQUAL 8) message(" 64-bit Windows ") else() message(" 32-bit Windows ") endif()
    set(PLATFORM_PC TRUE)
    set(PLATFORM_DESKTOP TRUE)
    message(" Windows(WIN32) build detected ")
elseif(MACOS)
    message(" MACOS build detected ")
    set(PLATFORM_DESKTOP TRUE)
elseif(LINUX AND NOT ANDROID)
    set(PLATFORM_LINUX TRUE)
    set(PLATFORM_PC TRUE)
    set(PLATFORM_DESKTOP TRUE)
    message(" Linux build detected ")
else()
    set(PLATFORM_UNKNOWN TRUE)
    message(" Unknown platform build detected ")
endif()

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

# --- Create build dir ---
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

# --- DEPENDENCIES ---
include(FetchContent)
find_package(Vulkan REQUIRED)

message(STATUS " Vulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS} ")
message(STATUS " Vulkan_LIBRARIES: ${Vulkan_LIBRARIES} ")

if(PLATFORM_DESKTOP)
    find_package(glfw3 CONFIG REQUIRED)
elseif(ANDROID)
    find_library(log-lib # Variable to store the path
        log) # Name of the library to find (liblog.so)

    find_library(android-lib # Variable
        android) # Name of the library (libandroid.so)

    # For ANativeWindow access, especially if you're getting it from a Java Surface
    find_library(jnigraphics-lib # For ANativeWindow_fromSurface
        jnigraphics)

    # Example: If using OpenGL ES
    # find_library(opengles2-lib EGL GLESv2) # Find EGL and GLESv2
endif()

# Find GTest if testing is enabled. This will make GTest targets available.
# The Android app's native library target (which includes native-lib.cpp)
# will need to be linked against GTest::gtest and GTest::gtest_main.
if(ENABLE_TESTING)
    find_package(GTest CONFIG)

    if(GTest_FOUND)
        message(STATUS " Found GTest via find_package(version ${GTest_VERSION}) ")
        include(GoogleTest)

        if(MSVC AND PLATFORM_DESKTOP) # MSVC specific flag for desktop
            set(gtest_force_shared_crt ON CACHE BOOL "Force shared CRT for GTest on MSVC" FORCE)
        endif()

        set(VKNCONFIG_GTEST_ENABLED_FLAG ON) # Pass this to child scopes
    else()
        message(WARNING "GTest not found. Testing will be disabled.")
        set(ENABLE_TESTING OFF) # Disable testing if GTest is not found
        set(VKNCONFIG_GTEST_ENABLED_FLAG OFF) # Pass this to child scopes
    endif()
endif()

# ########## LIB AND INCLUDE ########### #
message(" Vulkan header version: ${VulkanHeaderVersion} ")
message(" Vulkan include directory: ${Vulkan_INCLUDE_DIRS} ")
message(" ^If this points to some other installed Vulkan header files and not your SDK install,
        maybe delete them and the CMakeCache and try again. ")
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

if(PLATFORM_DESKTOP)
    include_directories(
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
        ${glfw3_DIR}/include
    )
endif()

# --- Compiler Arguments (Cross-Platform) ---
add_library(VknConfig_args INTERFACE)
target_compile_features(VknConfig_args INTERFACE cxx_std_20)

if(CMAKE_BUILD_TYPE MATCHES Release)
    if(MSVC)
        target_compile_options(VknConfig_args INTERFACE /O2) # MSVC Release optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES " GNU ")
        target_compile_options(VknConfig_args INTERFACE -O3) # GCC Release optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES " Clang ")
        target_compile_options(VknConfig_args INTERFACE -O3) # Clang Release optimization
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    if(MSVC)
        target_compile_options(VknConfig_args INTERFACE /Od /Zi) # MSVC Debug optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES " GNU ")
        target_compile_options(VknConfig_args INTERFACE -Og -g) # GCC Debug optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES " Clang ")
        target_compile_options(VknConfig_args INTERFACE -Og -g) # Clang Debug optimization
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    if(MSVC)
        target_compile_options(VknConfig_args INTERFACE /O2 /Zi) # MSVC RelWithDebInfo optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES " GNU ")
        target_compile_options(VknConfig_args INTERFACE -O3 -g) # GCC RelWithDebInfo optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES " Clang ")
        target_compile_options(VknConfig_args INTERFACE -O3 -g) # Clang RelWithDebInfo optimization
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    if(MSVC)
        target_compile_options(VknConfig_args INTERFACE /O1) # MSVC MinSizeRel optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES " GNU ")
        target_compile_options(VknConfig_args INTERFACE -Os) # GCC MinSizeRel optimization
    elseif(CMAKE_CXX_COMPILER_ID MATCHES " Clang ")
        target_compile_options(VknConfig_args INTERFACE -Os) # Clang MinSizeRel optimization
    endif()
endif()

# --- Lib Dirs ---
if(PLATFORM_DESKTOP)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        link_directories(
            ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib
            ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin
        )
    elseif(CMAKE_BUILD_TYPE MATCHES Release OR
        CMAKE_BUILD_TYPE MATCHES RelWithDebInfo OR
        CMAKE_BUILD_TYPE MATCHES MinSizeRel)
        link_directories(
            ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
            ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin
        )
    else()
        message(" Unknown build type. ")
    endif()
endif()

# --- Single-header libraries
add_library(vma INTERFACE)
target_sources(vma INTERFACE single-headers/vma.cpp)

# --- subdir ---
add_subdirectory(VknConfig)

if(ENABLE_TESTING AND PLATFORM_DESKTOP)
    add_subdirectory(tests)
endif()

if(PLATFORM_DESKTOP)
    add_subdirectory(minTest)
endif()