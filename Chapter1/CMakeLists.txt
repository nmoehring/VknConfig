cmake_minimum_required(VERSION 3.29)

project(Chapter1 VERSION 0.9 LANGUAGES CXX)

add_executable(Chapter1 main.cpp)

target_link_libraries(Chapter1
    PRIVATE vkckbk_compiler_flags
    PRIVATE vma
    PRIVATE VulkanCore
    PRIVATE glfw
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    file(COPY resources DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
else()
    file(COPY resources DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()