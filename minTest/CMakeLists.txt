project(minTest)
add_executable(minTest minTest.cpp)
target_link_libraries(minTest VknConfig_args VknConfig glfw)

# project(minGLFWtest)
# add_executable(minGLFWtest minGLFWtest.cpp)
# target_link_libraries(minGLFWtest vulkan-1 vknconfig_compiler_args glfw)
copyResources()