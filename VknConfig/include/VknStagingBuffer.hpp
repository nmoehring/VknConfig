#pragma once

#include "VknObject.hpp"

namespace vkn
{
    class VknStagingBuffer: public VknObject
    {
    public:
        VknStagingBuffer(VknIdxs relIdxs, VknIdxs absIdxs);
        ~VknStagingBuffer();

        // Non-copyable, but movable
        VknStagingBuffer(const VknStagingBuffer &) = delete;
        VknStagingBuffer &operator=(const VknStagingBuffer &) = delete;
        VknStagingBuffer(VknStagingBuffer &&other) noexcept;
        VknStagingBuffer &operator=(VknStagingBuffer &&other) noexcept;

        void create();
        void demolish();

        VkBuffer *getVkBuffer() { return &s_engine->getObject<VkBuffer>(m_absIdxs); }
        VmaAllocation *getVmaAllocation();
        VkDeviceSize getSize() { return m_size; }
        void *getMappedData() { return m_mappedData; } // Valid if VMA_ALLOCATION_CREATE_MAPPED_BIT was used
        void *getDataArea();
        void setUploadData(void *data) { m_uploadData = data; }
        void setDownloadData(void *data) { m_downloadData = data; }
        void copyUploadData(void *data = nullptr, VkDeviceSize size = 0, VkDeviceSize offset = 0);
        void copyDownloadData(void *data = nullptr, VkDeviceSize size = 0, VkDeviceSize offset = 0);
        void setSize(uint32_t size);

        // Manual mapping/unmapping if not persistently mapped
        void *map();
        void unmap();

        // For non-coherent memory, if not using VMA_ALLOCATION_CREATE_MAPPED_BIT
        // or if VMA allocated non-coherent memory.
        void flush(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
        void invalidate(VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

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
        VkDeviceSize m_downloadDataSize{0};

        // state
        bool m_uploadable{false};
        bool m_downloadable{false};
        bool m_uploading{true};

    private:
        // Params
        VkMemoryPropertyFlags m_memFlags;
        VmaAllocationInfo m_allocInfo;
        uint32_t m_msgIdx{std::numeric_limits<uint32_t>::max()};
        std::atomic<uint32_t> m_msgSize{std::numeric_limits<uint32_t>::max()};
        VknDispatchRegistration m_reg{};

        // Members
        void *m_mappedData{nullptr}; // Stores pointer if persistently mapped by VMA

        // State
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
     * @brief A specialized buffer used as a temporary intermediary for transferring data
     * between the CPU and GPU, or between GPU resources that aren't directly compatible.
     * This implementation is for CPU-to-GPU transfers (uploads).
     */
    class VknUploadBuffer : public VknStagingBuffer
    {
    public:
        VknUploadBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknStagingBuffer(relIdxs, absIdxs)
        {
            m_transferType = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            m_memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
            m_allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            m_uploadable = true;
        }
    };

    /**
     * @brief A specialized buffer used as a temporary intermediary for transferring data
     * from the GPU to the CPU (downloads).
     */
    class VknDownloadBuffer : public VknStagingBuffer
    {
    public:
        VknDownloadBuffer(VknIdxs relIdxs, VknIdxs absIdxs)
            : VknStagingBuffer(relIdxs, absIdxs)
        {
            m_transferType = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            m_memoryUsage = VMA_MEMORY_USAGE_GPU_TO_CPU;
            m_allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
            m_downloadable = true;
        }
    };
}