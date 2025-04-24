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
#include "VknCommon.hpp"

namespace vkn
{
    enum VknAttachmentType : uint8_t
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
            std::span<VkDescriptorSetLayout> setLayouts,
            VknVector<VkPushConstantRange> pushConstantRanges,
            VkPipelineLayoutCreateFlags flags);
        VkPipelineCacheCreateInfo *fillPipelineCacheCreateInfo(
            size_t initialDataSize,
            const void *pInitialData,
            VkPipelineCacheCreateFlags flags);
        VkRenderPassCreateInfo *fillRenderpassCreateInfo(
            VknIdxs &relIdxs, uint32_t numAttachments,
            uint32_t numSubpasses, uint32_t numSubpassDeps, VkRenderPassCreateFlags flags);
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
            uint32_t attachIdx, VknAttachmentType attachmentType, uint32_t attachmentIdx,
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
            uint32_t numColor, uint32_t numInput, uint32_t numResolve, uint32_t numDepthStencil,
            uint32_t numPreserve, VknIdxs &relIdxs, uint32_t subpassIdx,
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

        VkFramebufferCreateInfo *fillFramebufferCreateInfo(VknIdxs &relIdxs, VkRenderPass *renderpass,
                                                           std::span<VkImageView> attachments, uint32_t width,
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
        VknSpace<float> m_queuePriorities{};      // Device>QueueFamily>QueuePriority
        VknVector<uint32_t> m_numQueueFamilies{}; // Device>NumFamilies

        // Info's
        VkApplicationInfo m_appInfo{};
        VkInstanceCreateInfo m_instanceCreateInfo{};
        VknSpace<VkDeviceQueueCreateInfo> m_queueCreateInfos{}; // Device>QueueFamilyInfos
        VknVector<VkDeviceCreateInfo> m_deviceCreateInfos{};    //>Infos

        VknSpace<VkPipelineLayoutCreateInfo> m_layoutCreateInfos{}; // Device>Renderpass>Subpass>infos
        VknSpace<VkPipelineCacheCreateInfo> m_cacheCreateInfos{};
        VknSpace<VkShaderModuleCreateInfo> m_shaderModuleCreateInfos{};                     // Device>Renderpass>Subpass>infos
        VknSpace<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos{};               // Device>Renderpass>Subpass>infos
        VknSpace<VkPipelineVertexInputStateCreateInfo> m_vertexInputStateCreateInfos{};     // Device>Renderpass>Subpass>infos
        VknSpace<VkPipelineInputAssemblyStateCreateInfo> m_inputAssemblyStateCreateInfos{}; // Device>Renderpass>Subpass>infos
        VknSpace<VkPipelineTessellationStateCreateInfo> m_tessellationStateCreateInfos{};
        VknSpace<VkPipelineViewportStateCreateInfo> m_viewportStateCreateInfos{};
        VknSpace<VkPipelineRasterizationStateCreateInfo> m_rasterizationStateCreateInfos{};
        VknSpace<VkPipelineMultisampleStateCreateInfo> m_multisampleStateCreateInfos{};
        VknSpace<VkPipelineDepthStencilStateCreateInfo> m_depthStencilStateCreateInfos{};
        VknSpace<VkPipelineColorBlendStateCreateInfo> m_colorBlendStateCreateInfos{};
        VknSpace<VkPipelineDynamicStateCreateInfo> m_dynamicStateCreateInfos{};
        VknSpace<VkGraphicsPipelineCreateInfo> m_gfxPipelineCreateInfos{}; // Device>Renderpass>Subpass,Pipeline
        VknSpace<VkSwapchainCreateInfoKHR> m_swapchainCreateInfos{};

        VknSpace<VkRenderPassCreateInfo> m_renderpassCreateInfos{};   // Device>infos
        VknSpace<VkAttachmentDescription> m_attachmentDescriptions{}; // Device>Renderpass>infos
        VknSpace<VkAttachmentReference> m_attachmentReferences{};     // Device>Renderpass>Subpass>AttachmentType>RefInfos
        VknSpace<uint32_t> m_preserveAttachments{};                   // Device>Renderpass>Subpass>infos
        VknSpace<VkSubpassDescription> m_subpassDescriptions{};       // Device>Renderpass>infos
        VknSpace<VkSubpassDependency> m_subpassDependencies{};        // Device>Renderpass>infos
        VknSpace<VkDescriptorSetLayoutCreateInfo> m_descriptorSetLayoutCreateInfos{};

        VknSpace<VkVertexInputBindingDescription> m_vertexInputBindings{};     // Device>Renderpass>Subpass>Infos
        VknSpace<VkVertexInputAttributeDescription> m_vertexInputAttributes{}; // Device>Renderpass>Subpass>Infos

        VknSpace<VkFramebufferCreateInfo> m_framebufferCreateInfos{}; // Device>Renderpass>Framebuffers
        VknSpace<VkImageViewCreateInfo> m_imageViewCreateInfos{};     // Device>Swapchain>ImageViews
        VknVector<VkImageCreateInfo> m_imageCreateInfos{};            //>Image
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
