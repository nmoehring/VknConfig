#include "include/VknImage.hpp"

namespace vkn
{
    VknImage::VknImage(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_instanceLock = this;
    }

    void VknImage::createImage()
    {
        if (m_createdVkImage)
            throw std::runtime_error("VknImage::createImage called but image already created.");

        VkImageCreateInfo *imageInfo = m_infos->fileImageCreateInfo(
            m_relIdxs, m_imageType, m_format, m_extent, m_mipLevels, m_arrayLayers,
            m_samples, m_tiling, m_usage, m_initialLayout, m_flags);

        // VMA Allocation
        VmaAllocationCreateInfo allocInfo = {};
        // For images, VMA_MEMORY_USAGE_GPU_ONLY is common unless it's a staging image or read by CPU.
        // This should be configurable if VknImage needs to support different use cases.
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; // Or a more specific usage
        // Add flags like VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT if needed for certain image types/usages

        // Assuming VknDevice stores the VmaAllocator and it's retrievable via m_engine and m_absIdxs
        // This might need adjustment based on how you expose the VmaAllocator.
        // If VknDevice has a getVmaAllocator() method:
        // VmaAllocator vmaAllocator = m_engine->getDevice(m_absIdxs.get<VkDevice>())->getVmaAllocator();
        // For now, let's assume it's directly accessible via m_engine->getObject

        VknResult res{vmaCreateImage(m_engine->getObject<VmaAllocator>(m_absIdxs),
                                     imageInfo, &allocInfo,
                                     &m_engine->getObject<VkImage>(m_absIdxs), // VknEngine stores the VkImage handle
                                     &m_engine->addNewAllocation<VkImage>(m_absIdxs),
                                     nullptr), // Optional: VmaAllocationInfo
                      "VMA Create Image"};

        m_createdVkImage = true;
    }

    void VknImage::demolishImage()
    {
        if (!m_createdVkImage)
            throw std::runtime_error("VknImage::demolishImage called before createImage().");
        ; // Or throw
        VmaAllocator vmaAllocator = m_engine->getObject<VmaAllocator>(m_absIdxs);
        vmaDestroyImage(vmaAllocator, m_engine->getObject<VkImage>(m_absIdxs), m_allocation);
        m_infos->removeImageCreateInfo(m_relIdxs);
        m_createdVkImage = false;
        m_allocation = VK_NULL_HANDLE; // Reset allocation handle
    }

    VkImage *VknImage::getVkImage()
    {
        return &m_engine->getObject<VkImage>(m_absIdxs);
    }

    // Setters remain the same
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
}