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
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<std::string> extensions;
    for (int i = 0; i < glfwExtensionCount; ++i)
    {
        extensions.push_back(glfwExtensions[i]);
    }
    vknConfig.enableExtensions(extensions);
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
        {vkn::VKN_VERTEX_STAGE, "simple_shader.vert.spv"},
        {vkn::VKN_FRAGMENT_STAGE, "simple_shader.frag.spv"}};
    std::vector<int> idxs;
    for (auto stage : stages)
    {
        int idx = vknConfig.getPipeline()->createShaderStage(stage.first, stage.second);
        idxs.push_back(idx);
    }
    auto vertexInputStateCreateInfo{infos->fillDefaultVertexInputState()};
    /*auto inputAssemblyStateCreateInfos{infos->fillInputAssemblyStateCreateInfo()};
    auto tessellationStateCreateInfos{infos->fillTessellationStateCreateInfo()};
    auto viewportStateCreateInfos{infos->fillViewportStateCreateInfo()};
    auto rasterizationStateCreateInfos{infos->fillRasterizationStateCreateInfo()};
    auto multisampleStateCreateInfos{infos->fillMultisampleStateCreateInfo()};
    auto depthStencilStateCreateInfos{infos->fillDepthStencilStateCreateInfo()};
    auto colorBlendStateCreateInfos{infos->fillColorBlendStateCreateInfo()}; */
    auto stageInfos{vknConfig.getPipeline()->getShaderStageInfos(idxs)};
    VkAttachmentReference colorAttachmentRef = vknConfig.getPipeline()->createAttachment();
    vknConfig.getPipeline()->createSubpass(
        VkSubpassDescriptionFlagBits{}, VK_PIPELINE_BIND_POINT_GRAPHICS,
        std::vector<VkAttachmentReference>{colorAttachmentRef});
    vknConfig.getPipeline()->createSubpassDependency();
    VkPipelineLayoutCreateInfo layoutInfo = vknConfig.getPipeline()->fillPipelineLayoutCreateInfo();
    VkPipelineLayout vknConfig.getPipeline()->createLayout(layoutInfo);
    VkRenderPassCreateInfo rpCreateInfo = vknConfig.getPipeline()->fillRenderPassCreateInfo();
    VkRenderPass rp = vknConfig.getPipeline()->createRenderPass(&rpCreateInfo);
    auto gfxPipelineCreateInfo{
        infos->fillGfxPipelineCreateInfo(stageInfos, , renderPass)
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