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
#include "VknData.hpp"

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

        template <typename ObjectType, typename ParentType>
        uint32_t push_back(ObjectType val, ParentType *parent)
        {
            VknVector<ObjectType> &vec{this->getVector<ObjectType>()};
            VknVector<ParentType *> &parentVec = this->getParentVector<ObjectType, ParentType>();
            size_t pos = vec.getSize();
            vec.append(val);
            parentVec.append(parent);
            return pos;
        }

        template <typename ObjectType, typename ParentType>
        uint32_t push_back(ParentType *parent)
        {
            return this->push_back<ObjectType, ParentType>(ObjectType{}, parent);
        }

        template <typename ObjectType>
        ObjectType &getObject(uint32_t idx)
        {
            return this->getVector<ObjectType>()(idx);
        }

        template <typename ObjectType>
        ObjectType &getObject(VknIdxs &absIdxs)
        {
            return this->getVector<ObjectType>()(absIdxs.get<ObjectType>());
        }

        template <typename ObjectType>
        VknVector<ObjectType> &getVector()
        {
            std::string vkTypeStr = typeToStr<ObjectType>();
            if (m_objectVectors.find(vkTypeStr) == m_objectVectors.end())

                m_objectVectors[vkTypeStr] = new VknVector<ObjectType>{};
            return *static_cast<VknVector<ObjectType> *>(m_objectVectors[vkTypeStr]);
        }

        template <typename ObjectType, typename ParentType>
        VknVector<ParentType *> &getParentVector()
        {
            std::string vkObjectTypeStr = typeToStr<ObjectType>();
            std::string vkParentTypeStr = typeToStr<ParentType>();
            if (m_parentVectors.find(vkObjectTypeStr) == m_parentVectors.end())
                m_parentVectors[vkObjectTypeStr] = new VknVector<ParentType *>{};
            return *static_cast<VknVector<ParentType *> *>(m_parentVectors[vkObjectTypeStr]);
        }

        template <typename ObjectType, typename ParentType>
        ParentType *getParentPointer(size_t idx)
        {
            return this->getParentVector<ObjectType, ParentType>()(idx);
        }

        template <typename ObjectType>
        VknVectorIterator<ObjectType> getVectorSlice(uint32_t startIdx, uint32_t length)
        {
            VknVector<ObjectType> &vec = this->getVector<ObjectType>();
            if (startIdx + length > vec.getSize())
                throw std::out_of_range("Slice range exceeds vector size.");
            return vec.getSlice(startIdx, length);
        }

        template <typename ObjectType>
        uint32_t getVectorSize()
        {
            return this->getVector<ObjectType>().getSize();
        }

        template <typename VknObjectType, typename VkObjectType, typename VkParentType>
        VknObjectType &addNewVknObject(uint32_t idx, std::list<VknObjectType> &objList,
                                       VknIdxs &relIdxs, VknIdxs &absIdxs, VknInfos *infos)
        {
            if (idx != objList.size())
                throw std::runtime_error("List index out of range or incorrect.");
            VknIdxs newRelIdxs = relIdxs;
            VknIdxs newAbsIdxs = absIdxs;
            VkParentType *parent = &this->getObject<VkParentType>(absIdxs);
            newAbsIdxs.add<VkObjectType>(this->push_back<VkObjectType, VkParentType>(parent));
            newRelIdxs.add<VkObjectType>(objList.size());
            return objList.emplace_back(this, newRelIdxs, newAbsIdxs, infos);
        }

        VkInstance *addVkInstance(VknIdxs &relIdxs, VknIdxs &absIdxs)
        {
            VknVector<VkInstance> &vec{this->getVector<VkInstance>()};
            size_t pos = vec.getSize();
            if (pos != 0)
                throw std::runtime_error("VkInstance already added.");
            vec.append(VkInstance{});

            absIdxs.add<VkInstance>(pos);
            relIdxs.add<VkInstance>(pos);
            return &vec(pos);
        }

    private:
        std::unordered_map<std::string, void *> m_objectVectors{};
        std::unordered_map<std::string, void *> m_parentVectors{};
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

        template <typename ObjectType, typename ParentType>
        void deleteVectors()
        {
            std::string vkTypeStr = typeToStr<ObjectType>();
            if (m_objectVectors.find(vkTypeStr) != m_objectVectors.end())
                delete static_cast<VknVector<ObjectType> *>(m_objectVectors[vkTypeStr]);
            if (m_parentVectors.find(vkTypeStr) != m_parentVectors.end())
                delete static_cast<VknVector<ParentType *> *>(m_parentVectors[vkTypeStr]);
        }

        template <typename ObjectType>
        void deleteVector()
        {
            std::string vkTypeStr = typeToStr<ObjectType>();
            if (m_objectVectors.find(vkTypeStr) != m_objectVectors.end())
                delete static_cast<VknVector<ObjectType> *>(m_objectVectors[vkTypeStr]);
        }
    }; // VknEngine

} // namespace vkn