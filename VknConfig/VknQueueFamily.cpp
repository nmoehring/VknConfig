#include <stdexcept>

#include <vulkan/vulkan.h>

#include "VknQueueFamily.hpp"

namespace vkn
{
    VknQueueFamily::VknQueueFamily() : m_placeholder{true}
    {
        m_properties.queueCount = 0;
        m_properties.queueFlags = 0;
    }

    VknQueueFamily::VknQueueFamily(VkQueueFamilyProperties properties)
        : m_properties{properties}, m_placeholder{false} {}

    bool VknQueueFamily::supportsGraphics()
    {
        if (m_placeholder)
            throw std::runtime_error("Member function called on a placeholder object.");
        return ((m_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) > 0);
    }

    bool VknQueueFamily::supportsCompute()
    {
        if (m_placeholder)
            throw std::runtime_error("Member function called on a placeholder object.");
        return ((m_properties.queueFlags & VK_QUEUE_COMPUTE_BIT) > 0);
    }

    bool VknQueueFamily::supportsTransfer()
    {
        if (m_placeholder)
            throw std::runtime_error("Member function called on a placeholder object.");
        return ((m_properties.queueFlags & VK_QUEUE_TRANSFER_BIT) > 0);
    }

    bool VknQueueFamily::supportsSparseBinding()
    {
        if (m_placeholder)
            throw std::runtime_error("Member function called on a placeholder object.");
        return ((m_properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) > 0);
    }

    bool VknQueueFamily::supportsMemoryProtection()
    {
        if (m_placeholder)
            throw std::runtime_error("Member function called on a placeholder object.");
        return ((m_properties.queueFlags & VK_QUEUE_PROTECTED_BIT) > 0);
    }

    int VknQueueFamily::getNumSelected()
    {
        if (m_placeholder)
            throw std::runtime_error("Member function called on a placeholder object.");
        return m_numSelected;
    }

    void VknQueueFamily::setNumSelected(int num)
    {
        if (m_placeholder)
            throw std::runtime_error("Member function called on a placeholder object.");
        if (num <= this->getNumAvailable())
            m_numSelected = num;
        else
            throw std::runtime_error("Tried to select too many queues.");
    }

    uint32_t VknQueueFamily::getNumAvailable()
    {
        if (m_placeholder)
            throw std::runtime_error("Member function called on a placeholder object.");
        return m_properties.queueCount;
    }
}