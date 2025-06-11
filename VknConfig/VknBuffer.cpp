#include "include/VknBuffer.hpp"

namespace vkn
{
    VknBuffer::VknBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine(engine), m_relIdxs(relIdxs), m_absIdxs(absIdxs), m_infos(infos)
    {
    }

    VknBuffer::~VknBuffer()
    {
        destroy();
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
            destroy(); // Clean up existing resources

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
        {
            throw std::runtime_error("VknBuffer already created.");
        }

        m_size = size;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = m_size;
        bufferInfo.usage = bufferUsage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Default, can be configured if needed

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memoryUsage;
        allocInfo.flags = allocationFlags;

        // Retrieve the VmaAllocator. Assuming it's stored by VknDevice and accessible via VknEngine.
        // The m_absIdxs for the buffer should be set up such that m_absIdxs.get<VkDevice>()
        // correctly identifies the parent device's index in the engine's VmaAllocator list (if stored that way).
        VmaAllocator vmaAllocator = m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>());

        VmaAllocationInfo allocationInfo{}; // To get mapped data if VMA_ALLOCATION_CREATE_MAPPED_BIT is set

        // VknEngine needs to store the VkBuffer handle at m_absIdxs
        VknResult res = {vmaCreateBuffer(vmaAllocator,
                                         &bufferInfo,
                                         &allocInfo,
                                         &m_engine->getObject<VkBuffer>(m_absIdxs), // VknEngine stores the VkBuffer
                                         &m_allocation,
                                         &allocationInfo), // Get allocation info
                         "VMA Create Buffer"};

        if (!res.isSuccess())
        {
            // m_engine->removeObject<VkBuffer>(m_absIdxs); // If VknEngine supports this
            throw std::runtime_error("Failed to create VMA buffer: " + res.getMessage());
        }

        m_vkBuffer = m_engine->getObject<VkBuffer>(m_absIdxs); // Store local handle for convenience

        if (allocationFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
        {
            m_mappedData = allocationInfo.pMappedData;
            m_isPersistentlyMapped = true;
        }
    }

    void VknBuffer::destroy()
    {
        if (m_vkBuffer == VK_NULL_HANDLE || m_allocation == VK_NULL_HANDLE)
        {
            return;
        }

        VmaAllocator vmaAllocator = m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>());

        // If mapped manually (not persistently by VMA), unmap it
        if (m_mappedData && !m_isPersistentlyMapped)
        {
            vmaUnmapMemory(vmaAllocator, m_allocation);
            m_mappedData = nullptr;
        }

        vmaDestroyBuffer(vmaAllocator, m_vkBuffer, m_allocation);

        // m_engine->removeObject<VkBuffer>(m_absIdxs); // If VknEngine supports this

