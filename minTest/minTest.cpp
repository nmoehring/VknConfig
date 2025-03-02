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
std::vector<const char *> noNames{};
bool initWindow(GLFWwindow **outWindow);

int main()
{
    vkn::VknConfig config{};
    vkn::VknDevice *device = config.getDevice(0);
    vkn::VknInfos *infos = config.getInfos();

    std::string appName{"MinTest"};
    std::string engineName{"MinVknConfig"};
    config.fillAppInfo(VK_API_VERSION_1_1, appName, engineName);

    const uint32_t instanceExtensionsSize{1};
    const char *instanceExtensions[instanceExtensionsSize] = {
        // VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        // VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME};
    const uint32_t layersSize{0};
    const char *layers[]{nullptr};

    config.fillInstanceCreateInfo(
        layers, layersSize, instanceExtensions, instanceExtensionsSize);
    config.createInstance();

    config.selectPhysicalDevice(0);
    config.requestQueueFamilies(0);
    auto limits{device->getPhysicalDevice()->getLimits()};
    std::cout << "maxVertexInputBindings=" << limits->maxVertexInputBindings << std::endl;
    std::cout << "maxVertexInputAttributes=" << limits->maxVertexInputAttributes << std::endl;
    const uint32_t numExtensions{1};
    const char *deviceExtensions[numExtensions] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    device->addExtensions(deviceExtensions, numExtensions);

    // VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
    // VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    // VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};
    config.selectQueues(0);
    device->fillDeviceQueuePrioritiesDefault();
    config.createDevice(0);

    int idx = 0;
    for (auto queue : device->getQueues())
    {
        std::cout << "Queue " << idx << ": " << std::endl;
        std::cout << "Graphics: " << queue.supportsGraphics() << std::endl;
        std::cout << "Compute: " << queue.supportsCompute() << std::endl;
        std::cout << "Transfer: " << queue.supportsTransfer() << std::endl;
        std::cout << "Sparse Binding: " << queue.supportsSparseBinding() << std::endl;
        std::cout << "Mem Protection: " << queue.supportsMemoryProtection() << std::endl;
        ++idx;
    }

    // RenderPass experiments
    // auto layoutCreateInfo{infos->fillPipelineLayoutCreateInfo()};
    // auto cacheCreateInfos{infos->fillPipelineCacheCreateInfo()};

    device->addRenderPass();
    vkn::VknRenderPass *renderPass = device->getRenderPass(0);
    std::vector<VkAttachmentReference *> attach;
    renderPass->createAttachment();
    renderPass->createSubpass();
    renderPass->createRenderPass();

    vkn::VknPipeline *pipeline = renderPass->getPipeline(0);
    pipeline->addShaderStage(vkn::VKN_VERTEX_STAGE, "simple_shader.vert.spv");
    pipeline->addShaderStage(vkn::VKN_FRAGMENT_STAGE, "simple_shader.frag.spv");
    vkn::VknVertexInputState *vertexInputState = pipeline->getVertexInputState();
    // vertexInputState->fillVertexAttributeDescription();
    // vertexInputState->fillVertexBindingDescription();
    vertexInputState->fillVertexInputStateCreateInfo();
    vkn::VknInputAssemblyState *inputAssemblyState = pipeline->getInputAssemblyState();
    inputAssemblyState->fillInputAssemblyStateCreateInfo();
    vkn::VknMultisampleState *multisampleState = pipeline->getMultisampleState();
    multisampleState->fillMultisampleStateCreateInfo();
    vkn::VknRasterizationState *rasterizationState = pipeline->getRasterizationState();
    rasterizationState->fillRasterizationStateCreateInfo();
    vkn::VknViewportState *viewportState = pipeline->getViewportState();
    viewportState->addViewport();
    viewportState->addScissor();
    viewportState->fillViewportStateCreateInfo();
    /*auto inputAssemblyStateCreateInfos{infos->fillInputAssemblyStateCreateInfo()};
    auto tessellationStateCreateInfos{infos->fillTessellationStateCreateInfo()};
    auto viewportStateCreateInfos{infos->fillViewportStateCreateInfo()};
    auto rasterizationStateCreateInfos{infos->fillRasterizationStateCreateInfo()};
    auto multisampleStateCreateInfos{infos->fillMultisampleStateCreateInfo()};
    auto depthStencilStateCreateInfos{infos->fillDepthStencilStateCreateInfo()};
    auto colorBlendStateCreateInfos{infos->fillColorBlendStateCreateInfo()}; */
    pipeline->fillPipelineLayoutCreateInfo();
    pipeline->createLayout();
    pipeline->fillPipelineCreateInfo();
    /*, layoutCreateInfo, renderpass, subpass, basepipelinehandle,
        basepipelineidx, flags, vertexInputStateCreateInfo, inputAssemblyStateCreateInfo,
        tessellationStateCreateInfo, viewportStateCreateInfo, rasterizationStateCreateInfo,
        multisampleStateCreateInfo, depthStencilStateCreateInfo, colorBlendStateCreateInfo);
    */

    renderPass->createPipelines();

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