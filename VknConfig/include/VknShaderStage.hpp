#pragma once

#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    enum VknShaderStageType
    {
        VKN_VERTEX_STAGE = VK_SHADER_STAGE_VERTEX_BIT,
        VKN_FRAGMENT_STAGE = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    class VknShaderStage
    {
    public:
        VknShaderStage() = delete;
        VknShaderStage(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void setShaderStageType(VknShaderStageType);
        void setFilename(std::string filename);
        void setFlags(VkPipelineShaderStageCreateFlags createFlags);
        void setSpecialization(VkSpecializationInfo m_specializationInfo);
        void fillShaderStageCreateInfo();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Params
        std::vector<char> m_code;
        VkShaderStageFlagBits m_shaderStageFlagBit{};
        std::string m_filename{};
        VkPipelineShaderStageCreateFlags m_createFlags{0};
        VkShaderModule m_shaderModule{VK_NULL_HANDLE};
        VkSpecializationInfo m_specializationInfo{};

        // State
        bool m_shaderModuleCreated{false};
        bool m_shaderStageTypeFilled{false};
        bool m_filenameFilled{false};
        bool m_specializationInfoFilled{false};

        void createShaderModule();
    };
}