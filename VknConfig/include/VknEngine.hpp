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

#include <functional>
#include <string>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include <unordered_map>
#include <optional>
#include <span> // For std::span
#include <stdexcept>
#include "VknData.hpp"

namespace vkn
{
    template <typename T>
    class VknInstanceLock
    {
        static inline T *target = nullptr;

    public:
        void operator=(T *newTarget) { target = newTarget; }
        void operator()(T *possibleTarget)
        {
            if (possibleTarget != target)
                throw std::runtime_error("Wrong instance locked! Can only edit the instance locked by VknInstanceLock.");
        }
    };

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
        else if constexpr (std::is_same_v<T, VkDeviceMemory>)
            return "deviceMemory";
        else if constexpr (std::is_same_v<T, VkDebugUtilsMessengerEXT>)
            return "dbgmsgr";
        else if constexpr (std::is_same_v<T, VkCommandPool>)
            return "CommandPool";
        else if constexpr (std::is_same_v<T, VkCommandBuffer *>)
            return "commandBuffer";
        else if constexpr (std::is_same_v<T, uint32_t>)
            return "NumCmdBuffers";
        else if constexpr (std::is_same_v<T, VkSemaphore>)
            return "semaphore";
        else if constexpr (std::is_same_v<T, VkFence>)
            return "fence";
        else if constexpr (std::is_same_v<T, VmaAllocator>)
            return "allocator";
        else if constexpr (std::is_same_v<T, VmaAllocation>)
            return "allocation";
        else if constexpr (std::is_same_v<T, VkBuffer>)
            return "buffer";
        else if constexpr (std::is_same_v<T, void>)
            return "VOID";
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

        template <typename T>
        bool exists()
        {
            std::string vkTypeStr = typeToStr<T>();
            return m_data.count(vkTypeStr);
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
            m_pos = vec.getSize();
            vec.appendOne(val);
            parentVec.appendOne(parent);
            return m_pos;
        }

        template <typename ObjectType, typename ParentType>
        uint32_t push_back(ParentType *parent)
        {
            ObjectType val{};
            return this->push_back<ObjectType, ParentType>(val, parent);
        }

        template <typename ObjectType>
        uint32_t push_back()
        {
            ObjectType val{};
            VknVector<ObjectType> &vec{this->getVector<ObjectType>()};
            m_pos = vec.getSize();
            vec.appendOne(val);
            return m_pos;
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
            m_vkTypeStr = typeToStr<ObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<ObjectType>{};
            return *static_cast<VknVector<ObjectType> *>(m_objectVectors[m_vkTypeStr]);
        }

        template <typename ObjectType, typename ParentType>
        VknVector<ParentType *> &getParentVector()
        {
            m_vkTypeStr = typeToStr<ObjectType>();
            if (m_parentVectors.find(m_vkTypeStr) == m_parentVectors.end())
                throw std::runtime_error("Parent vector not found!");
            return *static_cast<VknVector<ParentType *> *>(m_parentVectors[m_vkTypeStr]);
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
            return vec.getSlice(startIdx, length);
        }

        template <typename ObjectType>
        uint32_t getVectorSize()
        {
            m_vkTypeStr = typeToStr<ObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                return 0;
            return static_cast<VknVector<ObjectType> *>(m_objectVectors[m_vkTypeStr])->getNumPositions();
        }

        template <typename VknObjectType, typename VkObjectType, typename VkParentType>
        VknObjectType &addNewVknObject(uint32_t idx, std::list<VknObjectType> &objList,
                                       VknIdxs &relIdxs, VknIdxs &absIdxs)
        {
            if (idx != objList.size())
                throw std::runtime_error("List index out of range or incorrect.");
            m_vkTypeStr = typeToStr<VkObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<VkObjectType>{};
            if (m_parentVectors.find(m_vkTypeStr) == m_parentVectors.end())
                m_parentVectors[m_vkTypeStr] = new VknVector<VkParentType>{};

            VknIdxs newRelIdxs = relIdxs;
            VknIdxs newAbsIdxs = absIdxs;
            VkParentType *parent = &this->getObject<VkParentType>(absIdxs);
            newAbsIdxs.add<VkObjectType>(this->push_back<VkObjectType, VkParentType>(parent));
            newRelIdxs.add<VkObjectType>(objList.size());
            return objList.emplace_back(newRelIdxs, newAbsIdxs);
        }

