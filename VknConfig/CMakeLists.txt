project(VknConfig)

set(VKNCONFIG_LIB_TYPE STATIC) # Default to STATIC

if(ANDROID)
    set(VKNCONFIG_LIB_TYPE SHARED)
endif()

add_library(VknConfig ${VKNCONFIG_LIB_TYPE} VknConfig.cpp VknDevice.cpp VknInfos.cpp
    VknResult.cpp VknPhysicalDevice.cpp VknPipeline.cpp VknRenderpass.cpp
    VknVertexInputState.cpp VknInputAssemblyState.cpp VknMultisampleState.cpp
    VknShaderStage.cpp VknRasterizationState.cpp VknQueueFamily.cpp
    VknViewportState.cpp VknSwapchain.cpp VknImageView.cpp
    VknFramebuffer.cpp VknEngine.cpp VknData.cpp VknPipelineLayout.cpp
    VknDescriptorSetLayout.cpp VknImage.cpp VknFeatures.cpp
    VknColorBlendState.cpp VknCommandPool.cpp VknApp.cpp VknCycle.cpp
    presets/NoInput.cpp presets/DeviceInfo.cpp VknDynamicState.cpp)

if(ANDROID)
    target_sources(VknConfig PRIVATE native-lib.cpp)
    target_link_libraries(VknConfig PRIVATE
        log # For logging
        android # # For ANativeWindow, AAssetManager, etc.
        jnigraphics # # Specifically if using ANativeWindow_fromSurface
        vulkan
    )
elseif(PLATFORM_PC)
    target_link_libraries(VknConfig PUBLIC ${Vulkan_LIBRARIES}) # Android provides its own libraries
endif()

target_link_libraries(VknConfig PRIVATE VknConfig_args)
target_include_directories(VknConfig PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)