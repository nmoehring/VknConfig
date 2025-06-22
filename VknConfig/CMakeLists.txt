project(VknConfig)

# Get VknConfig library started
add_library(VknConfig STATIC VknConfig.cpp VknDevice.cpp VknInfos.cpp
    VknResult.cpp VknPhysicalDevice.cpp VknPipeline.cpp VknRenderpass.cpp
    VknVertexInputState.cpp VknInputAssemblyState.cpp VknMultisampleState.cpp
    VknShaderStage.cpp VknRasterizationState.cpp VknQueueFamily.cpp
    VknViewportState.cpp VknSwapchain.cpp VknImageView.cpp
    VknFramebuffer.cpp VknEngine.cpp VknData.cpp VknPipelineLayout.cpp
    VknDescriptorSetLayout.cpp VknImage.cpp VknFeatures.cpp
    VknColorBlendState.cpp VknCommandPool.cpp VknApp.cpp VknCycle.cpp
    presets/NoInput.cpp presets/DeviceInfo.cpp VknDynamicState.cpp
    VknPlatforms.cpp VknBuffer.cpp VknObject.cpp)

if(ANDROID)
    message(STATUS "VknConfig Android: NativeActivity mode is ${VKNCONFIG_ANDROID_NATIVE_ACTIVITY}")
    target_link_libraries(VknConfig PRIVATE
        Vulkan::Vulkan
        log # For logging
        android # # For ANativeWindow, AAssetManager, etc.
        jnigraphics # # Specifically if using ANativeWindow_fromSurface
    )

    if(VKNCONFIG_ANDROID_NATIVE_ACTIVITY)
        message(STATUS "VknConfig: Compiling with Native Activity window (Android_Window.cpp)")
        target_sources(VknConfig PRIVATE PlatformSpecific/Android_Window.cpp)
        target_link_libraries(VknConfig PRIVATE android_native_app_glue)
        target_compile_definitions(VknConfig PRIVATE VKN_NATIVE_ACTIVITY_MODE=1)
    else()
        message(STATUS "VknConfig: Compiling with JNI window (Android_WindowJNI.cpp)")
        target_sources(VknConfig PRIVATE PlatformSpecific/Android_WindowJNI.cpp)
        target_compile_definitions(VknConfig PRIVATE VKN_NATIVE_ACTIVITY_MODE=0)
    endif()
elseif(PLATFORM_DESKTOP)
    message(STATUS "VknConfig: Compiling with GLFW window (GLFW_Window.cpp)")
    target_sources(VknConfig PRIVATE PlatformSpecific/GLFW_Window.cpp)
    target_link_libraries(VknConfig PRIVATE glfw ${Vulkan_LIBRARIES})
endif()

target_link_libraries(VknConfig PRIVATE VknConfig_args vma)
target_include_directories(VknConfig PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Apply GTEST_ENABLED definition based on the flag from the parent CMakeLists.txt
if(VKNCONFIG_GTEST_ENABLED_FLAG)
    target_compile_definitions(VknConfig PRIVATE GTEST_ENABLED=1)
else()
    target_compile_definitions(VknConfig PRIVATE GTEST_ENABLED=0)
endif()