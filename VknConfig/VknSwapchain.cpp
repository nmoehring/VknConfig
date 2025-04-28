#include "include/VknSwapchain.hpp"

namespace vkn
{
    VknSwapchain::VknSwapchain(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        s_editable = this;
    }

    void VknSwapchain::setImageCount(uint32_t imageCount)
    {
        this->testEditability();

        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        if (m_setImageCount)
            throw std::runtime_error("Already set swapchain image count.");
        m_imageCount = imageCount;
        m_setImageCount = true;
    }

    void VknSwapchain::setImageDimensions(uint32_t imageWidth, uint32_t imageHeight)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_dimensions = {imageWidth, imageHeight};
    }

    void VknSwapchain::setSurfaceFormat(VkFormat format, VkColorSpaceKHR colorSpace)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_surfaceFormat = VkSurfaceFormatKHR{format, colorSpace};
    }

    void VknSwapchain::setNumImageLayers(uint32_t numImageLayers)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_numImageArrayLayers = numImageLayers;
    }

    void VknSwapchain::setUsage(VkImageUsageFlags usage)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_usage = usage;
    }

    void VknSwapchain::setSharingMode(VkSharingMode sharingMode)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_sharingMode = sharingMode;
    }

    void VknSwapchain::setPreTransform(VkSurfaceTransformFlagBitsKHR preTransform)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_preTransform = preTransform;
    }

    void VknSwapchain::setCompositeAlpha(VkCompositeAlphaFlagBitsKHR compositeAlpha)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_compositeAlpha = compositeAlpha;
    }

    void VknSwapchain::setPresentMode(VkPresentModeKHR presentMode)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_presentMode = presentMode;
    }

    void VknSwapchain::setClipped(bool clipped)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_clipped = clipped;
    }

    void VknSwapchain::setOldSwapchain(VkSwapchainKHR oldSwapchain)
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_oldSwapchain = oldSwapchain;
    }

    VkSwapchainKHR *VknSwapchain::getVkSwapchain()
    {
        return &m_engine->getObject<VkSwapchainKHR>(m_absIdxs);
    }

    VkSwapchainCreateInfoKHR *VknSwapchain::fillSwapchainCreateInfo()
    {
        this->testEditability();
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled swapchain create info.");
        if (!m_setSurface)
            throw std::runtime_error("Can't fill swapchain create info until surface is added.");
        VkSwapchainCreateInfoKHR *ci = m_infos->fillSwapchainCreateInfo(m_relIdxs,
                                                                        &m_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
                                                                        m_imageCount, m_dimensions, m_surfaceFormat,
                                                                        m_numImageArrayLayers, m_usage, m_sharingMode,
                                                                        m_preTransform, m_compositeAlpha, m_presentMode,
                                                                        m_clipped, m_oldSwapchain);
        m_filledCreateInfo = true;
        return ci;
    }

    void VknSwapchain::createSwapchain()
    {
        this->testEditability();
        if (!m_setSurface)
            throw std::runtime_error("Surface not set before trying to create swapchain.");
        if (m_createdSwapchain)
            throw std::runtime_error("Already created swapchain.");

        this->fillSwapchainCreateInfo();
        VkSwapchainCreateInfoKHR *createInfo{
            m_infos->getSwapchainCreateInfo(m_relIdxs)};
        VknResult res{
            vkCreateSwapchainKHR(m_engine->getObject<VkDevice>(m_absIdxs),
                                 createInfo, nullptr,
                                 m_engine->getVector<VkSwapchainKHR>().getData(1)),
            "Create swapchain"};

        m_createdSwapchain = true;
    }

    std::list<VknImageView> *VknSwapchain::createImages()
    {
        this->testEditability();
        VknVector<VkImage> *engineImages = &m_engine->getVector<VkImage>();
        m_imageStartIdx = engineImages->getSize();
        uint32_t imageCount{0};
        vkGetSwapchainImagesKHR(m_engine->getObject<VkDevice>(m_absIdxs),
                                m_engine->getObject<VkSwapchainKHR>(m_absIdxs),
                                &imageCount, VK_NULL_HANDLE);

        if (imageCount != m_imageCount)
            throw std::runtime_error("Swapchain imageCount does not equal what should have been set.");
        vkGetSwapchainImagesKHR(m_engine->getObject<VkDevice>(m_absIdxs),
                                m_engine->getObject<VkSwapchainKHR>(m_absIdxs),
                                &imageCount, engineImages->getData(m_imageCount));
        this->addImageViews();
        this->createImageViews();
        return &m_imageViews;
    }

    void VknSwapchain::addImageViews()
    {
        this->testEditability();
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't create image views before creating the swapchain.");

        VknVector<VkImageView> engineImageViews = m_engine->getVector<VkImageView>();
        m_imageViewStartIdx = engineImageViews.getSize();
        for (uint32_t i = 0; i < m_imageCount; ++i)
        {
            addNewVknObject<VknImageView, VkImageView>(
                i, m_imageViews, m_engine, m_relIdxs, m_absIdxs, m_infos);
            m_imageViews.back().setImage(m_imageStartIdx + i);
            m_imageViews.back().setFormat(m_surfaceFormat.format);
        }
    }

    void VknSwapchain::createImageViews()
    {
        this->testEditability();
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't create image views before creating the swapchain.");
        if (m_imageViews.size() != m_imageCount)
            throw std::runtime_error("Num of image views being added does not equal imageCount.");
        for (auto &imageView : m_imageViews)
            imageView.createImageView();
    }

    void VknSwapchain::setSurface(uint32_t surfaceIdx)
    {
        this->testEditability();
        if (m_setSurface)
            throw std::runtime_error("Surface already set on swapchain.");

        m_surfaceIdx = surfaceIdx;
        m_setSurface = true;
    }

    std::list<VknImageView> *VknSwapchain::getImageViews()
    {
        return &m_imageViews;
    }

    uint32_t VknSwapchain::getImageViewStartIdx()
    {
        return m_imageViewStartIdx;
    }

    uint32_t VknSwapchain::getNumImages()
    {
        return m_imageCount;
    }

    void VknSwapchain::testEditability()
    {
        if (s_editable != this)
            throw std::runtime_error("Members of a VknSwapchain must be added all at once so that they are stored contiguously.");
    }
}
