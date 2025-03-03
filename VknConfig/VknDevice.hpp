// I decided to use a Vkn prefix to stand apart from Vk prefixes of the API
// All the structs are in info.hpp

#pragma once

#include <vector>
#include <memory>
#include <deque>

#include "VknRenderPass.hpp"
#include "VknQueueFamily.hpp"
#include "VknPhysicalDevice.hpp"

namespace vkn
{
    class VknDevice
    {
    public:
        VknDevice();
        VknDevice(VknInfos *infos, VknResultArchive *archive, const VkInstance *instance, const bool *instanceCreated);
        ~VknDevice();
        void destroy();
        VknResult createDevice();
        void fillDeviceCreateInfo();
        bool getVkDeviceCreated() { return m_vkDeviceCreated; }
        VknRenderPass *getRenderPass(uint32_t renderPassIdx);
        void requestQueueFamilyProperties();
        int getNumQueueFamilies() { return m_queues.size(); }

        void fillSwapChainCreateInfo(
            VkSurfaceKHR surface, uint32_t imageCount, VkExtent2D dimensions,
            VkSurfaceFormatKHR surfaceFormat = VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            uint32_t numImageArrayLayers = 1, VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR, VkBool32 clipped = VK_TRUE,
            VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);
        void addExtensions(const char *ext[], uint32_t size);
        void createSwapChains();
        void fillDeviceQueuePriorities(uint32_t queueFamilyIdx, std::vector<float> priorities);
        void fillDeviceQueuePrioritiesDefault();

        VkPhysicalDeviceProperties *getPhysicalDeviceProperties()
        {
            return m_physicalDevice.getProperties();
        }
        std::vector<VknQueueFamily> &getQueues() { return m_queues; }
        VknQueueFamily &getQueue(int idx);
        VkDevice *getVkDevice();
        VknPhysicalDevice *getPhysicalDevice();
        uint32_t addRenderPass();

    private:
        static int s_numDevices;
        VkDevice m_logicalDevice{};
        VknPhysicalDevice m_physicalDevice;
        std::deque<VknRenderPass> m_renderPasses; // Deque, because elements don't need to be together, refs could be invalidated
        std::vector<VknQueueFamily> m_queues{};   // Vector fine, this shouldn't change.
        VknResultArchive *m_resultArchive{nullptr};
        VknInfos *m_infos{nullptr};
        const VkInstance *m_instance{nullptr};
        const bool *m_instanceCreated{nullptr};
        bool m_queuesRequested{false};
        uint32_t m_deviceIdx;

        const char *const *m_extensions{nullptr};
        uint32_t m_extensionsSize{0};
        VkPhysicalDeviceFeatures *m_features{nullptr};
        // Possibility for multiple swapchains > multiple screens, VR
        std::vector<VkSwapchainCreateInfoKHR *> m_swapChainCreateInfos; // Vector should be fine, it shouldn't change
        std::vector<VkSwapchainKHR> m_swapChains;                       // Doesn't need to change after creation

        bool m_destroyed{false};
        bool m_vkDeviceCreated{false};

        // Other utilities
        void archiveResult(VknResult res);
    };
}