cmake_minimum_required(VERSION 3.29)
project(VulkanCore)
add_library(${PROJECT_NAME}
    Common.cpp
    PhysicalDevice.cpp
    CommandQueueManager.cpp
    Buffer.cpp
    ShaderModule.cpp
    Swapchain.cpp
    Pipeline.cpp
    vma.cpp
    Utility.cpp
    Context.cpp
    Texture.cpp
    Framebuffer.cpp
    RenderPass.cpp
)
target_link_libraries(${PROJECT_NAME} PUBLIC vkckbk_compiler_flags INTERFACE volk)
message(STATUS "Vulkan DIR: " ${Vulkan_INCLUDE_DIR})
include_directories(${Vulkan_INCLUDE_DIR})
