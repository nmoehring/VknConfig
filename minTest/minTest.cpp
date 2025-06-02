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

int main()
{
    try
    {
        /*
        vkn::VknApp info_app{};
        info_app.configureWithPreset(vkn::deviceInfoConfig);
        info_app.run();
        info_app.exit();
        */

        vkn::VknApp noInputApp{};
        noInputApp.configureWithPreset(vkn::noInputConfig); // Configure before run
        // If validation layers are desired:
        // noInputApp.enableValidationLayer();
        noInputApp.addWindow();
        noInputApp.run();
        noInputApp.exit(); // Explicitly call exit
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}