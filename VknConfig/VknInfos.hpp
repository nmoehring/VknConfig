#pragma once
#include <string>
#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>

namespace vkn
{
    enum VknAttachmentType
    {
        COLOR_ATTACHMENT = 0,
        DEPTH_STENCIL_ATTACHMENT = 1,
        RESOLVE_ATTACHMENT = 2,
        INPUT_ATTACHMENT = 3,
        PRESERVE_ATTACHMENT = 4
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
        const char *const *getNamesPointer(std::vector<char[100]> *names);
        VkInstanceCreateInfo *getInstanceCreateInfo() { return &m_instanceCreateInfo; };
        VkDeviceQueueCreateInfo *getDeviceQueueCreateInfo(int index)
        {
            if (this->checkFill(DEVICE_QUEUE_CREATE_INFO))
                return &(m_queueCreateInfos[index]);
            else
                return nullptr;
        };
        VkDeviceCreateInfo *getDeviceCreateInfo()
        {
            if (this->checkFill(DEVICE_CREATE_INFO))
                return &m_deviceCreateInfo;
            else
                return nullptr;
        };
        std::vector<VkGraphicsPipelineCreateInfo> *getPipelineCreateInfos() { return &m_gfxPipelineCreateInfos; }

        int getNumDeviceQueues() { return m_queueCreateInfos.size(); };

        // ============FILL DEVICE INIT INFOS===============
        bool checkFill(checkFillFunctions functionName);

        void fillAppName(std::string name);
        void fillEngineName(std::string name);
        void fillInstanceExtensionNames(std::vector<std::string> names);
        void fillEnabledLayerNames(std::vector<std::string> names);
        void fillAppInfo(uint32_t apiVersion = VK_VERSION_1_1, uint32_t applicationVersion = 0, uint32_t engineVersion = 0);
        void fillInstanceCreateInfo(VkInstanceCreateFlags flags = 0);
        void fillDeviceExtensionNames(std::vector<std::string> names);
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
        void fillDeviceQueueCreateInfo(uint32_t queueFamilyIdx, uint32_t queueCount,
                                       VkApplicationInfo *pNext = nullptr,
                                       VkDeviceQueueCreateFlags flags = 0);
        VkDeviceCreateInfo *fillDeviceCreateInfo(uint32_t deviceIdx);

        VkSwapchainCreateInfoKHR *fillSwapChainCreateInfo(
            VkSurfaceKHR surface, uint32_t imageCount, VkExtent2D dimensions,
            VkSurfaceFormatKHR surfaceFormat = VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            uint32_t numImageArrayLayers = 1, VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR, VkBool32 clipped = VK_TRUE,
            VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);

