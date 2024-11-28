project(minTest)

add_executable(Direct-minTest Direct-minTest.cpp)

# add_executable(Cookbook-minTest Cookbook-minTest.cpp)
# add_executable(CkBk-minTest CkBk-minTest.cpp)
target_link_libraries(Direct-minTest PRIVATE vkckbk_compiler_flags PRIVATE vulkan-1)

# target_link_libraries(Cookbook-minTest PRIVATE vkckbk_compiler_flags PRIVATE vulkan-1)
# target_link_libraries(CkBk-minTest PRIVATE vkckbk_compiler_flags PRIVATE vulkan-1)
copyResources()