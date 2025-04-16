#include "include/VknFeatures.hpp"

namespace vkn
{
    bool VknFeatures::robustBufferAccess(bool toggle)
    {
        if (toggle)
            if (m_features["robustBufferAccess"])
                m_features["robustBufferAccess"] = false;
            else
                m_features["robustBufferAccess"] = true;
        return m_features["robustBufferAccess"];
    }
    bool VknFeatures::fullDrawIndexUint32(bool toggle)
    {
        if (toggle)
            if (m_features["fullDrawIndexUint32"])
                m_features["fullDrawIndexUint32"] = false;
            else
                m_features["fullDrawIndexUint32"] = true;
        return m_features["fullDrawIndexUint32"];
    }
    bool VknFeatures::imageCubeArray(bool toggle)
    {
        if (toggle)
            if (m_features["imageCubeArray"])
                m_features["imageCubeArray"] = false;
            else
                m_features["imageCubeArray"] = true;
        return m_features["imageCubeArray"];
    }
    bool VknFeatures::independentBlend(bool toggle)
    {
        if (toggle)
            if (m_features["independentBlend"])
                m_features["independentBlend"] = false;
            else
                m_features["independentBlend"] = true;
        return m_features["independentBlend"];
    }
    bool VknFeatures::geometryShader(bool toggle)
    {
        if (toggle)
            if (m_features["geometryShader"])
                m_features["geometryShader"] = false;
            else
                m_features["geometryShader"] = true;
        return m_features["geometryShader"];
    }
    bool VknFeatures::tessellationShader(bool toggle)
    {
        if (toggle)
            if (m_features["tessellationShader"])
                m_features["tessellationShader"] = false;
            else
                m_features["tessellationShader"] = true;
        return m_features["tessellationShader"];
    }
    bool VknFeatures::sampleRateShading(bool toggle)
    {
        if (toggle)
            if (m_features["sampleRateShading"])
                m_features["sampleRateShading"] = false;
            else
                m_features["sampleRateShading"] = true;
        return m_features["sampleRateShading"];
    }
    bool VknFeatures::dualSrcBlend(bool toggle)
    {
        if (toggle)
            if (m_features["dualSrcBlend"])
                m_features["dualSrcBlend"] = false;
            else
                m_features["dualSrcBlend"] = true;
        return m_features["dualSrcBlend"];
    }
    bool VknFeatures::logicOp(bool toggle)
    {
        if (toggle)
            if (m_features["logicOp"])
                m_features["logicOp"] = false;
            else
                m_features["logicOp"] = true;
        return m_features["logicOp"];
    }
    bool VknFeatures::multiDrawIndirect(bool toggle)
    {
        if (toggle)
            if (m_features["multiDrawIndirect"])
                m_features["multiDrawIndirect"] = false;
            else
                m_features["multiDrawIndirect"] = true;
        return m_features["multiDrawIndirect"];
    }
    bool VknFeatures::drawIndirectFirstInstance(bool toggle)
    {
        if (toggle)
            if (m_features["drawIndirectFirstInstance"])
                m_features["drawIndirectFirstInstance"] = false;
            else
                m_features["drawIndirectFirstInstance"] = true;
        return m_features["drawIndirectFirstInstance"];
    }
    bool VknFeatures::depthClamp(bool toggle)
    {
        if (toggle)
            if (m_features["depthClamp"])
                m_features["depthClamp"] = false;
            else
                m_features["depthClamp"] = true;
        return m_features["depthClamp"];
    }
    bool VknFeatures::depthBiasClamp(bool toggle)
    {
        if (toggle)
            if (m_features["depthBiasClamp"])
                m_features["depthBiasClamp"] = false;
            else
                m_features["depthBiasClamp"] = true;
        return m_features["depthBiasClamp"];
    }
    bool VknFeatures::fillModeNonSolid(bool toggle)
    {
        if (toggle)
            if (m_features["fillModeNonSolid"])
                m_features["fillModeNonSolid"] = false;
            else
                m_features["fillModeNonSolid"] = true;
        return m_features["fillModeNonSolid"];
    }
    bool VknFeatures::depthBounds(bool toggle)
    {
        if (toggle)
            if (m_features["depthBounds"])
                m_features["depthBounds"] = false;
            else
                m_features["depthBounds"] = true;
        return m_features["depthBounds"];
    }
    bool VknFeatures::wideLines(bool toggle)
    {
        if (toggle)
            if (m_features["wideLines"])
                m_features["wideLines"] = false;
            else
                m_features["wideLines"] = true;
        return m_features["wideLines"];
    }
    bool VknFeatures::largePoints(bool toggle)
    {
        if (toggle)
            if (m_features["largePoints"])
                m_features["largePoints"] = false;
            else
                m_features["largePoints"] = true;
        return m_features["largePoints"];
    }
    bool VknFeatures::alphaToOne(bool toggle)
    {
        if (toggle)
            if (m_features["alphaToOne"])
                m_features["alphaToOne"] = false;
            else
                m_features["alphaToOne"] = true;
        return m_features["alphaToOne"];
    }
    bool VknFeatures::multiViewport(bool toggle)
    {
        if (toggle)
            if (m_features["multiViewport"])
                m_features["multiViewport"] = false;
            else
                m_features["multiViewport"] = true;
        return m_features["multiViewport"];
    }
    bool VknFeatures::samplerAnisotropy(bool toggle)
    {
        if (toggle)
            if (m_features["samplerAnisotropy"])
                m_features["samplerAnisotropy"] = false;
            else
                m_features["samplerAnisotropy"] = true;
        return m_features["samplerAnisotropy"];
    }
    bool VknFeatures::textureCompressionETC2(bool toggle)
    {
        if (toggle)
            if (m_features["textureCompressionETC2"])
                m_features["textureCompressionETC2"] = false;
            else
                m_features["textureCompressionETC2"] = true;
        return m_features["textureCompressionETC2"];
    }
    bool VknFeatures::textureCompressionASTC_LDR(bool toggle)
    {
        if (toggle)
            if (m_features["textureCompressionASTC_LDR"])
                m_features["textureCompressionASTC_LDR"] = false;
            else
                m_features["textureCompressionASTC_LDR"] = true;
        return m_features["textureCompressionASTC_LDR"];
    }
    bool VknFeatures::textureCompressionBC(bool toggle)
    {
        if (toggle)
            if (m_features["textureCompressionBC"])
                m_features["textureCompressionBC"] = false;
            else
                m_features["textureCompressionBC"] = true;
        return m_features["textureCompressionBC"];
    }
    bool VknFeatures::occlusionQueryPrecise(bool toggle)
    {
        if (toggle)
            if (m_features["occlusionQueryPrecise"])
                m_features["occlusionQueryPrecise"] = false;
            else
                m_features["occlusionQueryPrecise"] = true;
        return m_features["occlusionQueryPrecise"];
    }
    bool VknFeatures::pipelineStatisticsQuery(bool toggle)
    {
        if (toggle)
            if (m_features["pipelineStatisticsQuery"])
                m_features["pipelineStatisticsQuery"] = false;
            else
                m_features["pipelineStatisticsQuery"] = true;
        return m_features["pipelineStatisticsQuery"];
    }
    bool VknFeatures::vertexPipelineStoresAndAtomics(bool toggle)
    {
        if (toggle)
            if (m_features["vertexPipelineStoresAndAtomics"])
                m_features["vertexPipelineStoresAndAtomics"] = false;
            else
                m_features["vertexPipelineStoresAndAtomics"] = true;
        return m_features["vertexPipelineStoresAndAtomics"];
    }
    bool VknFeatures::fragmentStoresAndAtomics(bool toggle)
    {
        if (toggle)
            if (m_features["fragmentStoresAndAtomics"])
                m_features["fragmentStoresAndAtomics"] = false;
            else
                m_features["fragmentStoresAndAtomics"] = true;
        return m_features["fragmentStoresAndAtomics"];
    }
    bool VknFeatures::shaderTessellationAndGeometryPointSize(bool toggle)
    {
        if (toggle)
            if (m_features["shaderTessellationAndGeometryPointSize"])
                m_features["shaderTessellationAndGeometryPointSize"] = false;
            else
                m_features["shaderTessellationAndGeometryPointSize"] = true;
        return m_features["shaderTessellationAndGeometryPointSize"];
    }
    bool VknFeatures::shaderImageGatherExtended(bool toggle)
    {
        if (toggle)
            if (m_features["shaderImageGatherExtended"])
                m_features["shaderImageGatherExtended"] = false;
            else
                m_features["shaderImageGatherExtended"] = true;
        return m_features["shaderImageGatherExtended"];
    }
    bool VknFeatures::shaderStorageImageExtendedFormats(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageExtendedFormats"])
                m_features["shaderStorageImageExtendedFormats"] = false;
            else
                m_features["shaderStorageImageExtendedFormats"] = true;
        return m_features["shaderStorageImageExtendedFormats"];
    }
    bool VknFeatures::shaderStorageImageMultisample(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageMultisample"])
                m_features["shaderStorageImageMultisample"] = false;
            else
                m_features["shaderStorageImageMultisample"] = true;
        return m_features["shaderStorageImageMultisample"];
    }
    bool VknFeatures::shaderStorageImageReadWithoutFormat(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageReadWithoutFormat"])
                m_features["shaderStorageImageReadWithoutFormat"] = false;
            else
                m_features["shaderStorageImageReadWithoutFormat"] = true;
        return m_features["shaderStorageImageReadWithoutFormat"];
    }
    bool VknFeatures::shaderStorageImageWriteWithoutFormat(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageWriteWithoutFormat"])
                m_features["shaderStorageImageWriteWithoutFormat"] = false;
            else
                m_features["shaderStorageImageWriteWithoutFormat"] = true;
        return m_features["shaderStorageImageWriteWithoutFormat"];
    }
    bool VknFeatures::shaderUniformBufferArrayDynamicIndexing(bool toggle)
    {
        if (toggle)
            if (m_features["shaderUniformBufferArrayDynamicIndexing"])
                m_features["shaderUniformBufferArrayDynamicIndexing"] = false;
            else
                m_features["shaderUniformBufferArrayDynamicIndexing"] = true;
        return m_features["shaderUniformBufferArrayDynamicIndexing"];
    }
    bool VknFeatures::shaderSampledImageArrayDynamicIndexing(bool toggle)
    {
        if (toggle)
            if (m_features["shaderSampledImageArrayDynamicIndexing"])
                m_features["shaderSampledImageArrayDynamicIndexing"] = false;
            else
                m_features["shaderSampledImageArrayDynamicIndexing"] = true;
        return m_features["shaderSampledImageArrayDynamicIndexing"];
    }
    bool VknFeatures::shaderStorageBufferArrayDynamicIndexing(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageBufferArrayDynamicIndexing"])
                m_features["shaderStorageBufferArrayDynamicIndexing"] = false;
            else
                m_features["shaderStorageBufferArrayDynamicIndexing"] = true;
        return m_features["shaderStorageBufferArrayDynamicIndexing"];
    }
    bool VknFeatures::shaderStorageImageArrayDynamicIndexing(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageArrayDynamicIndexing"])
                m_features["shaderStorageImageArrayDynamicIndexing"] = false;
            else
                m_features["shaderStorageImageArrayDynamicIndexing"] = true;
        return m_features["shaderStorageImageArrayDynamicIndexing"];
    }
    bool VknFeatures::shaderClipDistance(bool toggle)
    {
        if (toggle)
            if (m_features["shaderClipDistance"])
                m_features["shaderClipDistance"] = false;
            else
                m_features["shaderClipDistance"] = true;
        return m_features["shaderClipDistance"];
    }
    bool VknFeatures::shaderCullDistance(bool toggle)
    {
        if (toggle)
            if (m_features["shaderCullDistance"])
                m_features["shaderCullDistance"] = false;
            else
                m_features["shaderCullDistance"] = true;
        return m_features["shaderCullDistance"];
    }
    bool VknFeatures::shaderFloat64(bool toggle)
    {
        if (toggle)
            if (m_features["shaderFloat64"])
                m_features["shaderFloat64"] = false;
            else
                m_features["shaderFloat64"] = true;
        return m_features["shaderFloat64"];
    }
    bool VknFeatures::shaderInt64(bool toggle)
    {
        if (toggle)
            if (m_features["shaderInt64"])
                m_features["shaderInt64"] = false;
            else
                m_features["shaderInt64"] = true;
        return m_features["shaderInt64"];
    }
    bool VknFeatures::shaderInt16(bool toggle)
    {
        if (toggle)
            if (m_features["shaderInt16"])
                m_features["shaderInt16"] = false;
            else
                m_features["shaderInt16"] = true;
        return m_features["shaderInt16"];
    }
    bool VknFeatures::shaderResourceResidency(bool toggle)
    {
        if (toggle)
            if (m_features["shaderResourceResidency"])
                m_features["shaderResourceResidency"] = false;
            else
                m_features["shaderResourceResidency"] = true;
        return m_features["shaderResourceResidency"];
    }
    bool VknFeatures::shaderResourceMinLod(bool toggle)
    {
        if (toggle)
            if (m_features["shaderResourceMinLod"])
                m_features["shaderResourceMinLod"] = false;
            else
                m_features["shaderResourceMinLod"] = true;
        return m_features["shaderResourceMinLod"];
    }
    bool VknFeatures::sparseBinding(bool toggle)
    {
        if (toggle)
            if (m_features["sparseBinding"])
                m_features["sparseBinding"] = false;
            else
                m_features["sparseBinding"] = true;
        return m_features["sparseBinding"];
    }
    bool VknFeatures::sparseResidencyBuffer(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidencyBuffer"])
                m_features["sparseResidencyBuffer"] = false;
            else
                m_features["sparseResidencyBuffer"] = true;
        return m_features["sparseResidencyBuffer"];
    }
    bool VknFeatures::sparseResidencyImage2D(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidencyImage2D"])
                m_features["sparseResidencyImage2D"] = false;
            else
                m_features["sparseResidencyImage2D"] = true;
        return m_features["sparseResidencyImage2D"];
    }
    bool VknFeatures::sparseResidencyImage3D(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidencyImage3D"])
                m_features["sparseResidencyImage3D"] = false;
            else
                m_features["sparseResidencyImage3D"] = true;
        return m_features["sparseResidencyImage3D"];
    }
    bool VknFeatures::sparseResidency2Samples(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidency2Samples"])
                m_features["sparseResidency2Samples"] = false;
            else
                m_features["sparseResidency2Samples"] = true;
        return m_features["sparseResidency2Samples"];
    }
    bool VknFeatures::sparseResidency4Samples(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidency4Samples"])
                m_features["sparseResidency4Samples"] = false;
            else
                m_features["sparseResidency4Samples"] = true;
        return m_features["sparseResidency4Samples"];
    }
    bool VknFeatures::sparseResidency8Samples(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidency8Samples"])
                m_features["sparseResidency8Samples"] = false;
            else
                m_features["sparseResidency8Samples"] = true;
        return m_features["sparseResidency8Samples"];
    }
    bool VknFeatures::sparseResidency16Samples(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidency16Samples"])
                m_features["sparseResidency16Samples"] = false;
            else
                m_features["sparseResidency16Samples"] = true;
        return m_features["sparseResidency16Samples"];
    }
    bool VknFeatures::sparseResidencyAliased(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidencyAliased"])
                m_features["sparseResidencyAliased"] = false;
            else
                m_features["sparseResidencyAliased"] = true;
        return m_features["sparseResidencyAliased"];
    }
    bool VknFeatures::variableMultisampleRate(bool toggle)
    {
        if (toggle)
            if (m_features["variableMultisampleRate"])
                m_features["variableMultisampleRate"] = false;
            else
                m_features["variableMultisampleRate"] = true;
        return m_features["variableMultisampleRate"];
    }
    bool VknFeatures::inheritedQueries(bool toggle)
    {
        if (toggle)
            if (m_features["inheritedQueries"])
                m_features["inheritedQueries"] = false;
            else
                m_features["inheritedQueries"] = true;
        return m_features["inheritedQueries"];
    }

