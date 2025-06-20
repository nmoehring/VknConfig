#include "include/VknBuffer.hpp"

namespace vkn
{
    VknBuffer::VknBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine(engine), m_relIdxs(relIdxs), m_absIdxs(absIdxs), m_infos(infos)
    {
    }

    VknBuffer::~VknBuffer()
    {
        demolish();
    }

    VknBuffer::VknBuffer(VknBuffer &&other) noexcept
        : m_engine(other.m_engine),
          m_relIdxs(std::move(other.m_relIdxs)),
          m_absIdxs(std::move(other.m_absIdxs)),
          m_infos(other.m_infos),
          m_vkBuffer(other.m_vkBuffer),
          m_allocation(other.m_allocation),
          m_size(other.m_size),
          m_mappedData(other.m_mappedData),
          m_isPersistentlyMapped(other.m_isPersistentlyMapped)
    {
        other.m_vkBuffer = VK_NULL_HANDLE;
        other.m_allocation = VK_NULL_HANDLE;
        other.m_mappedData = nullptr;
        other.m_size = 0;
        other.m_isPersistentlyMapped = false;
    }

    VknBuffer &VknBuffer::operator=(VknBuffer &&other) noexcept
    {
        if (this != &other)
        {
            demolish(); // Clean up existing resources

            m_engine = other.m_engine;
            m_relIdxs = std::move(other.m_relIdxs);
            m_absIdxs = std::move(other.m_absIdxs);
            m_infos = other.m_infos;
            m_vkBuffer = other.m_vkBuffer;
            m_allocation = other.m_allocation;
            m_size = other.m_size;
            m_mappedData = other.m_mappedData;
            m_isPersistentlyMapped = other.m_isPersistentlyMapped;

            other.m_vkBuffer = VK_NULL_HANDLE;
            other.m_allocation = VK_NULL_HANDLE;
            other.m_mappedData = nullptr;
            other.m_size = 0;
            other.m_isPersistentlyMapped = false;
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

        m_size = size;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = m_size;
        bufferInfo.usage = bufferUsage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Default, can be configured if needed

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = memoryUsage;
        allocCreateInfo.flags = allocationFlags;

        // VknEngine needs to store the VkBuffer handle at m_absIdxs
        VknResult res = {vmaCreateBuffer(m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()),
                                         &bufferInfo,
                                         &allocCreateInfo,
                                         &m_engine->getObject<VkBuffer>(m_absIdxs), // VknEngine stores the VkBuffer
                                         &m_engine->addNewAllocation<VkBuffer>(m_absIdxs),
                                         &m_allocInfo), // To get mapped data if VMA_ALLOCATION_CREATE_MAPPED_BIT is set
                         "VMA Create Buffer"};

        m_vkBuffer = m_engine->getObject<VkBuffer>(m_absIdxs); // Store local handle for convenience
        vmaGetMemoryTypeProperties(m_engine->getObject<VmaAllocator>(m_absIdxs), m_allocInfo.memoryType, &m_memFlags);

        if (m_memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            m_isPersistentlyMapped = true;
        else
        {
            m_stagingBuffer = new VknStagingBuffer(m_engine, m_relIdxs, m_absIdxs, m_infos, m_size);
            m_needStagingBuffer = true;
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
        if (m_vkBuffer == VK_NULL_HANDLE || m_allocation == VK_NULL_HANDLE)
            return;

        // If mapped manually (not persistently by VMA), unmap it
        if (m_mappedData && !m_isPersistentlyMapped)
            this->unmap();

        m_vkBuffer = VK_NULL_HANDLE;
        m_allocation = VK_NULL_HANDLE;
        m_mappedData = nullptr;
        m_size = 0;
        m_isPersistentlyMapped = false;
    }

    void *VknBuffer::map()
    {
        if (m_mappedData)
            return m_mappedData; // Already mapped by VMA
        if (m_allocation == VK_NULL_HANDLE)
            throw std::runtime_error("Cannot map buffer: VMA allocation is null.");

        m_mapResult = vmaMapMemory(m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()),
                                   m_allocation, &m_mappedData);

        return m_mappedData;
    }

    void VknBuffer::unmap()
    {
        if (m_isPersistentlyMapped)
            throw std::runtime_error("Trying to unmap a persistently-mapped buffer.");
        if (m_mappedData && m_allocation != VK_NULL_HANDLE)
        {
            vmaUnmapMemory(m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()), m_allocation);
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
            m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()), m_allocation, offset, size);
    }

    void VknBuffer::invalidate(VkDeviceSize offset, VkDeviceSize size)
    {
        if (m_allocation == VK_NULL_HANDLE)
            throw std::runtime_error("Buffer not created, cannot invalidate.");
        m_invalidateResult = vmaInvalidateAllocation(
            m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()), m_allocation, offset, size);
    }

    void VknBuffer::uploadData(const void *data, VkDeviceSize dataSize, VkDeviceSize offset)
    {
        if (m_allocation == VK_NULL_HANDLE)
            throw std::runtime_error("Buffer not created, cannot upload data.");
        if (offset + dataSize > m_size)
            throw std::out_of_range("Upload data size + offset exceeds buffer's logical size.");

        void *mappedPtr = nullptr;
        if (!m_isPersistentlyMapped)
        {
            // This buffer is not host visible. A staging buffer would be needed.
            // For simplicity, this example throws. A real implementation might use a helper
            // function to transfer via a temporary staging buffer.
            throw std::runtime_error("Buffer memory is not host visible. Direct uploadData requires HOST_VISIBLE memory. Use a staging buffer.");
        }

        // Flush if not coherent and mapped
        if (m_mustFlushAndInvalidate)
            vmaFlushAllocation(m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>()), m_allocation, offset, dataSize);
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
        if (m_needStagingBuffer)
            return m_stagingBuffer->getDataArea();
        return m_mappedData;
    }

} // namespace vkn
