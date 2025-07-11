#include "include/VknEngine.hpp"

namespace vkn
{
    void VknEngine::demolishDebugUtilsMessengerEXT(
        VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
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
        this->demolishAllocations<VkBuffer>(vmaDestroyBuffer);

        this->demolishCommandBuffers();
        this->demolishObjects<VkCommandPool, VkDevice>(vkDestroyCommandPool);
        this->demolishObjects<VkSwapchainKHR, VkDevice>(vkDestroySwapchainKHR);
        this->demolishObjects<VkSemaphore, VkDevice>(vkDestroySemaphore);
        this->demolishObjects<VkFence, VkDevice>(vkDestroyFence);

        this->demolishAllocators();
        this->demolishObjects<VkDeviceMemory, VkDevice>(vkFreeMemory);

        this->demolishDevices();
        this->demolishObjects<VkSurfaceKHR, VkInstance>(vkDestroySurfaceKHR);
        this->demolishDebugObjects<VkDebugUtilsMessengerEXT, VkInstance>(
            [this](VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *pAllocator)
            {
                this->demolishDebugUtilsMessengerEXT(instance, messenger, pAllocator);
            });
        this->demolishInstance();

        if (this->exists<VkQueueFamilyProperties>())
            this->deleteVector<VkQueueFamilyProperties>();
        this->deleteVector<VkPhysicalDevice>();

        if (this->exists<uint32_t>())
            this->deleteVector<uint32_t>(); // i think it was num command buffers for each command pool

        delete static_cast<VknVector<size_t> *>(m_emptyVec);
        m_emptyVec = nullptr;

        if (!m_objectVectors.empty())
            throw std::runtime_error("Some keys were not deleted from m_objectVectors.");

        if (!m_parentVectors.empty())
            throw std::runtime_error("Some keys were not deleted from m_parentVectors.");

        if (!m_allocations.empty())
            throw std::runtime_error("Some keys were not deleted from m_allocations.");
    }
}