    VkPhysicalDeviceFeatures VknFeatures::createInfo()
    {
        VkPhysicalDeviceFeatures info = VkPhysicalDeviceFeatures{};
        info.robustBufferAccess = robustBufferAccess();
        info.fullDrawIndexUint32 = fullDrawIndexUint32();
        info.imageCubeArray = imageCubeArray();
        info.independentBlend = independentBlend();
        info.geometryShader = geometryShader();
        info.tessellationShader = tessellationShader();
        info.sampleRateShading = sampleRateShading();
        info.dualSrcBlend = dualSrcBlend();
        info.logicOp = logicOp();
        info.multiDrawIndirect = multiDrawIndirect();
        info.drawIndirectFirstInstance = drawIndirectFirstInstance();
        info.depthClamp = depthClamp();
        info.depthBiasClamp = depthBiasClamp();
        info.fillModeNonSolid = fillModeNonSolid();
        info.depthBounds = depthBounds();
        info.wideLines = wideLines();
        info.largePoints = largePoints();
        info.alphaToOne = alphaToOne();
        info.multiViewport = multiViewport();
        info.samplerAnisotropy = samplerAnisotropy();
        info.textureCompressionETC2 = textureCompressionETC2();
        info.textureCompressionASTC_LDR = textureCompressionASTC_LDR();
        info.textureCompressionBC = textureCompressionBC();
        info.occlusionQueryPrecise = occlusionQueryPrecise();
        info.pipelineStatisticsQuery = pipelineStatisticsQuery();
        info.vertexPipelineStoresAndAtomics = vertexPipelineStoresAndAtomics();
        info.fragmentStoresAndAtomics = fragmentStoresAndAtomics();
        info.shaderTessellationAndGeometryPointSize = shaderTessellationAndGeometryPointSize();
        info.shaderImageGatherExtended = shaderImageGatherExtended();
        info.shaderStorageImageExtendedFormats = shaderStorageImageExtendedFormats();
        info.shaderStorageImageMultisample = shaderStorageImageMultisample();
        info.shaderStorageImageReadWithoutFormat = shaderStorageImageReadWithoutFormat();
        info.shaderStorageImageWriteWithoutFormat = shaderStorageImageWriteWithoutFormat();
        info.shaderUniformBufferArrayDynamicIndexing = shaderUniformBufferArrayDynamicIndexing();
        info.shaderSampledImageArrayDynamicIndexing = shaderSampledImageArrayDynamicIndexing();
        info.shaderStorageBufferArrayDynamicIndexing = shaderStorageBufferArrayDynamicIndexing();
        info.shaderStorageImageArrayDynamicIndexing = shaderStorageImageArrayDynamicIndexing();
        info.shaderClipDistance = shaderClipDistance();
        info.shaderCullDistance = shaderCullDistance();
        info.shaderFloat64 = shaderFloat64();
        info.shaderInt64 = shaderInt64();
        info.shaderInt16 = shaderInt16();
        info.shaderResourceResidency = shaderResourceResidency();
        info.shaderResourceMinLod = shaderResourceMinLod();
        info.sparseBinding = sparseBinding();
        info.sparseResidencyBuffer = sparseResidencyBuffer();
        info.sparseResidencyImage2D = sparseResidencyImage2D();
        info.sparseResidencyImage3D = sparseResidencyImage3D();
        info.sparseResidency2Samples = sparseResidency2Samples();
        info.sparseResidency4Samples = sparseResidency4Samples();
        info.sparseResidency8Samples = sparseResidency8Samples();
        info.sparseResidency16Samples = sparseResidency16Samples();
        info.sparseResidencyAliased = sparseResidencyAliased();
        info.variableMultisampleRate = variableMultisampleRate();
        info.inheritedQueries = inheritedQueries();
        return info;
    }
}