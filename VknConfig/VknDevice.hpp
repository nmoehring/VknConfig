// I decided to use a Vkn prefix to stand apart from Vk prefixes of the API
// All the structs are in info.hpp

#pragma once

#include <vector>
#include <memory>

#include "VknRenderPass.hpp"
#include "VknQueueFamily.hpp"
#include "VknPhysicalDevice.hpp"
#include "VknSwapchain.hpp"

namespace vkn
{
    class VknDevice
    {
    public:
        VknDevice();
        VknDevice(uint32_t deviceIdx, VknInfos *infos, VknResultArchive *archive, const VkInstance *instance,
                  const bool *instanceCreated);
        ~VknDevice();
        void destroy();
        VknResult createDevice();
        bool getVkDeviceCreated() { return m_vkDeviceCreated; }
        VknRenderPass *getRenderPass(uint32_t renderPassIdx);
        void requestQueueFamilyProperties();
        int getNumQueueFamilies() { return m_queues.size(); }
        void setSwapchainDimensions();
        void addExtensions(const char *ext[], uint32_t size);
        void fillDeviceQueuePriorities(uint32_t queueFamilyIdx, std::vector<float> priorities);
        void fillDeviceQueuePrioritiesDefault();
        void addSwapchain(uint32_t swapchainIdx, VkSurfaceKHR surface, uint32_t imageCount,
                          uint32_t imageWidth, uint32_t imageHeight);
        void fillSwapchainCreateInfos();
        VknSwapchain *getSwapchain(uint32_t swapchainIdx);
        VkPhysicalDeviceProperties *getPhysicalDeviceProperties()
        {
            return m_physicalDevice.getProperties();
        }
        std::vector<VknQueueFamily> &getQueues() { return m_queues; }
        VknQueueFamily &getQueue(int idx);
        VkDevice *getVkDevice();
        VknPhysicalDevice *getPhysicalDevice();
        void addRenderPass(uint32_t newRenderPassIdx);
        void selectQueues(bool chooseAllAvailableQueues = false);
        void fillQueueCreateInfos();

    private:
        static int s_numDevices;
        VkDevice m_logicalDevice{};
        VknPhysicalDevice m_physicalDevice;
        std::list<VknRenderPass> m_renderPasses; // List, because elements don't need to be together, refs could be invalidated
        std::vector<VknQueueFamily> m_queues{};  // Vector fine, this shouldn't change.
        VknResultArchive *m_resultArchive{nullptr};
        VknInfos *m_infos{nullptr};
        const VkInstance *m_instance{nullptr};
        const bool *m_instanceCreated{nullptr};
        bool m_queuesRequested{false};
        bool m_queuesSelected{false};
        uint32_t m_deviceIdx;

        const char *const *m_extensions{nullptr};
        uint32_t m_extensionsSize{0};
        VkPhysicalDeviceFeatures *m_features{nullptr};
        // Possibility for multiple swapchains > multiple screens, VR
        std::vector<VkSwapchainCreateInfoKHR *> m_swapchainCreateInfos; // Vector should be fine, it shouldn't change
        std::vector<VknSwapchain> m_swapchains{};                       // Doesn't need to change after creation

        bool m_destroyed{false};
        bool m_vkDeviceCreated{false};
        bool m_placeholder;
        bool m_filledQueueCreateInfos{false};
        uint32_t m_numRenderPasses{0};

        // Other utilities
        void archiveResult(VknResult res);
    };
}