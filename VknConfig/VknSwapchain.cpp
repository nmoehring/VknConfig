#include "VknSwapchain.hpp"

namespace vkn
{
    VknSwapchain::VknSwapchain() : m_deviceIdx{0}, m_swapchainIdx{0}, m_infos{nullptr},
                                   m_archive{nullptr}, m_surface{VK_NULL_HANDLE},
                                   m_dimensions{0, 0}, m_imageCount{0}, m_placeholder{true},
                                   m_vkDevice{nullptr}, m_createdVkDevice{nullptr} {}

    VknSwapchain::VknSwapchain(uint32_t deviceIdx, uint32_t swapchainIdx, VkDevice *vkDevice,
                               const bool *createdVkDevice, VknInfos *infos, VknResultArchive *archive,
                               uint32_t imageCount, VkSurfaceKHR surface, uint32_t imageWidth,
                               uint32_t imageHeight)
        : m_deviceIdx{deviceIdx}, m_swapchainIdx{swapchainIdx}, m_infos{infos}, m_archive{archive},
          m_imageCount{imageCount}, m_surface{surface}, m_dimensions{imageWidth, imageHeight},
          m_placeholder{false}, m_vkDevice{vkDevice}
    {
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

        vkDestroySwapchainKHR(*m_vkDevice, m_vkSwapchain, VK_NULL_HANDLE);

        m_destroyed = true;
    }

    void VknSwapchain::setSurfaceFormat(VkFormat format, VkColorSpaceKHR colorSpace)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
    }

    void VknSwapchain::setNumImageLayers(uint32_t numImageLayers)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
    }

    void VknSwapchain::setUsage(VkImageUsageFlags usage)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
    }

    void VknSwapchain::setSharingMode(VkSharingMode sharingMode)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
    }

    void VknSwapchain::setPreTransform(VkSurfaceTransformFlagBitsKHR preTransform)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
    }

    void VknSwapchain::setCompositeAlpha(VkCompositeAlphaFlagBitsKHR compositeAlpha)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
    }

    void VknSwapchain::setPresentMode(VkPresentModeKHR presentMode)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
    }

    void VknSwapchain::setClipped(bool clipped)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
    }

    void VknSwapchain::setOldSwapchain(VkSwapchainKHR oldSwapchain)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
    }

    VkSwapchainKHR *VknSwapchain::getVkSwapchain()
    {
        return &m_vkSwapchain;
    }

    void VknSwapchain::fillSwapchainCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled swapchain create info.");
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
        if (!m_createdVkDevice)
            throw std::runtime_error("Logical device not created before trying to create swapchain.");

        VkSwapchainCreateInfoKHR *createInfo{m_infos->getSwapchainCreateInfo(m_deviceIdx, m_swapchainIdx)};
        VknResult res{
            vkCreateSwapchainKHR(*m_vkDevice, createInfo, nullptr, &m_vkSwapchain),
            "Create swapchain"};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating swapchain."));
        m_archive->store(res);
    }
}