/**
 * @file VknConfig.hpp
 * @brief Manages the Vulkan instance, devices, and surfaces.
 *
 * VknConfig is the top-level class of the VknConfig project.
 * It is responsible for creating and managing the Vulkan instance,
 * physical devices, logical devices, and surfaces.
 * VknConfig depends on VknEngine and VknInfos.
 *
 * Hierarchy Graph:
 * [VknConfig] (Top-Level) <<=== YOU ARE HERE
 *     |
 *     +-- [VknDevice]
 *         |
 *         +-- [VknPhysicalDevice]
 *         |   |
 *         |   +-- [VknQueueFamily]
 *         |
 *         +-- [VknSwapchain]
 *         |   |
 *         |   +-- [VknImageView]
 *         |
 *         +-- [VknRenderpass]
 *             |
 *             +-- [VknFramebuffer]
 *             |
 *             +-- [VknPipeline]
 *                 |
 *                 +-- [VknPipelineLayout]
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout]
 *                 |
 *                 +-- [VknVertexInputState]
 *                 +-- [VknInputAssemblyState]
 *                 +-- [VknMultisampleState]
 *                 +-- [VknRasterizationState]
 *                 +-- [VknShaderStage]
 *                 +-- [VknViewportState]
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN

#pragma once

#include <list>

#include <memory>
#include <functional>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "VknDevice.hpp"
#include "VknData.hpp"

namespace vkn
{
    class VknConfig
    {
    public:
        // Overloads
        VknConfig() = delete;
        VknConfig(VknEngine *engine, VknInfos *infos);
        VknConfig(const VknConfig &) = delete;
        VknConfig &operator=(const VknConfig &) = delete;
        VknConfig(VknConfig &&) = delete;
        VknConfig &operator=(VknConfig &&) = delete;

        // Members
        VknDevice *addDevice(uint32_t deviceIdx);
        void addWindow_GLFW(GLFWwindow *window);

        // Config
        void fillAppInfo(uint32_t apiVersion, std::string appName,
                         std::string engineName,
                         VkApplicationInfo *pNext = nullptr,
                         uint32_t applicationVersion = 0,
                         uint32_t engineVersion = 0);
        void fillInstanceCreateInfo(VkInstanceCreateFlags flags = 0);
        void enableExtensions(VknVector<std::string> extensions);
        void setupDebugMessenger();
        void addInstanceExtension(std::string &extension);
        void addInstanceExtension(VknVector<char> &chars);
        void addLayer(std::string &layer);
        void addLayer(VknVector<char> &chars);
        VkSurfaceKHR *createSurface(uint32_t surfaceIdx);
        VkSurfaceKHR *createWindowSurface_GLFW(uint32_t surfaceIdx);
        void setValidationEnabled() { m_validationLayerAdded = true; }

        // Create
        VknResult createInstance();

        // Getters
        VkInstance *getInstance() { return &m_engine->getObject<VkInstance>(0); }
        bool getInstanceCreated() { return m_createdInstance; }
        VknDevice *getDevice(uint32_t deviceIdx);
        std::list<VknDevice> &getDevices() { return m_devices; }

        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE; // Add member for debug messenger

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        VknVector<std::string> m_instanceExtensions{}; // Fine, because this list won't need to change
        GLFWwindow *m_GLFWwindow{nullptr};

        // Members
        std::list<VknDevice> m_devices;
        std::list<VknSwapchain> m_swapchains;

        // State
        bool m_selectedQueues{false};
        bool m_filledInstanceCreateInfo{false};
        bool m_createdInstance{false};
        bool m_filledAppInfo{false};
        bool m_validationLayerAdded{false};
    };

    // Helper function to set up the debug messenger create info
    VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();

    // Debug callback function
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData);

    // Helper functions to get extension function pointers
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

}