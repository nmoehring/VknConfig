/**
 * @file VknInfos.hpp
 * @brief Provides methods for filing Vulkan info structures.
 *
 * VknInfos is a free/top-level class within the VknConfig project.
 * It is responsible for filing various Vulkan info structures, such as
 * VkApplicationInfo, VkInstanceCreateInfo, VkDeviceCreateInfo, etc.
 * VknInfos does not depend on any other Vkn classes.
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
 * [VknInfos] (Free/Top-Level) <<=== YOU ARE HERE
 * [VknResult] (Free/Top-Level)
 */

#pragma once
#include <string>

#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <vector>

#include <vulkan/vulkan.h>
#include "VknEngine.hpp"
#include "VknResult.hpp"
#include "VknInfos.hpp"
#include "VknFeatures.hpp"
#include "VknData.hpp"

namespace vkn
{
    enum VknAttachmentType : uint32_t
    {
        COLOR_ATTACHMENT = 0,
        DEPTH_STENCIL_ATTACHMENT = 1,
        RESOLVE_ATTACHMENT = 2,
        INPUT_ATTACHMENT = 3,
        NUM_ATTACHMENT_TYPES,
        PRESERVE_ATTACHMENT
    };

    class VknInfos
    {
    public:
        enum checkfileFunctions
        {
            APP_INFO = 0,
            DEVICE_QUEUE_CREATE_INFO = 1,
            DEVICE_CREATE_INFO = 2,
            INSTANCE_CREATE_INFO = 3
        };

        VknInfos();
        //~VknInfos();

        // getters
        VkApplicationInfo *getAppInfo()
        {
            if (this->checkfile(APP_INFO))
                return &m_appInfo;
            else
                return nullptr;
        };
        VkInstanceCreateInfo *getInstanceCreateInfo() { return &m_instanceCreateInfo; };
        VkDeviceQueueCreateInfo *getDeviceQueueCreateInfo(uint32_t deviceIdx, uint32_t queueFamilyIdx)
        {
            if (!(this->checkfile(DEVICE_QUEUE_CREATE_INFO)))
                throw std::runtime_error("DeviceQueueCreateInfo not filed before get.");
            return &m_queueCreateInfos[deviceIdx](queueFamilyIdx);
        };

        VkRenderPassCreateInfo *getRenderpassCreateInfo(VknIdxs &relIdxs)
        {
            return &m_renderpassCreateInfos[relIdxs.get<VkDevice>()](relIdxs.get<VkRenderPass>());
        }

