#include "include/VknDevice.hpp"
#include "include/VknCommon.hpp"

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
        if (swapchainIdx + 1 > m_swapchains.size())
            throw std::runtime_error("AddSwapchain index invalid.");
        VknIdxs absIdxs = m_absIdxs;
        VknIdxs relIdxs = m_relIdxs;
        absIdxs.swapchainIdx = m_engine->push_back(VkSwapchainKHR{});
        relIdxs.swapchainIdx = m_swapchains.size();
        VknSwapchain &swapchain = m_swapchains.emplace_back(m_engine, relIdxs, absIdxs, m_infos);

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
        return getListElement(swapchainIdx, m_swapchains);
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
        return getListElement(renderpassIdx, m_renderpasses);
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
        VknIdxs relIdxs = m_relIdxs;
        VknIdxs absIdxs = m_absIdxs;
        absIdxs.renderpassIdx = m_engine->push_back(VkRenderPass{});
        relIdxs.renderpassIdx = m_renderpasses.size();
        return &m_renderpasses.emplace_back(m_engine, relIdxs, absIdxs, m_infos);
    }
} // namespace vkn