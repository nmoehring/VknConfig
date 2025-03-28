#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>

#include <vulkan/vulkan.h>
#include "VknEngine.hpp"

namespace vkn
{
    enum VknAttachmentType
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

        template <typename T>
        void initVectors(uint32_t idx1, uint32_t idx2, uint32_t idx3, uint32_t idx4, uint32_t idx5,
                         std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>> &vectors)
        {
            if (vectors.size() < idx1 + 1)
                vectors.resize(idx1 + 1);

            this->initVectors<T>(idx2, idx3, idx4, idx5, vectors[idx1]);
        }

        template <typename T>
        void initVectors(uint32_t idx1, uint32_t idx2, uint32_t idx3, uint32_t idx4,
                         std::vector<std::vector<std::vector<std::vector<T>>>> &vectors)
        {
            if (vectors.size() < idx1 + 1)
                vectors.resize(idx1 + 1);

            this->initVectors<T>(idx2, idx3, idx4, vectors[idx1]);
        }

        template <typename T>
        void initVectors(uint32_t idx1, uint32_t idx2, uint32_t idx3,
                         std::vector<std::vector<std::vector<T>>> &vectors)
        {
            if (vectors.size() < idx1 + 1)
                vectors.resize(idx1 + 1);

            this->initVectors<T>(idx2, idx3, vectors[idx1]);
        }

        template <typename T>
        void initVectors(uint32_t idx1, uint32_t idx2, std::vector<std::vector<T>> &vectors)
        {
            if (vectors.size() < idx1 + 1)
                vectors.resize(idx1 + 1);

            this->initVectors<T>(idx2, vectors[idx1]);
        }

        template <typename T>
        void initVectors(uint32_t idx1, std::vector<T> &vectors)
        {
            if (vectors.size() < idx1 + 1)
                vectors.resize(idx1 + 1);
        }

        template <typename T>
        bool areVectorsFilled(
            uint32_t idx1, uint32_t idx2, uint32_t idx3,
            std::vector<std::vector<std::vector<std::vector<T>>>> &vectors, int32_t expected = -1)
        {
            if (vectors.size() == 0)
                return false;
            return this->areVectorsFilled(idx2, idx3, vectors[idx1], expected);
        }

        template <typename T>
        bool areVectorsFilled(uint32_t idx1, uint32_t idx2, std::vector<std::vector<std::vector<T>>> &vectors, int32_t expected = -1)
        {
            if (vectors.size() == 0)
                return false;
            return this->areVectorsFilled(idx2, vectors[idx1], expected);
        }

        template <typename T>
        bool areVectorsFilled(uint32_t idx1, std::vector<std::vector<T>> &vectors, int32_t expected = -1)
        {
            if (vectors.size() == 0)
                return false;
            return this->areVectorsFilled(vectors[idx1], expected);
        }