        VkDeviceCreateInfo *getDeviceCreateInfo(uint32_t deviceIdx)
        {
            if (!(this->checkfile(DEVICE_CREATE_INFO)))
                throw std::runtime_error("DeviceCreateInfo not filed before get.");
            return &m_deviceCreateInfos(deviceIdx);
        };
        VknSpace<VkGraphicsPipelineCreateInfo> *getPipelineCreateInfos(VknIdxs &relIdxs)
        {
            return &m_gfxPipelineCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
        }
        VknSpace<VkPipelineShaderStageCreateInfo> *getShaderStageCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_shaderStageCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
        }
        VkPipelineVertexInputStateCreateInfo *getVertexInputStateCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_vertexInputStateCreateInfos[deviceIdx][renderpassIdx](subpassIdx);
        }
        VkPipelineInputAssemblyStateCreateInfo *getInputAssemblyStateCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_inputAssemblyStateCreateInfos[deviceIdx][renderpassIdx](subpassIdx);
        }
        VkPipelineRasterizationStateCreateInfo *getRasterizationStateCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_rasterizationStateCreateInfos[deviceIdx][renderpassIdx](subpassIdx);
        }
        VkPipelineMultisampleStateCreateInfo *getMultisampleStateCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_multisampleStateCreateInfos[deviceIdx][renderpassIdx](subpassIdx);
        }
        VkPipelineLayoutCreateInfo *getPipelineLayoutCreateInfo(
            VknIdxs &relIdxs)
        {
            return &m_layoutCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>());
        }
        VkSwapchainCreateInfoKHR *getSwapchainCreateInfo(VknIdxs &relIdxs);
        uint32_t getNumDeviceQueueFamilies(uint32_t deviceIdx)
        {
            return m_numQueueFamilies(deviceIdx);
        }
        void setNumDeviceQueueFamilies(int num, uint32_t deviceIdx);
        void fileDeviceQueuePriorities(
            VknIdxs &relIdxs, uint32_t queueFamilyIdx, VknVector<float> priorities);

        // ============file DEVICE INIT INFOS===============
        bool checkfile(checkfileFunctions functionName);

        void fileAppName(std::string name);
        void fileEngineName(std::string name);
        void fileInstanceExtensionNames(const char *const *names, uint32_t size);
        void fileEnabledLayerNames(const char *const *names, uint32_t size);
        void fileAppInfo(uint32_t apiVersion, uint32_t applicationVersion, uint32_t engineVersion);
        void fileInstanceCreateInfo(VkInstanceCreateFlags flags);
        void fileDeviceExtensionNames(uint32_t deviceIdx, const char *const *names, uint32_t size);
        void fileDeviceFeatures(VknFeatures features);
        void fileDeviceQueueCreateInfo(VknIdxs &relIdxs, uint32_t queueFamilyIdx,
                                       uint32_t queueCount, VkDeviceQueueCreateFlags flags);
        VkDeviceCreateInfo *fileDeviceCreateInfo(uint32_t deviceIdx);

        VkSwapchainCreateInfoKHR *fileSwapchainCreateInfo(
            VknIdxs &relIdxs, VkSurfaceKHR *surface, uint32_t imageCount, VkExtent2D dimensions,
            VkSurfaceFormatKHR surfaceFormat,
            uint32_t numImageArrayLayers, VkImageUsageFlags usage,
            VkSharingMode sharingMode,
            VkSurfaceTransformFlagBitsKHR preTransform,
            VkCompositeAlphaFlagBitsKHR compositeAlpha,
            VkPresentModeKHR presentMode, VkBool32 clipped,
            VkSwapchainKHR oldSwapchain);

        //================file PIPELINE INFOS===================
        VkShaderModuleCreateInfo *fileShaderModuleCreateInfo(
            VknIdxs &relIdxs, std::vector<char> *code);
        VkPipelineShaderStageCreateInfo *fileShaderStageCreateInfo(
            VknIdxs &relIdxs, VkShaderModule *module, VkShaderStageFlagBits *stage,
            VkPipelineShaderStageCreateFlags *flags,
            VkSpecializationInfo *pSpecializationInfo);
        VkPipelineVertexInputStateCreateInfo *fileVertexInputStateCreateInfo(
            VknIdxs &relIdxs, uint32_t numBindings, uint32_t numAttributes);
        VkPipelineInputAssemblyStateCreateInfo *fileInputAssemblyStateCreateInfo(
            VknIdxs &relIdxs, VkPrimitiveTopology topology,
            VkBool32 primitiveRestartEnable);
        VkPipelineTessellationStateCreateInfo *fileTessellationStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx, uint32_t patchControlPoints);
        VkPipelineViewportStateCreateInfo *fileViewportStateCreateInfo(
            VknIdxs &relIdxs,
            VknVector<VkViewport> *viewports, VknVector<VkRect2D> *scissors);
        VkPipelineRasterizationStateCreateInfo *fileRasterizationStateCreateInfo(
            VknIdxs &relIdxs,
            VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace,
            float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor,
            float lineWidth, VkBool32 depthClampEnable,
            VkBool32 rasterizerDiscardEnable, VkBool32 depthBiasEnable);
        VkPipelineMultisampleStateCreateInfo *fileMultisampleStateCreateInfo(
            VknIdxs &relIdxs,
            float minSampleShading, VkSampleMask *pSampleMask,
            VkSampleCountFlagBits rasterizationSamples,
            VkBool32 sampleShadingEnable, VkBool32 alphaToCoverageEnable,
            VkBool32 alphaToOneEnable);
        VkPipelineDepthStencilStateCreateInfo *fileDepthStencilStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
            VkCompareOp depthCompareOp, VkStencilOpState front, VkStencilOpState back,
            float minDepthBounds, float maxDepthBounds,
            VkPipelineDepthStencilStateCreateFlags flags,
            VkBool32 depthTestEnable,
            VkBool32 depthWriteEnable,
            VkBool32 depthBoundsTestEnable, VkBool32 stencilTestEnable);
        VkPipelineColorBlendStateCreateInfo *fileColorBlendStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
            VkLogicOp logicOp, VknVector<VkPipelineColorBlendAttachmentState> &attachments, // Pass by reference
            float blendConstants[4], VkBool32 logicOpEnable,
            VkPipelineColorBlendStateCreateFlags flags);
        VkPipelineDynamicStateCreateInfo *fileDynamicStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
            VknVector<VkDynamicState> &dynamicStates);
        VkGraphicsPipelineCreateInfo *fileGfxPipelineCreateInfo(
            VknIdxs &relIdxs, VkRenderPass &renderpass,
            VkPipelineLayout *layout,
            VkPipeline basePipelineHandle, int32_t basePipelineIndex,
            VkPipelineCreateFlags flags);
        VkPipelineLayoutCreateInfo *filePipelineLayoutCreateInfo(
            VknIdxs &relIdxs,
            VknVectorIterator<VkDescriptorSetLayout> setLayouts,
            VknVector<VkPushConstantRange> pushConstantRanges,
            VkPipelineLayoutCreateFlags flags);
        VkPipelineCacheCreateInfo *filePipelineCacheCreateInfo(
            size_t initialDataSize,
            const void *pInitialData,
            VkPipelineCacheCreateFlags flags);
        VkRenderPassCreateInfo *fileRenderpassCreateInfo(
            VknIdxs &relIdxs, VkRenderPassCreateFlags flags);
        VkAttachmentDescription *fileAttachmentDescription(
            VknIdxs &relIdxs, uint32_t attachIdx,
            VkFormat format,
            VkSampleCountFlagBits samples,
            VkAttachmentLoadOp loadOp,
            VkAttachmentStoreOp storeOp,
            VkAttachmentLoadOp stencilLoadOp,
            VkAttachmentStoreOp stencilStoreOp,
            VkImageLayout initialLayout,
            VkImageLayout finalLayout,
            VkAttachmentDescriptionFlags flags);
        void fileAttachmentReference(
            VknIdxs &relIdxs, uint32_t subpassIdx,
            uint32_t refIdx, VknAttachmentType attachmentType, uint32_t attachmentIdx,
            VkImageLayout layout);
        VknSpace<VkAttachmentReference> *getRenderpassAttachmentReferences(
            VknIdxs &relIdxs);
        VknSpace<VkAttachmentDescription> *getRenderpassAttachmentDescriptions(VknIdxs &relIdxs);
        VknSpace<VkAttachmentReference> *getSubpassAttachmentReferences(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        VkImageCreateInfo *fileImageCreateInfo(VknIdxs &relId,
                                               VkImageType imageType,
                                               VkFormat format,
                                               VkExtent3D extent,
                                               uint32_t mipLevels,
                                               uint32_t arrayLayers,
                                               VkSampleCountFlagBits samples,
                                               VkImageTiling tiling,
                                               VkImageUsageFlags usage,
                                               VkImageLayout initialLayout,
                                               VkImageCreateFlags flags);
        VknSpace<uint32_t> *getSubpassPreserveAttachments(uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        VknVector<VknVector<VknVector<uint32_t>>> *getDevicePreserveAttachments(uint32_t deviceIdx);
        VknSpace<uint32_t> *getRenderpassPreserveAttachments(uint32_t deviceIdx, uint32_t renderpassIdx);
        VkSubpassDescription *fileSubpassDescription(
            VknIdxs &relIdxs, uint32_t subpassIdx,
            VkPipelineBindPoint pipelineBindPoint,
            VkSubpassDescriptionFlags flags);
        VkSubpassDependency *fileSubpassDependency(
            VknIdxs &relIdxs,
            uint32_t srcSubpass, uint32_t dstSubpass,
            VkPipelineStageFlags srcStageMask,
            VkAccessFlags srcAccessMask,
            VkPipelineStageFlags dstStageMask,
            VkAccessFlags dstAccessMask);
        VkDescriptorSetLayoutCreateInfo *fileDescriptorSetLayoutCreateInfo(
            VknVector<VkDescriptorSetLayoutBinding> bindings,
            VkDescriptorSetLayoutCreateFlags flags);
        VkVertexInputBindingDescription *fileVertexInputBindingDescription(
            VknIdxs &relIdxs,
            uint32_t bindIdx, uint32_t binding, uint32_t stride,
            VkVertexInputRate inputRate);
        VkVertexInputAttributeDescription *fileVertexInputAttributeDescription(
            VknIdxs &relIdxs, uint32_t attributeIdx,
            uint32_t binding, uint32_t location, VkFormat format,
            uint32_t offset);

        VknSpace<VkVertexInputBindingDescription> *getVertexInputBindings(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        VknSpace<VkVertexInputAttributeDescription> *getVertexInputAttributes(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        VkImageViewCreateInfo *getImageViewCreateInfo(VknIdxs &relIdxs);
        VkFramebufferCreateInfo *getFramebufferCreateInfo(VknIdxs &relIdxs);

        VkFramebufferCreateInfo *fileFramebufferCreateInfo(VknIdxs &relIdxs, VkRenderPass *renderpass,
                                                           VknVectorIterator<VkImageView> attachments, uint32_t width,
                                                           uint32_t height, uint32_t numLayers, VkFramebufferCreateFlags &flags);
        VkImageViewCreateInfo *fileImageViewCreateInfo(VknIdxs &relIdxs, VkImage &image,
                                                       VkImageViewType &viewType,
                                                       VkFormat &format, VkComponentMapping &components,
                                                       VkImageSubresourceRange &subresourceRange, VkImageViewCreateFlags &flags);

        VkMemoryAllocateInfo *fileMemoryAllocateInfo(VknIdxs &relIdxs, VkDeviceSize allocationSize, uint32_t memoryTypeIndex);
        VkBufferCreateInfo *fileBufferCreateInfo(VknIdxs &relIdxs, VkDeviceSize size, VkBufferUsageFlags usage,
                                                 VkSharingMode sharingMode, uint32_t queueFamilyIndexCount, const uint32_t *pQueueFamilyIndices,
                                                 VkBufferCreateFlags flags);
        VmaAllocationInfo *fileVmaAllocationInfo(VknIdxs &relIdxs);

        void addInstanceExtension(std::string name);
        void addLayer(std::string name);
        void addDeviceExtension(std::string name, VknIdxs relIdxs);
        void storeName(std::string &name, std::list<std::string> &store, VknVector<const char *> &pointers);

        VkDebugUtilsMessengerCreateInfoEXT &setDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT createInfo)
        {
            m_debugMessengerCreateInfo = createInfo;
            return m_debugMessengerCreateInfo;
        }
        void removeFramebufferCreateInfo(VknIdxs relIdxs);
        void removeImageViewCreateInfo(VknIdxs relIdxs);
        void removeImageCreateInfo(VknIdxs relIdxs);
        void removePipelineCreateInfo(VknIdxs relIdxs);
        void removeViewportStateCreateInfo(VknIdxs relIdxs);
        VknFeatures *getDeviceFeaturesObject();
        void fileDeviceFeaturesInfo();

    private:
        std::string m_appName{};
        std::string m_engineName{};
        std::list<std::string> m_instanceExtensions_Store{};
        VknVector<const char *> m_instanceExtensions_NamePointers{};
        std::list<std::string> m_deviceExtensions_Store{1};
        VknSpace<const char *> m_deviceExtensions_NamePointers{1u};
        std::list<std::string> m_layers_Store{};
        VknVector<const char *> m_layers_NamePointers{};
        VknFeatures m_enabledFeatures{};
        VkPhysicalDeviceFeatures2 m_featuresInfo{};
        VknSpace<float> m_queuePriorities{2u};    // Device>QueueFamily>Queue#QueuePriority
        VknVector<uint32_t> m_numQueueFamilies{}; // Device#NumQueueFamilies
        VkDebugUtilsMessengerCreateInfoEXT m_debugMessengerCreateInfo{};

        // Info's
        VkApplicationInfo m_appInfo{};
        VkInstanceCreateInfo m_instanceCreateInfo{};
        VknSpace<VkDeviceQueueCreateInfo> m_queueCreateInfos{1u}; // Device>QueueFamily#Infos
        VknVector<VkDeviceCreateInfo> m_deviceCreateInfos{};      // Device#Info

        VknSpace<VkPipelineLayoutCreateInfo> m_layoutCreateInfos{2u};                         // Device>Renderpass>Subpass#info
        VknSpace<VkPipelineCacheCreateInfo> m_cacheCreateInfos{2u};                           // Device>Renderpass>Subpass#info
        VknSpace<VkShaderModuleCreateInfo> m_shaderModuleCreateInfos{3u};                     // Device>Renderpass>Subpass>Shader#info
        VknSpace<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos{3u};               // Device>Renderpass>Subpass>Shader#info
        VknSpace<VkPipelineVertexInputStateCreateInfo> m_vertexInputStateCreateInfos{2u};     // Device>Renderpass>Subpass#info
        VknSpace<VkPipelineInputAssemblyStateCreateInfo> m_inputAssemblyStateCreateInfos{2u}; // Device>Renderpass>Subpass#info
        VknSpace<VkPipelineTessellationStateCreateInfo> m_tessellationStateCreateInfos{2u};   // Device>Renderpass>Subpass#info
        VknSpace<VkPipelineViewportStateCreateInfo> m_viewportStateCreateInfos{2u};           // Device>Renderpass>Subpass#info
        VknSpace<VkPipelineRasterizationStateCreateInfo> m_rasterizationStateCreateInfos{2u}; // Device>Renderpass>Subpass#info
        VknSpace<VkPipelineMultisampleStateCreateInfo> m_multisampleStateCreateInfos{2u};     // Device>Renderpass>Subpass#info
        VknSpace<VkPipelineDepthStencilStateCreateInfo> m_depthStencilStateCreateInfos{2u};   // Device>Renderpass>Subpass#info
        VknSpace<VkPipelineColorBlendStateCreateInfo> m_colorBlendStateCreateInfos{2u};       // Device>Renderpass>Subpass#info
        VknSpace<VkPipelineDynamicStateCreateInfo> m_dynamicStateCreateInfos{2u};             // Device>Renderpass>Subpass#info
        VknSpace<VkGraphicsPipelineCreateInfo> m_gfxPipelineCreateInfos{2u};                  // Device>Renderpass>Subpass#info
        VknSpace<VkSwapchainCreateInfoKHR> m_swapchainCreateInfos{1u};                        // Device>Swapchain#info

        VknSpace<VkRenderPassCreateInfo> m_renderpassCreateInfos{1u};                   // Device>Renderpass#info (multi, some per device)
        VknSpace<VkAttachmentDescription> m_attachmentDescriptions{2u};                 // Device>Renderpass>Attachment#description
        VknSpace<VkAttachmentReference> m_attachmentReferences{4u};                     // Device>Renderpass>Subpass>AttachmentType>Attachment#ref
        VknSpace<uint32_t> m_preserveAttachments{3u};                                   // Device>Renderpass>Subpass>Attachment#ref
        VknSpace<VkSubpassDescription> m_subpassDescriptions{2u};                       // Device>Renderpass>Subpass#info
        VknSpace<VkSubpassDependency> m_subpassDependencies{2u};                        // Device>Renderpass>Dependency#description
        VknSpace<VkDescriptorSetLayoutCreateInfo> m_descriptorSetLayoutCreateInfos{3u}; // Device>Renderpass>PipelineLayout>DescriptorSetlayout#info

        VknSpace<VkVertexInputBindingDescription> m_vertexInputBindings{3u};     // Device>Renderpass>Subpass>InputBiding#Infos
        VknSpace<VkVertexInputAttributeDescription> m_vertexInputAttributes{3u}; // Device>Renderpass>Subpass>InputAttribute#Infos

        VknSpace<VkFramebufferCreateInfo> m_framebufferCreateInfos{2u}; // Device>Renderpass>Framebuffer#CreateInfo
        VknVector<VkImageViewCreateInfo> m_imageViewCreateInfos{};      // >ImageViews
        VknVector<VkImageCreateInfo> m_imageCreateInfos{};              //>Image

        VknSpace<VkMemoryAllocateInfo> m_deviceMemoryAllocateInfos{1u}; // Device>DeviceMemory#AllocateInfo
        VknSpace<VkBufferCreateInfo> m_bufferCreateInfos{1u};           // Device>Buffer#CreateInfo
        VknSpace<VmaAllocation> m_vmaAllocationInfos{1u};               // Device>VmaAllocation#Info

        const char m_mainEntry[5] = "main";

        // Required file checklist
        bool m_filedAppInfo{false};
        bool m_filedInstanceCreateInfo{false};
        bool m_filedDeviceQueueCreateInfo{false};
        bool m_filedDeviceCreateInfo{false};
        bool m_filedAppName{false};
        bool m_filedEngineName{false};
        bool m_filedLayerNames{false};
        bool m_filedInstanceExtensionNames{false};
        bool m_filedDeviceExtensionNames{false};
        bool m_filedFeaturesInfo{false};

        bool m_filedVertexInputStateInfo{false};
        bool m_filedInputAssemblyStateInfo{false};
        bool m_filedTessellationStateInfo{false};
        bool m_filedViewportStateInfo{false};
        bool m_filedRasterizationStateInfo{false};
        bool m_filedMultisampleStateInfo{false};
        bool m_filedDepthStencilStateInfo{false};
        bool m_filedColorBlendStateInfo{false};
        bool m_filedDynamicStateInfo{false};
        bool m_deviceQueuePrioritiesfiled{false};
    };
}
