#include <filesystem>

#include <CCUtilities.hpp>
#include "VknShaderStage.hpp"

namespace vkn
{
    VknShaderStage::VknShaderStage()
        : m_deviceIdx{0}, m_renderPassIdx{0}, m_subpassIdx{0}, m_shaderIdx{0}
    {
        m_infos = nullptr;
        m_vkDevice = nullptr;
        m_archive = nullptr;
        m_shaderModuleCreateInfo = nullptr;
        m_createInfo = nullptr;
        m_shaderModule = VK_NULL_HANDLE;
    }
    VknShaderStage::VknShaderStage(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                   uint32_t shaderIdx, VknInfos *infos, VknResultArchive *archive,
                                   VkDevice *device, VknShaderStageType shaderStageType, std::string filename,
                                   VkPipelineShaderStageCreateFlags shaderStageCreateFlags)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_shaderIdx{shaderIdx}, m_infos{infos}, m_vkDevice{device}, m_archive{archive}
    {
        this->createShaderStage(shaderStageType, filename, shaderStageCreateFlags);
    }

    VknShaderStage::~VknShaderStage()
    {
        if (!m_destroyed)
            this->destroy();
    }

    void VknShaderStage::destroy()
    {
        if (m_shaderModuleCreated && !m_destroyed)
        {
            if (m_shaderModule != VK_NULL_HANDLE)
                vkDestroyShaderModule(*m_vkDevice, m_shaderModule, nullptr);
            m_destroyed = true;
        }
    }

    void VknShaderStage::createShaderStage(VknShaderStageType shaderStageType, std::string filename,
                                           VkPipelineShaderStageCreateFlags createFlags)
    {
        VkShaderStageFlagBits shaderStageFlagBits{};
        switch (shaderStageType)
        {
        case VKN_VERTEX_STAGE:
            shaderStageFlagBits = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case VKN_FRAGMENT_STAGE:
            shaderStageFlagBits = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        default:
            throw std::runtime_error("Shader stage not recognized.");
        }
        this->createShaderModule(filename);
        m_createInfo = m_infos->fillShaderStageCreateInfo(m_deviceIdx, m_renderPassIdx, m_subpassIdx,
                                                          m_shaderModule, shaderStageFlagBits, createFlags);
    }

    void VknShaderStage::createShaderModule(const std::string filename)
    {
        std::filesystem::path shaderDir = std::filesystem::current_path() / "resources" / "shaders";
        std::vector<char> code{CCUtilities::readBinaryFile(shaderDir / filename)};
        for (auto c : code)
            m_code.push_back(c);
        m_shaderModuleCreateInfo =
            m_infos->fillShaderModuleCreateInfo(m_deviceIdx, m_renderPassIdx, m_subpassIdx, m_code);
        VknResult res{
            vkCreateShaderModule(*m_vkDevice, m_shaderModuleCreateInfo, VK_NULL_HANDLE, &m_shaderModule),
            "Create shader module."};
        if (!res.isSuccess())
            throw std::runtime_error("Failed to create shader module!");
        m_archive->store(res);
        m_shaderModuleCreated = true;
    }
}