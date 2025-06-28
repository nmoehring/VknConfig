#include "include/VknDevice.hpp"

namespace vkn
{
    VknDevice::VknDevice(VknIdxs relIdxs, VknIdxs absIdxs) : VknObject(relIdxs, absIdxs)
    {
        m_instanceLock = this;
        VknPhysicalDevice &physicalDevice = s_engine->addNewVknObject<VknPhysicalDevice, VkPhysicalDevice>(
            m_relIdxs.get<VkDevice>(), m_physicalDevices, m_relIdxs, m_absIdxs);
        m_absIdxs.add<VkPhysicalDevice>(physicalDevice.getPhysicalDeviceAbsIdxs().get<VkPhysicalDevice>());
        m_absIdxs.add<VmaAllocator>(s_engine->getVectorSize<VmaAllocator>());
        features = s_infos->getDeviceFeaturesObject();
    }

    VknSwapchain *VknDevice::addSwapchain(uint32_t swapchainIdx)
    {
        if (!m_swapchainExtensionEnabled)
            throw std::runtime_error("Swapchain extension not enabled before adding swapchain.");
        if (!m_createdVkDevice)
            throw std::runtime_error("Device not created before adding swapchain.");
        if (s_engine->getVector<VkSurfaceKHR>().isEmpty())
            throw std::runtime_error("Surface not created before adding swapchain.");

        m_instanceLock(this);
        VknSwapchain &swapchain = s_engine->addNewVknObject<VknSwapchain, VkSwapchainKHR, VkDevice>(
            swapchainIdx, m_swapchains, m_relIdxs, m_absIdxs);
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
        return &s_engine->getObject<VkDevice>(m_absIdxs);
    }

    VknRenderpass *VknDevice::getRenderpass(uint32_t renderpassIdx)
    {
        return getListElement(renderpassIdx, m_renderpasses);
    }

    uint32_t VknDevice::findQueueFamily(QueueType type)
    {
        // First pass: Look for ideal, dedicated queues or required queues.
        for (int i = 0; i < this->getPhysicalDevice()->getNumQueueFamilies(); ++i)
        {
            VknQueueFamily &queue = getPhysicalDevice()->getQueue(i);
            switch (type)
            {
            case QueueType::PRESENT:
                if (queue.supportsPresent())
                    return i;
                break;
            case QueueType::GRAPHICS:
                if (queue.supportsGraphics())
                    return i;
                break;
            case QueueType::COMPUTE:
                // Prefer a queue that is compute but not graphics for async compute
                if (queue.supportsCompute() && !queue.supportsGraphics())
                    return i;
                break; // Prevent fallthrough
            case QueueType::TRANSFER:
                // Prefer a dedicated transfer queue
                if (queue.supportsTransfer() && !queue.supportsGraphics() && !queue.supportsCompute())
                    return i;
                break;
            }
        }

        // Second pass (fallback): If a dedicated queue wasn't found, find any suitable queue.
        for (int i = 0; i < this->getPhysicalDevice()->getNumQueueFamilies(); ++i)
        {
            VknQueueFamily &queue = getPhysicalDevice()->getQueue(i);
            switch (type)
            {
            case QueueType::COMPUTE:
                if (queue.supportsCompute())
                    return i; // Any compute-capable queue is fine.
                break;
            case QueueType::TRANSFER:
                if (queue.supportsTransfer())
                    return i; // Any transfer-capable queue is fine.
                break;
            default: // No simple fallback for GRAPHICS or PRESENT, they must be found in the first pass.
                break;
            }
        }
        return -1;
    }

