cmake_minimum_required(VERSION 3.29)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(VkCkBk VERSION 0.1 LANGUAGES CXX)
message("Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")

# ===============INCLUDE MODULES & PACKAGES===================== #
message("================ Finding modules and packages. ======================")
include(FetchContent)
find_package(Vulkan REQUIRED SPIRV-Tools)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
message("Vulkan header version: ${VulkanHeaderVersion}")

# =============CREATE COMPILE FLAGS INTERFACE ================== #
add_library(vkckbk_compiler_flags INTERFACE)
target_compile_features(vkckbk_compiler_flags INTERFACE cxx_std_20)

# override the absl install option so that the symbols needed for re2 to build get exported
set(ABSL_ENABLE_INSTALL ON CACHE BOOL "Enable install rule" FORCE)

# =============DECLARE DEPENDENCIES============= #
message("================ Declaring dependencies ======================")
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git)
FetchContent_Declare(gli GIT_REPOSITORY https://github.com/g-truc/gli.git)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git)
FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git)
FetchContent_Declare(volk GIT_REPOSITORY https://github.com/zeux/volk.git)
FetchContent_Declare(vma GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git)
FetchContent_Declare(absl GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git)
FetchContent_Declare(pthread GIT_REPOSITORY https://github.com/GerHobbelt/pthread-win32.git)

# These repositories use a 'main' branch rather than a 'master' branch
FetchContent_Declare(spirv-headers
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
    GIT_TAG origin/main)
FetchContent_Declare(spirv-tools
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
    GIT_TAG origin/main)
FetchContent_Declare(spirv-reflect
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
    GIT_TAG origin/main)
FetchContent_Declare(glslang
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang
    GIT_TAG origin/main)
FetchContent_Declare(effcee
    GIT_REPOSITORY https://github.com/google/effcee.git
    GIT_TAG origin/main)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG origin/main)
FetchContent_Declare(re2
    GIT_REPOSITORY https://github.com/google/re2.git
    GIT_TAG origin/main)
FetchContent_Declare(llvm
    GIT_REPOSITORY https://github.com/llvm/llvm-project.git
    GIT_TAG origin/main
    SOURCE_SUBDIR llvm)

# =============POPULATE DEPENDENCIES============== #
message("================ Populating dependencies ======================")
FetchContent_MakeAvailable(googletest pthread llvm spirv-headers spirv-tools spirv-reflect glslang volk vma glfw glm gli stb)

# FetchContent_Populate(absl)
# add_subdirectory()
# add_subdirectory(${absl_SOURCE_DIR} ${absl_BINARY_DIR} EXCLUDE_FROM_ALL)
# FetchContent_MakeAvailable(re2 effcee)

# I just keep getting "no such file or directory". It doesn't come from cmd, so I imagine it's CMake.
# Python gives a "No such file or directory" which starts with a capitalized 'No', so it probably isn't it.
message("CMAKE_COMMAND: ${CMAKE_COMMAND}, CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}, CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Check if the patch file exists
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/scripts/patch_gli_cmake_version.py")
    message(FATAL_ERROR "Patch file not found: ${CMAKE_SOURCE_DIR}/patches/my_patch.patch")
endif()

# Run a Python script to manually patch the minimum required version of GLI
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/patch_gli_cmake_version.py 3.29
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/_deps/gli-source/
    OUTPUT_VARIABLE gli_patch_output
    RESULT_VARIABLE gli_patch_result
    ERROR_VARIABLE error_output)
message(DEBUG ${gli_patch_output})
message(DEBUG ${gli_patch_result})
message(DEBUG ${gli_patch_error_output})

# ==============INCLUDE DEPENDENCY DIRECTORIES================= #
include_directories(

    # supposedly I don't need these because of MakeAvailable, try that later.
    ${CMAKE_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIR}
    vulkancore
    enginecore
    ${spirv-tools_SOURCE_DIR}/include
    ${spirv-headers_SOURCE_DIR}/include
    ${spirv-reflect_SOURCE_DIR}/include
    ${glslang_SOURCE_DIR}/glslang/include
    ${effcee_SOURCE_DIR}
    ${googletest_SOURCE_DIR}/googletest/include
    ${vma_SOURCE_DIR}/include
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
