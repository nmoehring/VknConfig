cmake_minimum_required(VERSION 3.29)
project(Chapter1 VERSION 1.0 LANGUAGES CXX)
add_executable(Chapter1_Triangle main.cpp)
target_link_libraries(
    Chapter1_Triangle
    PRIVATE vkckbk_compiler_flags
    PRIVATE VulkanCore
    PRIVATE glfw
)
copyResources()