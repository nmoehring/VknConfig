#pragma once

#include <cstring> // For memcpy
#include <vector>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h> // Include VMA header
#include <stdexcept>

#include "VknObject.hpp"
#include "VknResult.hpp" // For VknResult
#include "VknStagingBuffer.hpp"

namespace vkn
{
    class VknUploadBuffer;
    class VknDownloadBuffer;

    enum BufferType
    {
        VERTEX_BUFFER,
        INDEX_BUFFER,
        UNIFORM_BUFFER,
        STORAGE_BUFFER,
        INDIRECT_BUFFER,
        COMPUTE_VERTEX_BUFFER,
        BUFFER_TYPE_SIZE,
        BUFFER_TYPE_NULL
    };

    /**
     * @brief Manages a Vulkan VkBuffer and its associated memory using VMA.
     *
     * VknBuffer can be configured for various purposes like vertex, index, uniform,
     * or staging buffers by specifying appropriate usage flags and VMA memory usage.
     */
    class VknBuffer : public VknObject
    {
    public:
        VknBuffer(VknIdxs relIdxs, VknIdxs absIdxs);
        ~VknBuffer();

        // Non-copyable, but movable
        VknBuffer(const VknBuffer &) = delete;
        VknBuffer &operator=(const VknBuffer &) = delete;
        VknBuffer(VknBuffer &&other) noexcept;
        VknBuffer &operator=(VknBuffer &&other) noexcept;

        void create();
        void demolish();

        VkBuffer *getVkBuffer() { return &s_engine->getObject<VkBuffer>(m_absIdxs); }
        VmaAllocation *getVmaAllocation();
        VkDeviceSize getSize() { return m_size; }
        void *getMappedData() { return m_mappedData; } // Valid if VMA_ALLOCATION_CREATE_MAPPED_BIT was used
        void *getDataArea();
        VkBuffer *getUploadVkBuffer() const;
        VkBuffer *getDownloadVkBuffer() const;
        void setUploadData(void *data) { m_uploadData = data; }
        void setDownloadData(void *data) { m_downloadData = data; }
        void copyUploadData();
        void copyDownloadData();
        VknDispatchRegistration *getDispatchRegistration();
        void enableUpload() { m_uploading = true; }
        void enableDownload() { m_downloading = true; }
        void setIntegrated(bool integrated)
        {
            if (integrated)
            {
                m_memoryUsage = VMA_MEMORY_USAGE_AUTO;
                m_transferType = 0u;
                m_allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
                m_uploadable = true;
                m_downloadable = true;
                m_integrated = true;
            }
        }
        void setSize(uint32_t size);

        // Manual mapping/unmapping if not persistently mapped
        void *map();
        void unmap();

        // For non-coherent memory, if not using VMA_ALLOCATION_CREATE_MAPPED_BIT
        // or if VMA allocated non-coherent memory.
        void flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
        void invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        // Helper to upload data. If buffer is host visible, maps and copies.
        // For DEVICE_LOCAL, this would typically involve a staging buffer (more complex, not shown here).
        void uploadData();
        // Helper to download data. Only works for host-visible memory.
        // For DEVICE_LOCAL, use VknDevice::downloadDataFromBuffer.
        void downloadData();

        VkDescriptorBufferInfo getDescriptorInfo(VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);

    protected:
        // args
        VkDeviceSize m_size = 0;
        VkBufferUsageFlags m_bufferUsage{0u};
        VmaMemoryUsage m_memoryUsage{};
        VmaAllocationCreateFlags m_allocationFlags{0u};
        VkBufferUsageFlags m_bufferType{0u};
        VkBufferUsageFlags m_transferType{0u};
        void *m_uploadData{nullptr};
        VkDeviceSize m_uploadDataSize{0};
        VkDeviceSize m_uploadDataOffset{0};
        void *m_downloadData{nullptr};
        VkDeviceSize m_downloadDataSize{0};
        VkDeviceSize m_downloadDataOffset{0};

        // state
        bool m_uploadable{false};
        bool m_downloadable{false};
        bool m_uploading{false};
        bool m_downloading{false};

