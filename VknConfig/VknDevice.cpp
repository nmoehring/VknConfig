#include "include/VknDevice.hpp"

namespace vkn
{
    VknDevice::VknDevice(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_engine->addNewVknObject<VknPhysicalDevice, VkPhysicalDevice, VkInstance>(
            m_relIdxs.get<VkDevice>(), m_physicalDevices, m_relIdxs, m_absIdxs, m_infos);
    }

    VknDevice::~VknDevice()
    {
        if (m_createdVkDevice)
        {
            // Wait for device idle before destroying resources
            vkDeviceWaitIdle(*getVkDevice());

            // Destroy synchronization objects
            for (auto &fence : m_inFlightFences)
                vkDestroyFence(*getVkDevice(), fence, nullptr);
            for (auto &semaphore : m_renderFinishedSemaphores)
                vkDestroySemaphore(*getVkDevice(), semaphore, nullptr);
            for (auto &semaphore : m_imageAvailableSemaphores)
                vkDestroySemaphore(*getVkDevice(), semaphore, nullptr);
        }
    }

    VknSwapchain *VknDevice::addSwapchain(uint32_t swapchainIdx)
    {
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

    void VknDevice::addExtensions(const char *ext[], uint32_t size)
    {
        m_extensions = ext;
        m_extensionsSize = size;
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
        if (!m_createdVkDevice)
            throw std::runtime_error("Logical device not created before creating command pool.");
        return &m_engine->addNewVknObject<VknCommandPool, VkCommandPool, VkDevice>(
            newCommandPoolIdx, m_commandPools, m_relIdxs, m_absIdxs, m_infos);
    }

    void VknDevice::createSyncObjects(uint32_t maxFramesInFlight)
    {
        if (m_syncObjectsCreated)
            throw std::runtime_error("Synchronization objects already created.");
        if (!m_createdVkDevice)
            throw std::runtime_error("Logical device not created before creating sync objects.");

        m_imageAvailableSemaphores.resize(maxFramesInFlight);
        m_renderFinishedSemaphores.resize(maxFramesInFlight);
        m_inFlightFences.resize(maxFramesInFlight, VK_NULL_HANDLE); // Initialize fences to VK_NULL_HANDLE

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Create fences in signaled state

        for (size_t i = 0; i < maxFramesInFlight; ++i)
        {
            VknResult res1{vkCreateSemaphore(*getVkDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]), "Create image available semaphore"};
            VknResult res2{vkCreateSemaphore(*getVkDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]), "Create render finished semaphore"};
            VknResult res3{vkCreateFence(*getVkDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]), "Create in flight fence"};
        }
        m_syncObjectsCreated = true;
    }

    VknResult VknDevice::createDevice()
    {
        if (!getListElement(0, m_physicalDevices)->areQueuesSelected())
            throw std::runtime_error("Queues not selected before trying to create device.");
        if (m_createdVkDevice)
            throw std::runtime_error("Device already created.");
        m_infos->fillDeviceExtensionNames(m_relIdxs.get<VkDevice>(), m_extensions, m_extensionsSize);
        m_infos->fillDeviceFeatures(features);
        getListElement(0, m_physicalDevices)->fillQueueCreateInfos();
        m_infos->fillDeviceCreateInfo(m_relIdxs.get<VkDevice>());
        VknResult res{
            vkCreateDevice(
                *(getListElement(0, m_physicalDevices)->getVkPhysicalDevice()),
                m_infos->getDeviceCreateInfo(m_relIdxs.get<VkDevice>()),
                nullptr,
                &m_engine->getObject<VkDevice>(m_absIdxs)),
            "Create device"};

        // Get queue handles (assuming we selected at least one graphics queue)
        // In a real app, you'd store these handles based on queue family index and type.
        // For this demo, let's just get the first graphics queue.
        uint32_t graphicsQueueFamilyIndex = -1;
        for (int i = 0; i < getPhysicalDevice()->getNumQueueFamilies(); ++i)
        {
            if (getPhysicalDevice()->getQueue(i).supportsGraphics())
            {
                graphicsQueueFamilyIndex = i;
                break;
            }
        }
        if (graphicsQueueFamilyIndex == -1)
            throw std::runtime_error("Failed to find a graphics queue family after device creation.");

        vkGetDeviceQueue(*getVkDevice(), graphicsQueueFamilyIndex, 0, &m_graphicsQueue); // Store the graphics queue handle

        m_createdVkDevice = true;
        return res;
    }

    VknRenderpass *VknDevice::addRenderpass(uint32_t renderpassIdx)
    {
        return &m_engine->addNewVknObject<VknRenderpass, VkRenderPass, VkDevice>(
            renderpassIdx, m_renderpasses, m_relIdxs, m_absIdxs, m_infos);
    }

    // Need a way to get the graphics queue handle
    VkQueue VknDevice::getGraphicsQueue() const
    {
        if (!m_createdVkDevice)
            throw std::runtime_error("Logical device not created before getting graphics queue.");
        // Ensure m_graphicsQueue was populated in createDevice
        if (m_graphicsQueue == VK_NULL_HANDLE)
            throw std::runtime_error("Graphics queue handle not stored.");
        return m_graphicsQueue;
    }
} // namespace vkn