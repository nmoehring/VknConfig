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
target_link_libraries(
    ${PROJECT_NAME} 
    vkckbk_compiler_flags
    GLTFSDK
    imgui
)