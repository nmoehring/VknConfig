#include <vector>
#include <iostream>
#include <filesystem>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stb_image.h>

#include "VknConfig/VknConfig.hpp"
// #include "VknShaderStage.hpp"

// GLFWwindow *window_ = nullptr;

void printDevProps(std::vector<VkPhysicalDeviceProperties> devProps);
void printFamProps(std::vector<VkQueueFamilyProperties> famProps);
std::vector<char> readFile(const std::string &filename);
std::vector<const char *> noNames{};
bool initWindow(GLFWwindow **outWindow);

int main()
{
    GLFWwindow *window = nullptr;
    initWindow(&window);
    vkn::VknConfig config{window};
    config.testNoInputs();
    auto device = config.getDevice(0);
    auto renderpass = device->getRenderpass(0);
    auto pipeline = renderpass->getPipeline(0);

    auto limits = device->getPhysicalDevice()->getLimits();
    std::cout << "maxVertexInputBindings=" << limits->maxVertexInputBindings << std::endl;
    std::cout << "maxVertexInputAttributes=" << limits->maxVertexInputAttributes << std::endl;

    int idx = 0;
    for (auto &queue : device->getQueues())
    {
        std::cout << "Queue " << idx << ": " << std::endl;
        std::cout << "Graphics: " << queue.supportsGraphics() << std::endl;
        std::cout << "Compute: " << queue.supportsCompute() << std::endl;
        std::cout << "Transfer: " << queue.supportsTransfer() << std::endl;
        std::cout << "Sparse Binding: " << queue.supportsSparseBinding() << std::endl;
        std::cout << "Mem Protection: " << queue.supportsMemoryProtection() << std::endl;
        ++idx;
    }

    while (!glfwWindowShouldClose(window))
    {
        // Poll for and process events
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
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