project(VkExtCheck)
add_executable(VkExtCheck VkExtCheck.cpp)
target_link_libraries(VkExtCheck PRIVATE vknconfig_compiler_args PRIVATE vulkan-1)
copyResources()