#include "include/VknDevice.hpp"

namespace vkn
{
    VknDevice::VknDevice(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_instanceLock = this;
        VknPhysicalDevice &physicalDevice = m_engine->addNewVknObject<VknPhysicalDevice, VkPhysicalDevice>(
            m_relIdxs.get<VkDevice>(), m_physicalDevices, m_relIdxs, m_absIdxs, m_infos);
        m_absIdxs.add<VkPhysicalDevice>(physicalDevice.getPhysicalDeviceAbsIdxs().get<VkPhysicalDevice>());
        m_absIdxs.add<VmaAllocator>(m_engine->getVectorSize<VmaAllocator>());
        features = m_infos->getDeviceFeaturesObject();
    }

    VknSwapchain *VknDevice::addSwapchain(uint32_t swapchainIdx)
    {
        if (!m_swapchainExtensionEnabled)
            throw std::runtime_error("Swapchain extension not enabled before adding swapchain.");
        if (!m_createdVkDevice)
            throw std::runtime_error("Device not created before adding swapchain.");
        if (m_engine->getVector<VkSurfaceKHR>().isEmpty())
            throw std::runtime_error("Surface not created before adding swapchain.");

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

        m_maxFramesInFlightForSyncObjects = maxFramesInFlight; // Store for validation in getters

        // Record starting indices in the VknEngine's global vectors
        m_imageAvailableSemaphoreStartIdx = m_engine->getVectorSize<VkSemaphore>();
        m_inFlightFenceStartIdx = m_engine->getVectorSize<VkFence>();

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
                               *getVkDevice(), &semaphoreInfo, nullptr,
                               &m_engine->getVector<VkSemaphore>()(m_imageAvailableSemaphoreStartIdx + i * 2)),
                           "Create image available semaphore"};
            VknResult res2{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr,
                               &m_engine->getVector<VkSemaphore>()(m_imageAvailableSemaphoreStartIdx + i * 2 + 1)),
                           "Create render finished semaphore"};
            VknResult res3{vkCreateFence(
                               *getVkDevice(), &fenceInfo, nullptr,
                               &m_engine->getVector<VkFence>()(m_inFlightFenceStartIdx + i)),
                           "Create in flight fence"};
        }
        m_syncObjectsCreated = true;
    }

    VkSemaphore &VknDevice::getImageAvailableSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= m_maxFramesInFlightForSyncObjects)
            throw std::out_of_range("frameInFlight out of range for getImageAvailableSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return m_engine->getVector<VkSemaphore>()(m_imageAvailableSemaphoreStartIdx + frameInFlight * 2);
    }

    VkSemaphore &VknDevice::getRenderFinishedSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= m_maxFramesInFlightForSyncObjects)
            throw std::out_of_range("frameInFlight out of range for getRenderFinishedSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return m_engine->getVector<VkSemaphore>()(m_imageAvailableSemaphoreStartIdx + frameInFlight * 2 + 1);
    }

    VkFence &VknDevice::getFence(uint32_t frameInFlight)
    {
        if (frameInFlight >= m_maxFramesInFlightForSyncObjects)
            throw std::out_of_range("frameInFlight out of range for getFence");
        return m_engine->getVector<VkFence>()(m_inFlightFenceStartIdx + frameInFlight);
    }

    VknResult VknDevice::createDevice()
    {
        VknPhysicalDevice *physicalDevice = getListElement(0, m_physicalDevices);
        if (m_createdVkDevice)
            throw std::runtime_error("Device already created.");
        if (!physicalDevice->areQueuePrioritiesFiled())
            physicalDevice->fileDeviceQueuePrioritiesDefault(); // Subtle initiation of chain-reaction default configurations
        physicalDevice->fileQueueCreateInfos();
        m_absIdxs.add<VkPhysicalDevice>(physicalDevice->getPhysicalDeviceAbsIdx());
        m_infos->fileDeviceFeaturesInfo();

        m_infos->fileDeviceCreateInfo(m_relIdxs.get<VkDevice>());
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
        if (!m_createdVkDevice)
            throw std::runtime_error("Device not created before adding renderpass.");
        bool allSwapchainsCreated{true};
        for (auto &swapchain : m_swapchains)
            if (!swapchain.isSwapchainCreated())
                allSwapchainsCreated = false;
        if (!allSwapchainsCreated)
            throw std::runtime_error("Swapchain not created before adding renderpass.");

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

    VmaAllocator *VknDevice::addAllocator()
    {
        if (!m_createdVkDevice)
            throw std::runtime_error("Device must be created before adding allocator.");

        VmaAllocator &allocator = m_engine->addNewObject<VmaAllocator, VkDevice>(m_absIdxs);
        VmaAllocatorCreateInfo allocatorInfo{};
        // allocatorInfo.pVulkanFunctions = &m_vmaVulkanFunctions; //This needs to come back if volk ever gets used.
        allocatorInfo.physicalDevice = m_engine->getObject<VkPhysicalDevice>(m_absIdxs);
        allocatorInfo.device = m_engine->getObject<VkDevice>(m_absIdxs);
        allocatorInfo.instance = m_engine->getObject<VkInstance>(m_absIdxs);
        allocatorInfo.vulkanApiVersion = m_infos->getAppInfo()->apiVersion;

        VknResult res{"Create VMA allocator."};
        res = vmaCreateAllocator(&allocatorInfo, &allocator);
        return &allocator;
    }

    void VknDevice::addVertexBuffer(uint_fast32_t size)
    {
        m_vertexBuffers.emplace_back(m_engine, m_relIdxs, m_absIdxs, m_infos, size);
    }

    void VknDevice::addIndexBuffer(uint_fast32_t size)
    {
        m_indexBuffer.emplace_back(m_engine, m_relIdxs, m_absIdxs, m_infos, size);
    }

    void VknDevice::addCpuUniformBuffer(uint_fast32_t size)
    {
        m_cpuUniformBuffer.emplace_back(m_engine, m_relIdxs, m_absIdxs, m_infos, size);
    }

    void VknDevice::addGpuUniformBuffer(uint_fast32_t size)
    {
        m_gpuUniformBuffer.emplace_back(m_engine, m_relIdxs, m_absIdxs, m_infos, size);
    }

    void VknDevice::addStorageBuffer(uint_fast32_t size)
    {
        m_storageBuffer.emplace_back(m_engine, m_relIdxs, m_absIdxs, m_infos, size);
    }

    void VknDevice::addIndirectBuffer(uint_fast32_t size)
    {
        m_indirectBuffer.emplace_back(m_engine, m_relIdxs, m_absIdxs, m_infos, size);
    }

} // namespace vkn