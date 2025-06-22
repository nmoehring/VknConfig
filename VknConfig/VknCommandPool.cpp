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

    void VknCommandPool::createCommandBuffers(uint32_t numSwapchainImages)
    {
        if (m_commandBuffersAllocated)
            throw std::runtime_error("Command buffers already allocated.");
        if (!m_commandPoolCreated)
            throw std::runtime_error("Command pool not created before allocating command buffers.");

        s_engine->addVkCommandBuffers(m_absIdxs, numSwapchainImages);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = s_engine->getObject<VkCommandPool>(m_absIdxs);
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Primary can be submitted to queues
        allocInfo.commandBufferCount = numSwapchainImages;

        VknResult res{vkAllocateCommandBuffers(
                          s_engine->getObject<VkDevice>(m_absIdxs), &allocInfo, s_engine->getObject<VkCommandBuffer *>(m_absIdxs)),
                      "Allocate command buffers"};
        m_commandBuffersAllocated = true;
    }

    VkCommandBuffer *VknCommandPool::getCommandBuffer(uint32_t imageIdx)
    {
        if (!m_commandBuffersAllocated)
            throw std::runtime_error("Command buffers not allocated yet.");
        return &s_engine->getObject<VkCommandBuffer *>(m_absIdxs)[imageIdx];
    }
}