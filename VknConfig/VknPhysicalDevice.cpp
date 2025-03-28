#include <iostream>

#include "include/VknPhysicalDevice.hpp"
#include "include/VknEngine.hpp"

namespace vkn
{
    bool s_enumeratedPhysicalDevices{false};
    uint32_t s_deviceCount{0};

    VknPhysicalDevice::VknPhysicalDevice(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknPhysicalDevice::fillDeviceQueuePriorities(uint32_t queueFamilyIdx, std::vector<float> priorities)
    {
        if (!m_requestedQueues)
            throw std::runtime_error("Queue properties not requested before filling queue priorities.");
        m_infos->fillDeviceQueuePriorities(m_relIdxs, queueFamilyIdx, priorities);
    }

    void VknPhysicalDevice::fillDeviceQueuePrioritiesDefault()
    {
        if (!m_requestedQueues)
            throw std::runtime_error("Queue properties not requested before filling queue priorities.");
        for (int i = 0; i < m_queues.size(); ++i)
            m_infos->fillDeviceQueuePriorities(m_relIdxs, i, std::vector<float>(m_queues[i].getNumSelected(), 1.0f));
    }

    void VknPhysicalDevice::requestQueueFamilyProperties()
    {
        if (m_requestedQueues)
            throw std::runtime_error("Queue properties already requested.");
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before requesting queue properties.");
        uint32_t propertyCount{0};
        vkGetPhysicalDeviceQueueFamilyProperties(
            m_vkPhysicalDevice, &propertyCount, VK_NULL_HANDLE);
        if (propertyCount == 0)
            throw std::runtime_error("No available queue families found.");
        std::vector<VkQueueFamilyProperties> queues;
        m_queues.resize(propertyCount);
        if (m_queues.size() > 0)
            throw std::runtime_error("m_queues already filled before requesting queue properties.");

        vkGetPhysicalDeviceQueueFamilyProperties(
            m_vkPhysicalDevice,
            &propertyCount,
            queues.data());
        for (auto &props : queues)
            m_queues.emplace_back(props);
        m_requestedQueues = true;
    }

    void VknPhysicalDevice::fillQueueCreateInfos()
    {
        if (m_filledQueueCreateInfos)
            throw std::runtime_error("Already filled queue create infos.");
        for (int i = 0; i < m_queues.size(); ++i)
            m_infos->fillDeviceQueueCreateInfo(m_relIdxs, i, m_queues[i].getNumSelected());
        m_filledQueueCreateInfos = true;
    }

    void VknPhysicalDevice::selectQueues(bool chooseAllAvailableQueues)
    {
        if (m_selectedQueues)
            throw std::runtime_error("Queues already selected.");
        if (!m_requestedQueues)
            throw std::runtime_error("Queue families not requested before trying to select queues.");

        for (int i = 0; i < m_queues.size(); ++i)
        {
            int numSelected = 1;
            if (chooseAllAvailableQueues)
                numSelected = m_queues[i].getNumAvailable();
            this->m_queues[i].setNumSelected(numSelected);
        }
        m_selectedQueues = true;
    }

    VknResult VknPhysicalDevice::enumeratePhysicalDevices()
    {
        if (s_enumeratedPhysicalDevices)
            throw std::runtime_error("Already enumerated physical devices.");
        VknResult res1{vkEnumeratePhysicalDevices(m_engine->getInstance(), &s_deviceCount, nullptr),
                       "Enumerate physical devices."};

        if (s_deviceCount == 0)
            throw std::runtime_error("No GPU's supporting Vulkan found.");
        else if (s_deviceCount > 1)
            std::cerr << "Found more than one GPU supporting Vulkan. Selecting device at index 0." << std::endl;

        s_physicalDevices.resize(s_deviceCount);
        VknResult res2{vkEnumeratePhysicalDevices(m_engine->getInstance(), &s_deviceCount,
                                                  s_physicalDevices.data()),
                       "Enum physical devices and store."};
        s_enumeratedPhysicalDevices = true;
        return res2;
    }

    VknResult VknPhysicalDevice::selectPhysicalDevice()
    {
        if (m_selectedPhysicalDevice)
            throw std::runtime_error("Already selected a physical device.");
        VknResult res("Enumerate physical devices.");
        if (!s_enumeratedPhysicalDevices)
            res = this->enumeratePhysicalDevices();

        m_relIdxs.physicalDeviceIdx = 0;
        m_selectedPhysicalDevice = true;
        this->queryProperties();

        return res;
    }

    bool VknPhysicalDevice::getSurfaceSupport(VkSurfaceKHR &surface, uint32_t queueFamilyIdx)
    {
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before getting surface support.");

        VkBool32 presentSupport = false;
        VknResult res{
            vkGetPhysicalDeviceSurfaceSupportKHR(
                s_physicalDevices[m_relIdxs.physicalDeviceIdx.value()], queueFamilyIdx, surface, &presentSupport),
            "Get Surface Support"};
        return presentSupport;
    }

    VkPhysicalDeviceLimits *VknPhysicalDevice::getLimits()
    {
        {
            if (!m_selectedPhysicalDevice)
                throw std::runtime_error("Physical device not selected before getting device limits.");
            return &s_properties[m_relIdxs.physicalDeviceIdx.value()].limits;
        }
    }

    void VknPhysicalDevice::queryProperties()
    {
        if (!s_enumeratedPhysicalDevices)
            throw std::runtime_error("Physical devices not enumerated before getting physical device properties.");
        for (auto &physicalDevice : s_physicalDevices)
        {
            s_properties.push_back(VkPhysicalDeviceProperties{});
            vkGetPhysicalDeviceProperties(physicalDevice, &s_properties.back());
        }
    }

    VkPhysicalDevice *VknPhysicalDevice::getVkPhysicalDevice()
    {
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before getting VkPhysicalDevice.");
        return &m_engine->getObject<VkPhysicalDevice>(m_absIdxs.physicalDeviceIdx.value());
    }

    VkPhysicalDeviceProperties &VknPhysicalDevice::getProperties()
    {
        if (!s_enumeratedPhysicalDevices)
            throw std::runtime_error("Physical devices not enumerated before getting physical device properties.");
        return s_properties[m_relIdxs.physicalDeviceIdx.value()];
    }

    std::vector<VkPhysicalDeviceProperties> &VknPhysicalDevice::getAllProperties()
    {
        if (!s_enumeratedPhysicalDevices)
            throw std::runtime_error("Physical device not enumerated before getting physical device properties vector.");
        return s_properties;
    }

}