        //================FILL PIPELINE INFOS===================
        VkShaderModuleCreateInfo *fillShaderModuleCreateInfo(
            uint32_t pipelineIdx, std::vector<char> &code, VkShaderModuleCreateFlags flags = 0);
        VkPipelineShaderStageCreateInfo *fillShaderStageCreateInfo(
            uint32_t pipelineIdx, VkShaderModule module, VkShaderStageFlagBits stage,
            VkPipelineShaderStageCreateFlags flags = 0,
            VkSpecializationInfo *pSpecializationInfo = nullptr);
        VkPipelineVertexInputStateCreateInfo *fillVertexInputStateCreateInfo(uint32_t pipelineIdx);
        VkPipelineInputAssemblyStateCreateInfo *fillInputAssemblyStateCreateInfo(
            uint32_t pipelineIdx,
            VkPrimitiveTopology topology = VkPrimitiveTopology{},
            VkBool32 primitiveRestartEnable = VK_FALSE);
        VkPipelineTessellationStateCreateInfo *fillTessellationStateCreateInfo(uint32_t pipelineIdx, uint32_t patchControlPoints = 0);
        VkPipelineViewportStateCreateInfo *fillViewportStateCreateInfo(
            uint32_t pipelineIdx, std::vector<VkViewport> viewports, std::vector<VkRect2D> scissors);
        VkPipelineRasterizationStateCreateInfo *fillRasterizationStateCreateInfo(
            uint32_t pipelineIdx, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace,
            float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor,
            float lineWidth = 0, VkBool32 depthClampEnable = VK_FALSE,
            VkBool32 rasterizerDiscardEnable = VK_FALSE, VkBool32 depthBiasEnable = VK_FALSE);
        VkPipelineMultisampleStateCreateInfo *fillMultisampleStateCreateInfo(
            uint32_t pipelineIdx, float minSampleShading, VkSampleMask *pSampleMask,
            VkSampleCountFlagBits rasterizationSamples = VkSampleCountFlagBits{},
            VkBool32 sampleShadingEnable = VK_FALSE,
            VkBool32 alphaToCoverageEnable = VK_FALSE, VkBool32 alphaToOneEnable = VK_FALSE);
        VkPipelineDepthStencilStateCreateInfo *fillDepthStencilStateCreateInfo(
            uint32_t pipelineIdx, VkCompareOp depthCompareOp, VkStencilOpState front, VkStencilOpState back,
            float minDepthBounds, float maxDepthBounds,
            VkPipelineDepthStencilStateCreateFlags flags = 0,
            VkBool32 depthTestEnable = VK_FALSE,
            VkBool32 depthWriteEnable = VK_FALSE,
            VkBool32 depthBoundsTestEnable = VK_FALSE, VkBool32 stencilTestEnable = VK_FALSE);
        VkPipelineColorBlendStateCreateInfo *fillColorBlendStateCreateInfo(
            uint32_t pipelineIdx, VkLogicOp logicOp, std::vector<VkPipelineColorBlendAttachmentState> attachments,
            float blendConstants[4], VkBool32 logicOpEnable = VK_FALSE,
            VkPipelineColorBlendStateCreateFlags flags = 0);
        VkPipelineDynamicStateCreateInfo *fillDynamicStateCreateInfo(uint32_t pipelineIdx, std::vector<VkDynamicState> dynamicStates);
        VkGraphicsPipelineCreateInfo *fillGfxPipelineCreateInfo(
            uint32_t pipelineIdx,
            std::vector<VkPipelineShaderStageCreateInfo *> &stages,
            VkPipelineLayout *layout = nullptr,
            VkRenderPass *renderPass = nullptr, uint32_t subpass = uint32_t{},
            VkPipeline basePipelineHandle = VkPipeline{}, int32_t basePipelineIndex = int32_t{},
            VkPipelineCreateFlags flags = 0,
            VkPipelineVertexInputStateCreateInfo *pVertexInputState = nullptr,
            VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState = nullptr,
            VkPipelineTessellationStateCreateInfo *pTessellationState = nullptr,
            VkPipelineViewportStateCreateInfo *pViewportState = nullptr,
            VkPipelineRasterizationStateCreateInfo *pRasterizationState = nullptr,
            VkPipelineMultisampleStateCreateInfo *pMultisampleState = nullptr,
            VkPipelineDepthStencilStateCreateInfo *pDepthStencilState = nullptr,
            VkPipelineColorBlendStateCreateInfo *pColorBlendState = nullptr,
            VkPipelineDynamicStateCreateInfo *pDynamicState = nullptr);
        VkPipelineLayoutCreateInfo *fillPipelineLayoutCreateInfo(
            uint32_t pipelineIdx,
            std::vector<VkDescriptorSetLayout> setLayouts = std::vector<VkDescriptorSetLayout>{},
            std::vector<VkPushConstantRange> pushConstantRanges = std::vector<VkPushConstantRange>{},
            VkPipelineLayoutCreateFlags flags = 0);
        VkPipelineCacheCreateInfo *fillPipelineCacheCreateInfo(
            size_t initialDataSize = 0,
            const void *pInitialData = nullptr,
            VkPipelineCacheCreateFlags flags = 0);
        VkRenderPassCreateInfo *fillRenderPassCreateInfo(VkRenderPassCreateFlags flags = 0);
        VkAttachmentDescription *fillAttachmentDescription(
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
            uint32_t subpassIdx,
            VknAttachmentType attachmentType,
            uint32_t attachmentIdx,
            VkImageLayout layout);
        std::vector<std::vector<std::vector<VkAttachmentReference>>> *getAllAttachmentReferences();
        std::vector<std::vector<VkAttachmentReference>> *getAttachmentReferences(uint32_t subpassIdx);
        std::vector<uint32_t> *getPreserveAttachments(uint32_t subpassIdx);
        std::vector<std::vector<uint32_t>> *getAllPreserveAttachments();
        VkSubpassDescription *fillSubpassDescription(
            uint32_t subpassIdx,
            VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            VkSubpassDescriptionFlags flags = 0);
        VkSubpassDependency *fillSubpassDependency(
            uint32_t srcSubpass = VK_SUBPASS_EXTERNAL, uint32_t dstSubpass = 0,
            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags srcAccessMask = 0,
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        VkDescriptorSetLayoutCreateInfo *fillDescriptorSetLayoutCreateInfo(
            std::vector<VkDescriptorSetLayoutBinding> bindings = std::vector<VkDescriptorSetLayoutBinding>{},
            VkDescriptorSetLayoutCreateFlags flags = 0);
        void fillVertexInputBindingDescription(uint32_t idx, uint32_t binding = 0, uint32_t stride = 0,
                                               VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
        void fillVertexInputAttributeDescription(uint32_t idx, uint32_t binding = 0, uint32_t location = 0,
                                                 VkFormat format = VK_FORMAT_UNDEFINED, uint32_t offset = 0);

    private:
        std::vector<char> *m_appName{nullptr};
        std::vector<char> *m_engineName{nullptr};
        std::vector<char[100]> *m_enabledInstanceExtensionNames{nullptr};
        std::vector<char[100]> *m_enabledLayerNames{nullptr};
        std::vector<VkPhysicalDeviceFeatures> m_enabledFeatures;
        std::vector<std::vector<char[100]> *> m_enabledDeviceExtensionNames;
        std::vector<std::vector<float>> m_queuePriorities;

        // Info's
        VkApplicationInfo m_appInfo{};
        VkInstanceCreateInfo m_instanceCreateInfo{};
        std::vector<VkDeviceQueueCreateInfo> m_queueCreateInfos;
        VkDeviceCreateInfo m_deviceCreateInfo{};

        std::vector<std::vector<VkPipelineLayoutCreateInfo>> m_layoutCreateInfos;
        std::vector<VkPipelineCacheCreateInfo> m_cacheCreateInfos;
        std::vector<std::vector<VkShaderModuleCreateInfo>> m_shaderModuleCreateInfos;
        std::vector<std::vector<VkPipelineShaderStageCreateInfo>> m_shaderStageCreateInfos;
        std::vector<std::vector<VkPipelineVertexInputStateCreateInfo>> m_vertexInputStateCreateInfos;
        std::vector<std::vector<VkPipelineInputAssemblyStateCreateInfo>> m_inputAssemblyStateCreateInfos;
        std::vector<std::vector<VkPipelineTessellationStateCreateInfo>> m_tessellationStateCreateInfos;
        std::vector<std::vector<VkPipelineViewportStateCreateInfo>> m_viewportStateCreateInfos;
        std::vector<std::vector<VkPipelineRasterizationStateCreateInfo>> m_rasterizationStateCreateInfos;
        std::vector<std::vector<VkPipelineMultisampleStateCreateInfo>> m_multisampleStateCreateInfos;
        std::vector<std::vector<VkPipelineDepthStencilStateCreateInfo>> m_depthStencilStateCreateInfos;
        std::vector<std::vector<VkPipelineColorBlendStateCreateInfo>> m_colorBlendStateCreateInfos;
        std::vector<std::vector<VkPipelineDynamicStateCreateInfo>> m_dynamicStateCreateInfos;
        std::vector<VkGraphicsPipelineCreateInfo> m_gfxPipelineCreateInfos;
        std::vector<std::vector<VkSwapchainCreateInfoKHR>> m_swapChainCreateInfos;

        std::vector<VkRenderPassCreateInfo> m_renderPassCreateInfos;
        std::vector<VkAttachmentDescription> m_attachmentDescriptions;
        std::vector<std::vector<std::vector<VkAttachmentReference>>> m_attachmentReferences{};
        std::vector<std::vector<uint32_t>> m_preserveAttachments;
        std::vector<VkSubpassDescription> m_subpassDescriptions;
        std::vector<VkSubpassDependency> m_subpassDependencies;
        std::vector<VkDescriptorSetLayoutCreateInfo> m_descriptorSetLayoutCreateInfos;

        std::vector<std::vector<VkVertexInputBindingDescription>> m_vertexInputBindings;
        std::vector<std::vector<VkVertexInputAttributeDescription>> m_vertexInputAttributes;

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
    };
}
