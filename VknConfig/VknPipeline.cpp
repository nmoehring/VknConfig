#include <filesystem>

#include "VknPipeline.hpp"
#include "VknInfos.hpp"
#include "../Utilities/Utilities.hpp"

namespace vkn
{
    VknPipeline::VknPipeline() {}

    VknPipeline::VknPipeline(VknDevice *dev, VknInfos *infos, VknResultArchive *archive)
        : m_device{dev}, m_infos{infos}, m_archive{archive} {}

    VknPipeline::~VknPipeline()
    {
        if (!m_destroyed)
            this->destroy();
    }

    void VknPipeline::destroy()
    {
        vkDestroyPipeline(*(m_device->getVkDevice()), m_pipeline, nullptr);
        for (auto renderPass : m_renderPasses)
            vkDestroyRenderPass(*(m_device->getVkDevice()), renderPass, nullptr);
        vkDestroyPipelineLayout(*(m_device->getVkDevice()), m_layout, nullptr);
        vkDestroyDescriptorSetLayout(*(m_device->getVkDevice()), m_descriptorSetLayout, nullptr);
        for (auto module : m_shaderModules)
            vkDestroyShaderModule(*(m_device->getVkDevice()), module, nullptr);
        m_destroyed = true;
    }

    VkGraphicsPipelineCreateInfo &VknPipeline::fillPipelineCreateInfo(
        uint32_t subpass = uint32_t{},
        VkPipeline basePipelineHandle = VkPipeline{}, int32_t basePipelineIndex = int32_t{},
        VkPipelineCreateFlags flags = VkPipelineCreateFlags{},
        VkPipelineVertexInputStateCreateInfo *pVertexInputState = nullptr,
        VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState = nullptr,
        VkPipelineTessellationStateCreateInfo *pTessellationState = nullptr,
        VkPipelineViewportStateCreateInfo *pViewportState = nullptr,
        VkPipelineRasterizationStateCreateInfo *pRasterizationState = nullptr,
        VkPipelineMultisampleStateCreateInfo *pMultisampleState = nullptr,
        VkPipelineDepthStencilStateCreateInfo *pDepthStencilState = nullptr,
        VkPipelineColorBlendStateCreateInfo *pColorBlendState = nullptr,
        VkPipelineDynamicStateCreateInfo *pDynamicState = nullptr);
    )
    {
        return m_infos->fillGraphicsPipelineCreateInfo(m_shaderStageInfos, m_layout, m_renderPass);
    }

    void VknPipeline::createPipeline(VkGraphicsPipelineCreateInfo pipelineCreateInfo)
    {
        std::vector<VkGraphicsPipelineCreateInfo> infos{};
        infos.push_back(pipelineCreateInfo);
        VknResult res{vkCreateGraphicsPipelines(
                          *(m_device->getVkDevice()), nullptr, 1, infos.data(), nullptr, &m_pipeline),
                      "Create pipeline."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating pipeline."));
        m_archive->store(res);
    }

    VkDescriptorSetLayoutCreateInfo &VknPipeline::fillDescriptorSetLayoutCreateInfo(
        VkDescriptorSetLayoutCreateFlags flags)
    {
        return m_infos->fillDescriptorSetLayoutCreateInfo(m_bindings, flags);
    }

    void VknPipeline::createDescriptorSetLayoutBinding(
        uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
        VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers)
    {
        m_bindings.push_back(VkDescriptorSetLayoutBinding{});
        m_bindings.back().binding = binding;
        m_bindings.back().descriptorType = descriptorType;
        m_bindings.back().descriptorCount = descriptorCount;
        m_bindings.back().stageFlags = stageFlags;
        m_bindings.back().pImmutableSamplers = pImmutableSamplers;
    }
    {
        uint32_t binding;
        VkDescriptorType descriptorType;
        uint32_t descriptorCount;
        VkShaderStageFlags stageFlags;
        const VkSampler *pImmutableSamplers;
    }

    void VknPipeline::createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo)
    {
        VknResult res{
            vkCreateDescriptorSetLayout(
                *(m_device->getVkDevice()), &descriptorSetLayoutCreateInfo,
                nullptr, &m_descriptorSetLayout),
            "Create descriptor set layout."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating descriptor set layout."));
        m_archive->store(res);
    }

    void VknPipeline::createPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset,
                                              uint32_t size)
    {
        m_pushConstantRanges.push_back(VkPushConstantRange{});
        m_pushConstantRanges.back().stageFlags = stageFlags;
        m_pushConstantRanges.back().offset = offset;
        m_pushConstantRanges.back().size = size;
    }

    VkPipelineLayoutCreateInfo VknPipeline::fillPipelineLayoutCreateInfo(VkPipelineLayoutCreateFlags flags)
    {
        return m_infos->fillPipelineLayoutCreateInfo(m_setLayouts, m_pushConstantRanges, flags);
    }

    void VknPipeline::createLayout(VkPipelineLayoutCreateInfo &createInfo)
    {
        vkCreatePipelineLayout(*(m_device->getVkDevice()), &createInfo, nullptr, &m_layout);
    }

    int VknPipeline::createShaderModule(const std::string &filename)
    {
        std::filesystem::path shaderDir = std::filesystem::current_path() / "resources" / "shaders";
        std::vector<char> code{CCUtilities::readBinaryFile(shaderDir / filename)};
        auto createInfo{m_infos->fillShaderModuleCreateInfo(code)};
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(*(m_device->getVkDevice()), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module!");
        }
        m_shaderModules.push_back(shaderModule);
        return m_shaderModules.size() - 1;
    }

    int VknPipeline::createShaderStage(ShaderStage shaderStage, std::string filename)
    {
        int module_idx{this->createShaderModule(filename)};
        VkShaderStageFlagBits shaderStageFlagBits{};
        switch (shaderStage)
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
        m_shaderStageInfos.push_back(
            m_infos->fillShaderStageCreateInfo(m_shaderModules[module_idx], shaderStageFlagBits));
        return m_shaderStageInfos.size() - 1;
    }

    std::vector<VkPipelineShaderStageCreateInfo> VknPipeline::getShaderStageInfos(std::vector<int> idxs)
    {
        std::vector<VkPipelineShaderStageCreateInfo> infos;
        for (int idx : idxs)
            infos.push_back(m_shaderStageInfos[idx]);
        return infos;
    }

    VkAttachmentReference VknPipeline::createAttachment(
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkImageLayout attachmentRefLayout)
    {
        m_attachments.push_back(VkAttachmentDescription{});
        m_attachments.back().format = format; // Set to your swapchain image format
        m_attachments.back().samples = samples;
        m_attachments.back().loadOp = loadOp;
        m_attachments.back().storeOp = storeOp;
        m_attachments.back().stencilLoadOp = stencilLoadOp;
        m_attachments.back().stencilStoreOp = stencilStoreOp;
        m_attachments.back().initialLayout = initialLayout;
        m_attachments.back().finalLayout = finalLayout;

        VkAttachmentReference attachmentRef = {};
        attachmentRef.attachment = m_attachments.size() - 1;
        attachmentRef.layout = attachmentRefLayout;

        return attachmentRef;
    }

    void VknPipeline::createSubpass(
        VkSubpassDescriptionFlags flags, VkPipelineBindPoint pipelineBindPoint,
        std::vector<VkAttachmentReference> colorAttachments,
        VkAttachmentReference *depthStencilAttachment,
        std::vector<VkAttachmentReference> inputAttachments,
        std::vector<VkAttachmentReference> resolveAttachments,
        std::vector<uint32_t> preserveAttachments)
    {
        m_subpasses.push_back(VkSubpassDescription{});
        m_subpasses.back().flags = flags;
        m_subpasses.back().pipelineBindPoint = pipelineBindPoint;
        m_subpasses.back().inputAttachmentCount = inputAttachments.size();
        if (!inputAttachments.empty())
            m_subpasses.back().pInputAttachments = inputAttachments.data();
        m_subpasses.back().colorAttachmentCount = colorAttachments.size();
        if (!colorAttachments.empty())
            m_subpasses.back().pColorAttachments = colorAttachments.data();
        if (!resolveAttachments.empty())
            m_subpasses.back().pResolveAttachments = resolveAttachments.data();
        m_subpasses.back().pDepthStencilAttachment = depthStencilAttachment;
        m_subpasses.back().preserveAttachmentCount = preserveAttachments.size();
        if (!preserveAttachments.empty())
            m_subpasses.back().pPreserveAttachments = preserveAttachments.data();
    }

    void VknPipeline::createSubpassDependency()
    {
        m_dependencies.push_back(VkSubpassDependency{});
        m_dependencies.back().srcSubpass = VK_SUBPASS_EXTERNAL;
        m_dependencies.back().dstSubpass = 0;
        m_dependencies.back().srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        m_dependencies.back().srcAccessMask = 0;
        m_dependencies.back().dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        m_dependencies.back().dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    VkRenderPass VknPipeline::createRenderPass(VkRenderPassCreateInfo *renderPassInfo)
    {
        VkRenderPass renderPass;
        VknResult res{vkCreateRenderPass(
                          *(m_device->getVkDevice()), renderPassInfo, nullptr, &renderPass),
                      "Create renderpass."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating renderpass."));
        m_archive->store(res);
        m_renderPasses.push_back(renderPass);
        return renderPass;
    }

    VkRenderPassCreateInfo VknPipeline::fillRenderPassCreateInfo(VkRenderPassCreateFlags flags)
    {
        return m_infos->fillRenderPassCreateInfo(m_attachments, m_subpasses, m_dependencies, flags);
    }
}