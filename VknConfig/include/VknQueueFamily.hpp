#pragma once

#include "VknResult.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknQueueFamily
    {
    public:
        VknQueueFamily() = delete;
        VknQueueFamily(VkQueueFamilyProperties properties);

        bool supportsGraphics();
        bool supportsCompute();
        bool supportsTransfer();
        bool supportsSparseBinding();
        bool supportsMemoryProtection();
        int getNumSelected();
        void setNumSelected(int num);
        uint32_t getNumAvailable();

    private:
        // Members
        VkQueueFamilyProperties m_properties{};

        // State
        int m_numSelected{0};
    };
}