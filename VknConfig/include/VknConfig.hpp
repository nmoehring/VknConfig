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
        void addWindow(void *window);

        // Config
        void setupDebugMessenger();
        void addInstanceExtension(std::string &extension);
        void addLayer(std::string &layer);
        VkSurfaceKHR *createSurface(uint32_t surfaceIdx);
        VkSurfaceKHR *createWindowSurface_GLFW(uint32_t surfaceIdx);
        VkSurfaceKHR *createWindowSurface_Android(uint32_t surfaceIdx);
        void setValidationEnabled() { m_validationLayerAdded = true; }
        VkDebugUtilsMessengerCreateInfoEXT &populateDebugMessengerCreateInfo();
        void setInstanceCreateFlags(VkInstanceCreateFlags flags) { m_flags = flags; }
        void setAppName(std::string appName);
        void setEngineName(std::string engineName);
        void setApiVersion(unsigned int apiVersion);
        void setNumHardCodedVertices(uint32_t numVertices) { m_numHardCodedVertices = numVertices; }

        // Create
        VknResult createInstance();
        VkResult createDebugUtilsMessengerEXT(
            VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
            const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
        void demolish();

        // Getters
        VkInstance *getInstance()
        {
            return &m_engine->getObject<VkInstance>(0);
        }
        bool getInstanceCreated() { return m_createdInstance; }
        VknDevice *getDevice(uint32_t deviceIdx);
        std::list<VknDevice> &getDevices() { return m_devices; }
        uint32_t getNumHardCodedVertices() { return m_numHardCodedVertices; }
        bool hasWindow() { return m_window != nullptr; } // Generic check
        bool hasGLFWConfig() { return m_hasGlfwWindow; }
        void *getWindow() { return m_window; }

        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE; // Add member for debug messenger

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        VknVector<std::string> m_instanceExtensions{}; // Fine, because this list won't need to change
        void *m_window{nullptr};
        VkInstanceCreateFlags m_flags{0};
        std::string m_appName{"My App"};
        std::string m_engineName{"void* Engine"};
        uint32_t m_apiVersion{VK_API_VERSION_1_1};
        uint32_t m_appVersion{0};
        uint32_t m_engineVersion{0};
        uint32_t m_numHardCodedVertices{0};

        // Members
        std::list<VknDevice> m_devices;

        // State
        bool m_selectedQueues{false};
        bool m_filledInstanceCreateInfo{false};
        bool m_createdInstance{false};
        bool m_filledAppInfo{false};
        bool m_validationLayerAdded{false};
        bool m_createdSurface{false};
        bool m_hasGlfwWindow{false};
        bool m_hasAndroidWindow{false};

        void fillAppInfo();
        void fillInstanceCreateInfo();
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
    // VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
}