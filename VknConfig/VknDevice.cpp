#include "include/VknDevice.hpp"
#include "include/VknCommon.hpp"

namespace vkn
{
    VknDevice::VknDevice(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        addNewVknObject<VknPhysicalDevice, VkPhysicalDevice>(
            m_relIdxs.get<VkDevice>(), m_physicalDevices, m_engine, m_relIdxs, m_absIdxs, m_infos);
    }

    VknSwapchain *VknDevice::addSwapchain(
        uint32_t swapchainIdx, VkSurfaceKHR *surface, uint32_t imageCount,
        uint32_t imageWidth, uint32_t imageHeight)
    {
        VknSwapchain &swapchain = addNewVknObject<VknSwapchain, VkSwapchainKHR>(
            swapchainIdx, m_swapchains, m_engine, m_relIdxs, m_absIdxs, m_infos);
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
        return &m_physicalDevices[0];
    }

    VkDevice *VknDevice::getVkDevice()
    {
        if (!m_createdVkDevice)
            throw std::runtime_error("Logical device not created before retrieving it.");
        return &m_engine->getObject<VkDevice>(m_absIdxs.get<VkDevice>());
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
        if (!m_physicalDevices[0].areQueuesSelected())
            throw std::runtime_error("Queues not selected before trying to create device.");
        if (m_createdVkDevice)
            throw std::runtime_error("Device already created.");
        m_infos->fillDeviceExtensionNames(m_relIdxs.get<VkDevice>(), m_extensions, m_extensionsSize);
        m_infos->fillDeviceFeatures(m_features);
        m_physicalDevices[0].fillQueueCreateInfos();
        m_infos->fillDeviceCreateInfo(m_relIdxs.get<VkDevice>());
        VknResult res{
            vkCreateDevice(
                *(m_physicalDevices[0].getVkPhysicalDevice()),
                m_infos->getDeviceCreateInfo(m_relIdxs.get<VkPhysicalDevice>()),
                nullptr,
                &m_engine->getObject<VkDevice>(m_absIdxs.get<VkDevice>())),
            "Create device"};
        m_createdVkDevice = true;
        return res;
    }

    VknRenderpass *VknDevice::addRenderpass(uint32_t renderpassIdx)
    {
        return &addNewVknObject<VknRenderpass, VkRenderPass>(
            renderpassIdx, m_renderpasses, m_engine, m_relIdxs, m_absIdxs, m_infos);
    }
} // namespace vkn