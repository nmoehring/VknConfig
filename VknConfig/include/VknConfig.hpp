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
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "VknDevice.hpp"
#include "VknCommon.hpp"

namespace vkn
{
    class VknConfig
    {
    public:
        // Config
        void deviceInfo(uint32_t deviceIdx); // Create a simple program with just this call to get some device info
        void testNoInputs();

        // Overloads
        VknConfig() = delete;
        VknConfig(VknEngine *engine, VknInfos *infos);
        VknConfig(const VknConfig &) = delete;
        VknConfig &operator=(const VknConfig &) = delete;
        VknConfig(VknConfig &&) = delete;
        VknConfig &operator=(VknConfig &&) = delete;

        // Add
        VknDevice *addDevice(uint32_t deviceIdx);
        void addWindow(GLFWwindow *window);

        // Config
        void fillAppInfo(uint32_t apiVersion, std::string appName,
                         std::string engineName,
                         VkApplicationInfo *pNext = nullptr,
                         uint32_t applicationVersion = 0,
                         uint32_t engineVersion = 0);
        void fillInstanceCreateInfo(const char *const *enabledLayerNames,
                                    uint32_t enabledLayerNamesSize,
                                    const char *const *enabledExtensionNames,
                                    uint32_t enabledExtensionNamesSize,
                                    VkInstanceCreateFlags flags = 0);
        void enableExtensions(std::vector<std::string> extensions);

        // Create
        VknResult createInstance();
        void createWindowSurface(uint32_t surfaceIdx);

        // Getters
        VkInstance *getInstance() { return &m_engine->getObject<VkInstance>(0); }
        bool getInstanceCreated() { return m_createdInstance; }
        VknDevice *getDevice(uint32_t deviceIdx);

    private:
        // Engine
        VknEngine *m_engine;

        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        std::vector<std::string> m_instanceExtensions{}; // Fine, because this list won't need to change
        GLFWwindow *m_window{nullptr};

        // Members
        std::list<VknDevice> m_devices;

        // State
        bool m_selectedQueues{false};
        bool m_filledInstanceCreateInfo{false};
        bool m_createdInstance{false};
        bool m_filledAppInfo{false};
    };
}