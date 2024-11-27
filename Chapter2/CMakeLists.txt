cmake_minimum_required(VERSION 3.29)
project(Chapter2 VERSION 0.1 LANGUAGES CXX)

set (
    recipe_names
     "Bindfull"
     "MeshOptimized"
     "MultiDrawIndirect"
)

foreach (recipe ${recipe_names})
    add_executable(Chapter2_${recipe} Chapter2_${recipe}.cpp)
    target_link_libraries(
        Chapter2_${recipe}       
        PRIVATE vkckbk_compiler_flags
        PRIVATE VulkanCore
        PRIVATE EngineCore
        PRIVATE glfw
    )
endforeach()

copyResources()