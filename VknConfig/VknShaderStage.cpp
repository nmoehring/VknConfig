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
        m_shaderStageCreateInfo = nullptr;
        m_shaderModule = VK_NULL_HANDLE;
    }

    VknShaderStage::VknShaderStage(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                   uint32_t shaderIdx, VknInfos *infos, VknResultArchive *archive,
                                   VkDevice *device)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_shaderIdx{shaderIdx}, m_infos{infos}, m_vkDevice{device}, m_archive{archive}
    {
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

    void VknShaderStage::setShaderStageType(VknShaderStageType shaderStageType)
    {
        switch (shaderStageType)
        {
        case VKN_VERTEX_STAGE:
            m_shaderStageFlagBit = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case VKN_FRAGMENT_STAGE:
            m_shaderStageFlagBit = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        default:
            throw std::runtime_error("Shader stage not recognized.");
        }
        m_shaderStageTypeFilled = true;
    }

    void VknShaderStage::setFilename(std::string filename)
    {
        m_filename = filename;
        this->createShaderModule();
        m_filenameFilled = true;
    }

    void VknShaderStage::setFlags(VkPipelineShaderStageCreateFlags createFlags)
    {
        m_createFlags = createFlags;
    }

    void VknShaderStage::setSpecialization(VkSpecializationInfo specializationInfo)
    {
        m_specializationInfo = specializationInfo;
        m_specializationInfoSet = true;
    }

    void VknShaderStage::createShaderStage()
    {
        if (!m_filenameFilled || !m_shaderStageTypeFilled)
            throw std::runtime_error("Both filename and shader stage type fields must be filled before shader stage creation.");

        VkSpecializationInfo *specialization = nullptr;
        if (m_specializationInfoSet)
            specialization = &m_specializationInfo;
        m_shaderStageCreateInfo = m_infos->fillShaderStageCreateInfo(m_deviceIdx, m_renderPassIdx, m_subpassIdx,
                                                                     &m_shaderModule, &m_shaderStageFlagBit, &m_createFlags,
                                                                     specialization);
    }

    void VknShaderStage::createShaderModule()
    {
        std::filesystem::path shaderDir = std::filesystem::current_path() / "resources" / "shaders";
        std::vector<char> code{CCUtilities::readBinaryFile(shaderDir / m_filename)};
        for (auto c : code)
            m_code.push_back(c);
        m_shaderModuleCreateInfo =
            m_infos->fillShaderModuleCreateInfo(m_deviceIdx, m_renderPassIdx, m_subpassIdx, &m_code);
        VknResult res{
            vkCreateShaderModule(*m_vkDevice, m_shaderModuleCreateInfo, VK_NULL_HANDLE, &m_shaderModule),
            "Create shader module."};
        if (!res.isSuccess())
            throw std::runtime_error("Failed to create shader module!");
        m_archive->store(res);
        m_shaderModuleCreated = true;
    }
}