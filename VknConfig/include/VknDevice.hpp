// I decided to use a Vkn prefix to stand apart from Vk prefixes of the API
// All the structs are in info.hpp

#pragma once

#include <vector>
#include <memory>

#include "VknRenderpass.hpp"
#include "VknPhysicalDevice.hpp"
#include "VknSwapchain.hpp"

namespace vkn
{
    class VknDevice
    {
    public:
        VknDevice() = delete;
        VknDevice(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void addExtensions(const char *ext[], uint32_t size);
        VknResult createDevice();

        void setSwapchainDimensions();
        void fillSwapchainCreateInfos();
        void addSwapchain(uint32_t swapchainIdx, VkSurfaceKHR *surface, uint32_t imageCount,
                          uint32_t imageWidth, uint32_t imageHeight);

        void addRenderpass(uint32_t newRenderpassIdx);

        VknPhysicalDevice *getPhysicalDevice();
        VknSwapchain *getSwapchain(uint32_t swapchainIdx);
        VknRenderpass *getRenderpass(uint32_t renderpassIdx);
        VkDevice *getVkDevice();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Wrapped object
        VkDevice *m_vkDevice{};

        // Members
        std::list<VknRenderpass> m_renderpasses{}; // List, because elements don't need to be together, refs could be invalidated
        std::vector<VknSwapchain> m_swapchains{};  // Doesn't need to change after creation
        VknPhysicalDevice m_physicalDevice;

        // Params
        const char *const *m_extensions{nullptr};
        uint32_t m_extensionsSize{0};
        VkPhysicalDeviceFeatures *m_features{nullptr};

        // State
        bool m_createdVkDevice{false};
        bool m_filledQueueCreateInfos{false};
        uint32_t m_numRenderpasses{0};
    };
}