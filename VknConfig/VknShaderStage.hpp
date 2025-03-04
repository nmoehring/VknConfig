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
        VknShaderStage();
        VknShaderStage(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                       uint32_t shaderIdx, VknInfos *infos, VknResultArchive *archive,
                       VkDevice *device);
        ~VknShaderStage();
        void destroy();

        void setShaderStageType(VknShaderStageType);
        void setFilename(std::string filename);
        void setFlags(VkPipelineShaderStageCreateFlags createFlags);
        void setSpecialization(VkSpecializationInfo m_specializationInfo);
        void createShaderStage();

    private:
        uint32_t m_deviceIdx;
        uint32_t m_renderPassIdx;
        uint32_t m_subpassIdx;
        uint32_t m_shaderIdx;
        std::vector<char> m_code;
        bool m_placeholder;

        VknInfos *m_infos;
        VknResultArchive *m_archive;
        VkDevice *m_vkDevice;

        VkShaderStageFlagBits m_shaderStageFlagBit{};
        std::string m_filename{};
        VkPipelineShaderStageCreateFlags m_createFlags{0};

        VkShaderModule m_shaderModule{VK_NULL_HANDLE};
        VkSpecializationInfo m_specializationInfo{};

        bool m_destroyed{false};
        bool m_shaderModuleCreated{false};
        bool m_shaderStageTypeFilled{false};
        bool m_filenameFilled{false};
        bool m_specializationInfoFilled{false};

        void createShaderModule();
    };
}