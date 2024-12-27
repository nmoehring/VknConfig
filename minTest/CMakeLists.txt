project(minTest)

add_executable(Direct-minTest Direct-minTest.cpp)
add_executable(CkBk-minTest CkBk-minTest.cpp)
add_executable(VoxTest VoxTest.cpp VoxTree.cpp)

target_link_libraries(Direct-minTest PRIVATE vkckbk_compiler_flags PRIVATE vulkan-1)
target_link_libraries(CkBk-minTest PRIVATE vkckbk_compiler_flags PRIVATE VknConfig)
target_link_libraries(VoxTest PRIVATE vkckbk_compiler_flags)
copyResources()