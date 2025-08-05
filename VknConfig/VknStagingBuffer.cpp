#include "include/VknStagingBuffer.hpp"

namespace vkn
{
    VknStagingBuffer::VknStagingBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
        // This must call the base class constructor.
        : VknObject(relIdxs, absIdxs)
    {
    }

    VknStagingBuffer::~VknStagingBuffer()
    {
        demolish();
    }

    VknStagingBuffer::VknStagingBuffer(VknStagingBuffer &&other) noexcept
        // Move construct the base class, and then this class's members.
        // Static members (s_engine, s_infos) cannot be in an initializer list.
        : VknObject(std::move(other)),
          m_size(other.m_size),
          m_memFlags(other.m_memFlags),
          m_allocInfo(other.m_allocInfo),
          m_mappedData(other.m_mappedData),
          m_manualMapping(other.m_manualMapping),
          m_isPersistentlyMapped(other.m_isPersistentlyMapped),
          m_mustFlushAndInvalidate(other.m_mustFlushAndInvalidate),
          m_setSize(other.m_setSize),
          m_createdBuffer(other.m_createdBuffer)
    {
        other.m_mappedData = nullptr;
        other.m_size = 0;
        other.m_isPersistentlyMapped = false;
        other.m_createdBuffer = false;
    }

    VknStagingBuffer &VknStagingBuffer::operator=(VknStagingBuffer &&other) noexcept
    {
        if (this != &other)
        {
            demolish(); // Clean up existing resources

            VknObject::operator=(std::move(other)); // Move assign the base part
            m_size = other.m_size;
            m_memFlags = other.m_memFlags;
            m_allocInfo = other.m_allocInfo;
            m_mappedData = other.m_mappedData;
            m_manualMapping = other.m_manualMapping;
            m_isPersistentlyMapped = other.m_isPersistentlyMapped;
            m_mustFlushAndInvalidate = other.m_mustFlushAndInvalidate;
            m_setSize = other.m_setSize;
            m_createdBuffer = other.m_createdBuffer;

            other.m_mappedData = nullptr;
            other.m_size = 0;
            other.m_isPersistentlyMapped = false;
            other.m_createdBuffer = false;
        }
        return *this;
    }

    void VknStagingBuffer::create()
    {
        if (m_createdBuffer)
            throw std::runtime_error("VknStagingBuffer already created.");
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

    void VknStagingBuffer::demolish()
    {
        if (!m_createdBuffer)
            return;

        // If it was mapped manually (not persistently by VMA), unmap it
        if (m_mappedData && !m_isPersistentlyMapped)
            this->unmap();

        m_mappedData = nullptr;
        m_size = 0;
        m_isPersistentlyMapped = false;
        m_createdBuffer = false;
    }

    void *VknStagingBuffer::map()
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

    void VknStagingBuffer::unmap()
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

    void VknStagingBuffer::flush(VkDeviceSize offset, VkDeviceSize size)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Buffer not created, cannot flush.");
        // VMA handles checking for HOST_COHERENT internally for vmaFlushAllocation.
        // If it's coherent, flush is a no-op.
        m_flushResult = vmaFlushAllocation(
            s_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()),
            *s_engine->getAllocation<VkBuffer>(m_absIdxs), offset, size);
    }

    void VknStagingBuffer::invalidate(VkDeviceSize offset, VkDeviceSize size)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Buffer not created, cannot invalidate.");
        m_invalidateResult = vmaInvalidateAllocation(
            s_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()),
            *s_engine->getAllocation<VkBuffer>(m_absIdxs), offset, size);
    }

    void VknStagingBuffer::waitForUploadData(void *data, VkDeviceSize size, VkDeviceSize offset)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Buffer not created, cannot copy upload data.");

        m_msgSize->wait(0);
        this->flush(m_uploadDataOffset, m_uploadDataSize);
    }

    VkDescriptorBufferInfo VknStagingBuffer::getDescriptorInfo(VkDeviceSize offset, VkDeviceSize range)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Cannot get descriptor info, buffer is null.");
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = s_engine->getObject<VkBuffer>(m_absIdxs);
        bufferInfo.offset = offset;
        bufferInfo.range = (range == VK_WHOLE_SIZE) ? m_size : range;
        return bufferInfo;
    }

    void VknStagingBuffer::setSize(uint32_t size)
    {
        m_size = size;
        m_setSize = true;
        this->create();
    }

    void *VknStagingBuffer::getDataArea()
    {
        return m_mappedData;
    }

    void VknStagingBuffer::setMsgSize(std::atomic<uint32_t> *msgSize)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Cannot set message size, buffer is null.");
        m_msgSize = msgSize;
    }

} // namespace vkn