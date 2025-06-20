#pragma once

#include <cstring> // For memcpy
#include <vector>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h> // Include VMA header
#include <stdexcept>

#include "VknEngine.hpp"
#include "VknInfos.hpp"  // If needed for buffer create info details
#include "VknDevice.hpp" // For VmaAllocator
#include "VknResult.hpp" // For VknResult

namespace vkn
{
    /**
     * @brief Manages a Vulkan VkBuffer and its associated memory using VMA.
     *
     * VknBuffer can be configured for various purposes like vertex, index, uniform,
     * or staging buffers by specifying appropriate usage flags and VMA memory usage.
     */
    class VknBuffer
    {
    public:
        VknBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);
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
        VkDeviceSize getSize() const { return m_size; }
        void *getDataArea();
        void *getMappedData() const { return m_mappedData; } // Valid if VMA_ALLOCATION_CREATE_MAPPED_BIT was used
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
        void uploadData(const void *data, VkDeviceSize size, VkDeviceSize offset = 0);

        VkDescriptorBufferInfo getDescriptorInfo(VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE) const;

    private:
        // Engine
        VknEngine *m_engine = nullptr;
        VknIdxs m_relIdxs;           // Relative to parent (e.g. VknDevice)
        VknIdxs m_absIdxs;           // Absolute within VknEngine
        VknInfos *m_infos = nullptr; // For accessing VkBufferCreateInfo if needed, though VMA handles most

        // Params
        VkDeviceSize m_size = 0;
        VkMemoryPropertyFlags m_memFlags;
        VmaAllocationInfo m_allocInfo;

        // Members
        VkBuffer m_vkBuffer = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
        void *m_mappedData{nullptr}; // Stores pointer if persistently mapped by VMA
        VknStagingBuffer *m_stagingBuffer{nullptr};

        // State
        bool m_needStagingBuffer{false};
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
        VknVertexBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
            : VknBuffer(engine, relIdxs, absIdxs, infos)
        {
            create(
                size,
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
        VknIndexBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
            : VknBuffer(engine, relIdxs, absIdxs, infos)
        {
            create(
                size,
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
        VknCpuUniformBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
            : VknBuffer(engine, relIdxs, absIdxs, infos)
        {
            create(
                size,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VMA_MEMORY_USAGE_CPU_TO_GPU, // Optimized for CPU writes, GPU reads
                VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
        }
    };

    // Doesn't change much, so don't plan to update often
    class VknGpuUniformBuffer : public VknBuffer
    {
    public:
        VknGpuUniformBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
            : VknBuffer(engine, relIdxs, absIdxs, infos)
        {
            create(
                size,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
        }
    };

    /**
     * @brief A specialized buffer used as a temporary intermediary for transferring data
     * between the CPU and GPU, or between GPU resources that aren't directly compatible.
     * This implementation is for CPU-to-GPU transfers (uploads).
     */
    class VknStagingBuffer : public VknBuffer
    {
    public:
        VknStagingBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
            : VknBuffer(engine, relIdxs, absIdxs, infos)
        {
            create(
                size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // Source for copy operations
                VMA_MEMORY_USAGE_CPU_ONLY,        // CPU writes, GPU reads (for transfer)
                                                  // VMA_MEMORY_USAGE_CPU_TO_GPU is also a good option
                VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
        }
        // Consider adding another constructor or a parameter for GPU-to-CPU staging buffers
        // (VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU)
    };

    /**
     * @brief A specialized buffer for general-purpose storage that can be read and written by shaders.
     * Useful for compute shaders, GPU-driven rendering techniques, and large data sets.
     * Typically resides in device-local memory.
     */
    class VknStorageBuffer : public VknBuffer
    {
    public:
        VknStorageBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
            : VknBuffer(engine, relIdxs, absIdxs, infos)
        {
            create(
                size,
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
        VknIndirectBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
            : VknBuffer(engine, relIdxs, absIdxs, infos)
        {
            create(
                size,
                VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                // Storage bit if compute shaders write to it, transfer bit to upload initial data or copy from another buffer
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE // Prefer fastest GPU memory
            );
        }
    };

} // namespace vkn