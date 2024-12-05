// I just don't want to look at all these structs every time I look at my code.

#pragma once
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace vkn
{
    class VknInfos
    {
    public:
        enum checkFillFunctions
        {
            APP_INFO = 0,
            DEVICE_QUEUE_CREATE_INFO = 1
        };

        VknInfos();
        //~VknInfos();
        void setNumQueueFamilies(int num)
        {
            m_queuePriorities.resize(num);
            m_queueCreateInfos.resize(num);
        };

        // getters
        VkApplicationInfo *getAppInfo()
        {
            if (this->checkFill(APP_INFO))
                return &m_appInfo;
            else
                throw std::runtime_error("ApplicationInfo not filled before get call.");
        };
        VkInstanceCreateInfo *getInstanceCreateInfo() { return &m_instanceCreateInfo; };
        VkDeviceQueueCreateInfo *getDeviceQueueCreateInfo(int index)
        {
            std::cout << "create device checkfill : " << checkFill(DEVICE_QUEUE_CREATE_INFO) << std::endl;
            if (this->checkFill(DEVICE_QUEUE_CREATE_INFO))
                return &(m_queueCreateInfos[index]);
            else
                throw std::runtime_error("DeviceQueueCreateInfos not filled before get call.");
        };
        VkDeviceCreateInfo *getDeviceCreateInfo() { return &m_deviceCreateInfo; };

    private:
        std::string m_appName = "Default App Name";
        std::string m_engineName = "Default Engine Name";
        std::vector<float> m_queuePriorities{1.0f};

        // Info's
        VkApplicationInfo m_appInfo;
        VkInstanceCreateInfo m_instanceCreateInfo;
        std::vector<VkDeviceQueueCreateInfo> m_queueCreateInfos;
        VkDeviceCreateInfo m_deviceCreateInfo;

        // Defaults
        VkApplicationInfo getDefaultAppInfo();
        VkInstanceCreateInfo getDefaultInstanceCreateInfo();
        VkDeviceQueueCreateInfo getDefaultDeviceQueueCreateInfo(int index);
        VkDeviceCreateInfo getDefaultDeviceCreateInfo();

        // Fill
        bool checkFill(checkFillFunctions functionName);
        void fillDefaultInfos();

        void fillAppInfo(std::string appName, std::string engineName,
                         VkApplicationInfo *pNext = nullptr, uint32_t applicationVersion = 0,
                         uint32_t engineVersion = 0, uint32_t apiVersion = VK_API_VERSION_1_1);
        void fillDeviceQueueCreateInfo(uint32_t queueFamilyIdx,
                                       VkApplicationInfo *pNext, VkDeviceQueueCreateFlags flags,
                                       uint32_t queueCount, float queuePriorities);

        // Required fill checklist
        bool m_filledAppInfo{false};
        bool m_filledDeviceQueueCreateInfo{false};
    };
}
