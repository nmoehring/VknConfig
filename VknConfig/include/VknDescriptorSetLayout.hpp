#pragma once

#include "VknEngine.hpp"
#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknDescriptorSetLayout
    {
    public:
        // Overloads
        VknDescriptorSetLayout() = default;
        VknDescriptorSetLayout(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Config
        void addBinding(
            uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
            VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers = VK_NULL_HANDLE);
        void setCreateFlags(VkDescriptorSetLayoutCreateFlags createFlags) { m_createFlags = createFlags; }

        // Create
        void createDescriptorSetLayout();

        // Get
        uint32_t getStartIdx();

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        std::vector<VkDescriptorSetLayoutBinding> m_bindings{};
        VkDescriptorSetLayoutCreateFlags m_createFlags{0};

        // State
        bool m_createdDescriptorSetLayout{false};
        uint32_t m_numBindings{0};
        std::vector<uint32_t> m_descriptorSetLayoutIdxs{};
        static VknDescriptorSetLayout *s_editable;

        void testEditability();
    };
} // namespace vkn