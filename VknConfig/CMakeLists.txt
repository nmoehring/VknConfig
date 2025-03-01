project(VknConfig)
add_library(VknConfig VknConfig.cpp VknDevice.cpp VknInfos.cpp
    VknResult.cpp VknPhysicalDevice.cpp VknPipeline.cpp VknRenderPass.cpp 
    VknVertexInputState.cpp VknInputAssemblyState.cpp)
target_link_libraries(VknConfig vknconfig_compiler_args vulkan-1)