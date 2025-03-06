#include <filesystem>

#include <CCUtilities.hpp>
#include "VknShaderStage.hpp"

namespace vkn
{
    VknShaderStage::VknShaderStage()
        : m_deviceIdx{0}, m_renderpassIdx{0}, m_subpassIdx{0}, m_shaderIdx{0}, m_placeholder{true}
    {
        m_infos = nullptr;
        m_vkDevice = nullptr;
        m_archive = nullptr;
    }

    VknShaderStage::VknShaderStage(uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
                                   uint32_t shaderIdx, VknInfos *infos, VknResultArchive *archive,
                                   VkDevice *device)
        : m_deviceIdx{deviceIdx}, m_renderpassIdx{renderpassIdx}, m_subpassIdx{subpassIdx},
          m_shaderIdx{shaderIdx}, m_infos{infos}, m_vkDevice{device}, m_archive{archive},
          m_placeholder{false}
    {
    }

    VknShaderStage::~VknShaderStage()
    {
        if (!m_destroyed && !m_placeholder)
            this->destroy();
    }

    void VknShaderStage::destroy()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to destroy a placeholder object.");
        if (m_destroyed)
            throw std::runtime_error("ShaderStage object already destroyed.");

        if (m_shaderModule != VK_NULL_HANDLE)
            vkDestroyShaderModule(*m_vkDevice, m_shaderModule, nullptr);
        m_destroyed = true;
        std::cout << "VknShaderStage DESTROYED." << std::endl;
    }

    void VknShaderStage::setShaderStageType(VknShaderStageType shaderStageType)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
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
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        m_filename = filename;
        this->createShaderModule();
        m_filenameFilled = true;
    }

    void VknShaderStage::setFlags(VkPipelineShaderStageCreateFlags createFlags)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        m_createFlags = createFlags;
    }

    void VknShaderStage::setSpecialization(VkSpecializationInfo specializationInfo)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        m_specializationInfo = specializationInfo;
        m_specializationInfoFilled = true;
    }

    void VknShaderStage::createShaderStage()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_filenameFilled || !m_shaderStageTypeFilled)
            throw std::runtime_error("Both filename and shader stage type fields must be filled before shader stage creation.");

        VkSpecializationInfo *specialization = nullptr;
        if (m_specializationInfoFilled)
            specialization = &m_specializationInfo;
        else
            specialization = VK_NULL_HANDLE;
        m_infos->fillShaderStageCreateInfo(m_deviceIdx, m_renderpassIdx, m_subpassIdx, m_shaderIdx,
                                           &m_shaderModule, &m_shaderStageFlagBit, &m_createFlags,
                                           specialization);
    }

    void VknShaderStage::createShaderModule()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_shaderModuleCreated)
            throw std::runtime_error("Shader module already created.");
        std::filesystem::path shaderDir = std::filesystem::current_path() / "resources" / "shaders";
        std::vector<char> code{CCUtilities::readBinaryFile(shaderDir / m_filename)};
        for (auto &c : code)
            m_code.push_back(c);
        VkShaderModuleCreateInfo *shaderModuleCreateInfo =
            m_infos->fillShaderModuleCreateInfo(
                m_deviceIdx, m_renderpassIdx, m_subpassIdx, m_shaderIdx, &m_code);
        VknResult res{
            vkCreateShaderModule(*m_vkDevice, shaderModuleCreateInfo, VK_NULL_HANDLE, &m_shaderModule),
            "Create shader module."};
        if (!res.isSuccess())
            throw std::runtime_error("Failed to create shader module!");
        m_archive->store(res);
        m_shaderModuleCreated = true;
    }
}