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
        for (size_t i = 0; i < this->getVectorSize<VkShaderModule>(); ++i)
            vkDestroyShaderModule(
                *this->getParentPointer<VkShaderModule, VkDevice>(i),
                this->getObject<VkShaderModule>(i),
                VK_NULL_HANDLE);
        for (size_t i = 0; i < this->getVectorSize<VkDescriptorSetLayout>(); ++i)
            vkDestroyDescriptorSetLayout(
                *this->getParentPointer<VkDescriptorSetLayout, VkDevice>(i),
                this->getObject<VkDescriptorSetLayout>(i),
                VK_NULL_HANDLE);
        for (size_t i = 0; i < this->getVectorSize<VkPipelineLayout>(); ++i)
            vkDestroyPipelineLayout(
                *this->getParentPointer<VkPipelineLayout, VkDevice>(i),
                this->getObject<VkPipelineLayout>(i),
                VK_NULL_HANDLE);
        for (size_t i = 0; i < this->getVectorSize<VkPipelineCache>(); ++i)
            vkDestroyPipelineCache(
                *this->getParentPointer<VkPipelineCache, VkDevice>(i),
                this->getObject<VkPipelineCache>(i),
                VK_NULL_HANDLE);
        for (size_t i = 0; i < this->getVectorSize<VkPipeline>(); ++i)
            vkDestroyPipeline(
                *this->getParentPointer<VkPipeline, VkDevice>(i),
                this->getObject<VkPipeline>(i),
                VK_NULL_HANDLE);

        for (size_t i = 0; i < this->getVectorSize<VkFramebuffer>(); ++i)
            vkDestroyFramebuffer(
                *this->getParentPointer<VkFramebuffer, VkDevice>(i),
                this->getObject<VkFramebuffer>(i),
                VK_NULL_HANDLE);
        for (size_t i = 0; i < this->getVectorSize<VkRenderPass>(); ++i)
            vkDestroyRenderPass(
                *this->getParentPointer<VkRenderPass, VkDevice>(i),
                this->getObject<VkRenderPass>(i),
                VK_NULL_HANDLE);

        for (size_t i = 0; i < this->getVectorSize<VkImageView>(); ++i)
            vkDestroyImageView(
                *this->getParentPointer<VkImageView, VkDevice>(i),
                this->getObject<VkImageView>(i),
                VK_NULL_HANDLE);
        for (size_t i = 0; i < this->getVectorSize<VkImage>(); ++i)
            vkDestroyImage(
                *this->getParentPointer<VkImage, VkDevice>(i),
                this->getObject<VkImage>(i),
                VK_NULL_HANDLE);
        for (size_t i = 0; i < this->getVectorSize<VkSwapchainKHR>(); ++i)
            vkDestroySwapchainKHR(
                *this->getParentPointer<VkSwapchainKHR, VkDevice>(i),
                this->getObject<VkSwapchainKHR>(i),
                VK_NULL_HANDLE);

        for (auto &device : this->getVector<VkDevice>())
            vkDestroyDevice(device, VK_NULL_HANDLE);
        for (auto &surface : this->getVector<VkSurfaceKHR>())
            vkDestroySurfaceKHR(this->getObject<VkInstance>(0), surface, VK_NULL_HANDLE);
        for (auto &instance : this->getVector<VkInstance>())
            vkDestroyInstance(instance, VK_NULL_HANDLE);

        // Maybe don't need to be in engine, idk
        this->deleteVector<VkQueueFamilyProperties>();
        this->deleteVector<VkPhysicalDevice>();

        // These do
        this->deleteVector<VkShaderModule>();
        this->deleteVector<VkDescriptorSetLayout>();
        this->deleteVector<VkPipelineLayout>();
        this->deleteVector<VkPipelineCache>();
        this->deleteVector<VkPipeline>();
        this->deleteVector<VkFramebuffer>();
        this->deleteVector<VkRenderPass>();
        this->deleteVector<VkImageView>();
        this->deleteVector<VkImage>();
        this->deleteVector<VkSwapchainKHR>();
        this->deleteVector<VkDevice>();
        this->deleteVector<VkSurfaceKHR>();
        this->deleteVector<VkInstance>();
        m_poweredOn = false;
    }
}