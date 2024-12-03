// I just don't want to look at all these structs every time I look at my code.

#pragma once
#include <string>

#include <vulkan/vulkan.h>

namespace vkn
{
    class VknInfos
    {
    public:
        typedef enum checkFillFunctions
        {
            APP_INFO = 0,
            DEVICE_QUEUE_CREATE_INFO = 1
        };

        VknInfos();
        //~VknInfos();

        // getters
        VkApplicationInfo *getAppInfo()
        {
            if (this->checkFill(APP_INFO))
                return &m_appInfo;
        };
        VkInstanceCreateInfo *getInstanceCreateInfo() { return &m_instanceCreateInfo; };
        VkDeviceQueueCreateInfo *getDeviceQueueCreateInfo()
        {
            if (this->checkFill(DEVICE_QUEUE_CREATE_INFO))
                return &m_deviceQueueCreateInfo;
        };
        VkDeviceCreateInfo *getDeviceCreateInfo() { return &m_deviceCreateInfo; };

    private:
        std::string m_appName = "Default App Name";
        std::string m_engineName = "Default Engine Name";
        float m_queuePriorities = 1.0f;

        // Info's
        VkApplicationInfo m_appInfo;
        VkInstanceCreateInfo m_instanceCreateInfo;
        VkDeviceQueueCreateInfo m_deviceQueueCreateInfo;
        VkDeviceCreateInfo m_deviceCreateInfo;

        // Defaults
        VkApplicationInfo getDefaultAppInfo();
        VkInstanceCreateInfo getDefaultInstanceCreateInfo();
        VkDeviceQueueCreateInfo getDefaultDeviceQueueCreateInfo();
        VkDeviceCreateInfo getDefaultDeviceCreateInfo();

        // Fill
        bool checkFill(checkFillFunctions functionName);
        void fillDefaultInfos();

        void fillAppInfo(std::string appName, std::string engineName,
                         VkApplicationInfo *pNext = nullptr, uint32_t applicationVersion = 0,
                         uint32_t engineVersion = 0, uint32_t apiVersion = VK_API_VERSION_1_1);
        void fillDeviceQueueCreateInfo(uint32_t queueFamilyIdx,
                                       VkApplicationInfo *pNext = nullptr,
                                       VkDeviceQueueCreateFlags flags = 0,
                                       uint32_t queueCount = 1,
                                       float queuePriorities = 1.0f);

        // Required fill checklist
        bool m_filledAppInfo = false;
        bool m_filledDeviceQueueCreateInfo = false;
    };
}
