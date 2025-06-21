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
          m_vkBuffer(other.m_vkBuffer),
          m_allocation(other.m_allocation),
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
        other.m_vkBuffer = VK_NULL_HANDLE;
        other.m_allocation = VK_NULL_HANDLE;
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
            m_vkBuffer = other.m_vkBuffer;
            m_allocation = other.m_allocation;
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

            other.m_vkBuffer = VK_NULL_HANDLE;
            other.m_allocation = VK_NULL_HANDLE;
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

    void VknBuffer::create(
        VkDeviceSize size,
        VkBufferUsageFlags bufferUsage,
        VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags allocationFlags)
    {
        if (m_vkBuffer != VK_NULL_HANDLE)
            throw std::runtime_error("VknBuffer already created.");
        if (!m_setSize)
            throw std::runtime_error("Size not set before creating buffer.");

        m_size = size;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = m_size;
        bufferInfo.usage = bufferUsage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Default, can be configured if needed

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = memoryUsage;
        allocCreateInfo.flags = allocationFlags;

        // The VknDevice factory method should have already called addNewObject<VkBuffer,...>
        // which reserves a spot in VknEngine's vector. We just need to create the buffer into that spot.
        VknResult res = {vmaCreateBuffer(s_engine.getObject<VmaAllocator>(m_absIdxs),
                                         &bufferInfo,
                                         &allocCreateInfo,
                                         &s_engine.getObject<VkBuffer>(m_absIdxs),        // VknEngine stores the VkBuffer
                                         &s_engine.addNewAllocation<VkBuffer>(m_absIdxs), // And the allocation
                                         &m_allocInfo),                                   // To get mapped data if VMA_ALLOCATION_CREATE_MAPPED_BIT is set
                         "VMA Create Buffer"};

        m_vkBuffer = s_engine.getObject<VkBuffer>(m_absIdxs); // Store local handle for convenience
        m_allocation = s_engine.getObject<VmaAllocation>(m_absIdxs);
        vmaGetMemoryTypeProperties(s_engine.getObject<VmaAllocator>(m_absIdxs), m_allocInfo.memoryType, &m_memFlags);

        if (m_memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            m_isPersistentlyMapped = true;
        else
        {
            m_uploadBuffer = new VknUploadBuffer(m_relIdxs, m_absIdxs);
            m_uploadBuffer->setSize(m_size);
            m_hasUploadBuffer = true;
        }

        if (allocationFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
        {
            m_manualMapping = false;
            m_mappedData = m_allocInfo.pMappedData;
        }
        if (m_memFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            m_mustFlushAndInvalidate = false;

        if (m_manualMapping)
            this->map();
        m_createdBuffer = true;
    }

    void VknBuffer::demolish()
    {
        if (!m_createdBuffer)
            return;

        // If it was mapped manually (not persistently by VMA), unmap it
        if (m_mappedData && !m_isPersistentlyMapped && m_allocation != VK_NULL_HANDLE)
            this->unmap();

        m_vkBuffer = VK_NULL_HANDLE;
        m_allocation = VK_NULL_HANDLE;
        m_mappedData = nullptr;
        m_size = 0;
        m_isPersistentlyMapped = false;
        m_createdBuffer = false;
    }

    void *VknBuffer::map()
    {
        if (m_mappedData) // Already mapped (either by VMA or manually)
            return m_mappedData;
        if (m_allocation == VK_NULL_HANDLE)
            throw std::runtime_error("Cannot map buffer: VMA allocation is null.");

        // Retrieve the VmaAllocator using the absolute index stored in the buffer's VknIdxs
        // This assumes the VmaAllocator index was added to m_absIdxs when the parent VknDevice was created.
        VmaAllocator allocator = s_engine.getObject<VmaAllocator>(m_absIdxs);

        m_mapResult = vmaMapMemory(allocator, m_allocation, &m_mappedData);

        return m_mappedData;
    }

    void VknBuffer::unmap()
    {
        if (m_isPersistentlyMapped)
            return; // Do not unmap buffers that VMA mapped persistently
        if (m_mappedData && m_allocation != VK_NULL_HANDLE)
        {
            VmaAllocator allocator = s_engine.getObject<VmaAllocator>(m_absIdxs);
            vmaUnmapMemory(allocator, m_allocation);
            m_mappedData = nullptr;
        }
    }

    void VknBuffer::flush(VkDeviceSize offset, VkDeviceSize size)
    {
        if (m_allocation == VK_NULL_HANDLE)
            throw std::runtime_error("Buffer not created, cannot flush.");
        // VMA handles checking for HOST_COHERENT internally for vmaFlushAllocation.
        // If it's coherent, flush is a no-op.
        m_flushResult = vmaFlushAllocation(
            s_engine.getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()), m_allocation, offset, size);
    }

    void VknBuffer::invalidate(VkDeviceSize offset, VkDeviceSize size)
    {
        if (m_allocation == VK_NULL_HANDLE)
            throw std::runtime_error("Buffer not created, cannot invalidate.");
        m_invalidateResult = vmaInvalidateAllocation(
            s_engine.getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()), m_allocation, offset, size);
    }

    VkBufferCopy *VknBuffer::uploadData(const void *data, VkDeviceSize dataSize, VkDeviceSize offset)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Buffer not created, cannot upload data.");
        if (offset + dataSize > m_size)
            throw std::out_of_range("Upload data size + offset exceeds buffer's logical size.");
        if (!m_isPersistentlyMapped)
        {
            m_copyRegion->srcOffset = offset; // Data is at the start of the staging buffer
            m_copyRegion->dstOffset = offset;
            m_copyRegion->size = dataSize;
            return m_copyRegion;
        }
        this->flush(offset, dataSize);
        return nullptr;
    }

    VkBufferCopy *VknBuffer::downloadData(void *data, VkDeviceSize dataSize, VkDeviceSize offset)
    {
        if (!m_createdBuffer)
            throw std::runtime_error("Buffer not created, cannot download data.");
        if (offset + dataSize > m_size)
            throw std::out_of_range("Download data size + offset exceeds buffer's logical size.");
        if (!m_isPersistentlyMapped)
        {
            m_copyRegion->srcOffset = offset; // Data is at the start of the staging buffer
            m_copyRegion->dstOffset = offset;
            m_copyRegion->size = dataSize;
            return m_copyRegion;
        }
        this->invalidate(offset, dataSize); // Ensure CPU sees GPU writes
        return nullptr;
    }

    VkDescriptorBufferInfo VknBuffer::getDescriptorInfo(VkDeviceSize offset, VkDeviceSize range) const
    {
        if (m_vkBuffer == VK_NULL_HANDLE)
            throw std::runtime_error("Cannot get descriptor info, buffer is null.");
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_vkBuffer;
        bufferInfo.offset = offset;
        bufferInfo.range = (range == VK_WHOLE_SIZE) ? m_size : range;
        return bufferInfo;
    }

    void VknBuffer::setSize(uint32_t size)
    {
        m_size = size;
        m_setSize = true;
    }

    void *VknBuffer::getDataArea()
    {
        if (m_hasUploadBuffer)
            return m_uploadBuffer->getDataArea();
        else if (m_hasDownloadBuffer)
            return m_downloadBuffer->getDataArea();
        return m_mappedData;
    }

} // namespace vkn
