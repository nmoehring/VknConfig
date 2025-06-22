/**
 * @file VknShaderStage.hpp
 * @brief Configures a shader stage for a graphics pipeline.
 *
 * VknShaderStage is a hierarchy-bound leaf class within the VknConfig project.
 * It is used by VknPipeline to configure a shader stage.
 * VknShaderStage depends on VknInfos and VknIdxs.
 * It does not have any classes that depend on it.
 *
 * Hierarchy Graph:
 * [VknConfig] (Top-Level)
 *     |
 *     +-- [VknDevice]
 *         |
 *         +-- [VknPhysicalDevice]
 *         |   |
 *         |   +-- [VknQueueFamily] ^ / \
 *         |
 *         +-- [VknSwapchain]
 *         |   |
 *         |   +-- [VknImageView] ^ / \
 *         |
 *         +-- [VknRenderpass]
 *             |
 *             +-- [VknFramebuffer] ^ / \
 *             |
 *             +-- [VknPipeline]
 *                 |
 *                 +-- [VknPipelineLayout]
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout]
 *                 |
 *                 +-- [VknVertexInputState] ^ / \
 *                 +-- [VknInputAssemblyState] ^ / \
 *                 +-- [VknMultisampleState] ^ / \
 *                 +-- [VknRasterizationState] ^ / \
 *                 +-- [VknShaderStage] ^ / \ <<=== YOU ARE HERE
 *                 +-- [VknViewportState] ^ / \
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#pragma once

#include <filesystem>

#include "VknObject.hpp"
#include "VknResult.hpp"
#include "VknData.hpp"

namespace vkn
{

    /** @brief Represents the type of shader stage. */
    enum VknShaderStageType
    {
        VKN_VERTEX_STAGE = VK_SHADER_STAGE_VERTEX_BIT,
        VKN_FRAGMENT_STAGE = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    class VknShaderStage : public VknObject
    {
    public:
        // Overloads
        VknShaderStage() = default;
        VknShaderStage(VknIdxs relIdxs, VknIdxs absIdxs);

        // Config
        void setShaderStageType(VknShaderStageType);
        void setFilename(std::string filename);
        void setFlags(VkPipelineShaderStageCreateFlags createFlags);
        void setSpecialization(VkSpecializationInfo m_specializationInfo);
        void setEntryName(std::string entryName);

        // Create
        /** @brief Creates the VkShaderModule object from the loaded shader code.*/
        void createShaderModule();
        void _fileShaderStageCreateInfo();

        // Get
        bool isShaderModuleCreated();
        VkShaderModule *getShaderModule();

    private:
        // Params
        std::vector<char> m_code; /**< Stores the shader's SPIR-V bytecode. */
        VkShaderStageFlagBits m_shaderStageFlagBit{};
        std::string m_filename{};
        VkPipelineShaderStageCreateFlags m_createFlags{0}; /**< Flags for shader stage creation */
        VkSpecializationInfo m_specializationInfo{};       /**< Specialization constants for the shader. */
        std::string m_entryName{"main"};

        // State
        bool m_createdShaderModule{false}; /**< True if the shader module has been created. */
        bool m_setShaderStageType{false};
        bool m_setFilename{false};
        bool m_filedSpecializationInfo{false};
    };
}