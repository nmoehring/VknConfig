#include "include/VknEngine.hpp"

namespace vkn
{
    VknEngine::VknEngine() {}

    VknEngine::~VknEngine()
    {
        for (auto &shaderModule : this->getObjectVector<VkShaderModule>())
            vkDestroyShaderModule(VK_NULL_HANDLE, shaderModule, VK_NULL_HANDLE);
        for (auto &descriptorSetLayout : this->getObjectVector<VkDescriptorSetLayout>())
            vkDestroyDescriptorSetLayout(VK_NULL_HANDLE, descriptorSetLayout, VK_NULL_HANDLE);
        for (auto &pipelineLayout : this->getObjectVector<VkPipelineLayout>())
            vkDestroyPipelineLayout(VK_NULL_HANDLE, pipelineLayout, VK_NULL_HANDLE);
        for (auto &pipelineCache : this->getObjectVector<VkPipelineCache>())
            vkDestroyPipelineCache(VK_NULL_HANDLE, pipelineCache, VK_NULL_HANDLE);
        for (auto &pipeline : this->getObjectVector<VkPipeline>())
            vkDestroyPipeline(VK_NULL_HANDLE, pipeline, VK_NULL_HANDLE);

        for (auto &framebuffer : this->getObjectVector<VkFramebuffer>())
            vkDestroyFramebuffer(VK_NULL_HANDLE, framebuffer, VK_NULL_HANDLE);
        for (auto &renderpass : this->getObjectVector<VkRenderPass>())
            vkDestroyRenderPass(VK_NULL_HANDLE, renderpass, VK_NULL_HANDLE);

        for (auto &imageView : this->getObjectVector<VkImageView>())
            vkDestroyImageView(VK_NULL_HANDLE, imageView, VK_NULL_HANDLE);
        for (auto &image : this->getObjectVector<VkImage>())
            vkDestroyImage(VK_NULL_HANDLE, image, VK_NULL_HANDLE);
        for (auto &swapchain : this->getObjectVector<VkSwapchainKHR>())
            vkDestroySwapchainKHR(VK_NULL_HANDLE, swapchain, VK_NULL_HANDLE);

        for (auto &device : this->getObjectVector<VkDevice>())
            vkDestroyDevice(device, VK_NULL_HANDLE);
        for (auto &surface : this->getObjectVector<VkSurfaceKHR>())
            vkDestroySurfaceKHR(VK_NULL_HANDLE, surface, VK_NULL_HANDLE);
        for (auto &instance : this->getObjectVector<VkInstance>())
            vkDestroyInstance(instance, VK_NULL_HANDLE);
    }
}