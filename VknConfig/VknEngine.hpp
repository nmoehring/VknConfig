#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <CCUtilities.hpp>

namespace vkn
{

    const uint32_t MAX_U32{4294967295};

    struct VknEngine
    {
    public:
        VknEngine();
        ~VknEngine();

        // Prevent copying
        VknEngine(const VknEngine &) = delete;
        VknEngine &operator=(const VknEngine &) = delete;

        // Prevent moving
        VknEngine(VknEngine &&) = delete;
        VknEngine &operator=(VknEngine &&) = delete;

        VkInstance instance{};
        template <typename T>
        std::vector<T> &getObjectVector(VknIdxs absIdxs)
        {
            if (constexpr(std::is_same_v<T, VkDevice>))
                return devices;
            else if (constexpr(std::is_same_v<T, VkRenderPass>))
                return renderpasses;
            else if (constexpr(std::is_same_v<T, VkPipeline>))
                return pipelines;
            else if (constexpr(std::is_same_v<T, VkSwapchainKHR>))
                return swapchains;
            else if (constexpr(std::is_same_v<T, VkImage>))
                return images;
            else if (constexpr(std::is_same_v<T, VkFramebuffer>))
                return framebuffers;
            else if (constexpr(std::is_same_v<T, VkImageView>))
                return imageViews;
            else if (constexpr(std::is_same_v<T, VkDescriptorSetLayout>))
                return descriptorSetLayouts;
            else if (constexpr(std::is_same_v<T, VkPipelineLayout>))
                return pipelineLayouts;
            else if (constexpr(std::is_same_v<T, VkPhysicalDevice>))
                return physicalDevices;
            else if (constexpr(std::is_same_v<T, VkShaderModule>))
                return shaderModules;
            else if (constexpr(std::is_same_v<T, VkPipelineCache>))
                return pipelineCaches;
            else if (constexpr(std::is_same_v<T, VkSurfaceKHR>))
                return surfaces;
            else
                throw std::runtime_error("Invalid object type passed to getElement() of VknEngine");
        }

        template <typename T>
        uint32_t push_back(T val)
        {
            std::vector<T> &vec{this->getObjectVector<T>()};
            idx = vec.size();
            this->getObjectVector<T>().push_back(val);
            return idx;
        }

        template <typename T>
        uint32_t push_back()
        {
            return this->push_back<T>(T{});
        }

        template <typename T>
        T &getObject(uint32_t idx)
        {
            return this->getObjectVector<T>()[idx];
        }

    private:
        bool instanceCreated{false};
        std::vector<VkDevice> devices{};
        std::vector<VkSurfaceKHR> surfaces{};
        std::vector<VkRenderPass> renderpasses{};
        std::vector<VkPipeline> pipelines{};
        std::vector<VkSwapchainKHR> swapchains{};
        std::vector<VkImage> images{};
        std::vector<VkFramebuffer> framebuffers{};
        std::vector<VkImageView> imageViews{};
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{}; // Takes bindings and push constant ranges ^
        std::vector<VkPipelineLayout> pipelineLayouts{};
        std::vector<VkPhysicalDevice> physicalDevices{};
        std::vector<VkShaderModule> shaderModules{};
        std::vector<VkPipelineCache> pipelineCaches{};

        VknResultArchive archive{};
    };

    struct VknIdxs
    {
        CCTL::Valueable deviceIdx{MAX_U32};
        CCTL::Valueable renderpassIdx{MAX_U32};
        CCTL::Valueable subpassIdx{MAX_U32};
        CCTL::Valueable shaderIdx{MAX_U32};
        CCTL::Valueable swapchainIdx{MAX_U32};
        CCTL::Valueable frameIdx{MAX_U32};
        CCTL::Valueable imageIdx{MAX_U32};
        CCTL::Valueable queueFamilyIdx{MAX_U32};
        CCTL::Valueable framebufferIdx{MAX_U32};
        CCTL::Valueable imageViewIdx{MAX_U32};
        CCTL::Valueable physicalDeviceIdx{MAX_U32};
    };
}