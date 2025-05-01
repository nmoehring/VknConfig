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

#include <unordered_map>
#include <optional>
#include <span> // For std::span
#include <stdexcept>
#include "VknCommon.hpp"

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

        void shutdown();

        template <typename T>
        uint32_t push_back(T val)
        {
            VknVector<T> &vec{this->getVector<T>()};
            uint32_t pos = vec.getSize();
            vec.append(val);
            return pos;
        }

        template <typename T>
        uint32_t push_back()
        {
            return this->push_back<T>(T{});
        }

        template <typename T>
        T &getObject(uint32_t idx)
        {
            return this->getVector<T>()(idx);
        }

        template <typename T>
        T &getObject(VknIdxs &absIdxs)
        {
            return this->getVector<T>()(absIdxs.get<T>());
        }

        template <typename T>
        VknVector<T> &getVector()
        {
            std::string vkTypeStr = typeToStr<T>();
            if (m_objectVectors.find(vkTypeStr) == m_objectVectors.end())

                m_objectVectors[vkTypeStr] = new VknVector<T>();
            return *static_cast<VknVector<T> *>(m_objectVectors[vkTypeStr]);
        }

        template <typename T>
        VknVectorIterator<T> getVectorSlice(uint32_t startIdx, uint32_t length)
        {
            VknVector<T> &vec = this->getVector<T>();
            if (startIdx + length > vec.getSize())
                throw std::out_of_range("Slice range exceeds vector size.");
            return vec.getSlice(startIdx, length);
        }

        template <typename T>
        uint32_t getVectorSize()
        {
            return this->getVector<T>().getSize();
        }

    private:
        std::unordered_map<std::string, void *> m_objectVectors{};
        bool m_poweredOn{true};
        /*VknVector<VkInstance> instances{};
        VknVector<VkDevice> devices{};
        VknVector<VkSurfaceKHR> surfaces{};
        VknVector<VkRenderPass> renderpasses{};
        VknVector<VkPipeline> pipelines{};
        VknVector<VkSwapchainKHR> swapchains{};
        VknVector<VkImage> images{};
        VknVector<VkFramebuffer> framebuffers{};
        VknVector<VkImageView> imageViews{};
        VknVector<VkDescriptorSetLayout> descriptorSetLayouts{}; // Takes bindings and push constant ranges ^
        VknVector<VkPipelineLayout> pipelineLayouts{};
        VknVector<VkPhysicalDevice> physicalDevices{};
        VknVector<VkShaderModule> shaderModules{};
        VknVector<VkPipelineCache> pipelineCaches{};*/

        template <typename T>
        void deleteVector()
        {
            std::string vkTypeStr = typeToStr<T>();
            if (m_objectVectors.find(vkTypeStr) != m_objectVectors.end())
                delete static_cast<VknVector<T> *>(m_objectVectors[vkTypeStr]);
        }
    }; // VknEngine

    template <typename VknT, typename VkT>
    VknT &addNewVknObject(uint32_t idx, std::list<VknT> &objList, VknEngine *engine,
                          VknIdxs &relIdxs, VknIdxs &absIdxs, VknInfos *infos)
    {
        if (idx > objList.size())
            throw std::runtime_error("List index out of range.");
        VknIdxs newRelIdxs = relIdxs;
        VknIdxs newAbsIdxs = absIdxs;
        newAbsIdxs.add<VkT>(engine->push_back(VkT{}));
        newRelIdxs.add<VkT>(objList.size());
        return objList.emplace_back(engine, newRelIdxs, newAbsIdxs, infos);
    }
} // namespace vkn