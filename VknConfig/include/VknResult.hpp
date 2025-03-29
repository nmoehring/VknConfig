/**
 * @file VknResult.hpp
 * @brief Represents a Vulkan API result and provides error handling.
 *
 * VknResult is a free/top-level class within the VknConfig project.
 * It is used to represent a Vulkan API result and provides methods for
 * checking for success/failure and getting a string representation of the result.
 * VknResult does not depend on any other Vkn classes.
 *
 * Hierarchy Graph:
 * [VknConfig] (Top-Level)
 *     |
 *     +-- [VknDevice] (Hierarchy-Bound)
 *         |
 *         +-- [VknPhysicalDevice] (Hierarchy-Bound)
 *         |   |
 *         |   +-- [VknQueueFamily] (Hierarchy-Bound Leaf)
 *         |
 *         +-- [VknSwapchain] (Hierarchy-Bound)
 *         |   |
 *         |   +-- [VknImageView] (Hierarchy-Bound Leaf)
 *         |
 *         +-- [VknRenderpass] (Hierarchy-Bound)
 *             |
 *             +-- [VknPipeline] (Hierarchy-Bound)
 *                 |
 *                 +-- [VknVertexInputState] (Hierarchy-Bound Leaf)
 *                 +-- [VknInputAssemblyState] (Hierarchy-Bound Leaf)
 *                 +-- [VknMultisampleState] (Hierarchy-Bound Leaf)
 *                 +-- [VknRasterizationState] (Hierarchy-Bound Leaf)
 *                 +-- [VknShaderStage] (Hierarchy-Bound Leaf)
 *                 +-- [VknViewportState] (Hierarchy-Bound Leaf)
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level) <<=== YOU ARE HERE
 */

#pragma once

#include <string>
#include <format>
#include <queue>

#include <vulkan/vulkan.h>

namespace vkn
{
    template <typename T>
    class VknArchive
    {
        std::queue<T> m_data{};
        uint32_t m_capacity{1000};

        void manage()
        {
            if (m_data.size() > m_capacity)
                for (uint64_t count{m_data.size() - m_capacity}; count > 0; --count)
                    m_data.pop();
        }

    public:
        void store(T res)
        {
            if (!res.isErrorStateEvaluated())
                throw std::runtime_error("Target function not called, no result to store.");
            m_data.push(res);
            this->manage();
        }
    };

    class VknResult
    {
    public:
        VknResult() = delete;
        VknResult(std::string opDesc);
        VknResult(VkResult result, std::string opDesc);

        void operator=(VkResult result) { this->evaluate(result); }
        void evaluate(VkResult result);

        std::string toString();
        std::string toErr(std::string msg);
        bool isErrorStateEvaluated() { return m_errorStateEvaluated; }

    private:
        // Members
        VkResult m_result{};
        std::string m_opDesc{""};
        static VknArchive<VknResult> s_archive;

        // State
        bool m_errorStateEvaluated{false};
    };
}