        template <typename VknObjectType, typename VkObjectType>
        VknObjectType &addNewVknObject(uint32_t idx, std::list<VknObjectType> &objList,
                                       VknIdxs &relIdxs, VknIdxs &absIdxs)
        {
            if (idx != objList.size())
                throw std::runtime_error("List index out of range or incorrect.");
            m_vkTypeStr = typeToStr<VkObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<VkObjectType>{};

            VknIdxs newRelIdxs = relIdxs;
            VknIdxs newAbsIdxs = absIdxs;
            newAbsIdxs.add<VkObjectType>(this->push_back<VkObjectType>());
            newRelIdxs.add<VkObjectType>(objList.size());
            return objList.emplace_back(newRelIdxs, newAbsIdxs);
        }

        template <typename VknObjectType, typename VkObjectType, typename VkParentType>
        uint32_t addNewVknObjects(uint32_t count, std::list<VknObjectType> &objList,
                                  VknIdxs &relIdxs, VknIdxs &absIdxs)
        {
            m_vkTypeStr = typeToStr<VkObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<VkObjectType>{};
            if (m_parentVectors.find(m_vkTypeStr) == m_parentVectors.end())
                m_parentVectors[m_vkTypeStr] = new VknVector<VkParentType>{};

            VknVector<VkObjectType> &vkObjectVec = this->getVector<VkObjectType>();
            uint32_t startPos = vkObjectVec.getDefragPos(count);
            VknVector<VkParentType *> &vkParentVec = this->getParentVector<VkObjectType, VkParentType>();
            VknIdxs newRelIdxs = relIdxs;
            VknIdxs newAbsIdxs = absIdxs;
            for (m_iter = 0; m_iter < count; ++m_iter)
            {
                newAbsIdxs.add<VkObjectType>(startPos + m_iter);
                vkObjectVec.insert(startPos + m_iter, VkObjectType{});
                newRelIdxs.add<VkObjectType>(objList.size());
                vkParentVec.insert(startPos + m_iter, &this->getObject<VkParentType>(absIdxs));
                objList.emplace_back(newRelIdxs, newAbsIdxs);
            }
            return startPos;
        }

        template <typename VknObjectType, typename VkObjectType>
        uint32_t addNewVknObjects(uint32_t count, std::list<VknObjectType> &objList,
                                  VknIdxs &relIdxs, VknIdxs &absIdxs)
        {
            m_vkTypeStr = typeToStr<VkObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<VkObjectType>{};

            VknVector<VkObjectType> &vkObjectVec = this->getVector<VkObjectType>();
            uint32_t startPos = vkObjectVec.getDefragPos(count);
            VknIdxs newRelIdxs = relIdxs;
            VknIdxs newAbsIdxs = absIdxs;
            for (m_iter = 0; m_iter < count; ++m_iter)
            {
                newAbsIdxs.add<VkObjectType>(startPos + m_iter);
                vkObjectVec.insert(startPos + m_iter, VkObjectType{});
                newRelIdxs.add<VkObjectType>(objList.size());
                objList.emplace_back(newRelIdxs, newAbsIdxs);
            }
            return startPos;
        }

