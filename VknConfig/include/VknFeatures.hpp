#pragma once
#include <vulkan/vulkan.h>

#include <string>
#include <unordered_map>

namespace vkn
{
    class VknFeatures1
    {
        std::unordered_map<std::string, bool> m_features{};

    public:
        VknFeatures1() = default;
        VkPhysicalDeviceFeatures createInfo();
        bool robustBufferAccess(bool toggle = false);
        bool fullDrawIndexUint32(bool toggle = false);
        bool imageCubeArray(bool toggle = false);
        bool independentBlend(bool toggle = false);
        bool geometryShader(bool toggle = false);
        bool tessellationShader(bool toggle = false);
        bool sampleRateShading(bool toggle = false);
        bool dualSrcBlend(bool toggle = false);
        bool logicOp(bool toggle = false);
        bool multiDrawIndirect(bool toggle = false);
        bool drawIndirectFirstInstance(bool toggle = false);
        bool depthClamp(bool toggle = false);
        bool depthBiasClamp(bool toggle = false);
        bool fillModeNonSolid(bool toggle = false);
        bool depthBounds(bool toggle = false);
        bool wideLines(bool toggle = false);
        bool largePoints(bool toggle = false);
        bool alphaToOne(bool toggle = false);
        bool multiViewport(bool toggle = false);
        bool samplerAnisotropy(bool toggle = false);
        bool textureCompressionETC2(bool toggle = false);
        bool textureCompressionASTC_LDR(bool toggle = false);
        bool textureCompressionBC(bool toggle = false);
        bool occlusionQueryPrecise(bool toggle = false);
        bool pipelineStatisticsQuery(bool toggle = false);
        bool vertexPipelineStoresAndAtomics(bool toggle = false);
        bool fragmentStoresAndAtomics(bool toggle = false);
        bool shaderTessellationAndGeometryPointSize(bool toggle = false);
        bool shaderImageGatherExtended(bool toggle = false);
        bool shaderStorageImageExtendedFormats(bool toggle = false);
        bool shaderStorageImageMultisample(bool toggle = false);
        bool shaderStorageImageReadWithoutFormat(bool toggle = false);
        bool shaderStorageImageWriteWithoutFormat(bool toggle = false);
        bool shaderUniformBufferArrayDynamicIndexing(bool toggle = false);
        bool shaderSampledImageArrayDynamicIndexing(bool toggle = false);
        bool shaderStorageBufferArrayDynamicIndexing(bool toggle = false);
        bool shaderStorageImageArrayDynamicIndexing(bool toggle = false);
        bool shaderClipDistance(bool toggle = false);
        bool shaderCullDistance(bool toggle = false);
        bool shaderFloat64(bool toggle = false);
        bool shaderInt64(bool toggle = false);
        bool shaderInt16(bool toggle = false);
        bool shaderResourceResidency(bool toggle = false);
        bool shaderResourceMinLod(bool toggle = false);
        bool sparseBinding(bool toggle = false);
        bool sparseResidencyBuffer(bool toggle = false);
        bool sparseResidencyImage2D(bool toggle = false);
        bool sparseResidencyImage3D(bool toggle = false);
        bool sparseResidency2Samples(bool toggle = false);
        bool sparseResidency4Samples(bool toggle = false);
        bool sparseResidency8Samples(bool toggle = false);
        bool sparseResidency16Samples(bool toggle = false);
        bool sparseResidencyAliased(bool toggle = false);
        bool variableMultisampleRate(bool toggle = false);
        bool inheritedQueries(bool toggle = false);
    };

    class VknFeatures2
    {
        std::unordered_map<std::string, bool> m_features{};
        VkPhysicalDeviceMultiviewFeatures m_multiview{};
        VkPhysicalDevice16BitStorageFeatures m_storage16bit{};
        VkPhysicalDeviceProtectedMemoryFeatures m_protectedMemory{};

    public:
        VknFeatures2() = default;
        VkPhysicalDeviceFeatures2 createInfo();

        // Enable Multiview rendering.
        bool multiview(bool toggle = false);

        // Enable 16-bit Storage.
        bool storage16bit(bool toggle = false);

        // Enable Protected Memory.
        bool protectedMemory(bool toggle = false);
    };

    class VknFeatures
    {
    public:
        VknFeatures() = default;
        VkPhysicalDeviceFeatures2 createInfo();

        VknFeatures1 features1{};
        VknFeatures2 features2{};
    };
}