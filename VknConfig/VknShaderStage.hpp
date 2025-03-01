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
        VknShaderStage() = default;
        VknShaderStage(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                       uint32_t shaderIdx, VknInfos *infos, VknResultArchive *archive,
                       VkDevice *device, VknShaderStageType shaderStageType, std::string filename,
                       VkPipelineShaderStageCreateFlags flags = 0);
        ~VknShaderStage();
        void destroy();

    private:
        uint32_t m_deviceIdx;
        uint32_t m_renderPassIdx;
        uint32_t m_subpassIdx;
        uint32_t m_shaderIdx;
        std::vector<char> m_code;

        VknInfos *m_infos;
        VknResultArchive *m_archive;
        VkDevice *m_vkDevice;

        VkShaderModuleCreateInfo *m_shaderModuleCreateInfo{nullptr};
        VkPipelineShaderStageCreateInfo *m_createInfo{nullptr};
        VkShaderModule m_shaderModule{};

        bool m_destroyed{false};
        bool m_shaderModuleCreated{false};

        void createShaderStage(VknShaderStageType shaderStageType, std::string filename,
                               VkPipelineShaderStageCreateFlags flags);
        void createShaderModule(const std::string filename);
    };
}