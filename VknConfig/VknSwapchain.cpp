#include "include/VknSwapchain.hpp"

namespace vkn
{
    VknSwapchain::VknSwapchain(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_instanceLock = this;
    }

    void VknSwapchain::setImageCount()
    {
        if (!m_setSurface)
            throw std::runtime_error("Can't set swapchain image count until surface is added.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        if (m_setImageCount)
            throw std::runtime_error("Already set swapchain image count.");
        VkSurfaceCapabilitiesKHR capabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            m_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
            &capabilities);
        m_imageCount = capabilities.minImageCount;
        m_setImageCount = true;
    }

    void VknSwapchain::setImageDimensions()
    {
        if (!m_setSurface)
            throw std::runtime_error("Can't set image dimensions until surface is added.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        VkSurfaceCapabilitiesKHR capabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            m_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
            &capabilities);
        m_dimensions = {capabilities.currentExtent.width,
                        capabilities.currentExtent.height};
        m_setImageDimensions = true;
    }

    void VknSwapchain::setSurfaceFormat(VkFormat format, VkColorSpaceKHR colorSpace)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        if (!m_setSurface)
            throw std::runtime_error("Can't set surface format until surface is added.");
        uint32_t surfaceFormatCount{0};
        VknVector<VkSurfaceFormatKHR> surfaceFormats{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            m_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
            &surfaceFormatCount, nullptr);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            m_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
            &surfaceFormatCount, surfaceFormats.getData(surfaceFormatCount));
        bool formatFound{false};
        for (uint32_t i = 0; i < surfaceFormatCount; ++i)
        {
            if (surfaceFormats(i).format == format &&
                surfaceFormats(i).colorSpace == colorSpace)
            {
                formatFound = true;
                break;
            }
        }
        if (!formatFound)
            throw std::runtime_error("Surface format not found.");
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
        return &m_engine->getObject<VkSwapchainKHR>(m_absIdxs);
    }

    VkSwapchainCreateInfoKHR *VknSwapchain::fillSwapchainCreateInfo()
    {
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
        if (!m_setSurface)
            this->setSurface();
        if (!m_setImageCount)
            this->setImageCount();
        if (!m_setImageDimensions)
            this->setImageDimensions();

        if (m_createdSwapchain)
            throw std::runtime_error("Already created swapchain.");

        this->fillSwapchainCreateInfo();
        VkSwapchainCreateInfoKHR *createInfo{
            m_infos->getSwapchainCreateInfo(m_relIdxs)};
        VknResult res{
            vkCreateSwapchainKHR(m_engine->getObject<VkDevice>(m_absIdxs),
                                 createInfo, nullptr,
                                 &m_engine->getObject<VkSwapchainKHR>(m_absIdxs)),
            "Create swapchain"};

        if (!m_gotSwapchainImages)
            this->getSwapchainImages();
        if (!m_addedSwapchainImageViews)
            this->addImageViews();
        if (!m_setImageViewSettings)
            this->setSwapchainImageViewSettings();
        if (!m_createdImageViews)
            this->createImageViews();

        m_createdSwapchain = true;
    }

    void VknSwapchain::getSwapchainImages()
    {
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't get swapchain image views before creating the swapchain.");

        uint32_t imageCount{0};
        vkGetSwapchainImagesKHR(m_engine->getObject<VkDevice>(m_absIdxs),
                                m_engine->getObject<VkSwapchainKHR>(m_absIdxs),
                                &imageCount, VK_NULL_HANDLE);

        if (imageCount != m_imageCount)
            throw std::runtime_error("Swapchain imageCount does not equal what should have been set.");
        vkGetSwapchainImagesKHR(m_engine->getObject<VkDevice>(m_absIdxs),
                                m_engine->getObject<VkSwapchainKHR>(m_absIdxs),
                                &imageCount, m_vkSwapchainImages.getData(m_imageCount));

        m_gotSwapchainImages = true;
    }

    void VknSwapchain::addImageViews()
    {
        m_instanceLock(this);
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't create image views before creating the swapchain.");

        VknVector<VkImageView> engineImageViews = m_engine->getVector<VkImageView>();
        m_imageViewStartIdx = engineImageViews.getSize();
        for (uint32_t i = 0; i < m_imageCount; ++i)
        {
            m_engine->addNewVknObject<VknImageView, VkImageView, VkDevice>(
                i, m_imageViews, m_relIdxs, m_absIdxs, m_infos);
            m_imageViews.back().setImage(&m_vkSwapchainImages(i));
            m_imageViews.back().setFormat(m_surfaceFormat.format);
        }
        m_addedSwapchainImageViews = true;
    }

    void VknSwapchain::setSwapchainImageViewSettings()
    {
        for (uint32_t i = 0; i < m_imageCount; ++i)
        {
            VknImageView *imageView = getListElement<VknImageView>(i, m_imageViews);
            imageView->setImage(&m_vkSwapchainImages(i));
            imageView->setFormat(m_surfaceFormat.format);
        }
        m_setImageViewSettings = true;
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

    uint32_t VknSwapchain::getImageViewStartIdx()
    {
        return m_imageViewStartIdx;
    }

    uint32_t VknSwapchain::getNumImages()
    {
        return m_imageCount;
    }

    void VknSwapchain::demolishSwapchain()
    {
        vkDestroySwapchainKHR(
            m_engine->getObject<VkDevice>(m_absIdxs),
            m_engine->getObject<VkSwapchainKHR>(m_absIdxs),
            nullptr);
    }

    void VknSwapchain::demolishImageViews()
    {
        for (uint32_t i = 0; i < m_imageCount; ++i)
            getListElement(i, m_imageViews)->demolishImageView();
    }

    void VknSwapchain::recreateSwapchain()
    {
        vkDeviceWaitIdle(m_engine->getObject<VkDevice>(m_absIdxs));
        this->demolishImageViews();
        this->demolishSwapchain();

        m_setImageDimensions = false;
        m_setImageCount = false;

        m_gotSwapchainImages = false;
        m_setImageViewSettings = false;
        m_createdImageViews = false;

        m_filledCreateInfo = false;
        m_createdSwapchain = false;

        this->createImageViews();
        this->createSwapchain();
    }
}
