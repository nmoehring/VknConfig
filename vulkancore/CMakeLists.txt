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
    Utility.cpp
    Context.cpp
    Texture.cpp
    Framebuffer.cpp
    RenderPass.cpp
    Sampler.cpp
    vma.cpp
)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${PROJECT_NAME} 
        PUBLIC vkckbk_compiler_flags 
        PRIVATE volk 
        PRIVATE glslangd
        PRIVATE glslang-default-resource-limitsd
    )
else()
    target_link_libraries(${PROJECT_NAME} 
        PUBLIC vkckbk_compiler_flags 
        PRIVATE volk 
        PRIVATE glslang
        PRIVATE glslang-default-resource-limits
    )
endif()
message(STATUS "Vulkan DIR: " ${Vulkan_INCLUDE_DIR})
include_directories(${Vulkan_INCLUDE_DIR})
