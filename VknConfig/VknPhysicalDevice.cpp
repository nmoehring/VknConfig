#include "include/VknPhysicalDevice.hpp"

namespace vkn
{
    VknVector<VkPhysicalDeviceProperties> VknPhysicalDevice::s_properties{};
    bool VknPhysicalDevice::s_enumeratedPhysicalDevices{false};

    VknPhysicalDevice::VknPhysicalDevice(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknPhysicalDevice::fillDeviceQueuePriorities(uint32_t queueFamilyIdx, VknVector<float> priorities)
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
        {
            VknVector<float> newVec{};
            newVec.append(1.0f, getListElement<VknQueueFamily>(i, m_queues)->getNumSelected());
            m_infos->fillDeviceQueuePriorities(
                m_relIdxs, i, newVec);
        }
    }

    void VknPhysicalDevice::requestQueueFamilyProperties()
    {
        if (m_requestedQueues)
            throw std::runtime_error("Queue properties already requested.");
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before requesting queue properties.");

        size_t oldVectorSize{m_engine->getVectorSize<VkQueueFamilyProperties>()};
        uint32_t propertyCount{0};
        vkGetPhysicalDeviceQueueFamilyProperties(
            m_engine->getObject<VkPhysicalDevice>(m_absIdxs),
            &propertyCount, VK_NULL_HANDLE);
        if (propertyCount == 0)
            throw std::runtime_error("No available queue families found.");

        for (int i = 0; i < propertyCount; ++i)
            m_engine->addNewVknObject<VknQueueFamily, VkQueueFamilyProperties, VkDevice>(
                i, m_queues, m_relIdxs, m_absIdxs, m_infos);

        VknVectorIterator<VkQueueFamilyProperties> queuesIterator{
            m_engine->getVectorSlice<VkQueueFamilyProperties>(oldVectorSize, propertyCount)};

        vkGetPhysicalDeviceQueueFamilyProperties(
            *this->getVkPhysicalDevice(),
            &propertyCount,
            queuesIterator.getData());
        m_requestedQueues = true;
    }

    void VknPhysicalDevice::fillQueueCreateInfos()
    {
        if (m_filledQueueCreateInfos)
            throw std::runtime_error("Already filled queue create infos.");
        for (int i = 0; i < m_queues.size(); ++i)
        {
            VknQueueFamily *fam = getListElement<VknQueueFamily>(i, m_queues);
            m_infos->fillDeviceQueueCreateInfo(
                m_relIdxs, i, fam->getNumSelected(), fam->getFlags());
        }
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
                           m_engine->getObject<VkInstance>(0u), &deviceCount, nullptr),
                       "Enumerate physical devices."};

        // What I'm doing : get the vector of physdevices, query the properties at the end.
        if (deviceCount == 0u)
            throw std::runtime_error("No GPU's supporting Vulkan found.");
        else if (deviceCount > 1u)
            std::cerr << "Found more than one GPU supporting Vulkan. Selecting device at index 0." << std::endl;
        VknVector<VkPhysicalDevice> *physDevices = &m_engine->getVector<VkPhysicalDevice>();
        physDevices->resize(deviceCount);
        VknResult res2{vkEnumeratePhysicalDevices(
                           m_engine->getObject<VkInstance>(0u), &deviceCount,
                           physDevices->getData()),
                       "Enum physical devices and store."};

        s_properties.clear();
        for (auto &vkDevice : *physDevices)
            vkGetPhysicalDeviceProperties(vkDevice, s_properties.getData(1));
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
        return &s_properties(m_relIdxs.get<VkPhysicalDevice>()).limits;
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
        return s_properties(m_relIdxs.get<VkPhysicalDevice>());
    }

    VknVector<VkPhysicalDeviceProperties> &VknPhysicalDevice::getAllProperties()
    {
        if (!s_enumeratedPhysicalDevices)
            throw std::runtime_error("Physical device not enumerated before getting physical device properties vector.");
        return s_properties;
    }

}