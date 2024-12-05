#include <stdexcept>
#include <iostream>

#include <vulkan/vulkan.h>
#include "VknDevice.hpp"
#include "VknQueueFamily.hpp"
#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    VknDevice::VknDevice() {}
    VknDevice::VknDevice(VkPhysicalDevice dev, VknInfos *infos)
    {
        m_physicalDevice = VknPhysicalDevice(dev);
        m_infos = infos;
        requestQueueFamilyProperties();
    }
    /*
        VknDevice::~VknDevice()
        {
            vkDestroyDevice(m_logicalDevice, nullptr);
        }
    */
    void VknDevice::archiveResult(VknResult res)
    {
        m_resultArchive.push_back(res);
    }

    void VknDevice::requestQueueFamilyProperties()
    {
        uint32_t propertyCount{0};
        vkGetPhysicalDeviceQueueFamilyProperties(
            m_physicalDevice.getDevice(), &propertyCount, nullptr);
        if (propertyCount == 0)
            throw std::runtime_error("No available queue families found.");
        m_queues.resize(propertyCount);
        std::vector<VkQueueFamilyProperties> queues(propertyCount);
        m_infos->setNumQueueFamilies(propertyCount);

        vkGetPhysicalDeviceQueueFamilyProperties(
            m_physicalDevice.getDevice(),
            &propertyCount,
            queues.data());
        if (queues.size() == 0)
            throw std::runtime_error("Error getting queue family properties.");
        for (auto props : queues)
            m_queues.push_back(VknQueueFamily(props));
    }

    VknResult VknDevice::createDevice()
    {
        std::cout << "createInfo is device: " << (m_infos->getDeviceCreateInfo()->sType == VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO) << std::endl;
        VknResult res{
            vkCreateDevice(
                m_physicalDevice.getDevice(),
                m_infos->getDeviceCreateInfo(),
                nullptr,
                &m_logicalDevice),
            "Create device"};
        return res;
    }
} // namespace vkn