#include "include/VknBuffer.hpp"

namespace vkn
{
    VknBuffer::VknBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
        // This must call the base class constructor.
        : VknObject(relIdxs, absIdxs)
    {
    }

    VknBuffer::~VknBuffer()
    {
        demolish();
    }

    VknBuffer::VknBuffer(VknBuffer &&other) noexcept
        // Move construct the base class, and then this class's members.
        // Static members (s_engine, s_infos) cannot be in an initializer list.
        : VknObject(std::move(other)),
          m_size(other.m_size),
          m_memFlags(other.m_memFlags),
          m_allocInfo(other.m_allocInfo),
          m_mappedData(other.m_mappedData),
          m_uploadBuffer(other.m_uploadBuffer),
          m_downloadBuffer(other.m_downloadBuffer),
          m_copyRegion(other.m_copyRegion),
          m_hasUploadBuffer(other.m_hasUploadBuffer),
          m_hasDownloadBuffer(other.m_hasDownloadBuffer),
          m_manualMapping(other.m_manualMapping),
          m_isPersistentlyMapped(other.m_isPersistentlyMapped),
          m_mustFlushAndInvalidate(other.m_mustFlushAndInvalidate),
          m_setSize(other.m_setSize),
          m_createdBuffer(other.m_createdBuffer)
    {
        other.m_mappedData = nullptr;
        other.m_size = 0;
        other.m_isPersistentlyMapped = false;
        other.m_uploadBuffer = nullptr;
        other.m_downloadBuffer = nullptr;
        other.m_copyRegion = nullptr;
        other.m_hasUploadBuffer = false;
        other.m_hasDownloadBuffer = false;
        other.m_createdBuffer = false;
    }

    VknBuffer &VknBuffer::operator=(VknBuffer &&other) noexcept
    {
        if (this != &other)
        {
            demolish(); // Clean up existing resources

            VknObject::operator=(std::move(other)); // Move assign the base part
            m_size = other.m_size;
            m_memFlags = other.m_memFlags;
            m_allocInfo = other.m_allocInfo;
            m_mappedData = other.m_mappedData;
            m_uploadBuffer = other.m_uploadBuffer;
            m_downloadBuffer = other.m_downloadBuffer;
            m_copyRegion = other.m_copyRegion;
            m_hasUploadBuffer = other.m_hasUploadBuffer;
            m_hasDownloadBuffer = other.m_hasDownloadBuffer;
            m_manualMapping = other.m_manualMapping;
            m_isPersistentlyMapped = other.m_isPersistentlyMapped;
            m_mustFlushAndInvalidate = other.m_mustFlushAndInvalidate;
            m_setSize = other.m_setSize;
            m_createdBuffer = other.m_createdBuffer;

            other.m_mappedData = nullptr;
            other.m_size = 0;
            other.m_isPersistentlyMapped = false;
            other.m_uploadBuffer = nullptr;
            other.m_downloadBuffer = nullptr;
            other.m_copyRegion = nullptr;
            other.m_hasUploadBuffer = false;
            other.m_hasDownloadBuffer = false;
            other.m_createdBuffer = false;
        }
        return *this;
    }