        m_vkBuffer = VK_NULL_HANDLE;
        m_allocation = VK_NULL_HANDLE;
        m_mappedData = nullptr;
        m_size = 0;
        m_isPersistentlyMapped = false;
    }

    void *VknBuffer::map()
    {
        if (m_isPersistentlyMapped && m_mappedData)
        {
            return m_mappedData; // Already mapped by VMA
        }
        if (m_allocation == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Cannot map buffer: VMA allocation is null.");
        }
        if (m_mappedData)
        { // Already manually mapped
            return m_mappedData;
        }

        VmaAllocator vmaAllocator = m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>());
        VkResult result = vmaMapMemory(vmaAllocator, m_allocation, &m_mappedData);
        if (result != VK_SUCCESS)
        {
            m_mappedData = nullptr;
            throw std::runtime_error("Failed to map VMA buffer memory. Error: " + std::to_string(result));
        }
        return m_mappedData;
    }

    void VknBuffer::unmap()
    {
        if (m_isPersistentlyMapped)
        {
            // Persistently mapped buffers are unmapped by VMA during destruction if necessary.
            // Or, if you want to explicitly unmap a persistently mapped buffer before destruction,
            // you might need to reconsider the m_isPersistentlyMapped logic or provide a forceUnmap.
            // Generally, for VMA_ALLOCATION_CREATE_MAPPED_BIT, you don't call unmap.
            return;
        }
        if (m_mappedData && m_allocation != VK_NULL_HANDLE)
        {
            VmaAllocator vmaAllocator = m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>());
            vmaUnmapMemory(vmaAllocator, m_allocation);
            m_mappedData = nullptr;
        }
    }

    void VknBuffer::flush(VkDeviceSize offset, VkDeviceSize size)
    {
        if (m_allocation == VK_NULL_HANDLE)
        {
            // Or log warning
            return;
        }
        VmaAllocator vmaAllocator = m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>());
        // VMA handles checking for HOST_COHERENT internally for vmaFlushAllocation.
        // If it's coherent, flush is a no-op.
        VkResult result = vmaFlushAllocation(vmaAllocator, m_allocation, offset, size);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to flush VMA allocation. Error: " + std::to_string(result));
        }
    }

    void VknBuffer::invalidate(VkDeviceSize offset, VkDeviceSize size)
    {
        if (m_allocation == VK_NULL_HANDLE)
        {
            return;
        }
        VmaAllocator vmaAllocator = m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>());
        VkResult result = vmaInvalidateAllocation(vmaAllocator, m_allocation, offset, size);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to invalidate VMA allocation. Error: " + std::to_string(result));
        }
    }

    void VknBuffer::uploadData(const void *data, VkDeviceSize dataSize, VkDeviceSize offset)
    {
        if (m_allocation == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Buffer not created, cannot upload data.");
        }
        if (offset + dataSize > m_size)
        {
            throw std::out_of_range("Upload data size + offset exceeds buffer's logical size.");
        }

        // Check if memory is host visible. VMA can tell us this.
        VmaAllocationInfo allocInfo;
        VmaAllocator vmaAllocator = m_engine->getObject<VmaAllocator>(m_absIdxs.get<VkDevice>());
        vmaGetAllocationInfo(vmaAllocator, m_allocation, &allocInfo);

        VkMemoryPropertyFlags memFlags;
        vmaGetMemoryTypeProperties(vmaAllocator, allocInfo.memoryType, &memFlags);

        if (!(memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
        {
            // This buffer is not host visible. A staging buffer would be needed.
            // For simplicity, this example throws. A real implementation might use a helper
            // function to transfer via a temporary staging buffer.
            throw std::runtime_error("Buffer memory is not host visible. Direct uploadData requires HOST_VISIBLE memory. Use a staging buffer.");
        }

        void *mappedPtr = nullptr;
        if (m_isPersistentlyMapped && m_mappedData)
        {
            mappedPtr = static_cast<char *>(m_mappedData) + offset;
        }
        else
        {
            // Map the specific region if not persistently mapped
            // Note: VMA's vmaMapMemory maps the whole allocation.
            // For partial writes, map the whole, then offset the pointer.
            VkResult mapResult = vmaMapMemory(vmaAllocator, m_allocation, &mappedPtr);
            if (mapResult != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to map memory for uploadData. Error: " + std::to_string(mapResult));
            }
            mappedPtr = static_cast<char *>(mappedPtr) + offset;
        }

        memcpy(mappedPtr, data, dataSize);

        // Flush if not coherent and mapped
        if (!(memFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            vmaFlushAllocation(vmaAllocator, m_allocation, offset, dataSize);
        }

        if (!m_isPersistentlyMapped)
        {
            vmaUnmapMemory(vmaAllocator, m_allocation);
        }
    }

    VkDescriptorBufferInfo VknBuffer::getDescriptorInfo(VkDeviceSize offset, VkDeviceSize range) const
    {
        if (m_vkBuffer == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Cannot get descriptor info, buffer is null.");
        }
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_vkBuffer;
        bufferInfo.offset = offset;
        bufferInfo.range = (range == VK_WHOLE_SIZE) ? m_size : range;
        return bufferInfo;
    }

} // namespace vkn
