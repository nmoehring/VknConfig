#include "include/VknFeatures.hpp"

namespace vkn
{
    VkPhysicalDeviceFeatures2 VknFeatures::createInfo()
    {
        // features2.multiview(true);
        VkPhysicalDeviceFeatures ci1{features1.createInfo()};
        VkPhysicalDeviceFeatures2 ci2{features2.createInfo()};
        ci2.features = ci1;
        return ci2;
    }

    bool VknFeatures1::robustBufferAccess(bool toggle)
    {
        if (toggle)
            if (m_features["robustBufferAccess"])
                m_features["robustBufferAccess"] = false;
            else
                m_features["robustBufferAccess"] = true;
        return m_features["robustBufferAccess"];
    }
    bool VknFeatures1::fullDrawIndexUint32(bool toggle)
    {
        if (toggle)
            if (m_features["fullDrawIndexUint32"])
                m_features["fullDrawIndexUint32"] = false;
            else
                m_features["fullDrawIndexUint32"] = true;
        return m_features["fullDrawIndexUint32"];
    }
    bool VknFeatures1::imageCubeArray(bool toggle)
    {
        if (toggle)
            if (m_features["imageCubeArray"])
                m_features["imageCubeArray"] = false;
            else
                m_features["imageCubeArray"] = true;
        return m_features["imageCubeArray"];
    }
    bool VknFeatures1::independentBlend(bool toggle)
    {
        if (toggle)
            if (m_features["independentBlend"])
                m_features["independentBlend"] = false;
            else
                m_features["independentBlend"] = true;
        return m_features["independentBlend"];
    }
    bool VknFeatures1::geometryShader(bool toggle)
    {
        if (toggle)
            if (m_features["geometryShader"])
                m_features["geometryShader"] = false;
            else
                m_features["geometryShader"] = true;
        return m_features["geometryShader"];
    }
    bool VknFeatures1::tessellationShader(bool toggle)
    {
        if (toggle)
            if (m_features["tessellationShader"])
                m_features["tessellationShader"] = false;
            else
                m_features["tessellationShader"] = true;
        return m_features["tessellationShader"];
    }
    bool VknFeatures1::sampleRateShading(bool toggle)
    {
        if (toggle)
            if (m_features["sampleRateShading"])
                m_features["sampleRateShading"] = false;
            else
                m_features["sampleRateShading"] = true;
        return m_features["sampleRateShading"];
    }
    bool VknFeatures1::dualSrcBlend(bool toggle)
    {
        if (toggle)
            if (m_features["dualSrcBlend"])
                m_features["dualSrcBlend"] = false;
            else
                m_features["dualSrcBlend"] = true;
        return m_features["dualSrcBlend"];
    }
    bool VknFeatures1::logicOp(bool toggle)
    {
        if (toggle)
            if (m_features["logicOp"])
                m_features["logicOp"] = false;
            else
                m_features["logicOp"] = true;
        return m_features["logicOp"];
    }
    bool VknFeatures1::multiDrawIndirect(bool toggle)
    {
        if (toggle)
            if (m_features["multiDrawIndirect"])
                m_features["multiDrawIndirect"] = false;
            else
                m_features["multiDrawIndirect"] = true;
        return m_features["multiDrawIndirect"];
    }
    bool VknFeatures1::drawIndirectFirstInstance(bool toggle)
    {
        if (toggle)
            if (m_features["drawIndirectFirstInstance"])
                m_features["drawIndirectFirstInstance"] = false;
            else
                m_features["drawIndirectFirstInstance"] = true;
        return m_features["drawIndirectFirstInstance"];
    }
    bool VknFeatures1::depthClamp(bool toggle)
    {
        if (toggle)
            if (m_features["depthClamp"])
                m_features["depthClamp"] = false;
            else
                m_features["depthClamp"] = true;
        return m_features["depthClamp"];
    }
    bool VknFeatures1::depthBiasClamp(bool toggle)
    {
        if (toggle)
            if (m_features["depthBiasClamp"])
                m_features["depthBiasClamp"] = false;
            else
                m_features["depthBiasClamp"] = true;
        return m_features["depthBiasClamp"];
    }
    bool VknFeatures1::fillModeNonSolid(bool toggle)
    {
        if (toggle)
            if (m_features["fillModeNonSolid"])
                m_features["fillModeNonSolid"] = false;
            else
                m_features["fillModeNonSolid"] = true;
        return m_features["fillModeNonSolid"];
    }
    bool VknFeatures1::depthBounds(bool toggle)
    {
        if (toggle)
            if (m_features["depthBounds"])
                m_features["depthBounds"] = false;
            else
                m_features["depthBounds"] = true;
        return m_features["depthBounds"];
    }
    bool VknFeatures1::wideLines(bool toggle)
    {
        if (toggle)
            if (m_features["wideLines"])
                m_features["wideLines"] = false;
            else
                m_features["wideLines"] = true;
        return m_features["wideLines"];
    }
    bool VknFeatures1::largePoints(bool toggle)
    {
        if (toggle)
            if (m_features["largePoints"])
                m_features["largePoints"] = false;
            else
                m_features["largePoints"] = true;
        return m_features["largePoints"];
    }
    bool VknFeatures1::alphaToOne(bool toggle)
    {
        if (toggle)
            if (m_features["alphaToOne"])
                m_features["alphaToOne"] = false;
            else
                m_features["alphaToOne"] = true;
        return m_features["alphaToOne"];
    }
    bool VknFeatures1::multiViewport(bool toggle)
    {
        if (toggle)
            if (m_features["multiViewport"])
                m_features["multiViewport"] = false;
            else
                m_features["multiViewport"] = true;
        return m_features["multiViewport"];
    }
    bool VknFeatures1::samplerAnisotropy(bool toggle)
    {
        if (toggle)
            if (m_features["samplerAnisotropy"])
                m_features["samplerAnisotropy"] = false;
            else
                m_features["samplerAnisotropy"] = true;
        return m_features["samplerAnisotropy"];
    }
    bool VknFeatures1::textureCompressionETC2(bool toggle)
    {
        if (toggle)
            if (m_features["textureCompressionETC2"])
                m_features["textureCompressionETC2"] = false;
            else
                m_features["textureCompressionETC2"] = true;
        return m_features["textureCompressionETC2"];
    }
    bool VknFeatures1::textureCompressionASTC_LDR(bool toggle)
    {
        if (toggle)
            if (m_features["textureCompressionASTC_LDR"])
                m_features["textureCompressionASTC_LDR"] = false;
            else
                m_features["textureCompressionASTC_LDR"] = true;
        return m_features["textureCompressionASTC_LDR"];
    }
    bool VknFeatures1::textureCompressionBC(bool toggle)
    {
        if (toggle)
            if (m_features["textureCompressionBC"])
                m_features["textureCompressionBC"] = false;
            else
                m_features["textureCompressionBC"] = true;
        return m_features["textureCompressionBC"];
    }
    bool VknFeatures1::occlusionQueryPrecise(bool toggle)
    {
        if (toggle)
            if (m_features["occlusionQueryPrecise"])
                m_features["occlusionQueryPrecise"] = false;
            else
                m_features["occlusionQueryPrecise"] = true;
        return m_features["occlusionQueryPrecise"];
    }
    bool VknFeatures1::pipelineStatisticsQuery(bool toggle)
    {
        if (toggle)
            if (m_features["pipelineStatisticsQuery"])
                m_features["pipelineStatisticsQuery"] = false;
            else
                m_features["pipelineStatisticsQuery"] = true;
        return m_features["pipelineStatisticsQuery"];
    }
    bool VknFeatures1::vertexPipelineStoresAndAtomics(bool toggle)
    {
        if (toggle)
            if (m_features["vertexPipelineStoresAndAtomics"])
                m_features["vertexPipelineStoresAndAtomics"] = false;
            else
                m_features["vertexPipelineStoresAndAtomics"] = true;
        return m_features["vertexPipelineStoresAndAtomics"];
    }
    bool VknFeatures1::fragmentStoresAndAtomics(bool toggle)
    {
        if (toggle)
            if (m_features["fragmentStoresAndAtomics"])
                m_features["fragmentStoresAndAtomics"] = false;
            else
                m_features["fragmentStoresAndAtomics"] = true;
        return m_features["fragmentStoresAndAtomics"];
    }
    bool VknFeatures1::shaderTessellationAndGeometryPointSize(bool toggle)
    {
        if (toggle)
            if (m_features["shaderTessellationAndGeometryPointSize"])
                m_features["shaderTessellationAndGeometryPointSize"] = false;
            else
                m_features["shaderTessellationAndGeometryPointSize"] = true;
        return m_features["shaderTessellationAndGeometryPointSize"];
    }
    bool VknFeatures1::shaderImageGatherExtended(bool toggle)
    {
        if (toggle)
            if (m_features["shaderImageGatherExtended"])
                m_features["shaderImageGatherExtended"] = false;
            else
                m_features["shaderImageGatherExtended"] = true;
        return m_features["shaderImageGatherExtended"];
    }
    bool VknFeatures1::shaderStorageImageExtendedFormats(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageExtendedFormats"])
                m_features["shaderStorageImageExtendedFormats"] = false;
            else
                m_features["shaderStorageImageExtendedFormats"] = true;
        return m_features["shaderStorageImageExtendedFormats"];
    }
    bool VknFeatures1::shaderStorageImageMultisample(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageMultisample"])
                m_features["shaderStorageImageMultisample"] = false;
            else
                m_features["shaderStorageImageMultisample"] = true;
        return m_features["shaderStorageImageMultisample"];
    }
    bool VknFeatures1::shaderStorageImageReadWithoutFormat(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageReadWithoutFormat"])
                m_features["shaderStorageImageReadWithoutFormat"] = false;
            else
                m_features["shaderStorageImageReadWithoutFormat"] = true;
        return m_features["shaderStorageImageReadWithoutFormat"];
    }
    bool VknFeatures1::shaderStorageImageWriteWithoutFormat(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageWriteWithoutFormat"])
                m_features["shaderStorageImageWriteWithoutFormat"] = false;
            else
                m_features["shaderStorageImageWriteWithoutFormat"] = true;
        return m_features["shaderStorageImageWriteWithoutFormat"];
    }
    bool VknFeatures1::shaderUniformBufferArrayDynamicIndexing(bool toggle)
    {
        if (toggle)
            if (m_features["shaderUniformBufferArrayDynamicIndexing"])
                m_features["shaderUniformBufferArrayDynamicIndexing"] = false;
            else
                m_features["shaderUniformBufferArrayDynamicIndexing"] = true;
        return m_features["shaderUniformBufferArrayDynamicIndexing"];
    }
    bool VknFeatures1::shaderSampledImageArrayDynamicIndexing(bool toggle)
    {
        if (toggle)
            if (m_features["shaderSampledImageArrayDynamicIndexing"])
                m_features["shaderSampledImageArrayDynamicIndexing"] = false;
            else
                m_features["shaderSampledImageArrayDynamicIndexing"] = true;
        return m_features["shaderSampledImageArrayDynamicIndexing"];
    }
    bool VknFeatures1::shaderStorageBufferArrayDynamicIndexing(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageBufferArrayDynamicIndexing"])
                m_features["shaderStorageBufferArrayDynamicIndexing"] = false;
            else
                m_features["shaderStorageBufferArrayDynamicIndexing"] = true;
        return m_features["shaderStorageBufferArrayDynamicIndexing"];
    }
    bool VknFeatures1::shaderStorageImageArrayDynamicIndexing(bool toggle)
    {
        if (toggle)
            if (m_features["shaderStorageImageArrayDynamicIndexing"])
                m_features["shaderStorageImageArrayDynamicIndexing"] = false;
            else
                m_features["shaderStorageImageArrayDynamicIndexing"] = true;
        return m_features["shaderStorageImageArrayDynamicIndexing"];
    }
    bool VknFeatures1::shaderClipDistance(bool toggle)
    {
        if (toggle)
            if (m_features["shaderClipDistance"])
                m_features["shaderClipDistance"] = false;
            else
                m_features["shaderClipDistance"] = true;
        return m_features["shaderClipDistance"];
    }
    bool VknFeatures1::shaderCullDistance(bool toggle)
    {
        if (toggle)
            if (m_features["shaderCullDistance"])
                m_features["shaderCullDistance"] = false;
            else
                m_features["shaderCullDistance"] = true;
        return m_features["shaderCullDistance"];
    }
    bool VknFeatures1::shaderFloat64(bool toggle)
    {
        if (toggle)
            if (m_features["shaderFloat64"])
                m_features["shaderFloat64"] = false;
            else
                m_features["shaderFloat64"] = true;
        return m_features["shaderFloat64"];
    }
    bool VknFeatures1::shaderInt64(bool toggle)
    {
        if (toggle)
            if (m_features["shaderInt64"])
                m_features["shaderInt64"] = false;
            else
                m_features["shaderInt64"] = true;
        return m_features["shaderInt64"];
    }
    bool VknFeatures1::shaderInt16(bool toggle)
    {
        if (toggle)
            if (m_features["shaderInt16"])
                m_features["shaderInt16"] = false;
            else
                m_features["shaderInt16"] = true;
        return m_features["shaderInt16"];
    }
    bool VknFeatures1::shaderResourceResidency(bool toggle)
    {
        if (toggle)
            if (m_features["shaderResourceResidency"])
                m_features["shaderResourceResidency"] = false;
            else
                m_features["shaderResourceResidency"] = true;
        return m_features["shaderResourceResidency"];
    }
    bool VknFeatures1::shaderResourceMinLod(bool toggle)
    {
        if (toggle)
            if (m_features["shaderResourceMinLod"])
                m_features["shaderResourceMinLod"] = false;
            else
                m_features["shaderResourceMinLod"] = true;
        return m_features["shaderResourceMinLod"];
    }
    bool VknFeatures1::sparseBinding(bool toggle)
    {
        if (toggle)
            if (m_features["sparseBinding"])
                m_features["sparseBinding"] = false;
            else
                m_features["sparseBinding"] = true;
        return m_features["sparseBinding"];
    }
    bool VknFeatures1::sparseResidencyBuffer(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidencyBuffer"])
                m_features["sparseResidencyBuffer"] = false;
            else
                m_features["sparseResidencyBuffer"] = true;
        return m_features["sparseResidencyBuffer"];
    }
    bool VknFeatures1::sparseResidencyImage2D(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidencyImage2D"])
                m_features["sparseResidencyImage2D"] = false;
            else
                m_features["sparseResidencyImage2D"] = true;
        return m_features["sparseResidencyImage2D"];
    }
    bool VknFeatures1::sparseResidencyImage3D(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidencyImage3D"])
                m_features["sparseResidencyImage3D"] = false;
            else
                m_features["sparseResidencyImage3D"] = true;
        return m_features["sparseResidencyImage3D"];
    }
    bool VknFeatures1::sparseResidency2Samples(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidency2Samples"])
                m_features["sparseResidency2Samples"] = false;
            else
                m_features["sparseResidency2Samples"] = true;
        return m_features["sparseResidency2Samples"];
    }
    bool VknFeatures1::sparseResidency4Samples(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidency4Samples"])
                m_features["sparseResidency4Samples"] = false;
            else
                m_features["sparseResidency4Samples"] = true;
        return m_features["sparseResidency4Samples"];
    }
    bool VknFeatures1::sparseResidency8Samples(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidency8Samples"])
                m_features["sparseResidency8Samples"] = false;
            else
                m_features["sparseResidency8Samples"] = true;
        return m_features["sparseResidency8Samples"];
    }
    bool VknFeatures1::sparseResidency16Samples(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidency16Samples"])
                m_features["sparseResidency16Samples"] = false;
            else
                m_features["sparseResidency16Samples"] = true;
        return m_features["sparseResidency16Samples"];
    }
    bool VknFeatures1::sparseResidencyAliased(bool toggle)
    {
        if (toggle)
            if (m_features["sparseResidencyAliased"])
                m_features["sparseResidencyAliased"] = false;
            else
                m_features["sparseResidencyAliased"] = true;
        return m_features["sparseResidencyAliased"];
    }
    bool VknFeatures1::variableMultisampleRate(bool toggle)
    {
        if (toggle)
            if (m_features["variableMultisampleRate"])
                m_features["variableMultisampleRate"] = false;
            else
                m_features["variableMultisampleRate"] = true;
        return m_features["variableMultisampleRate"];
    }
    bool VknFeatures1::inheritedQueries(bool toggle)
    {
        if (toggle)
            if (m_features["inheritedQueries"])
                m_features["inheritedQueries"] = false;
            else
                m_features["inheritedQueries"] = true;
        return m_features["inheritedQueries"];
    }

