#include <vector>
#include <iostream>
#include <filesystem>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stb_image.h>

#include "VknConfig/VknConfig.hpp"

// GLFWwindow *window_ = nullptr;

void printDevProps(std::vector<VkPhysicalDeviceProperties> devProps);
void printFamProps(std::vector<VkQueueFamilyProperties> famProps);
std::vector<char> readFile(const std::string &filename);
bool initWindow(GLFWwindow **outWindow);
int main()
{
    vkn::VknConfig vknConfig{};
    vknConfig.fillAppInfo(VK_API_VERSION_1_1, "MinTest", "MinVknEngine");
    vknConfig.createInstance();
    vknConfig.createDevice();
    auto infos = vknConfig.getInfos();

    int idx = 0;
    for (auto queue : vknConfig.getDevice()->getQueues())
    {
        std::cout << "Queue " << idx << ": " << std::endl;
        std::cout << "Graphics: " << queue.supportsGraphics() << std::endl;
        std::cout << "Compute: " << queue.supportsCompute() << std::endl;
        std::cout << "Transfer: " << queue.supportsTransfer() << std::endl;
        std::cout << "Sparse Binding: " << queue.supportsSparseBinding() << std::endl;
        std::cout << "Mem Protection: " << queue.supportsMemoryProtection() << std::endl;
        ++idx;
    }

    // Pipeline experiments
    // auto layoutCreateInfo{infos->fillPipelineLayoutCreateInfo()};
    // auto cacheCreateInfos{infos->fillPipelineCacheCreateInfo()};

    std::unordered_map<vkn::ShaderStage, std::string> stages{
        {vkn::VKN_VERTEX_STAGE, "triangle.vert"},
        {vkn::VKN_FRAGMENT_STAGE, "triangle.frag"}};
    std::vector<int> idxs;
    for (auto stage : stages)
    {
        int idx = vknConfig.getPipeline()->createShaderStage(stage.first, stage.second);
        idxs.push_back(idx);
    }
    /*auto vertexInputStateCreateInfos{infos->fillVertexInputStateCreateInfo()};
    auto inputAssemblyStateCreateInfos{infos->fillInputAssemblyStateCreateInfo()};
    auto tessellationStateCreateInfos{infos->fillTessellationStateCreateInfo()};
    auto viewportStateCreateInfos{infos->fillViewportStateCreateInfo()};
    auto rasterizationStateCreateInfos{infos->fillRasterizationStateCreateInfo()};
    auto multisampleStateCreateInfos{infos->fillMultisampleStateCreateInfo()};
    auto depthStencilStateCreateInfos{infos->fillDepthStencilStateCreateInfo()};
    auto colorBlendStateCreateInfos{infos->fillColorBlendStateCreateInfo()}; */
    auto stageInfos{vknConfig.getPipeline()->getShaderStageInfos(idxs)};
    auto gfxPipelineCreateInfo{
        infos->fillGfxPipelineCreateInfo(stageInfos)
        /*, layoutCreateInfo, renderpass, subpass, basepipelinehandle,
            basepipelineidx, flags, vertexInputStateCreateInfo, inputAssemblyStateCreateInfo,
            tessellationStateCreateInfo, viewportStateCreateInfo, rasterizationStateCreateInfo,
            multisampleStateCreateInfo, depthStencilStateCreateInfo, colorBlendStateCreateInfo);
        */
    };

    vknConfig.getPipeline()->createPipeline(gfxPipelineCreateInfo);

    GLFWwindow *window_ = nullptr;
    initWindow(&window_);
}

bool initWindow(GLFWwindow **outWindow)
{
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    const char *title = "Minimum VknConfig Test";

    uint32_t posX = 200;
    uint32_t posY = 200;
    uint32_t width = 800;
    uint32_t height = 600;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwSetWindowPos(window, posX, posY);

    glfwSetErrorCallback([](int error, const char *description)
                         { printf("GLFW Error (%i): %s\n", error, description); });

    glfwSetKeyCallback(
        window, [](GLFWwindow *window, int key, int, int action, int mods)
        {
        const bool pressed = action != GLFW_RELEASE;
        if (key == GLFW_KEY_ESCAPE && pressed) {
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (key == GLFW_KEY_ESCAPE && pressed)
          glfwSetWindowShouldClose(window, GLFW_TRUE); });

#if defined(WIN32)
    HWND hwnd = glfwGetWin32Window(window);
    SetWindowLongPtr(
        hwnd, GWL_STYLE,
        GetWindowLongPtrA(hwnd, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX));
#endif

    if (outWindow)
    {
        *outWindow = window;
    }

    return true;
}