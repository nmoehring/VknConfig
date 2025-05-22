#include "include/VknEngine.hpp"

namespace vkn
{
    void VknEngine::demolishDebugUtilsMessengerEXT(
        VkInstance instance, VkDebugUtilsMessengerEXT &debugMessenger, const VkAllocationCallbacks *pAllocator)
    {
        ((PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"))(
            instance, debugMessenger, pAllocator);
    }

    VknEngine::VknEngine() {}

    VknEngine::~VknEngine()
    {
        if (m_poweredOn)
            this->shutdown();
    }

    void VknEngine::shutdown()
    {
        // Wait for each device to idle before demolishing resources
        for (auto &device : this->getVector<VkDevice>())
            vkDeviceWaitIdle(device);
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

        for (size_t i = 0; i < this->getVectorSize<VkDeviceMemory>(); ++i)
            vkFreeMemory(*this->getParentPointer<VkDeviceMemory, VkDevice>(i),
                         this->getObject<VkDeviceMemory>(i), VK_NULL_HANDLE);
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
        for (size_t i = 0; i < this->getVectorSize<VkCommandBuffer *>(); ++i)
            vkFreeCommandBuffers(
                *this->getParentPointer<VkCommandPool, VkDevice>(i),
                this->getObject<VkCommandPool>(i),
                this->getObject<uint32_t>(i),
                this->getObject<VkCommandBuffer *>(i));
        for (size_t i = 0; i < this->getVectorSize<VkCommandPool>(); ++i)
            vkDestroyCommandPool(
                *this->getParentPointer<VkCommandPool, VkDevice>(i),
                this->getObject<VkCommandPool>(i),
                nullptr);
        for (size_t i = 0; i < this->getVectorSize<VkSwapchainKHR>(); ++i)
            vkDestroySwapchainKHR(
                *this->getParentPointer<VkSwapchainKHR, VkDevice>(i),
                this->getObject<VkSwapchainKHR>(i),
                VK_NULL_HANDLE);
        for (size_t i = 0; i < this->getVectorSize<VkSemaphore>(); ++i)
            vkDestroySemaphore(
                *this->getParentPointer<VkSemaphore, VkDevice>(i),
                this->getObject<VkSemaphore>(i),
                VK_NULL_HANDLE);
        for (size_t i = 0; i < this->getVectorSize<VkFence>(); ++i)
            vkDestroyFence(
                *this->getParentPointer<VkFence, VkDevice>(i),
                this->getObject<VkFence>(i),
                VK_NULL_HANDLE);

        for (auto &device : this->getVector<VkDevice>())
            vkDestroyDevice(device, VK_NULL_HANDLE);
        for (auto &surface : this->getVector<VkSurfaceKHR>())
            vkDestroySurfaceKHR(this->getObject<VkInstance>(0), surface, VK_NULL_HANDLE);
        for (auto &debugMsgr : this->getVector<VkDebugUtilsMessengerEXT>())
            demolishDebugUtilsMessengerEXT(this->getObject<VkInstance>(0),
                                           this->getObject<VkDebugUtilsMessengerEXT>(0), nullptr);
        for (auto &instance : this->getVector<VkInstance>())
            vkDestroyInstance(instance, VK_NULL_HANDLE);

        // Demolish synchronization objects (semaphores and fences)
        // Need to iterate through all devices and demolish their sync objects
        this->deleteVector<VkQueueFamilyProperties>();
        this->deleteVector<VkPhysicalDevice>();

        this->deleteVectors<VkShaderModule, VkDevice>();
        this->deleteVectors<VkDescriptorSetLayout, VkDevice>();
        this->deleteVectors<VkPipelineLayout, VkDevice>();
        this->deleteVectors<VkPipelineCache, VkDevice>();
        this->deleteVectors<VkPipeline, VkDevice>();
        this->deleteVectors<VkFramebuffer, VkDevice>();
        this->deleteVectors<VkRenderPass, VkDevice>();
        this->deleteVectors<VkDeviceMemory, VkDevice>();
        this->deleteVectors<VkImageView, VkDevice>();
        this->deleteVectors<VkImage, VkDevice>();
        for (uint32_t i = 0; i < this->getVectorSize<VkCommandBuffer *>(); ++i)
            delete[] this->getObject<VkCommandBuffer *>(i);
        this->deleteVectors<VkCommandBuffer *, VkDevice>();
        this->deleteVector<uint32_t>();
        this->deleteVectors<VkCommandPool, VkDevice>();
        this->deleteVectors<VkSwapchainKHR, VkDevice>();
        this->deleteVectors<VkSemaphore, VkDevice>();
        this->deleteVectors<VkFence, VkDevice>();
        this->deleteVectors<VkDevice, VkInstance>();
        this->deleteVectors<VkSurfaceKHR, VkInstance>();
        this->deleteVectors<VkDebugUtilsMessengerEXT, VkInstance>();
        this->deleteVector<VkInstance>();
        m_poweredOn = false;
    }
}