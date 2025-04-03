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
 * [VknEngine] (Free/Top-Level) <<=== YOU ARE HERE
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <optional>
#include <span> // For std::span
#include <stdexcept>

namespace vkn
{
    template <typename T>
    std::string typeToStr()
    {
        if constexpr (std::is_same_v<T, VkInstance>)
            return "instance";
        else if constexpr (std::is_same_v<T, VkDevice>)
            return "device";
        else if constexpr (std::is_same_v<T, VkRenderPass>)
            return "renderpass";
        else if constexpr (std::is_same_v<T, VkPipeline>)
            return "ppln"; // weird because I want to avoid hash table collisions
        else if constexpr (std::is_same_v<T, VkSwapchainKHR>)
            return "swapchain";
        else if constexpr (std::is_same_v<T, VkImage>)
            return "img";
        else if constexpr (std::is_same_v<T, VkFramebuffer>)
            return "framebuffer";
        else if constexpr (std::is_same_v<T, VkImageView>)
            return "imageView";
        else if constexpr (std::is_same_v<T, VkDescriptorSetLayout>)
            return "descriptorSetLayout";
        else if constexpr (std::is_same_v<T, VkPipelineLayout>)
            return "pipelineLayout";
        else if constexpr (std::is_same_v<T, VkPhysicalDevice>)
            return "physicalDevice";
        else if constexpr (std::is_same_v<T, VkShaderModule>)
            return "shaderModule";
        else if constexpr (std::is_same_v<T, VkPipelineCache>)
            return "pipelineCache";
        else if constexpr (std::is_same_v<T, VkSurfaceKHR>)
            return "surface";
        else if constexpr (std::is_same_v<T, VkQueueFamilyProperties>)
            return "qFamilyProperties";
        else
            throw std::runtime_error("Invalid object type passed to typeToStr().");
    } // typeToStr<T>()

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

        template <typename T>
        std::vector<T> &getObjectVector()
        {
            std::string vkTypeStr = typeToStr<T>();
            if (m_objectVectors.find(vkTypeStr) == m_objectVectors.end())

                m_objectVectors[vkTypeStr] = new std::vector<T>();
            return *static_cast<std::vector<T> *>(m_objectVectors[vkTypeStr]);
        }

        template <typename T>
        std::span<T> getObjectVectorSlice(uint32_t startIdx, uint32_t length)
        {
            std::vector<T> &vec = this->getObjectVector<T>();
            if (startIdx + length > vec.size())
                throw std::out_of_range("Slice range exceeds vector size.");
            return std::span<T>(vec.data() + startIdx, length);
        }

    private:
        std::unordered_map<std::string, void *> m_objectVectors{};
        /*std::vector<VkInstance> instances{};
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
        std::vector<VkPipelineCache> pipelineCaches{};*/
    }; // VknEngine

    class VknIdxs
    {
        std::unordered_map<std::string, uint32_t> m_data{};

    public:
        // Overloads
        VknIdxs() = default;
        ~VknIdxs() = default;
        VknIdxs(const VknIdxs &other) = default;
        VknIdxs &operator=(const VknIdxs &other) = default;
        VknIdxs(VknIdxs &&other) = default;
        VknIdxs &operator=(VknIdxs &&other) = default;

        // Getters
        template <typename T>
        uint32_t &get()
        {
            std::string vkTypeStr = typeToStr<T>();
            if (m_data.count(vkTypeStr) == 0)
                throw std::runtime_error("Key of type " + vkTypeStr + " not found in VknIdxs. Key may not be set yet.");
            return m_data.at(typeToStr<T>());
        }

        // Add a new key-value pair
        template <typename T>
        void add(uint32_t value)
        {
            m_data[typeToStr<T>()] = value;
        }

    }; // VknIdxs
} // namespace vkn