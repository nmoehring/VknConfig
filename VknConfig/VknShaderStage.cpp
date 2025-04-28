#include "include/VknShaderStage.hpp"

namespace vkn
{
    VknShaderStage *VknShaderStage::s_editable{nullptr};

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
        m_setShaderStageType = true;
    }

    void VknShaderStage::setFilename(std::string filename)
    {
        m_filename = filename;
        m_setFilename = true;
    }

    void VknShaderStage::setFlags(VkPipelineShaderStageCreateFlags createFlags)
    {
        m_createFlags = createFlags;
    }

    void VknShaderStage::setSpecialization(VkSpecializationInfo specializationInfo)
    {
        if (m_filledSpecializationInfo)
            throw std::runtime_error("Specialization info already filled.");
        m_specializationInfo = specializationInfo;
        m_filledSpecializationInfo = true;
    }

    void VknShaderStage::fillShaderStageCreateInfo()
    {
        if (!m_setFilename || !m_setShaderStageType)
            throw std::runtime_error("Both filename and shader stage type fields must be filled before shader stage creation.");
        if (!m_createdShaderModule)
            throw std::runtime_error("Shader module not created before shader stage creation.");
        VkSpecializationInfo *specialization = nullptr;
        if (m_filledSpecializationInfo)
            specialization = &m_specializationInfo;
        else
            specialization = VK_NULL_HANDLE;
        m_infos->fillShaderStageCreateInfo(m_relIdxs, this->getShaderModule(),
                                           &m_shaderStageFlagBit, &m_createFlags, specialization);
    }

    void VknShaderStage::createShaderModule()
    {
        if (m_createdShaderModule)
            throw std::runtime_error("Shader module already created.");
        std::filesystem::path shaderDir = std::filesystem::current_path() / "resources" / "shaders";
        VknVector<char> code{readBinaryFile(shaderDir / m_filename)};
        for (auto &c : code)
            m_code.append(c);
        VkShaderModuleCreateInfo *shaderModuleCreateInfo =
            m_infos->fillShaderModuleCreateInfo(m_relIdxs, &m_code);
        VknResult res{
            vkCreateShaderModule(m_engine->getObject<VkDevice>(m_absIdxs),
                                 shaderModuleCreateInfo, VK_NULL_HANDLE,
                                 m_engine->getVector<VkShaderModule>().getData(1)),
            "Create shader module."};
        m_createdShaderModule = true;
    }

    bool VknShaderStage::isShaderModuleCreated()
    {
        return m_createdShaderModule;
    }

    VkShaderModule *VknShaderStage::getShaderModule()
    {
        return &m_engine->getObject<VkShaderModule>(m_absIdxs);
    }

    void VknShaderStage::testEditability()
    {
        if (s_editable != this)
            throw std::runtime_error("Members of a VknShaderStage must be added all at once so that they are stored contiguously.");
    }
}