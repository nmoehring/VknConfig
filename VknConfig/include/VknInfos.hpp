/**
 * @file VknInfos.hpp
 * @brief Provides methods for filling Vulkan info structures.
 *
 * VknInfos is a free/top-level class within the VknConfig project.
 * It is responsible for filling various Vulkan info structures, such as
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
        enum checkFillFunctions
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
            if (this->checkFill(APP_INFO))
                return &m_appInfo;
            else
                return nullptr;
        };
        VkInstanceCreateInfo *getInstanceCreateInfo() { return &m_instanceCreateInfo; };
        VkDeviceQueueCreateInfo *getDeviceQueueCreateInfo(uint32_t deviceIdx, uint32_t queueFamilyIdx)
        {
            if (!(this->checkFill(DEVICE_QUEUE_CREATE_INFO)))
                throw std::runtime_error("DeviceQueueCreateInfo not filled before get.");
            return &m_queueCreateInfos[deviceIdx](queueFamilyIdx);
        };

        VkRenderPassCreateInfo *getRenderpassCreateInfo(VknIdxs &relIdxs)
        {
            return &m_renderpassCreateInfos[relIdxs.get<VkDevice>()](relIdxs.get<VkRenderPass>());
        }

        VkDeviceCreateInfo *getDeviceCreateInfo(uint32_t deviceIdx)
        {
            if (!(this->checkFill(DEVICE_CREATE_INFO)))
                throw std::runtime_error("DeviceCreateInfo not filled before get.");
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
        void fillDeviceQueuePriorities(
            VknIdxs &relIdxs, uint32_t queueFamilyIdx, VknVector<float> priorities);

        // ============FILL DEVICE INIT INFOS===============
        bool checkFill(checkFillFunctions functionName);

        void fillAppName(std::string name);
        void fillEngineName(std::string name);
        void fillInstanceExtensionNames(const char *const *names, uint32_t size);
        void fillEnabledLayerNames(const char *const *names, uint32_t size);
        void fillAppInfo(uint32_t apiVersion, uint32_t applicationVersion, uint32_t engineVersion);
        void fillInstanceCreateInfo(VkInstanceCreateFlags flags);
        void fillDeviceExtensionNames(uint32_t deviceIdx, const char *const *names, uint32_t size);
        void fillDeviceFeatures(VknFeatures features);
        void fillDeviceQueueCreateInfo(VknIdxs &relIdxs, uint32_t queueFamilyIdx,
                                       uint32_t queueCount, VkDeviceQueueCreateFlags flags);
        VkDeviceCreateInfo *fillDeviceCreateInfo(uint32_t deviceIdx);

        VkSwapchainCreateInfoKHR *fillSwapchainCreateInfo(
            VknIdxs &relIdxs, VkSurfaceKHR *surface, uint32_t imageCount, VkExtent2D dimensions,
            VkSurfaceFormatKHR surfaceFormat,
            uint32_t numImageArrayLayers, VkImageUsageFlags usage,
            VkSharingMode sharingMode,
            VkSurfaceTransformFlagBitsKHR preTransform,
            VkCompositeAlphaFlagBitsKHR compositeAlpha,
            VkPresentModeKHR presentMode, VkBool32 clipped,
            VkSwapchainKHR oldSwapchain);

        //================FILL PIPELINE INFOS===================
        VkShaderModuleCreateInfo *fillShaderModuleCreateInfo(
            VknIdxs &relIdxs, VknVector<char> *code);
        VkPipelineShaderStageCreateInfo *fillShaderStageCreateInfo(
            VknIdxs &relIdxs, VkShaderModule *module, VkShaderStageFlagBits *stage,
            VkPipelineShaderStageCreateFlags *flags,
            VkSpecializationInfo *pSpecializationInfo);
        VkPipelineVertexInputStateCreateInfo *fillVertexInputStateCreateInfo(
            VknIdxs &relIdxs, uint32_t numBindings, uint32_t numAttributes);
        VkPipelineInputAssemblyStateCreateInfo *fillInputAssemblyStateCreateInfo(
            VknIdxs &relIdxs, VkPrimitiveTopology topology,
            VkBool32 primitiveRestartEnable);
        VkPipelineTessellationStateCreateInfo *fillTessellationStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx, uint32_t patchControlPoints);
        VkPipelineViewportStateCreateInfo *fillViewportStateCreateInfo(
            VknIdxs &relIdxs,
            VknVector<VkViewport> *viewports, VknVector<VkRect2D> *scissors);
        VkPipelineRasterizationStateCreateInfo *fillRasterizationStateCreateInfo(
            VknIdxs &relIdxs,
            VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace,
            float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor,
            float lineWidth, VkBool32 depthClampEnable,
            VkBool32 rasterizerDiscardEnable, VkBool32 depthBiasEnable);
        VkPipelineMultisampleStateCreateInfo *fillMultisampleStateCreateInfo(
            VknIdxs &relIdxs,
            float minSampleShading, VkSampleMask *pSampleMask,
            VkSampleCountFlagBits rasterizationSamples,
            VkBool32 sampleShadingEnable, VkBool32 alphaToCoverageEnable,
            VkBool32 alphaToOneEnable);
        VkPipelineDepthStencilStateCreateInfo *fillDepthStencilStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
            VkCompareOp depthCompareOp, VkStencilOpState front, VkStencilOpState back,
            float minDepthBounds, float maxDepthBounds,
            VkPipelineDepthStencilStateCreateFlags flags,
            VkBool32 depthTestEnable,
            VkBool32 depthWriteEnable,
            VkBool32 depthBoundsTestEnable, VkBool32 stencilTestEnable);
        VkPipelineColorBlendStateCreateInfo *fillColorBlendStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
            VkLogicOp logicOp, VknVector<VkPipelineColorBlendAttachmentState> attachments,
            float blendConstants[4], VkBool32 logicOpEnable,
            VkPipelineColorBlendStateCreateFlags flags);
        VkPipelineDynamicStateCreateInfo *fillDynamicStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
            VknVector<VkDynamicState> dynamicStates);
        VkGraphicsPipelineCreateInfo *fillGfxPipelineCreateInfo(
            VknIdxs &relIdxs, VkRenderPass &renderpass,
            VkPipelineLayout *layout,
            VkPipeline basePipelineHandle, int32_t basePipelineIndex,
            VkPipelineCreateFlags flags);
        VkPipelineLayoutCreateInfo *fillPipelineLayoutCreateInfo(
            VknIdxs &relIdxs,
            VknVectorIterator<VkDescriptorSetLayout> setLayouts,
            VknVector<VkPushConstantRange> pushConstantRanges,
            VkPipelineLayoutCreateFlags flags);
        VkPipelineCacheCreateInfo *fillPipelineCacheCreateInfo(
            size_t initialDataSize,
            const void *pInitialData,
            VkPipelineCacheCreateFlags flags);
        VkRenderPassCreateInfo *fillRenderpassCreateInfo(
            VknIdxs &relIdxs, VkRenderPassCreateFlags flags);
        VkAttachmentDescription *fillAttachmentDescription(
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
        void fillAttachmentReference(
            VknIdxs &relIdxs, uint32_t subpassIdx,
            uint32_t refIdx, VknAttachmentType attachmentType, uint32_t attachmentIdx,
            VkImageLayout layout);
        VknSpace<VkAttachmentReference> *getRenderpassAttachmentReferences(
            VknIdxs &relIdxs);
        VknSpace<VkAttachmentDescription> *getRenderpassAttachmentDescriptions(VknIdxs &relIdxs);
        VknSpace<VkAttachmentReference> *getSubpassAttachmentReferences(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        VkImageCreateInfo *fillImageCreateInfo(VknIdxs &relId,
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
        VkSubpassDescription *fillSubpassDescription(
            VknIdxs &relIdxs, uint32_t subpassIdx,
            VkPipelineBindPoint pipelineBindPoint,
            VkSubpassDescriptionFlags flags);
        VkSubpassDependency *fillSubpassDependency(
            VknIdxs &relIdxs,
            uint32_t srcSubpass, uint32_t dstSubpass,
            VkPipelineStageFlags srcStageMask,
            VkAccessFlags srcAccessMask,
            VkPipelineStageFlags dstStageMask,
            VkAccessFlags dstAccessMask);
        VkDescriptorSetLayoutCreateInfo *fillDescriptorSetLayoutCreateInfo(
            VknVector<VkDescriptorSetLayoutBinding> bindings,
            VkDescriptorSetLayoutCreateFlags flags);
        VkVertexInputBindingDescription *fillVertexInputBindingDescription(
            VknIdxs &relIdxs,
            uint32_t bindIdx, uint32_t binding, uint32_t stride,
            VkVertexInputRate inputRate);
        VkVertexInputAttributeDescription *fillVertexInputAttributeDescription(
            VknIdxs &relIdxs, uint32_t attributeIdx,
            uint32_t binding, uint32_t location, VkFormat format,
            uint32_t offset);

        VknSpace<VkVertexInputBindingDescription> *getVertexInputBindings(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        VknSpace<VkVertexInputAttributeDescription> *getVertexInputAttributes(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        VkImageViewCreateInfo *getImageViewCreateInfo(VknIdxs &relIdxs);
        VkFramebufferCreateInfo *getFramebufferCreateInfo(VknIdxs &relIdxs);
        VkSwapchainCreateInfoKHR *getSwapchainCreateInfoKHR(uint32_t swapchainIdx)
        {
            return &m_swapchainCreateInfos(swapchainIdx);
        }

        VkFramebufferCreateInfo *fillFramebufferCreateInfo(VknIdxs &relIdxs, VkRenderPass *renderpass,
                                                           VknVectorIterator<VkImageView> attachments, uint32_t width,
                                                           uint32_t height, uint32_t numLayers, VkFramebufferCreateFlags &flags);
        VkImageViewCreateInfo *fillImageViewCreateInfo(VknIdxs &relIdxs, VkImage &image,
                                                       VkImageViewType &viewType,
                                                       VkFormat &format, VkComponentMapping &components,
                                                       VkImageSubresourceRange &subresourceRange, VkImageViewCreateFlags &flags);

    private:
        std::string m_appName{};
        std::string m_engineName{};
        const char *const *m_enabledInstanceExtensionNames;
        uint32_t m_enabledInstanceExtensionNamesSize{0};
        const char *const *m_enabledLayerNames;
        uint32_t m_enabledLayerNamesSize{0};
        VknVector<VkPhysicalDeviceFeatures> m_enabledFeatures{};
        VknVector<const char *const *> m_enabledDeviceExtensionNames{}; // Device>char_arr
        VknVector<uint32_t> m_enabledDeviceExtensionNamesSize{};
        VknSpace<float> m_queuePriorities{2u};    // Device>QueueFamily>Queue#QueuePriority
        VknVector<uint32_t> m_numQueueFamilies{}; // Device#NumQueueFamilies

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
        VknVector<VkSwapchainCreateInfoKHR> m_swapchainCreateInfos{};                         // >info (as many as needed, e.g. multiple windows)

        VknSpace<VkRenderPassCreateInfo> m_renderpassCreateInfos{1u};                   // Device>Renderpass#info (multi, some per device)
        VknSpace<VkAttachmentDescription> m_attachmentDescriptions{2u};                 // Device>Renderpass>Attachment#description
        VknSpace<VkAttachmentReference> m_attachmentReferences{4u};                     // Device>Renderpass>Subpass>AttachmentType>Attachment#ref
        VknSpace<uint32_t> m_preserveAttachments{3u};                                   // Device>Renderpass>Subpass>Attachment#ref
        VknSpace<VkSubpassDescription> m_subpassDescriptions{2u};                       // Device>Renderpass>Subpass#info
        VknSpace<VkSubpassDependency> m_subpassDependencies{3u};                        // Device>Renderpass>Subpass>Dependency#description
        VknSpace<VkDescriptorSetLayoutCreateInfo> m_descriptorSetLayoutCreateInfos{3u}; // Device>Renderpass>PipelineLayout>DescriptorSetlayout#info

        VknSpace<VkVertexInputBindingDescription> m_vertexInputBindings{3u};     // Device>Renderpass>Subpass>Infos
        VknSpace<VkVertexInputAttributeDescription> m_vertexInputAttributes{3u}; // Device>Renderpass>Subpass>Infos

        VknSpace<VkFramebufferCreateInfo> m_framebufferCreateInfos{2u}; // Device>Renderpass>Framebuffers
        VknVector<VkImageViewCreateInfo> m_imageViewCreateInfos{};      // >ImageViews
        VknVector<VkImageCreateInfo> m_imageCreateInfos{};              //>Image
        const char m_mainEntry[5] = "main";

        // Required fill checklist
        bool m_filledAppInfo{false};
        bool m_filledInstanceCreateInfo{false};
        bool m_filledDeviceQueueCreateInfo{false};
        bool m_filledDeviceCreateInfo{false};
        bool m_filledAppName{false};
        bool m_filledEngineName{false};
        bool m_filledLayerNames{false};
        bool m_filledInstanceExtensionNames{false};

        bool m_filledVertexInputStateInfo{false};
        bool m_filledInputAssemblyStateInfo{false};
        bool m_filledTessellationStateInfo{false};
        bool m_filledViewportStateInfo{false};
        bool m_filledRasterizationStateInfo{false};
        bool m_filledMultisampleStateInfo{false};
        bool m_filledDepthStencilStateInfo{false};
        bool m_filledColorBlendStateInfo{false};
        bool m_filledDynamicStateInfo{false};
        bool m_deviceQueuePrioritiesFilled{false};
    };
}
