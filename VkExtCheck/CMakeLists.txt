project(VkExtCheck)
add_executable(VkExtCheck VkExtCheck.cpp)
target_link_libraries(VkExtCheck PRIVATE vkckbk_compiler_flags PRIVATE vulkan-1)
copyResources()