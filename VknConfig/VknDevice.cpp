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

    VknSwapchain *VknDevice::getSwapchain()
    {
        return &m_swapchain.front();
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
        int score = -1;
        int idx = 500;
        for (int i = 0; i < this->getPhysicalDevice()->getNumQueueFamilies(); ++i)
        {
            VknQueueFamily &queue = getPhysicalDevice()->getQueue(i);
            if (queue.supportsPresent() && queue.supportsCompute() && queue.supportsTransfer())
            {
                if (8 > score)
                {
                    score = 8;
                    idx = i;
                }
            }
            switch (type)
            {
            case QueueType::PRESENT:
                if (queue.supportsPresent() && !queue.supportsCompute() && !queue.supportsTransfer())
                {
                    score = 1024;
                    idx = i;
                }
                else if (queue.supportsPresent() && queue.supportsCompute() && !queue.supportsTransfer())
                {
                    if (128 > score)
                    {
                        score = 128;
                        idx = i;
                    }
                }
                else if (queue.supportsPresent() && !queue.supportsCompute() && queue.supportsTransfer())
                {
                    if (32 > score)
                    {
                        score = 32;
                        idx = i;
                    }
                }
                break;
            case QueueType::GRAPHICS:
                if (queue.supportsGraphics() && !queue.supportsPresent() && !queue.supportsCompute() && !queue.supportsTransfer())
                {
                    if (1024 > score)
                    {
                        score = 1024;
                        idx = i;
                    }
                }
                else if (queue.supportsGraphics() && !queue.supportsPresent() && queue.supportsCompute() && !queue.supportsTransfer())
                {
                    if (128 > score)
                    {
                        score = 128;
                        idx = i;
                    }
                }
                else if (queue.supportsGraphics() && !queue.supportsPresent() && !queue.supportsCompute() && queue.supportsTransfer())
                {
                    if (16 > score)
                    {
                        score = 16;
                        idx = i;
                    }
                }
                break;
            case QueueType::COMPUTE:
                // Prefer a queue that is compute but not graphics for async compute
                if (queue.supportsCompute() && !queue.supportsGraphics() && !queue.supportsTransfer())
                {
                    score = 1024;
                    idx = i;
                }
                else if (queue.supportsCompute() && queue.supportsGraphics() && !queue.supportsPresent() && !queue.supportsTransfer())
                {
                    if (128 > score)
                    {
                        score = 128;
                        idx = i;
                    }
                }
                else if (queue.supportsCompute() && queue.supportsPresent() && !queue.supportsTransfer())
                {
                    if (32 > score)
                    {
                        score = 32;
                        idx = i;
                    }
                }
                else if (queue.supportsCompute() && !queue.supportsGraphics() && queue.supportsTransfer())
                {
                    if (16 > score)
                    {
                        score = 16;
                        idx = i;
                    }
                }
                break; // Prevent fallthrough
            case QueueType::TRANSFER:
                // Prefer a dedicated transfer queue
                if (queue.supportsTransfer() && !queue.supportsGraphics() && !queue.supportsCompute())
                {
                    score = 1024;
                    idx = i;
                }
                else if (queue.supportsTransfer() && queue.supportsCompute() && !queue.supportsGraphics())
                {
                    if (32 > score)
                    {
                        score = 32;
                        idx = i;
                    }
                }
                else if (queue.supportsTransfer() && queue.supportsGraphics() && !queue.supportsPresent() && !queue.supportsCompute())
                {
                    if (128 > score)
                    {
                        score = 128;
                        idx = i;
                    }
                }
                else if (queue.supportsTransfer() && queue.supportsPresent() && !queue.supportsCompute())
                {
                    if (64 > score)
                    {
                        score = 64;
                        idx = i;
                    }
                }
                break;
            }
        }

        return idx;
    }

    void VknDevice::addCommandPools()
    {
        if (m_commandPoolsCreated)
            return;

        // This map ensures we only create one command pool per unique queue family index.
        std::map<uint32_t, VknCommandPool *> uniquePools;

        for (uint_fast32_t i = 0; i < CommandBufferType::NUM_CB_TYPE; ++i)
        {
            QueueType type{QueueType::PRESENT};
            if (i == CommandBufferType::GRAPHICS_CB)
                continue; // Not implemented yet
            else if (i == CommandBufferType::PRESENT_CB)
                type = QueueType::PRESENT;
            else if (i == CommandBufferType::PRECOMPUTE_CB)
                type = QueueType::COMPUTE;
            else if (i == CommandBufferType::POSTCOMPUTE_CB)
                type = QueueType::COMPUTE;
            else if (i == CommandBufferType::UPLOAD_CB)
                type = QueueType::TRANSFER;
            else if (i == CommandBufferType::DOWNLOAD_CB)
                type = QueueType::TRANSFER;
            else
                throw std::runtime_error("Command buffer type not recognized.");

            uint32_t queueFamilyIdx = this->findQueueFamily(type);

            if (queueFamilyIdx != static_cast<uint32_t>(-1))
            {
                if (uniquePools.find(queueFamilyIdx) == uniquePools.end())
                {
                    // This is a new queue family, create a pool for it.
                    VknCommandPool &newPool = s_engine->addNewVknObject<VknCommandPool, VkCommandPool, VkDevice>(
                        m_commandPools.size(), m_commandPools, m_relIdxs, m_absIdxs);
                    newPool.createCommandPool(queueFamilyIdx);
                    newPool.createCommandBuffers(VknObject::s_maxFramesInFlight);
                    uniquePools[queueFamilyIdx] = &newPool;
                }
                // Map the QueueType to the (possibly shared) command pool.
                m_commandPoolMap[type] = uniquePools[queueFamilyIdx];
                m_queueFamilyMap[type] = queueFamilyIdx;
            }
        }

        m_commandPoolsCreated = true;
    }

    void VknDevice::createSyncObjects()
    {
        m_instanceLock(this);
        if (m_syncObjectsCreated)
            throw std::runtime_error("Synchronization objects already created.");
        if (!m_createdVkDevice)
            throw std::runtime_error("Swapchain not created before creating synchronization objects.");

        VknObject::s_maxFramesInFlight = m_swapchain.front().getNumImages(); // Store for validation in getters

        // Record starting indices in the VknEngine's global vectors
        m_semaphoreStartIdx = s_engine->getVectorSize<VkSemaphore>();
        m_inFlightFenceStartIdx = s_engine->getVectorSize<VkFence>();

        for (uint32_t i = 0; i < VknObject::s_maxFramesInFlight; ++i)
        {
            s_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            s_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            s_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            s_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            s_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            s_engine->addNewObject<VkSemaphore, VkDevice>(m_absIdxs);
            s_engine->addNewObject<VkFence, VkDevice>(m_absIdxs);
        }

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Create fences in signaled state

        for (size_t i = 0; i < VknObject::s_maxFramesInFlight; ++i)
        {
            VknResult res1{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr,
                               &s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + i * 6)),
                           "Create image available semaphore"};
            VknResult res2{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr,
                               &s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + i * 6 + 1)),
                           "Create render finished semaphore"};
            VknResult res3{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr,
                               &s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + i * 6 + 2)),
                           "Create uploads finished semaphore"};
            VknResult res4{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr,
                               &s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + i * 6 + 3)),
                           "Create preCompute finished semaphore"};
            VknResult res5{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr,
                               &s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + i * 6 + 4)),
                           "Create postCompute finished semaphore"};
            VknResult res6{vkCreateSemaphore(
                               *getVkDevice(), &semaphoreInfo, nullptr,
                               &s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + i * 6 + 5)),
                           "Create downloads finished semaphore"};
            VknResult res7{vkCreateFence(
                               *getVkDevice(), &fenceInfo, nullptr,
                               &s_engine->getVector<VkFence>()(m_inFlightFenceStartIdx + i)),
                           "Create in flight fence"};
        }
        m_syncObjectsCreated = true;
    }

    void VknDevice::recreateSyncObjects()
    {
        s_engine->demolishObjects<VkSemaphore, VkDevice>(vkDestroySemaphore);
        s_engine->demolishObjects<VkFence, VkDevice>(vkDestroyFence);
        m_syncObjectsCreated = false;
        createSyncObjects();
    }

    VkSemaphore &VknDevice::getImageAvailableSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= VknObject::s_maxFramesInFlight)
            throw std::out_of_range("frameInFlight out of range for getImageAvailableSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + frameInFlight * 6);
    }

    VkSemaphore &VknDevice::getRenderFinishedSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= VknObject::s_maxFramesInFlight)
            throw std::out_of_range("frameInFlight out of range for getRenderFinishedSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + frameInFlight * 6 + 1);
    }

    VkSemaphore &VknDevice::getUploadsFinishedSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= VknObject::s_maxFramesInFlight)
            throw std::out_of_range("frameInFlight out of range for getRenderFinishedSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + frameInFlight * 6 + 2);
    }

    VkSemaphore &VknDevice::getPreComputeStageFinishedSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= VknObject::s_maxFramesInFlight)
            throw std::out_of_range("frameInFlight out of range for getRenderFinishedSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + frameInFlight * 6 + 3);
    }

    VkSemaphore &VknDevice::getPostComputeFinishedSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= VknObject::s_maxFramesInFlight)
            throw std::out_of_range("frameInFlight out of range for getRenderFinishedSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + frameInFlight * 6 + 4);
    }

    VkSemaphore &VknDevice::getDownloadsFinishedSemaphore(uint32_t frameInFlight)
    {
        if (frameInFlight >= VknObject::s_maxFramesInFlight)
            throw std::out_of_range("frameInFlight out of range for getRenderFinishedSemaphore");
        // Assumes ImageAvailable and RenderFinished semaphores are created interleaved for each frame
        return s_engine->getVector<VkSemaphore>()(m_semaphoreStartIdx + frameInFlight * 6 + 5);
    }

    VkFence &VknDevice::getFence(uint32_t frameInFlight)
    {
        if (frameInFlight >= VknObject::s_maxFramesInFlight)
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

        if (s_engine->getVectorSize<VkSurfaceKHR>() > 0)
        {
            s_engine->addNewVknObject<VknSwapchain, VkSwapchainKHR, VkDevice>(
                0, m_swapchain, m_relIdxs, m_absIdxs);
            m_swapchain.front().createSwapchain();
        }

        // Ensure allocator is created after the device and its index is stored
        m_createdVkDevice = true;
        m_iGPU = physicalDevice->isIntegratedGPU();
        this->addAllocator();
        return res;
    }

    VkQueue *VknDevice::getQueue(QueueType type, uint32_t index)
    {
        // Check if we've already retrieved this queue
        uint32_t queueTypeIndex = static_cast<uint32_t>(type);
        if (m_queues.exists(queueTypeIndex))
            return &m_queues(queueTypeIndex);

        // If not, find its family, get the handle, store it, and return it
        uint32_t familyIndex = m_queueFamilyMap.count(type) ? m_queueFamilyMap[type] : -1;
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
            throw std::runtime_error("Command pool for requested queue type not found. Did you call addCommandPools()?");
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
        allocatorInfo.pVulkanFunctions = s_infos->getVmaVulkanFunctions();

        VknResult res{"Create VMA allocator."};
        res = vmaCreateAllocator(&allocatorInfo, &allocator);
        return &allocator;
    }

    VknVertexBuffer *VknDevice::addVertexBuffer(VkDeviceSize size)
    {
        uint32_t firstIdx = m_vertexBuffers.size();
        for (uint32_t i = 0; i < s_maxFramesInFlight; ++i)
        {
            s_engine->addNewVknObject<VknVertexBuffer, VkBuffer, VmaAllocator>(
                m_vertexBuffers.size(), m_vertexBuffers, m_relIdxs, m_absIdxs);
            m_vertexBuffers.back().setIntegrated(m_iGPU);
            m_vertexBuffers.back().setSize(size);
        }
        return getListElement(firstIdx, m_vertexBuffers);
    }

    VknIndexBuffer *VknDevice::addIndexBuffer(VkDeviceSize size)
    {
        uint32_t firstIdx = m_indexBuffers.size();
        for (uint32_t i = 0; i < s_maxFramesInFlight; ++i)
        {
            s_engine->addNewVknObject<VknIndexBuffer, VkBuffer, VmaAllocator>(
                m_indexBuffers.size(), m_indexBuffers, m_relIdxs, m_absIdxs);
            m_indexBuffers.back().setIntegrated(m_iGPU);
            m_indexBuffers.back().setSize(size);
        }
        return getListElement(firstIdx, m_indexBuffers);
    }

    VknCpuUniformBuffer *VknDevice::addCpuUniformBuffer(VkDeviceSize size)
    {
        uint32_t firstIdx = m_cpuUniformBuffers.size();
        for (uint32_t i = 0; i < s_maxFramesInFlight; ++i)
        {
            s_engine->addNewVknObject<VknCpuUniformBuffer, VkBuffer, VmaAllocator>(
                m_cpuUniformBuffers.size(), m_cpuUniformBuffers, m_relIdxs, m_absIdxs);
            m_cpuUniformBuffers.back().setIntegrated(m_iGPU);
            m_cpuUniformBuffers.back().setSize(size);
        }
        return getListElement(firstIdx, m_cpuUniformBuffers);
    }

    VknGpuUniformBuffer *VknDevice::addGpuUniformBuffer(VkDeviceSize size)
    {
        uint32_t firstIdx = m_gpuUniformBuffers.size();
        for (uint32_t i = 0; i < s_maxFramesInFlight; ++i)
        {
            s_engine->addNewVknObject<VknGpuUniformBuffer, VkBuffer, VmaAllocator>(
                m_gpuUniformBuffers.size(), m_gpuUniformBuffers, m_relIdxs, m_absIdxs);
            m_gpuUniformBuffers.back().setIntegrated(m_iGPU);
            m_gpuUniformBuffers.back().setSize(size);
        }
        return getListElement(firstIdx, m_gpuUniformBuffers);
    }

    VknStorageBuffer *VknDevice::addStorageBuffer(VkDeviceSize size)
    {
        uint32_t firstIdx = m_storageBuffers.size();
        for (uint32_t i = 0; i < s_maxFramesInFlight; ++i)
        {
            s_engine->addNewVknObject<VknStorageBuffer, VkBuffer, VmaAllocator>(
                m_storageBuffers.size(), m_storageBuffers, m_relIdxs, m_absIdxs);
            m_storageBuffers.back().setIntegrated(m_iGPU);
            m_storageBuffers.back().setSize(size);
        }
        return getListElement(firstIdx, m_storageBuffers);
    }

    VknIndirectBuffer *VknDevice::addIndirectBuffer(VkDeviceSize size)
    {
        uint32_t firstIdx = m_indirectBuffers.size();
        for (uint32_t i = 0; i < s_maxFramesInFlight; ++i)
        {
            s_engine->addNewVknObject<VknIndirectBuffer, VkBuffer, VmaAllocator>(
                m_indirectBuffers.size(), m_indirectBuffers, m_relIdxs, m_absIdxs);
            m_indirectBuffers.back().setIntegrated(m_iGPU);
            m_indirectBuffers.back().setSize(size);
        }
        return getListElement(firstIdx, m_indirectBuffers);
    }

    VknComputeVertexBuffer *VknDevice::addComputeVertexBuffer(VkDeviceSize size)
    {
        uint32_t firstIdx = m_computeVertexBuffers.size();
        for (uint32_t i = 0; i < s_maxFramesInFlight; ++i)
        {
            s_engine->addNewVknObject<VknComputeVertexBuffer, VkBuffer, VmaAllocator>(
                m_computeVertexBuffers.size(), m_computeVertexBuffers, m_relIdxs, m_absIdxs);
            m_computeVertexBuffers.back().setIntegrated(m_iGPU);
            m_computeVertexBuffers.back().setSize(size);
        }
        return getListElement(firstIdx, m_computeVertexBuffers);
    }

    uint32_t VknDevice::getQueueFamilyIdxByType(QueueType type)
    {
        if (m_queueFamilyMap.find(type) == m_queueFamilyMap.end())
            throw std::runtime_error("QueueType not found in queue family map.");
        return m_queueFamilyMap.at(type);
    }

} // namespace vkn