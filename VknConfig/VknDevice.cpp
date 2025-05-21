#include "include/VknDevice.hpp"

namespace vkn
{
    VknDevice::VknDevice(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_instanceLock = this;
        m_engine->addNewVknObject<VknPhysicalDevice, VkPhysicalDevice, VkInstance>(
            m_relIdxs.get<VkDevice>(), m_physicalDevices, m_relIdxs, m_absIdxs, m_infos);
    }

    VknSwapchain *VknDevice::addSwapchain(uint32_t swapchainIdx)
    {
        if (!m_swapchainExtensionEnabled)
            throw std::runtime_error("Swapchain extension not enabled before adding swapchain.");
        m_instanceLock(this);
        VknSwapchain &swapchain = m_engine->addNewVknObject<VknSwapchain, VkSwapchainKHR, VkDevice>(
            swapchainIdx, m_swapchains, m_relIdxs, m_absIdxs, m_infos);
        return &swapchain;
    }

    VknSwapchain *VknDevice::getSwapchain(uint32_t swapchainIdx)
    {
        return getListElement(swapchainIdx, m_swapchains);
    }

    VknPhysicalDevice *VknDevice::getPhysicalDevice()
    {
        return getListElement(0, m_physicalDevices);
    }

    VkDevice *VknDevice::getVkDevice()
    {
        if (!m_createdVkDevice)
            throw std::runtime_error("Logical device not created before retrieving it.");
        return &m_engine->getObject<VkDevice>(m_absIdxs);
    }

    VknRenderpass *VknDevice::getRenderpass(uint32_t renderpassIdx)
    {
        return getListElement(renderpassIdx, m_renderpasses);
    }

    void VknDevice::addExtension(std::string extension)
    {
        if (extension == VK_KHR_SWAPCHAIN_EXTENSION_NAME)
            m_swapchainExtensionEnabled = true;
        m_infos->addDeviceExtension(extension, m_relIdxs);
    }

    uint32_t VknDevice::findGraphicsQueue()
    {
        // Find a queue family that supports graphics operations.
        // In a real application, you'd select the best queue family based on your needs.
        // For this simple demo, we'll assume the first queue family with graphics support is sufficient.
        uint32_t graphicsQueueFamilyIndex = -1; // Invalid index
        for (int i = 0; i < this->getPhysicalDevice()->getNumQueueFamilies(); ++i)
        {
            if (getPhysicalDevice()->getQueue(i).supportsGraphics())
            {
                graphicsQueueFamilyIndex = i;
                break;
            }
        }
        if (graphicsQueueFamilyIndex == -1)
            throw std::runtime_error("Failed to find a graphics queue family.");
        return graphicsQueueFamilyIndex;
    }

    VknCommandPool *VknDevice::addCommandPool(uint32_t newCommandPoolIdx)
    {
        m_instanceLock(this);
        if (!m_createdVkDevice)
            throw std::runtime_error("Logical device not created before creating command pool.");
        return &m_engine->addNewVknObject<VknCommandPool, VkCommandPool, VkDevice>(
            newCommandPoolIdx, m_commandPools, m_relIdxs, m_absIdxs, m_infos);
    }

    void VknDevice::createSyncObjects(uint32_t maxFramesInFlight)
    {
        m_instanceLock(this);
        if (m_syncObjectsCreated)
            throw std::runtime_error("Synchronization objects already created.");
        if (!m_createdVkDevice)
            throw std::runtime_error("Logical device not created before creating sync objects.");

        for (uint32_t i = 0; i < maxFramesInFlight; ++i)
        {
            m_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            m_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            m_engine->addNewObject<VkFence, VkDevice>(m_absIdxs);
        }

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Create fences in signaled state

        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            VknResult res1{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr, &this->getImageAvailableSemaphore(i)),
                           "Create image available semaphore"};
            VknResult res2{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr, &this->getRenderFinishedSemaphore(i)),
                           "Create render finished semaphore"};
            VknResult res3{vkCreateFence(
                               *getVkDevice(), &fenceInfo, nullptr, &this->getFence(i)),
                           "Create in flight fence"};
        }
        m_syncObjectsCreated = true;
    }

    VkSemaphore &VknDevice::getImageAvailableSemaphore(uint32_t frameInFlight)
    {
        uint32_t startIdx = m_absIdxs.get<VkSemaphore>();
        return m_engine->getObject<VkSemaphore>(m_absIdxs.get<VkSemaphore>() - (frameInFlight * 2u));
    }

    VkSemaphore &VknDevice::getRenderFinishedSemaphore(uint32_t frameInFlight)
    {
        uint32_t startIdx = m_absIdxs.get<VkSemaphore>() - 1u;
        return m_engine->getObject<VkSemaphore>(m_absIdxs.get<VkSemaphore>() - (frameInFlight * 2u));
    }

    VkFence &VknDevice::getFence(uint32_t frameInFlight)
    {
        return m_engine->getObject<VkFence>(m_absIdxs.get<VkFence>() - frameInFlight);
    }

    VknResult VknDevice::createDevice()
    {
        VknPhysicalDevice *physicalDevice = getListElement(0, m_physicalDevices);
        if (m_createdVkDevice)
            throw std::runtime_error("Device already created.");
        if (!physicalDevice->areQueuePrioritiesFilled())
            physicalDevice->fillDeviceQueuePrioritiesDefault(); // Subtle initiation of chain-reaction default configurations
        m_infos->fillDeviceFeatures(features);
        physicalDevice->fillQueueCreateInfos();
        m_infos->fillDeviceCreateInfo(m_relIdxs.get<VkDevice>());
        VknResult res{
            vkCreateDevice(
                *(getListElement(0, m_physicalDevices)->getVkPhysicalDevice()),
                m_infos->getDeviceCreateInfo(m_relIdxs.get<VkDevice>()),
                nullptr,
                &m_engine->getObject<VkDevice>(m_absIdxs)),
            "Create device"};

        m_createdVkDevice = true;
        return res;
    }

    VkQueue *VknDevice::getGraphicsQueue(uint32_t queueIdx)
    {
        // Get queue handles (assuming we selected at least one graphics queue)
        // In a real app, you'd store these handles based on queue family index and type.
        // For this demo, let's just get the first graphics queue.
        uint32_t graphicsQueueFamilyIndex = -1;
        for (int i = 0; i < this->getPhysicalDevice()->getNumQueueFamilies(); ++i)
        {
            if (this->getPhysicalDevice()->getQueue(i).supportsGraphics())
            {
                graphicsQueueFamilyIndex = i;
                break;
            }
        }
        if (graphicsQueueFamilyIndex == -1)
            throw std::runtime_error("Failed to find a graphics queue family after device creation.");

        vkGetDeviceQueue(*getVkDevice(), graphicsQueueFamilyIndex, queueIdx, &m_lastUsedGraphicsQueue); // Store the graphics queue handle
        return &m_lastUsedGraphicsQueue;
    }

    VknRenderpass *VknDevice::addRenderpass(uint32_t renderpassIdx)
    {
        m_instanceLock(this);
        return &m_engine->addNewVknObject<VknRenderpass, VkRenderPass, VkDevice>(
            renderpassIdx, m_renderpasses, m_relIdxs, m_absIdxs, m_infos);
    }

    VknCommandPool *VknDevice::getCommandPool(uint32_t commandPoolIdx)
    {
        // getListElement is a template function, ensure it's accessible here
        // (it's in VknData.hpp, which should be included by VknDevice.hpp or .cpp)
        return getListElement(commandPoolIdx, m_commandPools);
    }
} // namespace vkn