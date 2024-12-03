project(VknConfig)
add_library(VknConfig VknConfig.cpp VknInfos.cpp VknResult.cpp)
target_link_libraries(VknConfig vkckbk_compiler_flags vulkan-1)