        template <typename VknObjectType, typename VkObjectType, typename VkParentType>
        void demolishVknObjects(uint32_t startPos, uint32_t count, std::list<VknObjectType> &objList)
        {
            VknVector<VkObjectType> &vkObjectVec = this->getVector<VkObjectType>();
            VknVector<VkParentType *> &vkParentVec = this->getParentVector<VkObjectType, VkParentType>();
            vkObjectVec.remove(startPos, count);
            vkParentVec.remove(startPos, count);
            for (m_iter = 0; m_iter < objList.size(); ++m_iter)
                getListElement(m_iter, objList)->demolish();
            objList.clear();
        }

        template <typename VkObjectType, typename VkParentType>
        VkObjectType &addNewObject(VknIdxs &absIdxs)
        {
            m_vkTypeStr = typeToStr<VkObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<VkObjectType>{};
            if (m_parentVectors.find(m_vkTypeStr) == m_parentVectors.end())
                m_parentVectors[m_vkTypeStr] = new VknVector<VkParentType>{};

            VkParentType *parent = &this->getObject<VkParentType>(absIdxs);
            absIdxs.add<VkObjectType>(this->push_back<VkObjectType, VkParentType>(parent));
            return this->getObject<VkObjectType>(absIdxs);
        }

        template <typename VkObjectType>
        VkObjectType &addNewObject(VknIdxs &absIdxs)
        {
            m_vkTypeStr = typeToStr<VkObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<VkObjectType>{};

            absIdxs.add<VkObjectType>(this->push_back<VkObjectType>());
            return this->getObject<VkObjectType>(absIdxs);
        }

        template <typename VkResourceType>
        VmaAllocation &addNewAllocation(VknIdxs &absIdxs)
        {
            m_vkTypeStr = typeToStr<VkResourceType>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<VkResourceType>{};
            if (m_parentVectors.find(m_vkTypeStr) == m_parentVectors.end())
                m_parentVectors[m_vkTypeStr] = new VknVector<VmaAllocator>{};
            if (m_allocations.find(m_vkTypeStr) == m_allocations.end())
                m_allocations[m_vkTypeStr] = new VknVector<VmaAllocation>{};

            VknVector<VmaAllocation> &allocationVec = *static_cast<VknVector<VmaAllocation> *>(m_allocations[m_vkTypeStr]);
            VknVector<VmaAllocator *> &allocatorVec = *static_cast<VknVector<VmaAllocator *> *>(m_parentVectors[m_vkTypeStr]);
            if (!absIdxs.exists<VmaAllocation>())
                absIdxs.add<VmaAllocation>(allocationVec.getDefragPos());
            allocationVec.insert(absIdxs.get<VmaAllocation>(), VmaAllocation{});
            allocatorVec.insert(absIdxs.get<VmaAllocation>(), &this->getObject<VmaAllocator>(absIdxs));
            return allocationVec(absIdxs.get<VmaAllocation>());
        }

        template <typename VkResourceType>
        VknVector<VmaAllocation> &getAllocationVector()
        {
            m_vkTypeStr = typeToStr<VkResourceType>();
            if (m_allocations.find(m_vkTypeStr) == m_allocations.end())
                throw std::runtime_error("Allocation vector not found!");

            return *static_cast<VknVector<VmaAllocation> *>(m_allocations[m_vkTypeStr]);
        }

        template <typename VkResourceType>
        VmaAllocation *getAllocation(uint_fast32_t pos)
        {
            return &(this->getAllocationVector<VkResourceType>()(pos));
        }

        template <typename VkResourceType>
        VmaAllocation *getAllocation(VknIdxs &absIdxs)
        {
            return &(this->getAllocationVector<VkResourceType>()(absIdxs.get<VmaAllocation>()));
        }

        VkInstance *addVkInstance(VknIdxs &relIdxs, VknIdxs &absIdxs)
        {
            m_vkTypeStr = typeToStr<VkInstance>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<VkInstance>{};

            VknVector<VkInstance> &vec{this->getVector<VkInstance>()};
            m_pos = vec.getSize();
            if (m_pos != 0)
                throw std::runtime_error("VkInstance already added.");
            vec.appendOne(VkInstance{VK_NULL_HANDLE});

            absIdxs.add<VkInstance>(m_pos);
            relIdxs.add<VkInstance>(m_pos);
            return &vec(m_pos);
        }

