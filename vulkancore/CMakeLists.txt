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
    DynamicRendering.cpp
)

# Deal with debug libraries suffixed with 'd'
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Linking debug libraries for VulkanCore.")
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE vkckbk_compiler_flags
        INTERFACE vma
        PRIVATE volk 
        PRIVATE glslangd
        PRIVATE glslang-default-resource-limitsd
    )
else()
    message("Linking release libraries for VulkanCore.")
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE vkckbk_compiler_flags 
        INTERFACE vma
        PRIVATE volk 
        PRIVATE glslang
        PRIVATE glslang-default-resource-limits
    )
endif()
