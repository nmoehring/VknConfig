project(minTest)
add_executable(minTest minTest.cpp)
target_link_libraries(minTest vknconfig_compiler_args VknConfig)
copyResources()