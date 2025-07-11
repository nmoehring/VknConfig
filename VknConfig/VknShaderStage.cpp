#include "include/VknShaderStage.hpp"

namespace vkn
{
    VknShaderStage::VknShaderStage(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
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
        if (m_filedSpecializationInfo)
            throw std::runtime_error("Specialization info already filed.");
        m_specializationInfo = specializationInfo;
        m_filedSpecializationInfo = true;
    }

    void VknShaderStage::setEntryName(std::string entryName)
    {
        m_entryName = entryName;
    }

    void VknShaderStage::_fileShaderStageCreateInfo()
    {
        if (!m_setFilename || !m_setShaderStageType)
            throw std::runtime_error("Both filename and shader stage type fields must be filed before shader stage creation.");
        if (!m_createdShaderModule)
            throw std::runtime_error("Shader module not created before shader stage creation.");
        VkSpecializationInfo *specialization = nullptr;
        if (m_filedSpecializationInfo)
            specialization = &m_specializationInfo;
        else
            specialization = VK_NULL_HANDLE;
        s_infos->fileShaderStageCreateInfo(m_relIdxs, this->getShaderModule(),
                                           &m_shaderStageFlagBit, m_entryName, &m_createFlags, specialization);
    }

    void VknShaderStage::createShaderModule()
    {
        if (m_createdShaderModule)
            throw std::runtime_error("Shader module already created.");
#ifdef __ANDROID__
        // On Android, shaders are typically in "shaders/" subdirectory of assets
        m_code = readAssetFile("shaders/" + m_filename);
#else
        std::filesystem::path shaderDir = std::filesystem::current_path() / "resources" / "shaders";
        m_code = readBinaryFile(shaderDir / m_filename);
#endif
        VkShaderModuleCreateInfo *shaderModuleCreateInfo =
            s_infos->fileShaderModuleCreateInfo(m_relIdxs, &m_code);
        VknResult res{
            vkCreateShaderModule(s_engine->getObject<VkDevice>(m_absIdxs),
                                 shaderModuleCreateInfo, VK_NULL_HANDLE,
                                 &s_engine->getObject<VkShaderModule>(m_absIdxs)),
            "Create shader module."};
        m_createdShaderModule = true;
    }

    bool VknShaderStage::isShaderModuleCreated()
    {
        return m_createdShaderModule;
    }

    VkShaderModule *VknShaderStage::getShaderModule()
    {
        return &s_engine->getObject<VkShaderModule>(m_absIdxs);
    }
}