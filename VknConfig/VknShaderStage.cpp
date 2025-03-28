#include <filesystem>

#include "include/VknShaderStage.hpp"
#include "../common.hpp"

namespace vkn
{
    VknShaderStage::VknShaderStage(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
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
        m_specializationInfoFilled = true;
    }

    void VknShaderStage::fillShaderStageCreateInfo()
    {
        if (!m_filenameFilled || !m_shaderStageTypeFilled)
            throw std::runtime_error("Both filename and shader stage type fields must be filled before shader stage creation.");

        VkSpecializationInfo *specialization = nullptr;
        if (m_specializationInfoFilled)
            specialization = &m_specializationInfo;
        else
            specialization = VK_NULL_HANDLE;
        m_infos->fillShaderStageCreateInfo(m_relIdxs, &m_shaderModule, &m_shaderStageFlagBit,
                                           &m_createFlags, specialization);
    }

    void VknShaderStage::createShaderModule()
    {
        if (m_shaderModuleCreated)
            throw std::runtime_error("Shader module already created.");
        std::filesystem::path shaderDir = std::filesystem::current_path() / "resources" / "shaders";
        std::vector<char> code{readBinaryFile(shaderDir / m_filename)};
        for (auto &c : code)
            m_code.push_back(c);
        VkShaderModuleCreateInfo *shaderModuleCreateInfo =
            m_infos->fillShaderModuleCreateInfo(m_relIdxs, &m_code);
        m_absIdxs.shaderIdx = m_engine->push_back(VkShaderModule{});
        VknResult res{
            vkCreateShaderModule(m_engine->getObject<VkDevice>(m_absIdxs.deviceIdx.value()),
                                 shaderModuleCreateInfo, VK_NULL_HANDLE,
                                 &m_engine->getObject<VkShaderModule>(m_absIdxs.shaderIdx.value())),
            "Create shader module."};
        m_shaderModuleCreated = true;
    }
}