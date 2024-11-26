cmake_minimum_required(VERSION 3.29)

project(imgui_impl_vulkan)
project(imgui_impl_glfw)

add_library(imgui_impl_vulkan imgui_impl_vulkan.cpp)
add_library(imgui_impl_glfw imgui_impl_glfw.cpp)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Linking debug libraries for imgui_impl_vulkan.")
    target_link_libraries(imgui_impl_vulkan imguid)
    target_link_libraries(imgui_impl_glfw imguid)
else()
    message("Linking release libraries for imgui_impl_vulkan.")
    target_link_libraries(imgui_impl_vulkan imgui)
    target_link_libraries(imgui_impl_glfw imgui)
endif()