    void VknBuffer::create()
    {
        if (m_createdBuffer)
            throw std::runtime_error("VknBuffer already created.");
        if (!m_setSize)
            throw std::runtime_error("Size not set before creating buffer.");

        m_bufferUsage = m_bufferType | m_transferType;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = m_size;
        bufferInfo.usage = m_bufferUsage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Default, can be configured if needed

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = m_memoryUsage;
        allocCreateInfo.flags = m_allocationFlags;

        // The VknDevice factory method should have already called addNewObject<VkBuffer,...>
        // which reserves a spot in VknEngine's vector. We just need to create the buffer into that spot.
        VknResult res = {vmaCreateBuffer(s_engine->getObject<VmaAllocator>(m_absIdxs),
                                         &bufferInfo,
                                         &allocCreateInfo,
                                         &s_engine->getObject<VkBuffer>(m_absIdxs),        // VknEngine stores the VkBuffer
                                         &s_engine->addNewAllocation<VkBuffer>(m_absIdxs), // And the allocation
                                         &m_allocInfo),                                    // To get mapped data if VMA_ALLOCATION_CREATE_MAPPED_BIT is set
                         "VMA Create Buffer"};

        vmaGetMemoryTypeProperties(s_engine->getObject<VmaAllocator>(m_absIdxs), m_allocInfo.memoryType, &m_memFlags);

        if (m_memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            m_isPersistentlyMapped = true;
        else
        {
            if (m_uploadable)
            {
                s_engine->addNewVknObject<VknUploadBuffer, VkBuffer, VmaAllocator>(
                    m_uploadBuffer, m_relIdxs, m_absIdxs);
                m_uploadBuffer->setSize(m_size);
                m_hasUploadBuffer = true;
            }
            if (m_downloadable)
            {
                s_engine->addNewVknObject<VknDownloadBuffer, VkBuffer, VmaAllocator>(
                    m_downloadBuffer, m_relIdxs, m_absIdxs);
                m_downloadBuffer->setSize(m_size);
                m_hasDownloadBuffer = true;
            }
        }

        if (m_allocationFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
        {
            m_manualMapping = false;
            m_mappedData = m_allocInfo.pMappedData;
        }
        if (m_memFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            m_mustFlushAndInvalidate = false;

        m_createdBuffer = true;
        // Only attempt to map manually if we are supposed to AND the memory is actually mappable.
        // If it's not host-visible, we must use the staging buffer.
        if (m_manualMapping && (m_memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
            this->map();
    }

    void VknBuffer::demolish()
    {
        if (!m_createdBuffer)
            return;

        // If it was mapped manually (not persistently by VMA), unmap it
        if (m_mappedData && !m_isPersistentlyMapped)
            this->unmap();

        if (m_downloadBuffer)
        {
            delete m_downloadBuffer;
            m_downloadBuffer = nullptr;
        }
        if (m_uploadBuffer)
        {
            delete m_uploadBuffer;
            m_uploadBuffer = nullptr;
        }

        m_mappedData = nullptr;
        m_size = 0;
        m_isPersistentlyMapped = false;
        m_createdBuffer = false;
    }

    void *VknBuffer::map()
    {
        if (m_mappedData) // Already mapped (either by VMA or manually)
            return m_mappedData;
        if (!m_createdBuffer)
            throw std::runtime_error("Cannot map buffer: buffer hasn't been created yet.");

        // Retrieve the VmaAllocator using the absolute index stored in the buffer's VknIdxs
        // This assumes the VmaAllocator index was added to m_absIdxs when the parent VknDevice was created.
        VmaAllocator allocator = s_engine->getObject<VmaAllocator>(m_absIdxs);

        m_mapResult = vmaMapMemory(allocator, *s_engine->getAllocation<VkBuffer>(m_absIdxs), &m_mappedData);

        return m_mappedData;
    }

    void VknBuffer::unmap()
    {
        if (m_isPersistentlyMapped)
            return; // Do not unmap buffers that VMA mapped persistently
        if (m_mappedData)
        {
            VmaAllocator allocator = s_engine->getObject<VmaAllocator>(m_absIdxs);
            vmaUnmapMemory(allocator, *s_engine->getAllocation<VkBuffer>(m_absIdxs));
            m_mappedData = nullptr;
        }
    }

    void VknBuffer::flush(VkDeviceSize offset, VkDeviceSize size)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Buffer not created, cannot flush.");
        // VMA handles checking for HOST_COHERENT internally for vmaFlushAllocation.
        // If it's coherent, flush is a no-op.
        m_flushResult = vmaFlushAllocation(
            s_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()),
            *s_engine->getAllocation<VkBuffer>(m_absIdxs), offset, size);
    }

    void VknBuffer::invalidate(VkDeviceSize offset, VkDeviceSize size)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Buffer not created, cannot invalidate.");
        m_invalidateResult = vmaInvalidateAllocation(
            s_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()),
            *s_engine->getAllocation<VkBuffer>(m_absIdxs), offset, size);
    }

    void VknBuffer::waitOnUploadData()
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Buffer not created, cannot copy upload data.");
        if (!m_uploading)
            return;

        if (m_hasUploadBuffer) // dGPU
            m_uploadBuffer->copyUploadData(m_uploadData, m_uploadDataSize, m_uploadDataOffset);
        else // copying directly to buffer without staging (iGPU or upload staging buffer)
        {
            m_msgSize->wait(0);
            m_copyRegion->size = m_msgSize->exchange(0);
            m_copyRegion->srcOffset = m_uploadDataOffset;
            m_copyRegion->dstOffset = m_uploadDataOffset;
            this->flush(m_uploadDataOffset, m_uploadDataSize);
        }
    }

