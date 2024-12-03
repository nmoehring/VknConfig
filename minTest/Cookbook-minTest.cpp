#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include <iostream>
#include <vector>
#include <memory>

#include <vulkancore/Context.hpp>
#include <enginecore/RingBuffer.hpp>
#include <enginecore/Model.hpp>
#include <enginecore/Camera.hpp>
#include <enginecore/GLFWUtils.hpp>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>

int main()
{
    // 1. CREATE CONTEXT
    GLFWwindow *window_ = nullptr;
    EngineCore::Camera camera(glm::vec3(-9.f, 2.f, 2.f));
    initWindow(&window_, &camera);

    std::vector<std::string> validationLayers;
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");

    VulkanCore::Context::enableDefaultFeatures();
    // VulkanCore::Context::enableBufferDeviceAddressFeature();
    VulkanCore::Context::enableDynamicRenderingFeature();

    const std::vector<std::string> instExtension = {
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
    };

    const std::vector<std::string> deviceExtension = {
        VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    };

    VulkanCore::Context context(
        (void *)glfwGetWin32Window(window_),
        validationLayers,      // layers
        instExtension,         // instance extensions
        deviceExtension,       // device extensions
        VK_QUEUE_GRAPHICS_BIT, // request a graphics queue only
        true);

    const VkExtent2D extents =
        context.physicalDevice().surfaceCapabilities().minImageExtent;

    const VkFormat swapChainFormat = VK_FORMAT_B8G8R8A8_UNORM;

    context.createSwapchain(swapChainFormat, VK_COLORSPACE_SRGB_NONLINEAR_KHR,
                            VK_PRESENT_MODE_FIFO_KHR, extents);

    static const uint32_t framesInFlight = (uint32_t)context.swapchain()->numberImages();

    // Create command pools
    auto commandMgr = context.createGraphicsCommandQueue(
        context.swapchain()->numberImages(), framesInFlight, "main command");

    UniformTransforms transform = {.model = glm::mat4(1.0f),
                                   .view = camera.viewMatrix(),
                                   .projection = camera.getProjectMatrix()};

    std::vector<std::shared_ptr<VulkanCore::Buffer>> buffers;
    std::vector<std::shared_ptr<VulkanCore::Texture>> textures;
    std::vector<std::shared_ptr<VulkanCore::Sampler>> samplers;
    EngineCore::RingBuffer cameraBuffer(context.swapchain()->numberImages(), context,
                                        sizeof(UniformTransforms));
    uint32_t numMeshes = 0;
    std::shared_ptr<EngineCore::Model> bistro;

    // 2. CHECK INSTANCE
    if (context.instance() != nullptr)
    {
        std::cout << "Instance created." << std::endl;
    }

    // 3. CHECK Device
    if (context.device() != nullptr)
    {
        std::cout << "Device created." << std::endl;
    }

    std::string blank = "";
    std::cin >> blank;

    glfwDestroyWindow(window_);
    glfwTerminate();

    return 0;
}
