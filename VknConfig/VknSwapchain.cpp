#include "include/VknSwapchain.hpp"

namespace vkn
{
    VknSwapchain::VknSwapchain(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknSwapchain::setImageCount(uint32_t imageCount)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        if (m_setImageCount)
            throw std::runtime_error("Already set swapchain image count.");
        m_imageCount = imageCount;
        m_setImageCount = true;
    }

    void VknSwapchain::setImageDimensions(uint32_t imageWidth, uint32_t imageHeight)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_dimensions = {imageWidth, imageHeight};
    }

    void VknSwapchain::setSurfaceFormat(VkFormat format, VkColorSpaceKHR colorSpace)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_surfaceFormat = VkSurfaceFormatKHR{format, colorSpace};
    }

    void VknSwapchain::setNumImageLayers(uint32_t numImageLayers)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_numImageArrayLayers = numImageLayers;
    }

    void VknSwapchain::setUsage(VkImageUsageFlags usage)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_usage = usage;
    }

    void VknSwapchain::setSharingMode(VkSharingMode sharingMode)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_sharingMode = sharingMode;
    }

    void VknSwapchain::setPreTransform(VkSurfaceTransformFlagBitsKHR preTransform)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_preTransform = preTransform;
    }

    void VknSwapchain::setCompositeAlpha(VkCompositeAlphaFlagBitsKHR compositeAlpha)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_compositeAlpha = compositeAlpha;
    }

    void VknSwapchain::setPresentMode(VkPresentModeKHR presentMode)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_presentMode = presentMode;
    }

    void VknSwapchain::setClipped(bool clipped)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_clipped = clipped;
    }

    void VknSwapchain::setOldSwapchain(VkSwapchainKHR oldSwapchain)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_oldSwapchain = oldSwapchain;
    }

    VkSwapchainKHR *VknSwapchain::getVkSwapchain()
    {
        if (!m_createdSwapchain)
            throw std::runtime_error("Trying to get swapchain before it's created.");
        return &m_engine->getObject<VkSwapchainKHR>(m_absIdxs.get<VkSwapchainKHR>());
    }

    void VknSwapchain::fillSwapchainCreateInfo()
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled swapchain create info.");
        if (!m_setSurface)
            throw std::runtime_error("Can't fill swapchain create info until surface is added.");
        m_infos->fillSwapchainCreateInfo(m_relIdxs,
                                         &m_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
                                         m_imageCount, m_dimensions, m_surfaceFormat,
                                         m_numImageArrayLayers, m_usage, m_sharingMode,
                                         m_preTransform, m_compositeAlpha, m_presentMode,
                                         m_clipped, m_oldSwapchain);
        m_filledCreateInfo = true;
    }

    void VknSwapchain::createSwapchain()
    {
        if (!m_filledCreateInfo)
            throw std::runtime_error("Didn't fill create info before trying to create swapchain.");
        if (!m_setSurface)
            throw std::runtime_error("Surface not set before trying to create swapchain.");
        if (m_createdSwapchain)
            throw std::runtime_error("Already created swapchain.");

        VkSwapchainCreateInfoKHR *createInfo{
            m_infos->getSwapchainCreateInfo(m_relIdxs)};
        VknResult res{
            vkCreateSwapchainKHR(m_engine->getObject<VkDevice>(m_absIdxs.get<VkDevice>()),
                                 createInfo, nullptr,
                                 &m_engine->getObject<VkSwapchainKHR>(m_absIdxs.get<VkSwapchainKHR>())),
            "Create swapchain"};

        m_createdSwapchain = true;
    }

    void VknSwapchain::createImages()
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");

        std::vector<VkImage> *engineImages = &m_engine->getObjectVector<VkImage>();
        engineImages->resize(m_imageCount);
        uint32_t imageCount{0};
        vkGetSwapchainImagesKHR(m_engine->getObject<VkDevice>(m_absIdxs.get<VkDevice>()),
                                m_engine->getObject<VkSwapchainKHR>(m_absIdxs.get<VkSwapchainKHR>()),
                                &imageCount, VK_NULL_HANDLE);
        std::vector<VkImage> swapChainImages(imageCount);
        if (imageCount != m_imageCount)
            throw std::runtime_error("Swapchain imageCount does not equal what should have been set.");
        vkGetSwapchainImagesKHR(m_engine->getObject<VkDevice>(m_absIdxs.get<VkDevice>()),
                                m_engine->getObject<VkSwapchainKHR>(m_absIdxs.get<VkSwapchainKHR>()),
                                &imageCount, engineImages->data() + m_imageStartAbsIdx);
    }

    void VknSwapchain::addImageViews()
    {
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't create image views before creating the swapchain.");

        std::vector<VkImageView> engineImageViews = m_engine->getObjectVector<VkImageView>();
        m_imageStartAbsIdx = engineImageViews.size();
        for (uint32_t i = 0; i < m_imageCount; ++i)
            addNewVknObject<VknImageView, VkImageView>(
                i, m_imageViews, m_engine, m_relIdxs, m_absIdxs, m_infos);
    }

    void VknSwapchain::createImageViews()
    {
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't create image views before creating the swapchain.");
        if (m_imageViews.size() != m_imageCount)
            throw std::runtime_error("Num of image views being added does not equal imageCount.");
        for (auto &imageView : m_imageViews)
            imageView.createImageView();
    }

    void VknSwapchain::setSurface(uint32_t surfaceIdx)
    {
        if (m_setSurface)
            throw std::runtime_error("Surface already set on swapchain.");

        m_surfaceIdx = surfaceIdx;
        m_setSurface = true;
    }
}
