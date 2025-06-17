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

    VknPhysicalDevice::~VknPhysicalDevice()
    {
        s_enumeratedPhysicalDevices = false;
        s_properties.clear();
    }

    void VknPhysicalDevice::fileDeviceQueuePriorities(uint32_t queueFamilyIdx, VknVector<float> priorities)
    {
        if (m_filedQueuePriorities)
            throw std::runtime_error("Already filed queue priorities.");
        if (!m_selectedQueues)
            this->selectQueues(true);
        m_infos->fileDeviceQueuePriorities(m_relIdxs, queueFamilyIdx, priorities);
        m_filedQueuePriorities = true;
    }

    void VknPhysicalDevice::fileDeviceQueuePrioritiesDefault()
    {
        if (m_filedQueuePriorities)
            throw std::runtime_error("Already filed queue priorities.");
        if (!m_selectedQueues)
            this->selectQueues(true);
        for (int i = 0; i < m_queues.size(); ++i)
        {
            VknVector<float> newVec{};
            newVec.append(1.0f, getListElement<VknQueueFamily>(i, m_queues)->getNumSelected());
            m_infos->fileDeviceQueuePriorities(
                m_relIdxs, i, newVec);
        }
        m_filedQueuePriorities = true;
    }

    void VknPhysicalDevice::requestQueueFamilyProperties()
    {
        if (m_requestedQueues)
            throw std::runtime_error("Queue properties already requested.");

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

    void VknPhysicalDevice::fileQueueCreateInfos()
    {
        if (m_filedQueueCreateInfos)
            throw std::runtime_error("Already filed queue create infos.");
        for (int i = 0; i < m_queues.size(); ++i)
        {
            VknQueueFamily *fam = getListElement<VknQueueFamily>(i, m_queues);
            m_infos->fileDeviceQueueCreateInfo(
                m_relIdxs, i, fam->getNumSelected(), fam->getFlags());
        }
        m_filedQueueCreateInfos = true;
    }

    void VknPhysicalDevice::selectQueues(bool chooseAllAvailableQueues)
    {
        if (m_selectedQueues)
            throw std::runtime_error("Queues already selected.");
        if (!m_selectedPhysicalDevice)
            this->selectPhysicalDevice();

        for (int i = 0; i < m_queues.size(); ++i)
        {
            int numSelected = 1;
            if (chooseAllAvailableQueues)
                numSelected = getListElement<VknQueueFamily>(i, m_queues)->getNumAvailable();
            getListElement<VknQueueFamily>(i, m_queues)->setNumSelected(numSelected);
        }
        m_selectedQueues = true;
    }

    void VknPhysicalDevice::enumeratePhysicalDevices()
    {
        if (!s_enumeratedPhysicalDevices)
        {
            uint32_t deviceCount{};
            VknResult res1{vkEnumeratePhysicalDevices(
                               m_engine->getObject<VkInstance>(0u), &deviceCount, nullptr),
                           "Enumerate physical devices."};

            if (deviceCount == 0u)
                throw std::runtime_error("No GPU's supporting Vulkan found.");
            VknVector<VkPhysicalDevice> *physDevices = &m_engine->getVector<VkPhysicalDevice>();
            VknResult res2{vkEnumeratePhysicalDevices(
                               m_engine->getObject<VkInstance>(0u), &deviceCount,
                               physDevices->getData()),
                           "Enum physical devices and store."};
            for (auto &vkDevice : *physDevices)
                vkGetPhysicalDeviceProperties(vkDevice, s_properties.getData(1));

            s_enumeratedPhysicalDevices = true;
        } // if (!s_enumeratedPhysicalDevices)
    }

    VkPhysicalDevice *VknPhysicalDevice::selectPhysicalDevice()
    {
        if (m_selectedPhysicalDevice)
            throw std::runtime_error("Already selected a physical device.");

        this->enumeratePhysicalDevices();

        uint_fast32_t bestScore{0};
        uint_fast32_t bestIdx{UINT32_MAX};
        uint_fast32_t score{0};
        VknVector<VkPhysicalDevice> &allPhysicalDevices = m_engine->getVector<VkPhysicalDevice>();
        VkPhysicalDevice currentDevice{};
        VkPhysicalDeviceProperties currentProps{};
        bool graphicsNeeded = m_engine->getVectorSize<VkSurfaceKHR>();
        for (uint_fast32_t i{0}; i < allPhysicalDevices.getSize(); ++i)
        {
            score = 0;
            currentDevice = allPhysicalDevices(i);
            currentProps = s_properties(i);

            if (graphicsNeeded)
                for (uint_fast32_t q_idx = 0; q_idx < m_engine->getVectorSize<VkQueueFamilyProperties>(); ++q_idx)
                {
                    if (m_engine->getObject<VkQueueFamilyProperties>(q_idx).queueFlags & VK_QUEUE_GRAPHICS_BIT) // Must support graphics
                    {
                        VkBool32 presentSupport = VK_FALSE;
                        vkGetPhysicalDeviceSurfaceSupportKHR(currentDevice, q_idx, m_engine->getObject<VkSurfaceKHR>(0), &presentSupport);
                        if (presentSupport)
                            score += 128;
                    }
                }
            else // graphics not needed
                if (currentProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                    score += 128;
                else if (currentProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                    score += 64;
                else if (currentProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
                    score += 32;
                else if (currentProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)
                    score += 16;
                else
                    score += 0;
            if (score > bestScore)
            {
                bestScore = score;
                bestIdx = i;
            }
        }

        if (bestIdx == UINT32_MAX)
            throw std::runtime_error("Failed to find a suitable GPU that supports presentation to the given surface.");

        m_absIdxs.add<VkPhysicalDevice>(bestIdx); // This is the index in VknEngine's global list
        // m_relIdxs.add<VkPhysicalDevice>(0); // Assuming VknDevice has only one VknPhysicalDevice, its relative index is 0.
        // This depends on how VknIdxs is used for VknPhysicalDevice within VknDevice.
        // If VknDevice stores VknPhysicalDevice in a list, this index would be its position in that list.
        // For now, let's assume it's always the first/only one for its parent VknDevice.
        m_selectedPhysicalDevice = true;
        this->requestQueueFamilyProperties();

        return this->getVkPhysicalDevice();
    }

    bool VknPhysicalDevice::getSurfaceSupport(VkSurfaceKHR surface, uint32_t queueFamilyIdx)
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

    VknQueueFamily &VknPhysicalDevice::getQueue(int idx)
    {
        if (!m_selectedQueues)
            throw std::runtime_error("Queues not selected before getting queue.");
        return *getListElement<VknQueueFamily>(idx, m_queues);
    }

    bool VknPhysicalDevice::areQueuePrioritiesfiled()
    {
        return m_filedQueuePriorities;
    }

    VkPhysicalDeviceType VknPhysicalDevice::getGPUType()
    {
        return s_properties(m_relIdxs.get<VkPhysicalDevice>()).deviceType;
    }
}