        VkCommandBuffer *addVkCommandBuffers(VknIdxs &absIdxs, uint32_t numCommandBuffers)
        {
            m_vkTypeStr = typeToStr<VkCommandBuffer *>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<VkCommandBuffer *>{};
            m_vkTypeStr = typeToStr<uint32_t>();
            if (m_objectVectors.find(m_vkTypeStr) == m_objectVectors.end())
                m_objectVectors[m_vkTypeStr] = new VknVector<uint32_t>{};

            absIdxs.add<VkCommandBuffer *>(this->getVectorSize<VkCommandBuffer *>());
            VknVector<VkCommandBuffer *> &cmdBufferVec{this->getVector<VkCommandBuffer *>()};
            VknVector<uint32_t> &numBuffersVec{this->getVector<uint32_t>()};
            uint32_t poolIdx{absIdxs.get<VkCommandPool>()};
            numBuffersVec.insert(poolIdx, numCommandBuffers);
            cmdBufferVec.insert(poolIdx, new VkCommandBuffer[numCommandBuffers]);
            for (m_iter = 0; m_iter < numCommandBuffers; ++m_iter)
                cmdBufferVec(poolIdx)[m_iter] = VK_NULL_HANDLE;
            return cmdBufferVec(poolIdx);
        }

        template <typename T>
        bool exists()
        {
            m_vkTypeStr = typeToStr<T>();
            return m_objectVectors.count(m_vkTypeStr);
        }

    private:
        std::unordered_map<std::string, void *> m_objectVectors{};
        std::unordered_map<std::string, void *> m_parentVectors{};
        std::unordered_map<std::string, void *> m_allocations{};
        void *m_emptyVec{new VknVector<size_t>()};

        // Allocate once, reuse
        uint_fast32_t m_iter{0};
        uint_fast32_t m_pos{0};
        std::string m_vkTypeStr{"LongStrNoReallocation!!"};
        std::string m_vkParentTypeStr{"LongStrNoReallocation!!"};

        // State
        bool m_poweredOn{true}; // State to track if shutdown has been called
        /*VknVector<VkPipelineCache> pipelineCaches{};*/

