#include "include/VknImage.hpp"

namespace vkn
{
    VknImage::VknImage(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknImage::createImage()
    {
        VkImageCreateInfo *imageInfo = m_infos->fillImageCreateInfo(
            m_relIdxs, m_imageType, m_format, m_extent, m_mipLevels, m_arrayLayers,
            m_samples, m_tiling, m_usage, m_initialLayout, m_flags);

        VknResult res{vkCreateImage(m_engine->getObject<VkDevice>(m_absIdxs),
                                    imageInfo, VK_NULL_HANDLE,
                                    &m_engine->getObject<VkImage>(m_absIdxs)),
                      "Create image."};

        m_createdVkImage = true;
    }

    void VknImage::setImageType(VkImageType imageType)
    {
        m_imageType = imageType;
    }

    void VknImage::setFormat(VkFormat format)
    {
        m_format = format;
    }

    void VknImage::setExtent(VkExtent3D extent)
    {
        m_extent = extent;
    }

    void VknImage::setMipLevels(uint32_t mipLevels)
    {
        m_mipLevels = mipLevels;
    }

    void VknImage::setArrayLayers(uint32_t arrayLayers)
    {
        m_arrayLayers = arrayLayers;
    }

    void VknImage::setSamples(VkSampleCountFlagBits samples)
    {
        m_samples = samples;
    }

    void VknImage::setTiling(VkImageTiling tiling)
    {
        m_tiling = tiling;
    }

    void VknImage::setUsage(VkImageUsageFlags usage)
    {
        m_usage = usage;
    }

    void VknImage::setInitialLayout(VkImageLayout initialLayout)
    {
        m_initialLayout = initialLayout;
    }

    void VknImage::setFlags(VkImageCreateFlags flags)
    {

        m_flags = flags;
    }

    VkImage *VknImage::getVkImage()
    {
        return &m_engine->getObject<VkImage>(m_absIdxs);
    }

    // Helper function (could be in VknPhysicalDevice or a utility header)
    uint32_t VknImage::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_engine->getObject<VkPhysicalDevice>(m_absIdxs), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        throw std::runtime_error("Failed to find suitable memory type!");
    }

    void VknImage::allocateAndBindMemory(VkMemoryPropertyFlags requiredMemoryProperties)
    {
        if (!m_createdVkImage)
        {
            throw std::runtime_error("VknImage::allocateAndBindMemory called before createImage().");
        }
        if (m_memoryBound)
        {
            throw std::runtime_error("VknImage::allocateAndBindMemory called but memory is already bound.");
        }

        VkDevice device = m_engine->getObject<VkDevice>(m_absIdxs);
        VkImage image = m_engine->getObject<VkImage>(m_absIdxs);
        // Assuming VknDevice holds a VknPhysicalDevice, and VknPhysicalDevice can give its VkPhysicalDevice handle.
        // This part needs to be adapted to how you access the VkPhysicalDevice handle from VknEngine/VknDevice.
        // For this example, let's assume m_engine can provide it or it's accessible via m_absIdxs.
        // This is a placeholder for how you get the physical device.
        // You'll likely need to get the VknDevice first, then its VknPhysicalDevice.
        VkPhysicalDevice physicalDevice = m_engine->getObject<VkPhysicalDevice>(m_absIdxs.get<VkDevice>()); // This assumes physical device 0 for device 0

        VkDeviceMemory *deviceMemory = &m_engine->addNewObject<VkDeviceMemory, VkDevice>(m_absIdxs);
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, image, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, requiredMemoryProperties);

        VknResult resAlloc{vkAllocateMemory(device, &allocInfo, nullptr, deviceMemory), "VknImage vkAllocateMemory"};
        VknResult resBind{vkBindImageMemory(device, image, *deviceMemory, 0), "VknImage vkBindImageMemory"};
        m_memoryBound = true;
    }
}