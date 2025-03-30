#include "include/VknDevice.hpp"

namespace vkn
{
    VknDevice::VknDevice(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos},
          m_physicalDevice{engine, relIdxs, absIdxs, m_infos}
    {
    }

    VknSwapchain *VknDevice::addSwapchain(
        uint32_t swapchainIdx, VkSurfaceKHR *surface, uint32_t imageCount,
        uint32_t imageWidth, uint32_t imageHeight)
    {
        m_relIdxs.swapchainIdx = swapchainIdx;
        VknSwapchain &swapchain = m_swapchains.emplace_back(m_engine, m_relIdxs, m_absIdxs, m_infos);
        swapchain.setImageCount(imageCount);
        swapchain.setImageDimensions(imageWidth, imageHeight);
        return &swapchain;
    }

    void VknDevice::fillSwapchainCreateInfos()
    {
        for (auto &swapchain : m_swapchains)
            swapchain.fillSwapchainCreateInfo();
    }

    VknSwapchain *VknDevice::getSwapchain(uint32_t swapchainIdx)
    {
        if (swapchainIdx + 1 > m_swapchains.size())
            throw std::runtime_error("GetSwapchain index out of range.");
        std::list<VknSwapchain>::iterator it = m_swapchains.begin();
        std::advance(it, swapchainIdx);
        return &(*it);
    }

    VknPhysicalDevice *VknDevice::getPhysicalDevice()
    {
        return &m_physicalDevice;
    }

    VkDevice *VknDevice::getVkDevice()
    {
        if (!m_createdVkDevice)
            throw std::runtime_error("Logical device not created before retrieving it.");
        return &m_engine->getObject<VkDevice>(m_absIdxs.deviceIdx.value());
    }

    VknRenderpass *VknDevice::getRenderpass(uint32_t renderpassIdx)
    {
        if (renderpassIdx + 1 > m_renderpasses.size())
            throw std::runtime_error("GetRenderpass index out of range.");
        std::list<VknRenderpass>::iterator it = m_renderpasses.begin();
        std::advance(it, renderpassIdx);
        return &(*it);
    }

    void VknDevice::addExtensions(const char *ext[], uint32_t size)
    {
        m_extensions = ext;
        m_extensionsSize = size;
    }

    VknResult VknDevice::createDevice()
    {
        if (!m_physicalDevice.areQueuesSelected())
            throw std::runtime_error("Queues not selected before trying to create device.");
        if (m_createdVkDevice)
            throw std::runtime_error("Device already created.");
        m_infos->fillDeviceExtensionNames(m_relIdxs.deviceIdx.value(), m_extensions, m_extensionsSize);
        m_infos->fillDeviceFeatures(m_features);
        m_physicalDevice.fillQueueCreateInfos();
        m_infos->fillDeviceCreateInfo(m_relIdxs.deviceIdx.value());
        m_absIdxs.deviceIdx = m_engine->push_back(VkDevice{});
        VknResult res{
            vkCreateDevice(
                *(m_physicalDevice.getVkPhysicalDevice()),
                m_infos->getDeviceCreateInfo(m_relIdxs.deviceIdx.value()),
                nullptr,
                &m_engine->getObject<VkDevice>(m_absIdxs.deviceIdx.value())),
            "Create device"};
        m_createdVkDevice = true;
        return res;
    }

    VknRenderpass *VknDevice::addRenderpass(uint32_t renderpassIdx)
    {
        if (renderpassIdx != m_renderpasses.size())
            throw std::runtime_error("RenderpassIdx passed to addRenderpass is invalid. Should be next idx.");
        m_relIdxs.renderpassIdx = renderpassIdx;
        return &m_renderpasses.emplace_back(m_engine, m_relIdxs, m_absIdxs, m_infos);
    }
} // namespace vkn