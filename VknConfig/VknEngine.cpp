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
        if (!m_poweredOn) // If already shut down, do nothing
            return;
        m_poweredOn = false; // Set immediately to prevent re-entrancy issues from destructor

        // Wait for each device to idle before demolishing resources
        for (auto &device : this->getVector<VkDevice>())
            vkDeviceWaitIdle(device);

        this->demolishObjects<VkShaderModule, VkDevice>(vkDestroyShaderModule);
        this->demolishObjects<VkDescriptorSetLayout, VkDevice>(vkDestroyDescriptorSetLayout);

        this->demolishObjects<VkPipelineLayout, VkDevice>(vkDestroyPipelineLayout);
        this->demolishObjects<VkPipelineCache, VkDevice>(vkDestroyPipelineCache);
        this->demolishObjects<VkPipeline, VkDevice>(vkDestroyPipeline);

        this->demolishObjects<VkFramebuffer, VkDevice>(vkDestroyFramebuffer);
        this->demolishObjects<VkRenderPass, VkDevice>(vkDestroyRenderPass);

        this->demolishObjects<VkImageView, VkDevice>(vkDestroyImageView);
        this->demolishAllocations<VkImage>(vmaDestroyImage);
        this->demolishCommandBuffers();
        this->demolishObjects<VkCommandPool, VkDevice>(vkDestroyCommandPool);
        this->demolishObjects<VkSwapchainKHR, VkDevice>(vkDestroySwapchainKHR);
        this->demolishObjects<VkSemaphore, VkDevice>(vkDestroySemaphore);
        this->demolishObjects<VkFence, VkDevice>(vkDestroyFence);

        if (this->exists<VmaAllocator>())
            for (auto &allocator : this->getVector<VmaAllocator>())
                vmaDestroyAllocator(allocator);
        this->demolishObjects<VkDeviceMemory, VkDevice>(vkFreeMemory);

        for (auto &device : this->getVector<VkDevice>())
            vkDestroyDevice(device, VK_NULL_HANDLE);
        this->demolishObjects<VkSurfaceKHR, VkInstance>(vkDestroySurfaceKHR);
        // What?
        if (this->exists<VkDebugUtilsMessengerEXT>())
            for (auto &debugMsgr : this->getVector<VkDebugUtilsMessengerEXT>())
                /*What?*/ demolishDebugUtilsMessengerEXT(this->getObject<VkInstance>(0),
                                                         this->getObject<VkDebugUtilsMessengerEXT>(0), nullptr);
        for (auto &instance : this->getVector<VkInstance>())
            vkDestroyInstance(instance, VK_NULL_HANDLE);

        if (this->exists<VkQueueFamilyProperties>())
            this->deleteVector<VkQueueFamilyProperties>();
        if (this->exists<VkPhysicalDevice>())
            this->deleteVector<VkPhysicalDevice>();

        this->deleteVectors<VkImage, VmaAllocator>();
        this->deleteAllocationVector<VkImage>();
        this->deleteVector<uint32_t>();
        this->deleteVector<VmaAllocator>();
        this->deleteVectors<VkDevice, VkInstance>();
        this->deleteVectors<VkDebugUtilsMessengerEXT, VkInstance>();
        this->deleteVector<VkInstance>();
    }
}