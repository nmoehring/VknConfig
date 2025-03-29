/**
 * @file VknEngine.hpp
 * @brief Manages core Vulkan objects and resources.
 *
 * VknEngine is a free/top-level class within the VknConfig project.
 * It is responsible for managing the Vulkan instance, physical devices,
 * logical devices, and other core Vulkan objects.
 * VknEngine does not depend on any other Vkn classes.
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
 * [VknEngine] (Free/Top-Level) <<=== YOU ARE HERE
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include "VknResult.hpp"

namespace vkn
{
    class VknEngine
    {
    public:
        VknEngine();
        ~VknEngine();

        // Prevent copying
        VknEngine(const VknEngine &) = delete;
        VknEngine &operator=(const VknEngine &) = delete;

        // Prevent moving
        VknEngine(VknEngine &&) = delete;
        VknEngine &operator=(VknEngine &&) = delete;

        template <typename T>
        std::vector<T> &getObjectVector()
        {
            if constexpr (std::is_same_v<T, VkDevice>)
                return devices;
            else if constexpr (std::is_same_v<T, VkRenderPass>)
                return renderpasses;
            else if constexpr (std::is_same_v<T, VkPipeline>)
                return pipelines;
            else if constexpr (std::is_same_v<T, VkSwapchainKHR>)
                return swapchains;
            else if constexpr (std::is_same_v<T, VkImage>)
                return images;
            else if constexpr (std::is_same_v<T, VkFramebuffer>)
                return framebuffers;
            else if constexpr (std::is_same_v<T, VkImageView>)
                return imageViews;
            else if constexpr (std::is_same_v<T, VkDescriptorSetLayout>)
                return descriptorSetLayouts;
            else if constexpr (std::is_same_v<T, VkPipelineLayout>)
                return pipelineLayouts;
            else if constexpr (std::is_same_v<T, VkPhysicalDevice>)
                return physicalDevices;
            else if constexpr (std::is_same_v<T, VkShaderModule>)
                return shaderModules;
            else if constexpr (std::is_same_v<T, VkPipelineCache>)
                return pipelineCaches;
            else if constexpr (std::is_same_v<T, VkSurfaceKHR>)
                return surfaces;
            else
                throw std::runtime_error("Invalid object type passed to getElement() of VknEngine");
        }

        template <typename T>
        uint32_t push_back(T val)
        {
            std::vector<T> &vec{this->getObjectVector<T>()};
            uint32_t idx = vec.size();
            this->getObjectVector<T>().push_back(val);
            return idx;
        }

        template <typename T>
        uint32_t push_back()
        {
            return this->push_back<T>(T{});
        }

        template <typename T>
        T &getObject(uint32_t idx)
        {
            return this->getObjectVector<T>()[idx];
        }

        VkInstance &getInstance() { return instance; }

    private:
        VkInstance instance{};
        bool instanceCreated{false};
        std::vector<VkDevice> devices{};
        std::vector<VkSurfaceKHR> surfaces{};
        std::vector<VkRenderPass> renderpasses{};
        std::vector<VkPipeline> pipelines{};
        std::vector<VkSwapchainKHR> swapchains{};
        std::vector<VkImage> images{};
        std::vector<VkFramebuffer> framebuffers{};
        std::vector<VkImageView> imageViews{};
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{}; // Takes bindings and push constant ranges ^
        std::vector<VkPipelineLayout> pipelineLayouts{};
        std::vector<VkPhysicalDevice> physicalDevices{};
        std::vector<VkShaderModule> shaderModules{};
        std::vector<VkPipelineCache> pipelineCaches{};
    }; // VknEngine

    struct VknIdxs
    {
        std::optional<uint32_t> deviceIdx = std::nullopt;
        std::optional<uint32_t> renderpassIdx = std::nullopt;
        std::optional<uint32_t> subpassIdx = std::nullopt;
        std::optional<uint32_t> shaderIdx = std::nullopt;
        std::optional<uint32_t> swapchainIdx = std::nullopt;
        std::optional<uint32_t> frameIdx = std::nullopt;
        std::optional<uint32_t> imageIdx = std::nullopt;
        std::optional<uint32_t> queueFamilyIdx = std::nullopt;
        std::optional<uint32_t> framebufferIdx = std::nullopt;
        std::optional<uint32_t> imageViewIdx = std::nullopt;
        std::optional<uint32_t> physicalDeviceIdx = std::nullopt;
        std::optional<uint32_t> surfaceIdx = std::nullopt;
    }; // VknIdxs

} // namespace vkn