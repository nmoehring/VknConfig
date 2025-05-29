#include <iostream>
#include <filesystem>

// #define GLFW_INCLUDE_VULKAN

#include "VknConfig/include/VknApp.hpp"
#include "VknConfig/include/VknData.hpp"

void printDevProps(vkn::VknVector<VkPhysicalDeviceProperties> devProps);
void printFamProps(vkn::VknVector<VkQueueFamilyProperties> famProps);
vkn::VknVector<char> readFile(const std::string &filename);
vkn::VknVector<const char *> noNames{};
GLFWwindow *initWindow();

// GLFW error callback
void glfwErrorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error: " << description << std::endl;
}

// GLFW key callback
void glfwKeyCallback(GLFWwindow *window, int key, int, int action, int mods)
{
    const bool pressed{action != GLFW_RELEASE};
    if (key == GLFW_KEY_ESCAPE && pressed)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main()
{
    GLFWwindow *window = initWindow();
    if (!glfwVulkanSupported())
        throw std::runtime_error("Vulkan graphics API not supported by any of your devices.");
    vkn::VknApp info_app{};
    info_app.configureWithPreset(vkn::deviceInfoConfig);
    info_app.exit();

    vkn::VknApp noInputTest{};
    noInputTest.addWindow(window);
    noInputTest.configureWithPreset(vkn::noInputConfig);

    while (!glfwWindowShouldClose(window))
    {
        // Poll for and process events
        glfwPollEvents();
        noInputTest.cycleEngine();
    }
    noInputTest.exit();
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow *initWindow()
{
    if (!glfwInit())
        return nullptr;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(640, 480, "Window Title", NULL, NULL);
    return window;
    /*

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
    glfwSetErrorCallback(glfwErrorCallback);
    glfwSetKeyCallback(window, glfwKeyCallback);
    /*glfwSetErrorCallback([](int error, const char *description)
                         { printf("GLFW Error (%i): %s\n", error, description); });

    glfwSetKeyCallback(
        window, [](GLFWwindow *window, int key, int, int action, int mods)
        {
        const bool pressed{action != GLFW_RELEASE};
        if (key == GLFW_KEY_ESCAPE && pressed) {
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (key == GLFW_KEY_ESCAPE && pressed)
          glfwSetWindowShouldClose(window, GLFW_TRUE); });


#if defined(WIN32)
    HWND hwnd{glfwGetWin32Window(window)};
    SetWindowLongPtr(
        hwnd, GWL_STYLE,
        GetWindowLongPtrA(hwnd, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX));
#endif

    if (outWindow)
    {
        *outWindow = window;
    }

    return true;

*/
}