    private:
        // Params
        VkMemoryPropertyFlags m_memFlags;
        VmaAllocationInfo m_allocInfo;
        uint32_t m_msgIdx{std::numeric_limits<uint32_t>::max()};
        std::atomic<uint32_t> m_msgSize{std::numeric_limits<uint32_t>::max()};
        VknDispatchRegistration *m_reg{nullptr};

        // Members
        void *m_mappedData{nullptr}; // Stores pointer if persistently mapped by VMA
        VknUploadBuffer *m_uploadBuffer{nullptr};
        VknDownloadBuffer *m_downloadBuffer{nullptr};
        VkBufferCopy *m_copyRegion{nullptr};

        // State
        bool m_hasUploadBuffer{false};
        bool m_hasDownloadBuffer{false};
        bool m_manualMapping{true};
        bool m_isPersistentlyMapped{false};
        bool m_mustFlushAndInvalidate{true};
        bool m_setSize{false};
        bool m_createdBuffer{false};
        VknResult m_mapResult{"Mapping buffer memory."};
        VknResult m_flushResult{"Flushing buffer memory."};
        VknResult m_invalidateResult{"Invalidating buffer memory."};
        bool m_integrated{false};
    };

    /**
     * @brief A specialized buffer for storing vertex data.
     * Typically resides in device-local memory for fast GPU access.
     * Data is usually uploaded via a staging buffer.
     */
    class VknVertexBuffer : public VknBuffer
    {
    public:
        VknVertexBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknBuffer(relIdxs, absIdxs)
        {
            m_bufferType = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            m_transferType = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            m_memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            m_uploadable = true;
        }
    };

    /**
     * @brief A specialized buffer for storing index data.
     * Used with indexed drawing commands to reduce vertex data duplication.
     * Typically resides in device-local memory for fast GPU access.
     * Data is usually uploaded via a staging buffer.
     */
    class VknIndexBuffer : public VknBuffer
    {
    public:
        VknIndexBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknBuffer(relIdxs, absIdxs)
        {
            m_bufferType = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            m_transferType = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            m_memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            m_uploadable = true;
        }
    };

    /**
     * @brief A specialized buffer for storing uniform data (e.g., transformation matrices, lighting parameters).
     * This implementation is optimized for CPU updates, making it host-visible and persistently mapped.
     * For UBOs that are rarely updated and primarily GPU-read, use GpuUniformBuffer
     */
    class VknCpuUniformBuffer : public VknBuffer
    {
    public:
        VknCpuUniformBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknBuffer(relIdxs, absIdxs)
        {
            m_bufferType = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            m_memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            m_allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }
    };

    // Doesn't change much, so don't plan to update often
    class VknGpuUniformBuffer : public VknBuffer
    {
    public:
        VknGpuUniformBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknBuffer(relIdxs, absIdxs)
        {
            m_bufferType = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            m_transferType = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            m_memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            m_uploadable = true;
        }
    };

    /**
     * @brief A specialized buffer for general-purpose storage that can be read and written by shaders.
     * Useful for compute shaders, GPU-driven rendering techniques, and large data sets.
     * Typically resides in device-local memory.
     */
    class VknStorageBuffer : public VknBuffer
    {
    public:
        VknStorageBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknBuffer(relIdxs, absIdxs)
        {
            m_bufferType = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            m_transferType = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            m_memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            m_uploadable = true;
            m_downloadable = true;
        }
    };

    /**
     * @brief A specialized buffer for storing parameters for indirect draw or dispatch commands.
     * Allows the GPU to determine rendering or compute parameters.
     * Often populated by compute shaders.
     */
    class VknIndirectBuffer : public VknBuffer
    {
    public:
        VknIndirectBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknBuffer(relIdxs, absIdxs)
        {
            m_bufferType = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            m_transferType = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            m_memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            m_uploadable = true;
            m_downloadable = true;
        }
    };

    class VknComputeVertexBuffer : public VknBuffer
    {
    public:
        VknComputeVertexBuffer(VknIdxs rel, VknIdxs abs)
            : VknBuffer(rel, abs)
        {
            // allow compute writes and vertex input reads
            m_bufferType = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            m_transferType = VK_BUFFER_USAGE_TRANSFER_DST_BIT; // optional
            m_memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            m_uploadable = true; // if you ever want to CPU upload
        }
    };

} // namespace vkn