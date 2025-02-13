#include <stdexcept>
#include <iostream>

#include <vulkan/vulkan.h>
#include "VknDevice.hpp"
#include "VknQueueFamily.hpp"
#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    VknDevice::VknDevice(VknInfos *infos, VknResultArchive *archive)
        : m_infos{infos}, m_resultArchive{archive}
    {
        m_physicalDevice = VknPhysicalDevice{m_resultArchive, m_infos};
    }

    VknDevice::~VknDevice()
    {
        if (!m_destroyed)
            this->destroy();
    }

    void VknDevice::destroy()
    {
        if (!m_destroyed && m_instanceAdded && m_vkDeviceCreated)
        {
            vkDestroyDevice(m_logicalDevice, nullptr);
            m_destroyed = true;
        }
    }

    VknPhysicalDevice *VknDevice::getPhysicalDevice()
    {
        if (!m_instanceAdded)
            throw std::runtime_error("Instance not added to device before retrieving physical device.");
        return &m_physicalDevice;
    }

    VkDevice *VknDevice::getVkDevice()
    {
        if (!m_vkDeviceCreated)
            throw std::runtime_error("VkDevice not created before retrieving VkDevice.");
        return &m_logicalDevice;
    }

    VknQueueFamily VknDevice::getQueue(int idx)
    {
        if (!m_instanceAdded)
            throw std::runtime_error("Instance not added to device before retrieving queue.");
        return m_queues[idx];
    }

    void VknDevice::addInstance(VkInstance *instance)
    {
        m_instance = instance;
        m_physicalDevice = VknPhysicalDevice{m_resultArchive, m_infos};
        m_physicalDevice.addInstance(instance);
        m_instanceAdded = true;
    }

    void VknDevice::archiveResult(VknResult res)
    {
        m_resultArchive->store(res);
    }

    void VknDevice::fillDeviceCreateInfo()
    {
        if (!m_instanceAdded)
            throw std::runtime_error("Instance not added to device before filling device create info.");
        m_infos->fillDeviceCreateInfo(m_extensions, m_features);
    }

    void VknDevice::addExtensions(std::vector<const char *> ext)
    {
        for (auto extension : ext)
            m_extensions.push_back(extension);
    }

    void VknDevice::requestQueueFamilyProperties()
    {
        if (!m_instanceAdded)
            throw std::runtime_error("Instance not added to device before requesting queue properties.");
        uint32_t propertyCount{0};
        vkGetPhysicalDeviceQueueFamilyProperties(
            m_physicalDevice.getVkPhysicalDevice(), &propertyCount, nullptr);
        if (propertyCount == 0)
            throw std::runtime_error("No available queue families found.");
        std::vector<VkQueueFamilyProperties> queues;
        queues.resize(propertyCount);

        vkGetPhysicalDeviceQueueFamilyProperties(
            m_physicalDevice.getVkPhysicalDevice(),
            &propertyCount,
            queues.data());
        if (queues.size() == 0)
            throw std::runtime_error("Error getting queue family properties.");
        for (auto props : queues)
            m_queues.push_back(VknQueueFamily(props));
    }

    VknResult VknDevice::createDevice()
    {
        if (!m_instanceAdded)
            throw std::runtime_error("Instance not added to device before creating VkDevice.");
        VknResult res{
            vkCreateDevice(
                m_physicalDevice.getVkPhysicalDevice(),
                m_infos->getDeviceCreateInfo(),
                nullptr,
                &m_logicalDevice),
            "Create device"};
        if (!(res.isSuccess()))
            throw std::runtime_error(res.toErr("Error creating device."));
        m_resultArchive->store(res);
        m_vkDeviceCreated = true;
        return res;
    }

    void VknDevice::fillSwapChainCreateInfo(
        VkSurfaceKHR surface, uint32_t imageCount, VkExtent2D dimensions,
        VkSurfaceFormatKHR surfaceFormat, uint32_t numImageArrayLayers, VkImageUsageFlags usage,
        VkSharingMode sharingMode, VkSurfaceTransformFlagBitsKHR preTransform,
        VkCompositeAlphaFlagBitsKHR compositeAlpha, VkPresentModeKHR presentMode, VkBool32 clipped,
        VkSwapchainKHR oldSwapchain)
    {
        if (!m_vkDeviceCreated)
            throw std::runtime_error("VkDevice not created before filling swapchain create info.");
        m_swapChainCreateInfos.push_back(m_infos->fillSwapChainCreateInfo(surface, imageCount, dimensions, surfaceFormat,
                                                                          numImageArrayLayers, usage, sharingMode, preTransform,
                                                                          compositeAlpha, presentMode, clipped, oldSwapchain));
    }

    void VknDevice::createSwapChains()
    {
        if (!m_vkDeviceCreated)
            throw std::runtime_error("VkDevice not created before creating swapchains.");
        for (auto swapchainInfo : m_swapChainCreateInfos)
        {
            m_swapChains.push_back(VkSwapchainKHR{});
            VknResult res{
                vkCreateSwapchainKHR(m_logicalDevice, &swapchainInfo, nullptr, &(m_swapChains.back())),
                "Create swapchain"};
            if (!res.isSuccess())
                throw std::runtime_error(res.toErr("Error creating swapchain."));
            m_resultArchive->store(res);
        }
    }
} // namespace vkn