        // Helper functions to get extension function pointers
        void demolishDebugUtilsMessengerEXT(
            VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

        template <typename ObjectType, typename ParentType>
        void deleteVectors()
        {
            m_vkTypeStr = typeToStr<ObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) != m_objectVectors.end())
            {
                delete static_cast<VknVector<ObjectType> *>(m_objectVectors[m_vkTypeStr]);
                m_objectVectors.erase(m_vkTypeStr);
            }
            if (m_parentVectors.find(m_vkTypeStr) != m_parentVectors.end())
            {
                delete static_cast<VknVector<ParentType *> *>(m_parentVectors[m_vkTypeStr]);
                m_parentVectors.erase(m_vkTypeStr);
            }
        }

        template <typename VkObjectType>
        void demolishObject(std::function<void __stdcall(VkObjectType)> func)
        {
            if (this->exists<VkObjectType>())
            {
                for (m_iter = 0; m_iter < this->getVectorSize<VkObjectType>(); ++m_iter)
                    func(this->getObject<VkObjectType>(m_iter));
                this->deleteVector<VkObjectType>();
            }
        }

        template <typename VkObjectType, typename VkParentType>
        void demolishObjects(std::function<void __stdcall(VkParentType, VkObjectType, const VkAllocationCallbacks *)> func)
        {
            if (this->exists<VkObjectType>())
            {
                for (m_iter = 0; m_iter < this->getVectorSize<VkObjectType>(); ++m_iter)
                    func(
                        *this->getParentPointer<VkObjectType, VkParentType>(m_iter),
                        this->getObject<VkObjectType>(m_iter), nullptr);
                this->deleteVectors<VkObjectType, VkParentType>();
            }
        }

        template <typename VkObjectType, typename VkParentType>
        void demolishDebugObjects(std::function<void(VkParentType, VkObjectType, const VkAllocationCallbacks *)> func)
        {
            if (this->exists<VkObjectType>())
            {
                for (m_iter = 0; m_iter < this->getVectorSize<VkObjectType>(); ++m_iter)
                    func(
                        *this->getParentPointer<VkObjectType, VkParentType>(m_iter),
                        this->getObject<VkObjectType>(m_iter), nullptr);
                this->deleteVectors<VkObjectType, VkParentType>();
            }
        }

        template <typename VkObjectType>
        void demolishAllocations(std::function<void(VmaAllocator, VkObjectType, VmaAllocation)> func)
        {
            if (this->exists<VkObjectType>())
            {
                for (m_iter = 0; m_iter < this->getVectorSize<VkObjectType>(); ++m_iter)
                {
                    VmaAllocation *allocation = this->getAllocation<VkObjectType>(m_iter);
                    VmaAllocator *allocator = this->getParentPointer<VkObjectType, VmaAllocator>(m_iter);
                    VkObjectType &vkObject = this->getObject<VkObjectType>(m_iter);

                    func(*allocator, vkObject, *allocation);
                }

                this->deleteVectors<VkObjectType, VmaAllocator>();
                this->deleteAllocationVector<VkObjectType>();
            }
        }

        void demolishCommandBuffers()
        {
            if (this->exists<VkCommandBuffer *>())
            {
                for (m_iter = 0; m_iter < this->getVectorSize<VkCommandBuffer *>(); ++m_iter)
                    vkFreeCommandBuffers(
                        *this->getParentPointer<VkCommandPool, VkDevice>(m_iter),
                        this->getObject<VkCommandPool>(m_iter),
                        this->getObject<uint32_t>(m_iter),
                        this->getObject<VkCommandBuffer *>(m_iter));
                for (m_iter = 0; m_iter < this->getVectorSize<VkCommandBuffer *>(); ++m_iter)
                    delete[] this->getObject<VkCommandBuffer *>(m_iter);
                this->deleteVector<VkCommandBuffer *>();
            }
        }

        void demolishDevices()
        {
            if (this->exists<VkDevice>())
            {
                for (auto &device : this->getVector<VkDevice>())
                    vkDestroyDevice(device, VK_NULL_HANDLE);
                this->deleteVectors<VkDevice, VkInstance>();
            }
        }

        void demolishInstance()
        {
            vkDestroyInstance(this->getObject<VkInstance>(0), nullptr);
            this->deleteVector<VkInstance>();
        }

        void demolishAllocators()
        {
            if (this->exists<VmaAllocator>())
            {
                for (m_iter = 0; m_iter < this->getVectorSize<VmaAllocator>(); ++m_iter)
                    vmaDestroyAllocator(this->getObject<VmaAllocator>(m_iter));
                this->deleteVector<VmaAllocator>();
            }
        }

        template <typename ObjectType>
        void deleteVector()
        {
            m_vkTypeStr = typeToStr<ObjectType>();
            if (m_objectVectors.find(m_vkTypeStr) != m_objectVectors.end())
            {
                delete static_cast<VknVector<ObjectType> *>(m_objectVectors[m_vkTypeStr]);
                m_objectVectors.erase(m_vkTypeStr);
            }
        }

        template <typename ObjectType>
        void deleteAllocationVector()
        {
            m_vkTypeStr = typeToStr<ObjectType>();
            if (m_allocations.find(m_vkTypeStr) != m_allocations.end())
            {
                delete static_cast<VknVector<VmaAllocation> *>(m_allocations[m_vkTypeStr]);
                m_allocations.erase(m_vkTypeStr);
            }
        }
    }; // VknEngine

} // namespace vkn