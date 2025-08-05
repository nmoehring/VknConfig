#include "include/VknCommandPool.hpp"

namespace vkn
{
    VknCommandPool::VknCommandPool(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
    {
    }

    void VknCommandPool::createCommandPool(uint32_t queueFamilyIndex)
    {
        if (m_commandPoolCreated)
            throw std::runtime_error("Command pool already created.");

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allows resetting individual command buffers
        poolInfo.queueFamilyIndex = queueFamilyIndex;

        VknResult res{vkCreateCommandPool(
                          s_engine->getObject<VkDevice>(m_absIdxs), &poolInfo, nullptr, &s_engine->getObject<VkCommandPool>(m_absIdxs)),
                      "Create command pool"};
        m_commandPoolCreated = true;
    }

    // return idx to the first of this type of command buffer
    uint32_t VknCommandPool::createCommandBuffers(uint32_t numUniqueBuffers)
    {
        if (m_commandBuffersAllocated)
            return;
        if (!m_commandPoolCreated)
            throw std::runtime_error("Command pool not created before allocating command buffers.");

        VkCommandBuffer *newArr{s_engine->addVkCommandBuffers(m_absIdxs, numUniqueBuffers * VknObject::s_maxFramesInFlight)};

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = s_engine->getObject<VkCommandPool>(m_absIdxs);
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Primary can be submitted to queues
        allocInfo.commandBufferCount = numUniqueBuffers * VknObject::s_maxFramesInFlight;

        VknResult res{vkAllocateCommandBuffers(
                          s_engine->getObject<VkDevice>(m_absIdxs), &allocInfo, newArr),
                      "Allocate command buffers"};
        m_commandBuffersAllocated = true;
    }

    VkCommandBuffer *VknCommandPool::getCommandBuffer(uint32_t frameNum, uint32_t imageIdx)
    {
        if (!m_commandBuffersAllocated)
            throw std::runtime_error("Command buffers not allocated yet.");
        if (frameNum >= VknObject::s_maxFramesInFlight)
            throw std::runtime_error("Frame number out of bounds.");
        return &s_engine->getObject<VkCommandBuffer *>(m_absIdxs)[(imageIdx * VknObject::s_maxFramesInFlight) + frameNum];
    }
}