    void VknBuffer::uploadData()
    {

        if (!VknObject::s_recordingUploadCommandBuffer)
            throw std::runtime_error("Transfer command buffer not recording, cannot upload data.");
        if (!m_uploading)
            return;
        // if getting data
        // wait for data
        // m_copyRegion stuff
        if (m_hasUploadBuffer)
        {
            vkCmdCopyBuffer(
                *VknObject::s_uploadCommandBuffer,
                *m_uploadBuffer->getVkBuffer(),
                *this->getVkBuffer(),
                1, m_copyRegion);
        }
    }

    void VknBuffer::msgToDownloadData()
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Buffer not created, cannot copy download data.");
        if (!m_downloading)
            return;

        if (m_hasDownloadBuffer)
            m_downloadBuffer->copyDownloadData(); // buffer has size and offset, since this is output data?
        else                                      // Copying directly from buffer (likely iGPU)
        {
            m_downloadMsg.processed.wait(false);
            m_downloadMsg.processed.store(false);
            uint32_t dataSize = static_cast<uint32_t *>(m_mappedData)[0];
            this->invalidate(0, dataSize);
            VknMessage msg{};
            m_downloadMsg.type = VknThreadMessageType_Transfer;
            m_downloadMsg.srcThreadName = VknThreadName::GpuThread;
            m_downloadMsg.dstThreadName = VknThreadName::AppThread;
            m_downloadMsg.dataSize = dataSize;
            m_downloadMsg.srcDataIndex = m_registrationIdx;
            m_downloadMsg.dstDataIndex = m_registrationIdx;
            VknObject::sendMessage(&msg);
        }
    }

    void VknBuffer::downloadData()
    {
        if (!VknObject::s_recordingDownloadCommandBuffer)
            throw std::runtime_error("Transfer command buffer not recording, cannot download data.");
        if (!m_downloading)
            return;

        if (m_hasDownloadBuffer)
        {
            vkCmdCopyBuffer(
                *VknObject::s_downloadCommandBuffer,
                *m_downloadBuffer->getVkBuffer(),
                *m_downloadBuffer->getVkBuffer(),
                1, m_copyRegion);
        }
    }

    VkDescriptorBufferInfo VknBuffer::getDescriptorInfo(VkDeviceSize offset, VkDeviceSize range)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Cannot get descriptor info, buffer is null.");
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = s_engine->getObject<VkBuffer>(m_absIdxs);
        bufferInfo.offset = offset;
        bufferInfo.range = (range == VK_WHOLE_SIZE) ? m_size : range;
        return bufferInfo;
    }

    void *VknBuffer::getDataArea()
    {
        if (m_hasUploadBuffer && m_uploading)
            return m_uploadBuffer->getDataArea();
        else if (m_hasDownloadBuffer && m_downloading)
            return m_downloadBuffer->getDataArea();
        return m_mappedData;
    }

    VkBuffer *VknBuffer::getUploadVkBuffer() const
    {
        if (!m_uploadBuffer)
            throw std::runtime_error("Buffer is not uploadable.");
        return m_uploadBuffer->getVkBuffer();
    }

    VkBuffer *VknBuffer::getDownloadVkBuffer() const
    {
        if (!m_downloadBuffer)
            throw std::runtime_error("Buffer is not downloadable.");
        return m_downloadBuffer->getVkBuffer();
    }

    void VknBuffer::registerBuffer_Gpu()
    {
        VknMessage registrationMsg{};
        registrationMsg.type = VknMessageType::VknThreadMessageType_GetRegistration;
        registrationMsg.srcThreadName = VknThreadName::GpuThread;
        VknObject::sendMessage(&registrationMsg);
        registrationMsg.processed.wait(false);
        VknDispatchRegistration *registration = static_cast<VknDispatchRegistration *>(registrationMsg.extraData[0]);
        m_registrationIdx = registration->registrationIdx;
        if (m_uploading)
            registration->receivePtr = this->getDataArea();
        if (m_downloading)
        {
            registration->sendPtr = this->getDataArea();
            m_msgSize = &registration->receiveDataSize;
        }
    }

} // namespace vkn
