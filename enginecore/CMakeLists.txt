cmake_minimum_required(VERSION 3.29)
project(EngineCore)
add_library(
    ${PROJECT_NAME} 
    AsyncDataUploader.cpp
    Camera.cpp
    GLBLoader.cpp
    GLFWUtils.cpp
    ImguiManager.cpp
    Model.cpp
    RingBuffer.cpp
)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("Linking debug libraries for EngineCore.")
    target_link_libraries(
        ${PROJECT_NAME} 
        vkckbk_compiler_flags
        GLTFSDK
        imguid
        meshoptimizer
        imgui_impl_glfw
        imgui_impl_vulkan
    )
else()
    message("Linking release libraries for EngineCore.")
    target_link_libraries(
        ${PROJECT_NAME} 
        vkckbk_compiler_flags
        GLTFSDK
        imgui
        meshoptimizer
        imgui_impl_glfw
        imgui_impl_vulkan   
    )
endif()