    VkPhysicalDeviceFeatures VknFeatures1::createInfo()
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

    VkPhysicalDeviceFeatures2 VknFeatures2::createInfo()
    {
        VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.pNext = nullptr;
        void **pNext = &deviceFeatures2.pNext;

        if (m_features["multiview"])
        {
            m_multiview.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
            m_multiview.multiview = VK_TRUE; // Enable if supported.
            m_multiview.pNext = nullptr;
            (*pNext) = &m_multiview;
            pNext = &m_multiview.pNext;
        }

        if (m_features["storage16bit"])
        {
            m_storage16bit.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES;
            m_storage16bit.storageBuffer16BitAccess = VK_TRUE; // And other 16-bit features as needed.
            m_storage16bit.pNext = nullptr;
            (*pNext) = &m_storage16bit;
            pNext = &m_storage16bit.pNext;
        }

        if (m_features["protectedMemory"])
        {
            m_protectedMemory.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES;
            m_protectedMemory.protectedMemory = VK_TRUE; // Enable if your app needs secure memory.
            m_protectedMemory.pNext = nullptr;
            (*pNext) = &m_protectedMemory;
            pNext = &m_protectedMemory.pNext;
        }
        /*
        // Query the physical device for its supported features.
        vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures2);
        */
        return deviceFeatures2;
    }

    // Enable Multiview rendering.
    bool VknFeatures2::multiview(bool toggle)
    {
        if (toggle)
            if (m_features["multiview"])
                m_features["multiview"] = false;
            else
                m_features["multiview"] = true;
        return m_features["multiview"];
    }

    // Enable 16-bit Storage.
    bool VknFeatures2::storage16bit(bool toggle)
    {
        if (toggle)
            if (m_features["storage16bit"])
                m_features["storage16bit"] = false;
            else
                m_features["storage16bit"] = true;
        return m_features["storage16bit"];
    }

    // Enable Protected Memory.
    bool VknFeatures2::protectedMemory(bool toggle)
    {
        if (toggle)
            if (m_features["protectedMemory"])
                m_features["protectedMemory"] = false;
            else
                m_features["protectedMemory"] = true;
        return m_features["protectedMemory"];
    }
}