#pragma once
#include <stdexcept>

#include <vulkan/vulkan.h>

namespace vkn
{
    class VknQueueFamily
    {
    public:
        VknQueueFamily() {}
        VknQueueFamily(VkQueueFamilyProperties properties)
            : m_properties{properties} {}

        bool supportsGraphics() { return ((m_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) > 0); }
        bool supportsCompute() { return ((m_properties.queueFlags & VK_QUEUE_COMPUTE_BIT) > 0); }
        bool supportsTransfer() { return ((m_properties.queueFlags & VK_QUEUE_TRANSFER_BIT) > 0); }
        bool supportsSparseBinding() { return ((m_properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) > 0); }
        bool supportsMemoryProtection() { return ((m_properties.queueFlags & VK_QUEUE_PROTECTED_BIT) > 0); }
        int getNumSelected() { return m_numSelected; }
        void setNumSelected(int num)
        {
            if (num <= this->getNumAvailable())
                m_numSelected = num;
            else
                throw std::runtime_error("Tried to select too many queues.");
        }
        uint32_t getNumAvailable() { return m_properties.queueCount; }

    private:
        int m_numSelected{0};
        VkQueueFamilyProperties m_properties;
    };
}