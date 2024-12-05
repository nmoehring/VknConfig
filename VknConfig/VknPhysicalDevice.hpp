#pragma once
#include <vulkan/vulkan.h>

class VknPhysicalDevice
{
public:
    VknPhysicalDevice() {}
    VknPhysicalDevice(VkPhysicalDevice dev) : m_device{dev} { queryProperties(); }
    VkPhysicalDevice getDevice() { return m_device; };
    VkPhysicalDeviceProperties getProperties() { return m_properties; };

private:
    VkPhysicalDevice m_device;
    VkPhysicalDeviceProperties m_properties;

    void queryProperties() { vkGetPhysicalDeviceProperties(m_device, &m_properties); }
};