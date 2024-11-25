cmake_minimum_required(VERSION 3.29)

project(Chapter1 VERSION 0.1 LANGUAGES CXX)

add_executable(Chapter1 main.cpp)

target_link_libraries(Chapter1
    PUBLIC vkckbk_compiler_flags
    PUBLIC VulkanCore
    PUBLIC glfw
)

file(COPY resources DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")