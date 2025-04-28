project(minTest)
add_executable(minTest minTest.cpp)
target_link_libraries(minTest VknConfig_args VknConfig glfw)

copyResources()