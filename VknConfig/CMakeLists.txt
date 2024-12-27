project(VknConfig)
add_library(VknConfig VknConfig.cpp VknDevice.cpp VknInfos.cpp VknResult.cpp VknPhysicalDevice.cpp)
target_link_libraries(VknConfig vkckbk_compiler_flags vulkan-1)