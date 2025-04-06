#include "include/VknPhysicalDevice.hpp"

namespace vkn
{
    std::vector<VkPhysicalDeviceProperties> VknPhysicalDevice::s_properties{};
    bool VknPhysicalDevice::s_enumeratedPhysicalDevices{false};

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
            m_infos->fillDeviceQueuePriorities(
                m_relIdxs, i, std::vector<float>(getListElement<VknQueueFamily>(i, m_queues)->getNumSelected(), 1.0f));
    }

    void VknPhysicalDevice::requestQueueFamilyProperties()
    {
        if (m_requestedQueues)
            throw std::runtime_error("Queue properties already requested.");
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before requesting queue properties.");

        uint32_t propertyCount{0};
        vkGetPhysicalDeviceQueueFamilyProperties(
            m_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            &propertyCount, VK_NULL_HANDLE);
        if (propertyCount == 0)
            throw std::runtime_error("No available queue families found.");

        std::vector<VkQueueFamilyProperties> *engineQueues = &m_engine->getVector<VkQueueFamilyProperties>();
        m_startAbsIdx = engineQueues->size();
        for (int i = 0; i < propertyCount; ++i)
            addNewVknObject<VknQueueFamily, VkQueueFamilyProperties>(
                i, m_queues, m_engine, m_relIdxs, m_absIdxs, m_infos);

        vkGetPhysicalDeviceQueueFamilyProperties(
            *this->getVkPhysicalDevice(),
            &propertyCount,
            engineQueues->data() + m_startAbsIdx);
        m_requestedQueues = true;
    }

    void VknPhysicalDevice::fillQueueCreateInfos()
    {
        if (m_filledQueueCreateInfos)
            throw std::runtime_error("Already filled queue create infos.");
        for (int i = 0; i < m_queues.size(); ++i)
            m_infos->fillDeviceQueueCreateInfo(
                m_relIdxs, i, getListElement<VknQueueFamily>(i, m_queues)->getNumSelected());
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
                numSelected = getListElement<VknQueueFamily>(i, m_queues)->getNumAvailable();
            getListElement<VknQueueFamily>(i, m_queues)->setNumSelected(numSelected);
        }
        m_selectedQueues = true;
    }

    VknResult VknPhysicalDevice::enumeratePhysicalDevices()
    {
        if (s_enumeratedPhysicalDevices)
            throw std::runtime_error("Already enumerated physical devices.");
        uint32_t deviceCount{};
        VknResult res1{vkEnumeratePhysicalDevices(
                           m_engine->getObject<VkInstance>(0), &deviceCount, nullptr),
                       "Enumerate physical devices."};

        // What I'm doing : get the vector of physdevices, query the properties at the end.
        if (deviceCount == 0)
            throw std::runtime_error("No GPU's supporting Vulkan found.");
        else if (deviceCount > 1)
            std::cerr << "Found more than one GPU supporting Vulkan. Selecting device at index 0." << std::endl;
        std::vector<VkPhysicalDevice> *physDevices = &m_engine->getVector<VkPhysicalDevice>();
        physDevices->resize(deviceCount);
        VknResult res2{vkEnumeratePhysicalDevices(
                           m_engine->getObject<VkInstance>(0), &deviceCount, physDevices->data()),
                       "Enum physical devices and store."};

        s_properties.resize(deviceCount);
        for (auto &vkDevice : *physDevices)
            vkGetPhysicalDeviceProperties(
                m_engine->getObject<VkPhysicalDevice>(0), s_properties.data());
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

        m_relIdxs.add<VkPhysicalDevice>(0);
        m_selectedPhysicalDevice = true;

        return res;
    }

    bool VknPhysicalDevice::getSurfaceSupport(VkSurfaceKHR &surface, uint32_t queueFamilyIdx)
    {
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before getting surface support.");

        VkBool32 presentSupport = false;
        VknResult res{
            vkGetPhysicalDeviceSurfaceSupportKHR(
                *this->getVkPhysicalDevice(),
                queueFamilyIdx, surface, &presentSupport),
            "Get Surface Support"};
        return presentSupport;
    }

    VkPhysicalDeviceLimits *VknPhysicalDevice::getLimits()
    {
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before getting device limits.");
        return &s_properties[m_relIdxs.get<VkPhysicalDevice>()].limits;
    }

    VkPhysicalDevice *VknPhysicalDevice::getVkPhysicalDevice()
    {
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before getting VkPhysicalDevice.");
        return &m_engine->getObject<VkPhysicalDevice>(m_absIdxs);
    }

    VkPhysicalDeviceProperties &VknPhysicalDevice::getProperties()
    {
        if (!s_enumeratedPhysicalDevices)
            throw std::runtime_error("Physical devices not enumerated before getting physical device properties.");
        return s_properties[m_relIdxs.get<VkPhysicalDevice>()];
    }

    std::vector<VkPhysicalDeviceProperties> &VknPhysicalDevice::getAllProperties()
    {
        if (!s_enumeratedPhysicalDevices)
            throw std::runtime_error("Physical device not enumerated before getting physical device properties vector.");
        return s_properties;
    }

}