project(VknConfig)
add_library(VknConfig VknConfig.cpp VknDevice.cpp VknInfos.cpp
    VknResult.cpp VknPhysicalDevice.cpp VknPipeline.cpp VknRenderpass.cpp
    VknVertexInputState.cpp VknInputAssemblyState.cpp VknMultisampleState.cpp
    VknShaderStage.cpp VknRasterizationState.cpp VknQueueFamily.cpp
    VknViewportState.cpp VknSwapchain.cpp VknImageView.cpp
    VknFramebuffer.cpp VknEngine.cpp VknCommon.cpp)
target_link_libraries(VknConfig vknconfig_compiler_args vulkan-1)
target_include_directories(VknConfig PUBLIC $CMAKE_CURRENT_SOURCE_DIR}/include)