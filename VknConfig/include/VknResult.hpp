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
 *     +-- [VknDevice]
 *         |
 *         +-- [VknPhysicalDevice]
 *         |   |
 *         |   +-- [VknQueueFamily] ^ / \
 *         |
 *         +-- [VknSwapchain]
 *         |   |
 *         |   +-- [VknImageView] ^ / \
 *         |
 *         +-- [VknRenderpass]
 *             |
 *             +-- [VknFramebuffer] ^ / \
 *             |
 *             +-- [VknPipeline]
 *                 |
 *                 +-- [VknPipelineLayout]
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout]
 *                 |
 *                 +-- [VknVertexInputState] ^ / \
 *                 +-- [VknInputAssemblyState] ^ / \
 *                 +-- [VknMultisampleState] ^ / \
 *                 +-- [VknRasterizationState] ^ / \
 *                 +-- [VknShaderStage] ^ / \
 *                 +-- [VknViewportState] ^ / \
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level) <<=== YOU ARE HERE
 */

#pragma once

#include <string>
#include <format>
#include <queue>
#include <string>
#include <stdexcept> // Ensure this is included for std::runtime_error
#include <format>    // Ensure this is included for std::format

#include <vulkan/vulkan.h>

namespace vkn
{
    /**
     * @brief Stores a history of VknResult objects.
     *
     * This class acts as a circular buffer to keep track of recent
     * VknResult objects, allowing for debugging and error analysis.
     */
    template <typename T>
    class VknArchive
    {
    private:
        std::queue<T> m_data{};
        uint32_t m_capacity{1000}; /**< The maximum number of results to store. */

        /** @brief Manages the size of the archive, removing old results if necessary. */
        void manage()
        {
            if (m_data.size() > m_capacity)
                for (uint64_t count{m_data.size() - m_capacity}; count > 0; --count)
                    m_data.pop();
        }

    public:
        /** @brief Stores a result in the archive.
         * @param res The result to store. */
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
        // Overloads
        VknResult() = delete;
        VknResult(std::string opDesc);
        VknResult(VkResult result, std::string opDesc);
        void operator=(VkResult result) { this->evaluate(result); }

        /** @brief Returns a string representation of the result.
         * @return A string representation of the result.*/
        std::string toString();

        /** @brief Returns an error string with the given message.
         * @param msg The message to include in the error string.
         * @return An error string. */
        std::string toErr(std::string msg);

        bool isErrorStateEvaluated() { return m_evaluatedErrorState; }

    private:
        // Members
        VkResult m_result{};
        std::string m_opDesc{""};               /**< A description of the operation that produced this result. */
        static VknArchive<VknResult> s_archive; /**< A static archive to store all VknResult objects. */

        // State
        bool m_evaluatedErrorState{false}; /**< True if the result has been evaluated. */

        /** @brief Evaluates the given VkResult and updates the internal state.
         * @param result The VkResult to evaluate. */
        void evaluate(VkResult result);
    };
}
