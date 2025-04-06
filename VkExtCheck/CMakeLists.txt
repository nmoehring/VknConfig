project(VkExtCheck)
add_executable(VkExtCheck VkExtCheck.cpp)
target_link_libraries(VkExtCheck PRIVATE VknConfig_args PRIVATE ${Vulkan_LIBRARIES})
copyResources()