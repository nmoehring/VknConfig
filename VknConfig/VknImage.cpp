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
        if (!m_createdVkImage)
            throw std::runtime_error("Image not created before getting.");
        return &m_engine->getObject<VkImage>(m_absIdxs);
    }
}