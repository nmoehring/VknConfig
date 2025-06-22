#include "include/VknQueueFamily.hpp"

namespace vkn
{
    VknQueueFamily::VknQueueFamily(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs) {}

    bool VknQueueFamily::supportsGraphics()
    {
        VkQueueFamilyProperties *props = &s_engine->getObject<VkQueueFamilyProperties>(m_absIdxs);
        return ((props->queueFlags & VK_QUEUE_GRAPHICS_BIT) > 0);
    }

    bool VknQueueFamily::supportsCompute()
    {
        VkQueueFamilyProperties *props = &s_engine->getObject<VkQueueFamilyProperties>(m_absIdxs);
        return ((props->queueFlags & VK_QUEUE_COMPUTE_BIT) > 0);
    }

    bool VknQueueFamily::supportsTransfer()
    {
        VkQueueFamilyProperties *props = &s_engine->getObject<VkQueueFamilyProperties>(m_absIdxs);
        return ((props->queueFlags & VK_QUEUE_TRANSFER_BIT) > 0);
    }

    bool VknQueueFamily::supportsSparseBinding()
    {
        VkQueueFamilyProperties *props = &s_engine->getObject<VkQueueFamilyProperties>(m_absIdxs);
        return ((props->queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) > 0);
    }

    bool VknQueueFamily::supportsMemoryProtection()
    {
        VkQueueFamilyProperties *props = &s_engine->getObject<VkQueueFamilyProperties>(m_absIdxs);
        return ((props->queueFlags & VK_QUEUE_PROTECTED_BIT) > 0);
    }

    int VknQueueFamily::getNumSelected()
    {
        return m_numSelected;
    }

    VkDeviceQueueCreateFlags VknQueueFamily::getFlags()
    {
        return m_createFlags;
    }

    void VknQueueFamily::setNumSelected(int num)
    {
        if (num <= this->getNumAvailable())
            m_numSelected = num;
        else
            throw std::runtime_error("Tried to select too many queues.");
    }

    void VknQueueFamily::setMemoryProtection()
    {
        if (!this->supportsMemoryProtection())
            throw std::runtime_error("Can't set memory protection. Not supported for this queue.");
        m_createFlags = VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT;
    }

    uint32_t VknQueueFamily::getNumAvailable()
    {
        VkQueueFamilyProperties *props = &s_engine->getObject<VkQueueFamilyProperties>(m_absIdxs);
        return props->queueCount;
    }
}