    void VknDevice::addCommandPools()
    {
        if (m_commandPoolCreated)
        {
            return;
        }

        // This map ensures we only create one command pool per unique queue family index.
        std::map<uint32_t, VknCommandPool *> uniquePools;

        for (uint_fast32_t i = 0; i < NUM_QUEUE_TYPES; ++i)
        {
            QueueType type = static_cast<QueueType>(i);
            uint32_t queueFamilyIdx = findQueueFamily(type);

            if (queueFamilyIdx != static_cast<uint32_t>(-1))
            {
                if (uniquePools.find(queueFamilyIdx) == uniquePools.end())
                {
                    // This is a new queue family, create a pool for it.
                    VknCommandPool &newPool = s_engine->addNewVknObject<VknCommandPool, VkCommandPool, VkDevice>(
                        m_commandPools.size(), m_commandPools, m_relIdxs, m_absIdxs);
                    newPool.createCommandPool(queueFamilyIdx);
                    uniquePools[queueFamilyIdx] = &newPool;
                }
                // Map the QueueType to the (possibly shared) command pool.
                m_commandPoolMap[type] = uniquePools[queueFamilyIdx];
            }
        }

        m_commandPoolCreated = true;
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
        m_imageAvailableSemaphoreStartIdx = s_engine->getVectorSize<VkSemaphore>();
        m_inFlightFenceStartIdx = s_engine->getVectorSize<VkFence>();

        for (uint32_t i = 0; i < maxFramesInFlight; ++i)
        {
            s_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            s_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            s_engine->addNewObject<VkFence, VkDevice>(m_absIdxs);
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
                               &s_engine->getVector<VkSemaphore>()(m_imageAvailableSemaphoreStartIdx + i * 2)),
                           "Create image available semaphore"};
            VknResult res2{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr,
                               &s_engine->getVector<VkSemaphore>()(m_imageAvailableSemaphoreStartIdx + i * 2 + 1)),
                           "Create render finished semaphore"};
            VknResult res3{vkCreateFence(
                               *getVkDevice(), &fenceInfo, nullptr,
                               &s_engine->getVector<VkFence>()(m_inFlightFenceStartIdx + i)),
                           "Create in flight fence"};
        }
        m_syncObjectsCreated = true;
    }

    VkSemaphore &VknDevice::getImageAvailableSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= m_maxFramesInFlightForSyncObjects)
            throw std::out_of_range("frameInFlight out of range for getImageAvailableSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return s_engine->getVector<VkSemaphore>()(m_imageAvailableSemaphoreStartIdx + frameInFlight * 2);
    }

    VkSemaphore &VknDevice::getRenderFinishedSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= m_maxFramesInFlightForSyncObjects)
            throw std::out_of_range("frameInFlight out of range for getRenderFinishedSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return s_engine->getVector<VkSemaphore>()(m_imageAvailableSemaphoreStartIdx + frameInFlight * 2 + 1);
    }

    VkFence &VknDevice::getFence(uint32_t frameInFlight)
    {
        if (frameInFlight >= m_maxFramesInFlightForSyncObjects)
            throw std::out_of_range("frameInFlight out of range for getFence");
        return s_engine->getVector<VkFence>()(m_inFlightFenceStartIdx + frameInFlight);
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
        s_infos->fileDeviceFeaturesInfo();

        s_infos->fileDeviceCreateInfo(m_relIdxs.get<VkDevice>());
        VknResult res{
            vkCreateDevice(
                *(getListElement(0, m_physicalDevices)->getVkPhysicalDevice()),
                s_infos->getDeviceCreateInfo(m_relIdxs.get<VkDevice>()),
                nullptr,
                &s_engine->getObject<VkDevice>(m_absIdxs)),
            "Create device"};

        m_createdVkDevice = true;
        return res;
    }

    VkQueue *VknDevice::getQueue(QueueType type, uint32_t index)
    {
        // Check if we've already retrieved this queue
        uint32_t queueTypeIndex = static_cast<uint32_t>(type);
        if (m_queues.exists(queueTypeIndex))
            return &m_queues(queueTypeIndex);

        // If not, find its family, get the handle, store it, and return it
        uint32_t familyIndex = findQueueFamily(type);
        if (familyIndex == static_cast<uint32_t>(-1))
        {
            // Provide a more descriptive error message.
            std::string q_type_str = "UNKNOWN";
            if (type == GRAPHICS)
                q_type_str = "GRAPHICS";
            else if (type == COMPUTE)
                q_type_str = "COMPUTE";
            else if (type == TRANSFER)
                q_type_str = "TRANSFER";
            else if (type == PRESENT)
                q_type_str = "PRESENT";
            throw std::runtime_error("VknDevice::getQueue: Could not find a queue family for type: " + q_type_str);
        }

        VkQueue queueHandle = VK_NULL_HANDLE;
        vkGetDeviceQueue(*getVkDevice(), familyIndex, index, &queueHandle);
        return &m_queues.insert(queueTypeIndex, queueHandle);
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
        return &s_engine->addNewVknObject<VknRenderpass, VkRenderPass, VkDevice>(
            renderpassIdx, m_renderpasses, m_relIdxs, m_absIdxs);
    }

    void VknDevice::addExtension(std::string extension)
    {
        if (extension == VK_KHR_SWAPCHAIN_EXTENSION_NAME)
            m_swapchainExtensionEnabled = true;
        s_infos->addDeviceExtension(extension, m_relIdxs);
    }

    VknCommandPool *VknDevice::getCommandPool(QueueType type)
    {
        if (m_commandPoolMap.find(type) == m_commandPoolMap.end())
        {
            throw std::runtime_error("Command pool for the requested queue type not found or not created.");
        }
        return m_commandPoolMap.at(type);
    }

    VmaAllocator *VknDevice::addAllocator()
    {
        if (!m_createdVkDevice)
            throw std::runtime_error("Device must be created before adding allocator.");

        VmaAllocator &allocator = s_engine->addNewObject<VmaAllocator, VkDevice>(m_absIdxs);
        VmaAllocatorCreateInfo allocatorInfo{};
        // allocatorInfo.pVulkanFunctions = &m_vmaVulkanFunctions; //This needs to come back if volk ever gets used.
        allocatorInfo.physicalDevice = s_engine->getObject<VkPhysicalDevice>(m_absIdxs);
        allocatorInfo.device = s_engine->getObject<VkDevice>(m_absIdxs);
        allocatorInfo.instance = s_engine->getObject<VkInstance>(m_absIdxs);
        allocatorInfo.vulkanApiVersion = s_infos->getAppInfo()->apiVersion;

        VknResult res{"Create VMA allocator."};
        res = vmaCreateAllocator(&allocatorInfo, &allocator);
        return &allocator;
    }

    VknVertexBuffer *VknDevice::addVertexBuffer(VkDeviceSize size)
    {
        s_engine->addNewVknObject<VknVertexBuffer, VkBuffer, VkDevice>(
            m_vertexBuffers.size(), m_vertexBuffers, m_relIdxs, m_absIdxs);
        m_vertexBuffers.back().setSize(size);
        return &m_vertexBuffers.back();
    }

    VknIndexBuffer *VknDevice::addIndexBuffer(VkDeviceSize size)
    {
        s_engine->addNewVknObject<VknIndexBuffer, VkBuffer, VkDevice>(
            m_indexBuffers.size(), m_indexBuffers, m_relIdxs, m_absIdxs);
        m_indexBuffers.back().setSize(size);
        return &m_indexBuffers.back();
    }

    VknCpuUniformBuffer *VknDevice::addCpuUniformBuffer(VkDeviceSize size)
    {
        s_engine->addNewVknObject<VknCpuUniformBuffer, VkBuffer, VkDevice>(
            m_cpuUniformBuffers.size(), m_cpuUniformBuffers, m_relIdxs, m_absIdxs);
        m_cpuUniformBuffers.back().setSize(size);
        return &m_cpuUniformBuffers.back();
    }

    VknGpuUniformBuffer *VknDevice::addGpuUniformBuffer(VkDeviceSize size)
    {
        s_engine->addNewVknObject<VknGpuUniformBuffer, VkBuffer, VkDevice>(
            m_gpuUniformBuffers.size(), m_gpuUniformBuffers, m_relIdxs, m_absIdxs);
        m_gpuUniformBuffers.back().setSize(size);
        return &m_gpuUniformBuffers.back();
    }

    VknStorageBuffer *VknDevice::addStorageBuffer(VkDeviceSize size)
    {
        s_engine->addNewVknObject<VknStorageBuffer, VkBuffer, VkDevice>(
            m_storageBuffers.size(), m_storageBuffers, m_relIdxs, m_absIdxs);
        m_storageBuffers.back().setSize(size);
        return &m_storageBuffers.back();
    }

    VknIndirectBuffer *VknDevice::addIndirectBuffer(VkDeviceSize size)
    {
        s_engine->addNewVknObject<VknIndirectBuffer, VkBuffer, VkDevice>(
            m_indirectBuffers.size(), m_indirectBuffers, m_relIdxs, m_absIdxs);
        m_indirectBuffers.back().setSize(size);
        return &m_indirectBuffers.back();
    }

} // namespace vkn