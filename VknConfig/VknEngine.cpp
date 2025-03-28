#include "include/VknEngine.hpp"

namespace vkn
{
    VknEngine::VknEngine() {}

    VknEngine::~VknEngine()
    {
        for (auto &shaderModule : shaderModules)
            vkDestroyShaderModule(VK_NULL_HANDLE, shaderModule, VK_NULL_HANDLE);
        for (auto &descriptorSetLayout : descriptorSetLayouts)
            vkDestroyDescriptorSetLayout(VK_NULL_HANDLE, descriptorSetLayout, VK_NULL_HANDLE);
        for (auto &pipelineLayout : pipelineLayouts)
            vkDestroyPipelineLayout(VK_NULL_HANDLE, pipelineLayout, VK_NULL_HANDLE);
        for (auto &pipelineCache : pipelineCaches)
            vkDestroyPipelineCache(VK_NULL_HANDLE, pipelineCache, VK_NULL_HANDLE);
        for (auto &pipeline : pipelines)
            vkDestroyPipeline(VK_NULL_HANDLE, pipeline, VK_NULL_HANDLE);

        for (auto &framebuffer : framebuffers)
            vkDestroyFramebuffer(VK_NULL_HANDLE, framebuffer, VK_NULL_HANDLE);
        for (auto &renderpass : renderpasses)
            vkDestroyRenderPass(VK_NULL_HANDLE, renderpass, VK_NULL_HANDLE);

        for (auto &imageView : imageViews)
            vkDestroyImageView(VK_NULL_HANDLE, imageView, VK_NULL_HANDLE);
        for (auto &swapchain : swapchains)
            vkDestroySwapchainKHR(VK_NULL_HANDLE, swapchain, VK_NULL_HANDLE);

        for (auto &device : devices)
            vkDestroyDevice(device, VK_NULL_HANDLE);
        if (instanceCreated)
            vkDestroyInstance(instance, VK_NULL_HANDLE);
    }
}