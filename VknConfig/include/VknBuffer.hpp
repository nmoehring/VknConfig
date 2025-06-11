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

// Forward declarations
namespace vkn
{
    class VknEngine;
    class VknInfos;
    class VknIdxs;
    class VknDevice; // If VknBuffer needs direct access to VknDevice methods
}

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

        void destroy();

        VkBuffer getVkBuffer() const { return m_vkBuffer; }
        VmaAllocation getVmaAllocation() const { return m_allocation; }
        VkDeviceSize getSize() const { return m_size; }
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
        VknEngine *m_engine = nullptr;
        VknIdxs m_relIdxs;           // Relative to parent (e.g. VknDevice)
        VknIdxs m_absIdxs;           // Absolute within VknEngine
        VknInfos *m_infos = nullptr; // For accessing VkBufferCreateInfo if needed, though VMA handles most

        VkBuffer m_vkBuffer = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
        VkDeviceSize m_size = 0;
        void *m_mappedData = nullptr; // Stores pointer if persistently mapped by VMA
        bool m_isPersistentlyMapped = false;
    };
}