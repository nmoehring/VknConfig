#include "include/VknSwapchain.hpp"

namespace vkn
{
    VknSwapchain::VknSwapchain(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
    {
        m_instanceLock = this;
    }

    void VknSwapchain::setImageCount()
    {
        if (!m_setSurface)
            throw std::runtime_error("Can't set swapchain image count until surface is added.");
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");
        if (m_setImageCount)
            throw std::runtime_error("Already set swapchain image count.");
        VkSurfaceCapabilitiesKHR capabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            s_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            s_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
            &capabilities);
        m_imageCount = capabilities.minImageCount;
        m_setImageCount = true;
    }

    void VknSwapchain::setImageDimensions()
    {
        if (!m_setSurface)
            throw std::runtime_error("Can't set image dimensions until surface is added.");
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");

        VkSurfaceCapabilitiesKHR capabilities{};

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            s_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            s_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
            &capabilities);
        m_dimensions = {capabilities.currentExtent.width,
                        capabilities.currentExtent.height};
        m_setImageDimensions = true;

        // This check is a safeguard. The primary prevention should be in VknCycle::recoverFromSwapchainError.
        if (m_dimensions.width == 0 || m_dimensions.height == 0)
        {
            throw std::runtime_error(
                "VknSwapchain::setImageDimensions trying to set zero extent. Window likely minimized. "
                "This should have been caught by pre-checks in VknCycle::recoverFromSwapchainError.");
        }
    }

    void VknSwapchain::setSurfaceFormat(VkFormat format, VkColorSpaceKHR colorSpace)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");
        if (!m_setSurface)
            throw std::runtime_error("Can't set surface format until surface is added.");
        uint32_t surfaceFormatCount{0};
        VknVector<VkSurfaceFormatKHR> surfaceFormats{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            s_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            s_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
            &surfaceFormatCount, nullptr);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            s_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            s_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
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
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");
        m_numImageArrayLayers = numImageLayers;
    }

    void VknSwapchain::setUsage(VkImageUsageFlags usage)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");
        m_usage = usage;
    }

    void VknSwapchain::setSharingMode(VkSharingMode sharingMode)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");
        m_sharingMode = sharingMode;
    }

    void VknSwapchain::setPreTransform(VkSurfaceTransformFlagBitsKHR preTransform)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");
        m_preTransform = preTransform;
    }

    void VknSwapchain::setCompositeAlpha(VkCompositeAlphaFlagBitsKHR compositeAlpha)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");
        m_compositeAlpha = compositeAlpha;
    }

    void VknSwapchain::setClipped(bool clipped)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");
        m_clipped = clipped;
    }

    void VknSwapchain::setOldSwapchain(VkSwapchainKHR oldSwapchain)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filed.");
        m_oldSwapchain = oldSwapchain;
    }

    VkSwapchainKHR *VknSwapchain::getVkSwapchain()
    {
        return &s_engine->getObject<VkSwapchainKHR>(m_absIdxs);
    }

    VkSwapchainCreateInfoKHR *VknSwapchain::fileSwapchainCreateInfo()
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Already filed swapchain create info.");
        if (!m_setSurface)
            throw std::runtime_error("Can't file swapchain create info until surface is added.");
        VkSwapchainCreateInfoKHR *ci = s_infos->fileSwapchainCreateInfo(m_relIdxs,
                                                                        &s_engine->getObject<VkSurfaceKHR>(m_surfaceIdx.value()),
                                                                        m_imageCount, m_dimensions, m_surfaceFormat,
                                                                        m_numImageArrayLayers, m_usage, m_sharingMode,
                                                                        m_preTransform, m_compositeAlpha, m_presentMode,
                                                                        m_clipped, m_oldSwapchain);
        m_filedCreateInfo = true;
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

        this->fileSwapchainCreateInfo();
        VkSwapchainCreateInfoKHR *createInfo{
            s_infos->getSwapchainCreateInfo(m_relIdxs)};
        VknResult res{
            vkCreateSwapchainKHR(s_engine->getObject<VkDevice>(m_absIdxs),
                                 createInfo, nullptr,
                                 &s_engine->getObject<VkSwapchainKHR>(m_absIdxs)),
            "Create swapchain"};

        m_createdSwapchain = true;

        if (!m_gotSwapchainImages)
            this->getSwapchainImages();
    }

    void VknSwapchain::getSwapchainImages()
    {
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't get swapchain image views before creating the swapchain.");

        if (m_vkSwapchainImages.isEmpty())
            m_vkSwapchainImages.grow(m_imageCount);

        uint32_t imageCount{0};
        vkGetSwapchainImagesKHR(s_engine->getObject<VkDevice>(m_absIdxs),
                                s_engine->getObject<VkSwapchainKHR>(m_absIdxs),
                                &imageCount, VK_NULL_HANDLE);

        if (imageCount != m_imageCount)
            throw std::runtime_error("Swapchain imageCount does not equal what should have been set.");
        vkGetSwapchainImagesKHR(s_engine->getObject<VkDevice>(m_absIdxs),
                                s_engine->getObject<VkSwapchainKHR>(m_absIdxs),
                                &imageCount, m_vkSwapchainImages.getData());

        m_gotSwapchainImages = true;
    }

    void VknSwapchain::initializeSwapchainImageViewFromFramebuffer(VknImageView *imageView, uint32_t framebufferIdx)
    {
        if (framebufferIdx >= m_imageCount)
            throw std::runtime_error("Trying to create too many swapchain imageviews.");

        this->setSwapchainImageViewSettings(imageView, framebufferIdx);
        this->createImageView(imageView);

        if (framebufferIdx == m_imageCount - 1u)
        {
            m_setImageViewSettings = true;
            m_createdImageViews = true;
        }
    }

    void VknSwapchain::setSwapchainImageViewSettings(VknImageView *imageView, uint32_t framebufferIdx)
    {
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't set swapchain imageview settings before creating swapchain.");
        if (m_setImageViewSettings)
            throw std::runtime_error("Already set image view settings.");

        imageView->setImage(&m_vkSwapchainImages(framebufferIdx));
        imageView->setFormat(m_surfaceFormat.format);
    }

    void VknSwapchain::createImageView(VknImageView *imageView)
    {
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't create image views before creating the swapchain.");
        if (m_createdImageViews)
            throw std::runtime_error("Already created image views.");

        imageView->createImageView();
    }

    void VknSwapchain::setSurface(uint32_t surfaceIdx)
    {
        if (m_setSurface)
            throw std::runtime_error("Surface already set on swapchain.");

        m_surfaceIdx = surfaceIdx;
        m_setSurface = true;
    }

    uint32_t VknSwapchain::getNumImages()
    {
        return m_imageCount;
    }

    void VknSwapchain::demolishSwapchain()
    {
        m_vkSwapchainImages.clear();
        vkDestroySwapchainKHR(
            s_engine->getObject<VkDevice>(m_absIdxs),
            s_engine->getObject<VkSwapchainKHR>(m_absIdxs),
            nullptr);

        m_setImageDimensions = false;
        m_setImageCount = false;

        m_gotSwapchainImages = false;
        m_setImageViewSettings = false;
        m_createdImageViews = false;

        m_filedCreateInfo = false;
        m_createdSwapchain = false;
    }

    void VknSwapchain::recreateSwapchain()
    {
        this->demolishSwapchain();
        this->createSwapchain();
    }
}
