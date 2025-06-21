#pragma once

#include <cstring> // For memcpy
#include <vector>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h> // Include VMA header
#include <stdexcept>

#include "VknObject.hpp"
#include "VknResult.hpp" // For VknResult

namespace vkn
{
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

        void create(
            VkDeviceSize size,
            VkBufferUsageFlags bufferUsage,
            VmaMemoryUsage memoryUsage,
            VmaAllocationCreateFlags allocationFlags = 0 // e.g., VMA_ALLOCATION_CREATE_MAPPED_BIT
        );

        void demolish();

        VkBuffer getVkBuffer() const { return m_vkBuffer; }
        VmaAllocation *getVmaAllocation() const;
        VkDeviceSize getSize() const { return m_size; }
        void *getMappedData() const { return m_mappedData; } // Valid if VMA_ALLOCATION_CREATE_MAPPED_BIT was used
        void *getDataArea();
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
        VkBufferCopy *uploadData(const void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        // Helper to download data. Only works for host-visible memory.
        // For DEVICE_LOCAL, use VknDevice::downloadDataFromBuffer.
        VkBufferCopy *downloadData(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        VkDescriptorBufferInfo getDescriptorInfo(VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE) const;

    protected:
        VkDeviceSize m_size = 0;

    private:
        // Params
        VkMemoryPropertyFlags m_memFlags;
        VmaAllocationInfo m_allocInfo;

        // Members
        VkBuffer m_vkBuffer = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
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
            create(
                m_size,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // For GPU vertex input and as a destination for transfers
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE                                   // Prefer fastest GPU memory
            );
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
            create(
                m_size,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // For GPU index input and as a destination for transfers
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE                                  // Prefer fastest GPU memory
            );
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
            create(
                m_size,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VMA_MEMORY_USAGE_CPU_TO_GPU, // Optimized for CPU writes, GPU reads
                VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
        }
    };

    // Doesn't change much, so don't plan to update often
    class VknGpuUniformBuffer : public VknBuffer
    {
    public:
        VknGpuUniformBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknBuffer(relIdxs, absIdxs)
        {
            create(
                m_size,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
        }
    };

    /**
     * @brief A specialized buffer used as a temporary intermediary for transferring data
     * between the CPU and GPU, or between GPU resources that aren't directly compatible.
     * This implementation is for CPU-to-GPU transfers (uploads).
     */
    class VknUploadBuffer : public VknBuffer
    {
    public:
        VknUploadBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknBuffer(relIdxs, absIdxs)
        {
            create(
                m_size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Source for copy operations
                VMA_MEMORY_USAGE_CPU_ONLY,        // CPU writes, GPU reads (for transfer)
                                                  // VMA_MEMORY_USAGE_CPU_TO_GPU is also a good option
                VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
        }
    };

    /**
     * @brief A specialized buffer used as a temporary intermediary for transferring data
     * from the GPU to the CPU (downloads).
     */
    class VknDownloadBuffer : public VknBuffer
    {
    public:
        VknDownloadBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknBuffer(relIdxs, absIdxs)
        {
            create(
                m_size,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Destination for copy operations
                VMA_MEMORY_USAGE_GPU_TO_CPU,      // Optimized for GPU writes, CPU reads
                VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);
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
            create(
                m_size,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                // Can be source/destination for transfers if needed
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE // Prefer fastest GPU memory
            );
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
            create(
                m_size,
                VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                // Storage bit if compute shaders write to it, transfer bit to upload initial data or copy from another buffer
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE // Prefer fastest GPU memory
            );
        }
    };

} // namespace vkn