#include "include/VknEngine.hpp"

namespace vkn
{
    VknEngine::VknEngine() {}

    VknEngine::~VknEngine()
    {
        if (m_poweredOn)
            this->shutdown();
    }

    void VknEngine::shutdown()
    {
        for (auto &shaderModule : this->getVector<VkShaderModule>())
            vkDestroyShaderModule(VK_NULL_HANDLE, shaderModule, VK_NULL_HANDLE);
        for (auto &descriptorSetLayout : this->getVector<VkDescriptorSetLayout>())
            vkDestroyDescriptorSetLayout(VK_NULL_HANDLE, descriptorSetLayout, VK_NULL_HANDLE);
        for (auto &pipelineLayout : this->getVector<VkPipelineLayout>())
            vkDestroyPipelineLayout(VK_NULL_HANDLE, pipelineLayout, VK_NULL_HANDLE);
        for (auto &pipelineCache : this->getVector<VkPipelineCache>())
            vkDestroyPipelineCache(VK_NULL_HANDLE, pipelineCache, VK_NULL_HANDLE);
        for (auto &pipeline : this->getVector<VkPipeline>())
            vkDestroyPipeline(VK_NULL_HANDLE, pipeline, VK_NULL_HANDLE);

        for (auto &framebuffer : this->getVector<VkFramebuffer>())
            vkDestroyFramebuffer(VK_NULL_HANDLE, framebuffer, VK_NULL_HANDLE);
        for (auto &renderpass : this->getVector<VkRenderPass>())
            vkDestroyRenderPass(VK_NULL_HANDLE, renderpass, VK_NULL_HANDLE);

        for (auto &imageView : this->getVector<VkImageView>())
            vkDestroyImageView(VK_NULL_HANDLE, imageView, VK_NULL_HANDLE);
        for (auto &image : this->getVector<VkImage>())
            vkDestroyImage(VK_NULL_HANDLE, image, VK_NULL_HANDLE);
        for (auto &swapchain : this->getVector<VkSwapchainKHR>())
            vkDestroySwapchainKHR(VK_NULL_HANDLE, swapchain, VK_NULL_HANDLE);

        for (auto &device : this->getVector<VkDevice>())
            vkDestroyDevice(device, VK_NULL_HANDLE);
        for (auto &surface : this->getVector<VkSurfaceKHR>())
            vkDestroySurfaceKHR(VK_NULL_HANDLE, surface, VK_NULL_HANDLE);
        for (auto &instance : this->getVector<VkInstance>())
            vkDestroyInstance(instance, VK_NULL_HANDLE);
        m_poweredOn = false;
    }
}