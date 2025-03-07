#include "VknSwapchain.hpp"

namespace vkn
{
    VknSwapchain::VknSwapchain() : m_deviceIdx{0}, m_swapchainIdx{0}, m_infos{nullptr},
                                   m_archive{nullptr}, m_surface{VK_NULL_HANDLE},
                                   m_dimensions{0, 0}, m_imageCount{0}, m_placeholder{true},
                                   m_vkDevice{nullptr}, m_createdVkDevice{nullptr} {}

    VknSwapchain::VknSwapchain(uint32_t deviceIdx, uint32_t swapchainIdx, VkDevice *vkDevice,
                               const bool *createdVkDevice, VknInfos *infos, VknResultArchive *archive,
                               VkSurfaceKHR *surface)
        : m_deviceIdx{deviceIdx}, m_swapchainIdx{swapchainIdx}, m_infos{infos}, m_archive{archive},
          m_surface{surface}, m_placeholder{false}, m_vkDevice{vkDevice}, m_createdVkDevice{createdVkDevice}
    {
        this->setImageCount(m_imageCount);
        this->addImageViews();
    }

    VknSwapchain::~VknSwapchain()
    {
        if (!m_placeholder && !m_destroyed)
            this->destroy();
    }

    void VknSwapchain::destroy()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to destroy a placeholder object.");

        if (m_destroyed)
            throw std::runtime_error("Swapchain already destroyed.");

        if (m_createdSwapchain)
            vkDestroySwapchainKHR(*m_vkDevice, m_vkSwapchain, VK_NULL_HANDLE);

        m_destroyed = true;
    }

    void VknSwapchain::setImageCount(uint32_t imageCount)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");

        m_images.resize(imageCount);
        m_imageViews.resize(imageCount);
        m_imageCount = imageCount;
    }

    void VknSwapchain::setImageDimensions(uint32_t imageWidth, uint32_t imageHeight)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_dimensions = {imageWidth, imageHeight};
    }

    void VknSwapchain::setSurfaceFormat(VkFormat format, VkColorSpaceKHR colorSpace)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
    }

    void VknSwapchain::setNumImageLayers(uint32_t numImageLayers)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
    }

    void VknSwapchain::setUsage(VkImageUsageFlags usage)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
    }

    void VknSwapchain::setSharingMode(VkSharingMode sharingMode)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
    }

    void VknSwapchain::setPreTransform(VkSurfaceTransformFlagBitsKHR preTransform)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
    }

    void VknSwapchain::setCompositeAlpha(VkCompositeAlphaFlagBitsKHR compositeAlpha)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
    }

    void VknSwapchain::setPresentMode(VkPresentModeKHR presentMode)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
    }

    void VknSwapchain::setClipped(bool clipped)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_clipped = clipped;
    }

    void VknSwapchain::setOldSwapchain(VkSwapchainKHR oldSwapchain)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");
        m_oldSwapchain = oldSwapchain;
    }

    VkSwapchainKHR *VknSwapchain::getVkSwapchain()
    {
        if (!m_createdSwapchain)
            throw std::runtime_error("Trying to get swapchain before it's created.");
        return &m_vkSwapchain;
    }

    void VknSwapchain::fillSwapchainCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled swapchain create info.");
        if (!m_surfaceSet)
            throw std::runtime_error("Can't fill swapchain create info until surface is added.");
        m_infos->fillSwapchainCreateInfo(m_deviceIdx, m_swapchainIdx, m_surface, m_imageCount,
                                         m_dimensions, m_surfaceFormat, m_numImageArrayLayers, m_usage,
                                         m_sharingMode, m_preTransform, m_compositeAlpha,
                                         m_presentMode, m_clipped, m_oldSwapchain);
        m_filledCreateInfo = true;
    }

    void VknSwapchain::createSwapchain()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_filledCreateInfo)
            throw std::runtime_error("Didn't fill create info before trying to create swapchain.");
        if (!(*m_createdVkDevice))
            throw std::runtime_error("Logical device not created before trying to create swapchain.");
        if (m_createdSwapchain)
            throw std::runtime_error("Already created swapchain.");

        VkSwapchainCreateInfoKHR *createInfo{m_infos->getSwapchainCreateInfo(m_deviceIdx, m_swapchainIdx)};
        VknResult res{
            vkCreateSwapchainKHR(*m_vkDevice, createInfo, nullptr, &m_vkSwapchain),
            "Create swapchain"};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating swapchain."));
        m_archive->store(res);

        m_createdSwapchain = true;
    }

    void VknSwapchain::getImages()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Trying to configure swapchain after create info already filled.");

        uint32_t imageCount{0};
        vkGetSwapchainImagesKHR(*m_vkDevice, m_vkSwapchain, &imageCount, VK_NULL_HANDLE);
        std::vector<VkImage> swapChainImages(imageCount);
        vkGetSwapchainImagesKHR(*m_vkDevice, m_vkSwapchain, &imageCount, m_images.data());
    }

    void VknSwapchain::addImageViews()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't create image views before creating the swapchain.");
        m_imageViews.resize(m_imageCount);
        for (uint32_t imageViewIdx = 0; imageViewIdx < m_imageCount; ++imageViewIdx)
            m_imageViews[imageViewIdx] = VknImageView{m_deviceIdx, m_swapchainIdx, imageViewIdx,
                                                      m_vkDevice, m_createdVkDevice, m_infos, m_archive,
                                                      &m_rawImageViews[imageViewIdx]};
    }

    void VknSwapchain::createImageViews()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_createdSwapchain)
            throw std::runtime_error("Can't create image views before creating the swapchain.");
        if (m_imageViews.size() != m_imageCount)
            throw std::runtime_error("Num of image views being added does not equal imageCount.");
        for (auto &imageView : m_imageViews)
            imageView.createImageView();
    }

    void VknSwapchain::setSurface(VkSurfaceKHR *surface)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_surfaceSet)
            throw std::runtime_error("Surface already set on swapchain.");
        m_surface = surface;
        m_surfaceSet = true;
    }

    std::vector<VkImageView> *VknSwapchain::getVkImageViews()
    {
        return &m_rawImageViews;
    }
}
