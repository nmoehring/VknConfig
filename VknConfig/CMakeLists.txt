project(VknConfig)
add_library(VknConfig VknConfig.cpp VknDevice.cpp VknInfos.cpp
    VknResult.cpp VknPhysicalDevice.cpp VknPipeline.cpp VknRenderpass.cpp
    VknVertexInputState.cpp VknInputAssemblyState.cpp VknMultisampleState.cpp
    VknShaderStage.cpp VknRasterizationState.cpp VknQueueFamily.cpp
    VknViewportState.cpp VknSwapchain.cpp VknImageView.cpp
    VknFramebuffer.cpp VknEngine.cpp VknData.cpp VknPipelineLayout.cpp
    VknDescriptorSetLayout.cpp VknImage.cpp VknFeatures.cpp
    VknColorBlendState.cpp VknCommandPool.cpp VknApp.cpp VknCycle.cpp
    presets/NoInput.cpp presets/DeviceInfo.cpp VknDynamicState.cpp)

if(CMAKE_BUILD_TYPE MATCHES Debug)
# target_link_directories(VknConfig PUBLIC ${Vulkan_LIBRARIES})
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo OR
    CMAKE_BUILD_TYPE MATCHES Release OR
    CMAKE_BUILD_TYPE MATCHES MinSizeRel)
# target_link_directories(VknConfig PUBLIC ${Vulkan_LIBRARIES})
else()
    message("Unknown build type.")
endif()

target_link_libraries(VknConfig VknConfig_args ${Vulkan_LIBRARIES})
target_include_directories(VknConfig PUBLIC $CMAKE_CURRENT_SOURCE_DIR}/include)