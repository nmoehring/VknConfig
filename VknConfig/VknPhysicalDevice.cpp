#include "include/VknPhysicalDevice.hpp"

namespace vkn
{
    // Static member definitions
    VknVector<VkPhysicalDeviceProperties> VknPhysicalDevice::s_properties{};
    VknVector<uint_fast32_t> VknPhysicalDevice::s_deviceQueuePropStartPos{};
    VknVector<uint_fast32_t> VknPhysicalDevice::s_numQueueFamilies{};
    std::list<VknQueueFamily> VknPhysicalDevice::s_queues{};
    uint_fast32_t s_physDevCount{0u};
    bool VknPhysicalDevice::s_requestedQueues{false};
    bool VknPhysicalDevice::s_enumeratedPhysicalDevices{false};

    VknPhysicalDevice::VknPhysicalDevice(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        ++s_physDevCount;
    }

    VknPhysicalDevice::~VknPhysicalDevice()
    {
        if (--s_physDevCount == 0)
        {
            s_properties.clear();
            s_deviceQueuePropStartPos.clear();
            s_numQueueFamilies.clear();
            s_queues.clear();
            s_requestedQueues = false;
            s_enumeratedPhysicalDevices = false;
        }
    }

    void VknPhysicalDevice::fileDeviceQueuePriorities(uint32_t queueFamilyIdx, VknVector<float> priorities)
    {
        if (m_filedQueuePriorities)
            throw std::runtime_error("Already filed queue priorities.");
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before setting queue priorities.");
        m_infos->fileDeviceQueuePriorities(m_relIdxs, queueFamilyIdx, priorities);
        m_filedQueuePriorities = true;
    }

    void VknPhysicalDevice::fileDeviceQueuePrioritiesDefault()
    {
        if (m_filedQueuePriorities)
            throw std::runtime_error("Already filed queue priorities.");
        if (!m_selectedPhysicalDevice)
            this->selectPhysicalDevice();
        uint_fast32_t startPos{s_deviceQueuePropStartPos(m_relIdxs.get<VkPhysicalDevice>())};
        VknVector<float> queueFamilyPriorities{};
        for (uint_fast32_t i = startPos; i < s_numQueueFamilies(m_relIdxs.get<VkPhysicalDevice>()) + startPos; ++i)
        {
            queueFamilyPriorities.clear();
            queueFamilyPriorities.appendRepeat(1.0f, getListElement<VknQueueFamily>(i, s_queues)->getNumSelected());
            m_infos->fileDeviceQueuePriorities(
                m_relIdxs, i, queueFamilyPriorities);
        }
        m_filedQueuePriorities = true;
    }

    void VknPhysicalDevice::requestQueueFamilyProperties()
    {
        if (s_requestedQueues)
            throw std::runtime_error("Queue properties already requested.");

        uint_fast32_t familyPropertyCount{0};
        VknVector<VkPhysicalDevice> &physDevices = m_engine->getVector<VkPhysicalDevice>();
        for (uint_fast32_t i = 0; i < physDevices.getSize(); ++i)
        {
            vkGetPhysicalDeviceQueueFamilyProperties(physDevices(i), &familyPropertyCount, nullptr);
            if (familyPropertyCount == 0)
                throw std::runtime_error("No available queue families found.");

            s_deviceQueuePropStartPos.appendOne(m_engine->addNewVknObjects<VknQueueFamily, VkQueueFamilyProperties>(
                familyPropertyCount, s_queues, m_relIdxs, m_absIdxs, m_infos));
            s_numQueueFamilies.appendOne(familyPropertyCount);

            VknVectorIterator<VkQueueFamilyProperties> queuesIterator{
                m_engine->getVectorSlice<VkQueueFamilyProperties>(s_deviceQueuePropStartPos(i), familyPropertyCount)};

            vkGetPhysicalDeviceQueueFamilyProperties(
                m_engine->getObject<VkPhysicalDevice>(i),
                &familyPropertyCount,
                queuesIterator.getData());

            for (auto &queue : s_queues)
                queue.setNumSelected(queue.getNumAvailable());
        }
        s_requestedQueues = true;
    }

    void VknPhysicalDevice::fileQueueCreateInfos()
    {
        if (m_filedQueueCreateInfos)
            throw std::runtime_error("Already filed queue create infos.");
        for (int i = 0; i < s_queues.size(); ++i)
        {
            VknQueueFamily *fam = getListElement<VknQueueFamily>(i, s_queues);
            m_infos->fileDeviceQueueCreateInfo(
                m_relIdxs, i, fam->getNumSelected(), fam->getFlags());
        }
        m_filedQueueCreateInfos = true;
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
        if (!s_enumeratedPhysicalDevices)
            this->enumeratePhysicalDevices();
        if (!s_requestedQueues)
            this->requestQueueFamilyProperties();

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
        if (!m_selectedPhysicalDevice)
            throw std::runtime_error("Physical device not selected before getting queue.");
        return *getListElement<VknQueueFamily>(idx, s_queues);
    }

    bool VknPhysicalDevice::areQueuePrioritiesFiled()
    {
        return m_filedQueuePriorities;
    }

    VkPhysicalDeviceType VknPhysicalDevice::getGPUType()
    {
        return s_properties(m_relIdxs.get<VkPhysicalDevice>()).deviceType;
    }
}