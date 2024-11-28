// TODO: Use VulkanCore for all of this
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <memory>
#include <fstream>
#include <algorithm>

std::vector<std::string> getExtensionList()
{
    std::vector<std::string> extList = std::vector<std::string>(2);
    std::unique_ptr<std::ifstream> file = std::make_unique<std::ifstream>();
    file->open("resources/extensionList.txt", std::ios::in);
    if (!file->is_open())
    {
        std::cout << "Failed to open extension list (extensionList.txt)!" << std::endl;
    }
    else
    {
        std::string extension = "";
        while (!file->eof())
        {
            *file >> extension;
            if (extension != "")
            {
                extList.push_back(extension);
            }
        }
    }
    file->close();
    return extList;
}

std::vector<VkExtensionProperties> enumerateExtensions()
{
    // TODO: Enumerate device extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    return extensions;
}

bool isExtensionSupported(const char *extensionName, std::vector<VkExtensionProperties> extensions)
{

    for (const auto &extension : extensions)
    {
        if (std::strcmp(extension.extensionName, extensionName) == 0)
        {
            return true;
        }
    }
    return false;
}

int main()
{
    std::vector<VkExtensionProperties> enumExt{enumerateExtensions()};
    std::vector<std::string> extToSearch{getExtensionList()};
    std::ranges::for_each(extToSearch, [enumExt](std::string ext)
                          { 
        const char *extensionToCheck = ext.c_str();
        std::cout << ext;
        if (isExtensionSupported(extensionToCheck, enumExt))
        {
            std::cout << " exists." << std::endl;
        }
        else
        {
            std::cout << " does not exist." << std::endl;
        } });

    std::cout << "\n\nAll extensions:" << std::endl;
    std::ranges::for_each(enumExt, [](VkExtensionProperties ext)
                          { std::cout << ext.extensionName << std::endl; });
    return 0;
}