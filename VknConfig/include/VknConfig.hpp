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

#pragma once

#include <list>

#include <memory>
#include <functional>
#include <stdexcept>

#include <vulkan/vulkan.h>

#include "VknPlatforms.hpp"
#include "VknDevice.hpp"
#include "VknData.hpp"
#include "VknWindow.hpp"

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
        void addWindow();

        // Config
        void setupDebugMessenger();
        void addInstanceExtension(std::string &extension);
        void addLayer(std::string &layer);
        VkSurfaceKHR *createSurface(uint32_t surfaceIdx);
        void setValidationEnabled() { m_validationLayerAdded = true; }
        VkDebugUtilsMessengerCreateInfoEXT &populateDebugMessengerCreateInfo();
        void setInstanceCreateFlags(VkInstanceCreateFlags flags) { m_flags = flags; }
        void setAppName(std::string appName);
        void setEngineName(std::string engineName);
        void setApiVersion(unsigned int apiVersion);
        void setNumHardCodedVertices(uint32_t numVertices) { m_numHardCodedVertices = numVertices; }
        void setNotPresentable() { m_presentable = false; }

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
        bool hasWindow() { return m_vknWindow != nullptr; } // Generic check
        VknWindow *getWindow() { return m_vknWindow; }

        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE; // Add member for debug messenger

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        VknVector<std::string> m_instanceExtensions{}; // Fine, because this list won't need to change
        VknWindow *m_vknWindow{nullptr};
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
        bool m_filedInstanceCreateInfo{false};
        bool m_createdInstance{false};
        bool m_filedAppInfo{false};
        bool m_validationLayerAdded{false};
        bool m_createdSurface{false};
        bool m_setPlatformExtensions{false};
        std::optional<bool> m_presentable{};

        void fileAppInfo();
        void fileInstanceCreateInfo();
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