        template <typename T>
        bool areVectorsFilled(std::vector<T> &vectors, int32_t expected = -1)
        {
            if (expected < -1)
                throw std::runtime_error("Incorrect expected size parameter in areVectorsFilled().");
            else if (expected > -1 && vectors.size() != expected)
                return false;
            return true;
        }

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
            return &m_queueCreateInfos[deviceIdx][queueFamilyIdx];
        };

        VkRenderPassCreateInfo *getRenderpassCreateInfo(VknIdxs &relIdxs)
        {
            return &m_renderpassCreateInfos[relIdxs.deviceIdx.value()][relIdxs.renderpassIdx.value()];
        }

        VkDeviceCreateInfo *getDeviceCreateInfo(uint32_t deviceIdx)
        {
            if (!(this->checkFill(DEVICE_CREATE_INFO)))
                throw std::runtime_error("DeviceCreateInfo not filled before get.");
            return &m_deviceCreateInfos[deviceIdx];
        };
        std::vector<VkGraphicsPipelineCreateInfo> *getPipelineCreateInfos(VknIdxs &relIdxs)
        {
            return &m_gfxPipelineCreateInfos[relIdxs.deviceIdx.value()][relIdxs.renderpassIdx.value()];
        }
        std::vector<VkPipelineShaderStageCreateInfo> *getShaderStageCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_shaderStageCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
        }
        VkPipelineVertexInputStateCreateInfo *getVertexInputStateCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_vertexInputStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
        }
        VkPipelineInputAssemblyStateCreateInfo *getInputAssemblyStateCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_inputAssemblyStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
        }
        VkPipelineRasterizationStateCreateInfo *getRasterizationStateCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_rasterizationStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
        }
        VkPipelineMultisampleStateCreateInfo *getMultisampleStateCreateInfos(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
        {
            return &m_multisampleStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
        }
        VkPipelineLayoutCreateInfo *getPipelineLayoutCreateInfo(
            VknIdxs &relIdxs)
        {
            return &m_layoutCreateInfos[relIdxs.deviceIdx.value()][relIdxs.renderpassIdx.value()][relIdxs.subpassIdx.value()];
        }
        void initRenderpass(VknIdxs &relIdxs);
        VkSwapchainCreateInfoKHR *getSwapchainCreateInfo(VknIdxs &relIdxs);

        uint32_t getNumDeviceQueueFamilies(uint32_t deviceIdx)
        {
            return m_numQueueFamilies[deviceIdx];
        }
        void setNumDeviceQueueFamilies(int num, uint32_t deviceIdx);
        void fillDeviceQueuePriorities(
            VknIdxs &relIdxs, uint32_t queueFamilyIdx, std::vector<float> priorities);

        // ============FILL DEVICE INIT INFOS===============
        bool checkFill(checkFillFunctions functionName);

        void fillAppName(std::string name);
        void fillEngineName(std::string name);
        void fillInstanceExtensionNames(const char *const *names, uint32_t size);
        void fillEnabledLayerNames(const char *const *names, uint32_t size);
        void fillAppInfo(uint32_t apiVersion = VK_VERSION_1_1, uint32_t applicationVersion = 0, uint32_t engineVersion = 0);
        void fillInstanceCreateInfo(VkInstanceCreateFlags flags = 0);
        void fillDeviceExtensionNames(uint32_t deviceIdx, const char *const *names, uint32_t size);
        void fillDeviceFeatures(
            bool robustBufferAccess = false, bool fullDrawIndexUint32 = false, bool imageCubeArray = false,
            bool independentBlend = false, bool geometryShader = false, bool tessellationShader = false,
            bool sampleRateShading = false, bool dualSrcBlend = false, bool logicOp = false,
            bool multiDrawIndirect = false, bool drawIndirectFirstInstance = false, bool depthClamp = false,
            bool depthBiasClamp = false, bool fillModeNonSolid = false, bool depthBounds = false,
            bool wideLines = false, bool largePoints = false, bool alphaToOne = false,
            bool multiViewport = false, bool samplerAnisotropy = false, bool textureCompressionETC2 = false,
            bool textureCompressionASTC_LDR = false, bool textureCompressionBC = false, bool occlusionQueryPrecise = false,
            bool pipelineStatisticsQuery = false, bool vertexPipelineStoresAndAtomics = false,
            bool fragmentStoresAndAtomics = false, bool shaderTessellationAndGeometryPointSize = false,
            bool shaderImageGatherExtended = false, bool shaderStorageImageExtendedFormats = false,
            bool shaderStorageImageMultisample = false, bool shaderStorageImageReadWithoutFormat = false,
            bool shaderStorageImageWriteWithoutFormat = false, bool shaderUniformBufferArrayDynamicIndexing = false,
            bool shaderSampledImageArrayDynamicIndexing = false, bool shaderStorageBufferArrayDynamicIndexing = false,
            bool shaderStorageImageArrayDynamicIndexing = false, bool shaderClipDistance = false,
            bool shaderCullDistance = false, bool shaderFloat64 = false, bool shaderInt64 = false,
            bool shaderInt16 = false, bool shaderResourceResidency = false, bool shaderResourceMinLod = false,
            bool sparseBinding = false, bool sparseResidencyBuffer = false, bool sparseResidencyImage2D = false,
            bool sparseResidencyImage3D = false, bool sparseResidency2Samples = false,
            bool sparseResidency4Samples = false, bool sparseResidency8Samples = false, bool sparseResidency16Samples = false,
            bool sparseResidencyAliased = false, bool variableMultisampleRate = false, bool inheritedQueries = false);
        void fillDeviceQueueCreateInfo(VknIdxs &relIdxs, uint32_t queueFamilyIdx, uint32_t queueCount,
                                       VkApplicationInfo *pNext = nullptr,
                                       VkDeviceQueueCreateFlags flags = 0);
        VkDeviceCreateInfo *fillDeviceCreateInfo(uint32_t deviceIdx);

        VkSwapchainCreateInfoKHR *fillSwapchainCreateInfo(
            VknIdxs &relIdxs, VkSurfaceKHR *surface, uint32_t imageCount, VkExtent2D dimensions,
            VkSurfaceFormatKHR surfaceFormat = VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            uint32_t numImageArrayLayers = 1, VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR, VkBool32 clipped = VK_TRUE,
            VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);

        //================FILL PIPELINE INFOS===================
        VkShaderModuleCreateInfo *fillShaderModuleCreateInfo(
            VknIdxs &relIdxs, std::vector<char> *code, VkShaderModuleCreateFlags flags = 0);
        VkPipelineShaderStageCreateInfo *fillShaderStageCreateInfo(
            VknIdxs &relIdxs, VkShaderModule *module, VkShaderStageFlagBits *stage,
            VkPipelineShaderStageCreateFlags *flags = 0,
            VkSpecializationInfo *pSpecializationInfo = nullptr);
        VkPipelineVertexInputStateCreateInfo *fillVertexInputStateCreateInfo(
            VknIdxs &relIdxs, uint32_t numBindings, uint32_t numAttributes);
        VkPipelineInputAssemblyStateCreateInfo *fillInputAssemblyStateCreateInfo(
            VknIdxs &relIdxs, VkPrimitiveTopology topology = VkPrimitiveTopology{},
            VkBool32 primitiveRestartEnable = VK_FALSE);
        VkPipelineTessellationStateCreateInfo *fillTessellationStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx, uint32_t patchControlPoints = 0);
        VkPipelineViewportStateCreateInfo *fillViewportStateCreateInfo(
            VknIdxs &relIdxs,
            std::vector<VkViewport> *viewports, std::vector<VkRect2D> *scissors);
        VkPipelineRasterizationStateCreateInfo *fillRasterizationStateCreateInfo(
            VknIdxs &relIdxs,
            VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace,
            float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor,
            float lineWidth = 0, VkBool32 depthClampEnable = VK_FALSE,
            VkBool32 rasterizerDiscardEnable = VK_FALSE, VkBool32 depthBiasEnable = VK_FALSE);
        VkPipelineMultisampleStateCreateInfo *fillMultisampleStateCreateInfo(
            VknIdxs &relIdxs,
            float minSampleShading, VkSampleMask *pSampleMask = VK_NULL_HANDLE,
            VkSampleCountFlagBits rasterizationSamples = VkSampleCountFlagBits{},
            VkBool32 sampleShadingEnable = VK_FALSE, VkBool32 alphaToCoverageEnable = VK_FALSE,
            VkBool32 alphaToOneEnable = VK_FALSE);
        VkPipelineDepthStencilStateCreateInfo *fillDepthStencilStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
            VkCompareOp depthCompareOp, VkStencilOpState front, VkStencilOpState back,
            float minDepthBounds, float maxDepthBounds,
            VkPipelineDepthStencilStateCreateFlags flags = 0,
            VkBool32 depthTestEnable = VK_FALSE,
            VkBool32 depthWriteEnable = VK_FALSE,
            VkBool32 depthBoundsTestEnable = VK_FALSE, VkBool32 stencilTestEnable = VK_FALSE);
        VkPipelineColorBlendStateCreateInfo *fillColorBlendStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
            VkLogicOp logicOp, std::vector<VkPipelineColorBlendAttachmentState> attachments,
            float blendConstants[4], VkBool32 logicOpEnable = VK_FALSE,
            VkPipelineColorBlendStateCreateFlags flags = 0);
        VkPipelineDynamicStateCreateInfo *fillDynamicStateCreateInfo(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx, std::vector<VkDynamicState> dynamicStates);
        VkGraphicsPipelineCreateInfo *fillGfxPipelineCreateInfo(
            VknIdxs &relIdxs, VkRenderPass &renderpass,
            VkPipelineLayout *layout = nullptr,
            VkPipeline basePipelineHandle = VkPipeline{}, int32_t basePipelineIndex = int32_t{},
            VkPipelineCreateFlags flags = 0);
        VkPipelineLayoutCreateInfo *fillPipelineLayoutCreateInfo(
            VknIdxs &relIdxs,
            std::vector<VkDescriptorSetLayout> setLayouts = std::vector<VkDescriptorSetLayout>{},
            std::vector<VkPushConstantRange> pushConstantRanges = std::vector<VkPushConstantRange>{},
            VkPipelineLayoutCreateFlags flags = 0);
        VkPipelineCacheCreateInfo *fillPipelineCacheCreateInfo(
            size_t initialDataSize = 0,
            const void *pInitialData = nullptr,
            VkPipelineCacheCreateFlags flags = 0);
        VkRenderPassCreateInfo *fillRenderpassCreateInfo(
            VknIdxs &relIdxs, uint32_t numAttachments,
            uint32_t numSubpasses, uint32_t numSubpassDeps, VkRenderPassCreateFlags flags = 0);
        VkAttachmentDescription *fillAttachmentDescription(
            VknIdxs &relIdxs, uint32_t attachIdx,
            VkFormat format = VK_FORMAT_B8G8R8A8_SRGB,
            VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
            VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VkAttachmentDescriptionFlags flags = 0);
        void fillAttachmentReference(
            VknIdxs &relIdxs, uint32_t subpassIdx,
            uint32_t attachIdx, VknAttachmentType attachmentType, uint32_t attachmentIdx,
            VkImageLayout layout);
        std::vector<std::vector<std::vector<VkAttachmentReference>>> *getRenderpassAttachmentReferences(
            uint32_t deviceIdx, uint32_t renderpassIdx);
        std::vector<std::vector<VkAttachmentReference>> *getSubpassAttachmentReferences(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        std::vector<uint32_t> *getSubpassPreserveAttachments(uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        std::vector<std::vector<std::vector<uint32_t>>> *getDevicePreserveAttachments(uint32_t deviceIdx);
        std::vector<std::vector<uint32_t>> *getRenderpassPreserveAttachments(uint32_t deviceIdx, uint32_t renderpassIdx);
        VkSubpassDescription *fillSubpassDescription(
            uint32_t numColor, uint32_t numInput, uint32_t numResolve, uint32_t numDepthStencil,
            uint32_t numPreserve, VknIdxs &relIdxs, uint32_t subpassIdx,
            VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            VkSubpassDescriptionFlags flags = 0);
        VkSubpassDependency *fillSubpassDependency(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassDepIdx,
            uint32_t srcSubpass = VK_SUBPASS_EXTERNAL, uint32_t dstSubpass = 0,
            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags srcAccessMask = 0,
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        VkDescriptorSetLayoutCreateInfo *fillDescriptorSetLayoutCreateInfo(
            std::vector<VkDescriptorSetLayoutBinding> bindings = std::vector<VkDescriptorSetLayoutBinding>{},
            VkDescriptorSetLayoutCreateFlags flags = 0);
        VkVertexInputBindingDescription *fillVertexInputBindingDescription(
            VknIdxs &relIdxs,
            uint32_t bindIdx, uint32_t binding = 0, uint32_t stride = 0,
            VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
        VkVertexInputAttributeDescription *fillVertexInputAttributeDescription(
            VknIdxs &relIdxs, uint32_t attributeIdx,
            uint32_t binding = 0, uint32_t location = 0, VkFormat format = VK_FORMAT_UNDEFINED,
            uint32_t offset = 0);

        std::vector<VkVertexInputBindingDescription> *getVertexInputBindings(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        std::vector<VkVertexInputAttributeDescription> *getVertexInputAttributes(
            uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx);
        VkImageViewCreateInfo *getImageViewCreateInfo(VknIdxs &relIdxs);
        VkFramebufferCreateInfo *getFramebufferCreateInfo(VknIdxs &relIdxs);

        void
        fillFramebufferCreateInfo(VknIdxs &relIdxs, VkRenderPass *renderpass,
                                  std::vector<VkImageView> *attachments, uint32_t width,
                                  uint32_t height, uint32_t numLayers, VkFramebufferCreateFlags &flags);
        void fillImageViewCreateInfo(VknIdxs &relIdxs, VkImage &image,
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
        std::vector<VkPhysicalDeviceFeatures> m_enabledFeatures{};
        std::vector<const char *const *> m_enabledDeviceExtensionNames{}; // Device>char_arr
        uint32_t m_enabledDeviceExtensionNamesSize{0};
        std::vector<std::vector<std::vector<float>>> m_queuePriorities{}; // Device>QueueFamily>QueuePriority
        std::vector<uint32_t> m_numQueueFamilies{};                       // Device>NumFamilies

        // Info's
        VkApplicationInfo m_appInfo{};
        VkInstanceCreateInfo m_instanceCreateInfo{};
        std::vector<std::vector<VkDeviceQueueCreateInfo>> m_queueCreateInfos{}; // Device>QueueFamilyInfos
        std::vector<VkDeviceCreateInfo> m_deviceCreateInfos{};                  //>Infos

        std::vector<std::vector<std::vector<VkPipelineLayoutCreateInfo>>> m_layoutCreateInfos{}; // Device>Renderpass>Subpass>infos
        std::vector<VkPipelineCacheCreateInfo> m_cacheCreateInfos{};
        std::vector<std::vector<std::vector<std::vector<VkShaderModuleCreateInfo>>>> m_shaderModuleCreateInfos{};        // Device>Renderpass>Subpass>infos
        std::vector<std::vector<std::vector<std::vector<VkPipelineShaderStageCreateInfo>>>> m_shaderStageCreateInfos{};  // Device>Renderpass>Subpass>infos
        std::vector<std::vector<std::vector<VkPipelineVertexInputStateCreateInfo>>> m_vertexInputStateCreateInfos{};     // Device>Renderpass>Subpass>infos
        std::vector<std::vector<std::vector<VkPipelineInputAssemblyStateCreateInfo>>> m_inputAssemblyStateCreateInfos{}; // Device>Renderpass>Subpass>infos
        std::vector<std::vector<std::vector<VkPipelineTessellationStateCreateInfo>>> m_tessellationStateCreateInfos{};
        std::vector<std::vector<std::vector<VkPipelineViewportStateCreateInfo>>> m_viewportStateCreateInfos{};
        std::vector<std::vector<std::vector<VkPipelineRasterizationStateCreateInfo>>> m_rasterizationStateCreateInfos{};
        std::vector<std::vector<std::vector<VkPipelineMultisampleStateCreateInfo>>> m_multisampleStateCreateInfos{};
        std::vector<std::vector<std::vector<VkPipelineDepthStencilStateCreateInfo>>> m_depthStencilStateCreateInfos{};
        std::vector<std::vector<std::vector<VkPipelineColorBlendStateCreateInfo>>> m_colorBlendStateCreateInfos{};
        std::vector<std::vector<std::vector<VkPipelineDynamicStateCreateInfo>>> m_dynamicStateCreateInfos{};
        std::vector<std::vector<std::vector<VkGraphicsPipelineCreateInfo>>> m_gfxPipelineCreateInfos{}; // Device>Renderpass>Subpass,Pipeline
        std::vector<std::vector<VkSwapchainCreateInfoKHR>> m_swapchainCreateInfos{};

        std::vector<std::vector<VkRenderPassCreateInfo>> m_renderpassCreateInfos{};                                      // Device>infos
        std::vector<std::vector<std::vector<VkAttachmentDescription>>> m_attachmentDescriptions{};                       // Device>Renderpass>infos
        std::vector<std::vector<std::vector<std::vector<std::vector<VkAttachmentReference>>>>> m_attachmentReferences{}; // Device>Renderpass>Subpass>AttachmentType>RefInfos
        std::vector<std::vector<std::vector<std::vector<uint32_t>>>> m_preserveAttachments{};                            // Device>Renderpass>Subpass>infos
        std::vector<std::vector<std::vector<VkSubpassDescription>>> m_subpassDescriptions{};                             // Device>Renderpass>infos
        std::vector<std::vector<std::vector<VkSubpassDependency>>> m_subpassDependencies{};                              // Device>Renderpass>infos
        std::vector<VkDescriptorSetLayoutCreateInfo> m_descriptorSetLayoutCreateInfos{};

        std::vector<std::vector<std::vector<std::vector<VkVertexInputBindingDescription>>>> m_vertexInputBindings{};     // Device>Renderpass>Subpass>Infos
        std::vector<std::vector<std::vector<std::vector<VkVertexInputAttributeDescription>>>> m_vertexInputAttributes{}; // Device>Renderpass>Subpass>Infos

        std::vector<std::vector<std::vector<VkFramebufferCreateInfo>>> m_framebufferCreateInfos{}; // Device>Renderpass>Framebuffers
        std::vector<std::vector<std::vector<VkImageViewCreateInfo>>> m_imageViewCreateInfos{};     // Device>Swapchain>ImageViews

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
