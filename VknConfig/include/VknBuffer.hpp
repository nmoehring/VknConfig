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
        VmaAllocation getVmaAllocation() const { return m_allocation; }
        VkDeviceSize getSize() const { return m_size; }
        bool isHostVisible() const;
        void *getMappedData() const { return m_mappedData; } // Valid if VMA_ALLOCATION_CREATE_MAPPED_BIT was used

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
        // VkBufferCreateFlags m_flags = 0;
        // VkDeviceSize m_size = 4096;                                                                        // Default to a 4KB buffer, adjust as needed
        // VkBufferUsageFlags m_usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; // Suitable for vertex data & uploads

        VkBuffer m_vkBuffer = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
        VkDeviceSize m_size = 0;
        void *m_mappedData = nullptr; // Stores pointer if persistently mapped by VMA
        bool m_isPersistentlyMapped = false;

        // VkBufferCreateInfo *fileBufferCreateInfo();
        // VmaAllocationCreateInfo *fileAllocationCreateInfo();
    };

    /**
     * @brief A specialized buffer for storing vertex data.
     * Typically resides in device-local memory for fast GPU access.
     * Data is usually uploaded via a staging buffer.
     */
    class VertexBuffer : public VknBuffer
    {
    public:
        VertexBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
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
    class IndexBuffer : public VknBuffer
    {
    public:
        IndexBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
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
    class CpuUniformBuffer : public VknBuffer
    {
    public:
        CpuUniformBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
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
    class GpuUniformBuffer : public VknBuffer
    {
    public:
        GpuUniformBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
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
    class StagingBuffer : public VknBuffer
    {
    public:
        StagingBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
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
    class StorageBuffer : public VknBuffer
    {
    public:
        StorageBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
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
    class IndirectBuffer : public VknBuffer
    {
    public:
        IndirectBuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos, VkDeviceSize size)
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