#include <stdexcept>

#include "VknInfos.hpp"
#include <vulkan/vulkan.h>

namespace vkn
{
    VknInfos::VknInfos()
    {
        m_queuePriorities.push_back(1.0f);
        fillDefaultInfos();
    }

    void VknInfos::fillDefaultInfos()
    {
        m_appInfo = this->getDefaultAppInfo();
        m_instanceCreateInfo = this->getDefaultInstanceCreateInfo();
        m_queueCreateInfos.push_back(this->getDefaultDeviceQueueCreateInfo());
        m_deviceCreateInfo = this->getDefaultDeviceCreateInfo();
    }

    VkApplicationInfo VknInfos::getDefaultAppInfo()
    {
        VkApplicationInfo info;
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pNext = nullptr;
        info.pApplicationName = m_appName.c_str();
        info.applicationVersion = 0;
        info.pEngineName = m_engineName.c_str();
        info.engineVersion = 0;
        // Vulkan 1.0 only compatible with 1.0 (maybe stick with 1.1 at least)
        info.apiVersion = VK_API_VERSION_1_1;
        return info;
    }

    VkInstanceCreateInfo VknInfos::getDefaultInstanceCreateInfo()
    {
        VkInstanceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = VkInstanceCreateFlagBits{};
        info.pApplicationInfo = &m_appInfo;
        info.enabledLayerCount = 0;
        info.ppEnabledLayerNames = nullptr;
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        return info;
    }

    VkDeviceQueueCreateInfo VknInfos::getDefaultDeviceQueueCreateInfo()
    {
        VkDeviceQueueCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // Only flag is a protected memory bit, for a queue family that supports it
        info.queueFamilyIndex = 0;
        info.queueCount = 1;
        info.pQueuePriorities = &(m_queuePriorities[0]);
        return info;
    }

    VkDeviceCreateInfo VknInfos::getDefaultDeviceCreateInfo()
    {
        VkDeviceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // flags reserved, must = 0
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = nullptr;
        // enabledLayerCount is deprecated and should not be used
        info.enabledLayerCount = 0; // ignored, value doesn't matter
        // ppEnabledLayerNames is deprecated and should not be used
        info.ppEnabledLayerNames = nullptr; // ignored, value doesn't matter
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        info.pEnabledFeatures = nullptr;
        return info;
    }

    bool VknInfos::checkFill(checkFillFunctions functionName)
    {
        switch (functionName)
        {
        case APP_INFO:
            if (!m_filledAppInfo)
                throw std::runtime_error("AppInfo not filled before get.");
        case DEVICE_QUEUE_CREATE_INFO:
            if (!m_filledDeviceQueueCreateInfo)
                throw std::runtime_error("DeviceQueueCreateInfo not filled before get.");
        case DEVICE_CREATE_INFO:
            if (!m_filledDeviceCreateInfo)
                throw std::runtime_error("DeviceCreateInfo not filled before get.");
        }
        return true;
    }

    void VknInfos::fillAppInfo(uint32_t apiVersion, std::string appName,
                               std::string engineName, VkApplicationInfo *pNext,
                               uint32_t applicationVersion, uint32_t engineVersion)
    {
        m_appName = appName;
        m_engineName = engineName;

        m_appInfo.pApplicationName = m_appName.c_str();
        m_appInfo.pEngineName = m_engineName.c_str();

        m_appInfo.pNext = nullptr;
        m_appInfo.pApplicationName = m_appName.c_str();
        m_appInfo.applicationVersion = 0;
        m_appInfo.pEngineName = m_engineName.c_str();
        m_appInfo.engineVersion = 0;
        // Vulkan 1.0 only compatible with 1.0 (maybe stick with 1.1 at least)
        m_appInfo.apiVersion = VK_API_VERSION_1_1;

        m_filledAppInfo = true;
        this->fillInstanceCreateInfo();
    }

    void VknInfos::fillInstanceCreateInfo(VkInstanceCreateInfo *pNext,
                                          VkInstanceCreateFlags flags,
                                          std::vector<std::string> enabledLayerNames,
                                          std::vector<std::string> enabledExtensionNames)
    {
        m_instanceCreateInfo.pNext = nullptr;
        m_instanceCreateInfo.flags = VkInstanceCreateFlagBits{};
        m_instanceCreateInfo.pApplicationInfo = &m_appInfo;
        m_instanceCreateInfo.enabledLayerCount = enabledLayerNames.size();
        std::vector<const char *> layerNames;
        for (auto name : enabledLayerNames)
            layerNames.push_back(name.c_str());
        m_instanceCreateInfo.ppEnabledLayerNames = layerNames.data();
        m_instanceCreateInfo.enabledExtensionCount = enabledExtensionNames.size();
        std::vector<const char *> extNames;
        for (auto name : enabledExtensionNames)
            extNames.push_back(name.c_str());
        m_instanceCreateInfo.ppEnabledExtensionNames = extNames.data();
    }

    void VknInfos::fillDeviceQueueCreateInfo(uint32_t queueFamilyIdx, uint32_t queueCount,
                                             VkApplicationInfo *pNext,
                                             VkDeviceQueueCreateFlags flags,
                                             float queuePriorities)
    {
        m_queueCreateInfos[queueFamilyIdx].queueFamilyIndex = queueFamilyIdx;
        if (pNext != nullptr)
            m_queueCreateInfos[queueFamilyIdx].pNext = pNext;
        if (flags != INT_MAX)
            m_queueCreateInfos[queueFamilyIdx].flags = flags; // Only flag is a protected memory bit, for a queue family that supports it
        m_queueCreateInfos[queueFamilyIdx].queueCount = queueCount;
        if (queuePriorities != -1)
            m_queuePriorities[queueFamilyIdx] = queuePriorities;
        m_queueCreateInfos[queueFamilyIdx].pQueuePriorities = &(m_queuePriorities[queueFamilyIdx]);
        if (queueFamilyIdx == (m_queueCreateInfos.size() - 1))
        {
            m_filledDeviceQueueCreateInfo = true;
            this->fillDeviceCreateInfo();
        }
    }

    void VknInfos::fillDeviceCreateInfo()
    {
        m_deviceCreateInfo.queueCreateInfoCount = m_queueCreateInfos.size();
        m_deviceCreateInfo.pQueueCreateInfos = m_queueCreateInfos.data();
        m_filledDeviceCreateInfo = true;
        // m_deviceCreateInfo.enabledExtensionCount = 0;
        // m_deviceCreateInfo.ppEnabledExtensionNames = nullptr;
        // m_deviceCreateInfo.pEnabledFeatures = nullptr;
    }
}