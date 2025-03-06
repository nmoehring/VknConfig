#include "VknDevice.hpp"

namespace vkn
{
    int VknDevice::s_numDevices{0};

    VknDevice::VknDevice()
        : m_infos{nullptr}, m_resultArchive{nullptr}, m_instance{nullptr}, m_instanceCreated{nullptr},
          m_placeholder{true}
    {
        m_physicalDevice = VknPhysicalDevice{m_resultArchive, m_infos, m_instance, m_instanceCreated};
        m_deviceIdx = s_numDevices;
    }

    VknDevice::VknDevice(uint32_t deviceIdx, VknInfos *infos, VknResultArchive *archive,
                         const VkInstance *instance, const bool *instanceCreated)
        : m_infos{infos}, m_resultArchive{archive}, m_instance{instance}, m_instanceCreated{instanceCreated},
          m_deviceIdx{deviceIdx}, m_placeholder{false}
    {
        m_physicalDevice = VknPhysicalDevice{m_resultArchive, m_infos, m_instance, m_instanceCreated};
    }

    VknDevice::~VknDevice()
    {
        if (!m_destroyed && !m_placeholder)
            this->destroy();
    }

    void VknDevice::destroy()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to destroy a placeholder object.");

        if (m_destroyed)
            throw std::runtime_error("Device object already destroyed.");

        for (auto &renderpass : m_renderpasses)
            renderpass.destroy();
        if (m_vkDeviceCreated)
            vkDestroyDevice(m_logicalDevice, nullptr);
        if (m_swapchains.size() > 0)
            for (auto &swapchain : m_swapchains)
                swapchain.destroy();
        m_destroyed = true;
        std::cout << "VknDevice DESTROYED." << std::endl;
    }

    void VknDevice::addSwapchain(
        uint32_t swapchainIdx, VkSurfaceKHR surface, uint32_t imageCount, uint32_t imageWidth, uint32_t imageHeight)
    {
        m_swapchains.emplace_back(m_deviceIdx, swapchainIdx, &m_logicalDevice, &m_vkDeviceCreated, m_infos,
                                  m_resultArchive, 1, surface, imageWidth, imageHeight);
    }

    void VknDevice::fillSwapchainCreateInfos()
    {
        for (auto &swapchain : m_swapchains)
            swapchain.fillSwapchainCreateInfo();
    }

    VknSwapchain *VknDevice::getSwapchain(uint32_t swapchainIdx)
    {
        if (swapchainIdx + 1 > m_swapchains.size())
            throw std::runtime_error("GetSwapchain index out of range.");
        return &m_swapchains[swapchainIdx];
    }

    VknPhysicalDevice *VknDevice::getPhysicalDevice()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        return &m_physicalDevice;
    }

    VkDevice *VknDevice::getVkDevice()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_vkDeviceCreated)
            throw std::runtime_error("Logical device not created before retrieving it.");
        return &m_logicalDevice;
    }

    VknQueueFamily &VknDevice::getQueue(int idx)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_queuesRequested)
            throw std::runtime_error("Queue properties not requested before retrieving queue properties.");
        return m_queues[idx];
    }

    void VknDevice::fillDeviceQueuePriorities(uint32_t queueFamilyIdx, std::vector<float> priorities)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_queuesRequested)
            throw std::runtime_error("Queue properties not requested before filling queue priorities.");
        m_infos->fillDeviceQueuePriorities(m_deviceIdx, queueFamilyIdx, priorities);
    }

    void VknDevice::fillDeviceQueuePrioritiesDefault()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_queuesRequested)
            throw std::runtime_error("Queue properties not requested before filling queue priorities.");
        for (int i = 0; i < m_queues.size(); ++i)
            m_infos->fillDeviceQueuePriorities(m_deviceIdx, i, std::vector<float>(m_queues[i].getNumSelected(), 1.0f));
    }

    void VknDevice::archiveResult(VknResult res)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        m_resultArchive->store(res);
    }

    VknRenderpass *VknDevice::getRenderpass(uint32_t renderpassIdx)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (renderpassIdx >= m_numRenderpasses)
            throw std::runtime_error("Render pass index out of range.");
        std::list<VknRenderpass>::iterator it = m_renderpasses.begin();
        std::advance(it, renderpassIdx);
        return &(*it);
    }

    void VknDevice::addExtensions(const char *ext[], uint32_t size)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        m_extensions = ext;
        m_extensionsSize = size;
    }

    void VknDevice::requestQueueFamilyProperties()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_queuesRequested)
            throw std::runtime_error("Queue properties already requested.");
        if (!(m_physicalDevice.getPhysicalDeviceSelected()))
            throw std::runtime_error("Physical device not selected before requesting queue properties.");
        uint32_t propertyCount{0};
        vkGetPhysicalDeviceQueueFamilyProperties(
            *(m_physicalDevice.getVkPhysicalDevice()), &propertyCount, VK_NULL_HANDLE);
        if (propertyCount == 0)
            throw std::runtime_error("No available queue families found.");
        std::vector<VkQueueFamilyProperties> queues;
        queues.resize(propertyCount);
        if (m_queues.size() > 0)
            throw std::runtime_error("m_queues already filled before requesting queue properties.");

        vkGetPhysicalDeviceQueueFamilyProperties(
            *(m_physicalDevice.getVkPhysicalDevice()),
            &propertyCount,
            queues.data());
        for (auto &props : queues)
            m_queues.emplace_back(props);
        m_queuesRequested = true;
    }

    void VknDevice::selectQueues(bool chooseAllAvailableQueues)
    {
        if (m_queuesSelected)
            throw std::runtime_error("Queues already selected.");
        if (!m_queuesRequested)
            throw std::runtime_error("Queue families not requested before trying to select queues.");

        for (int i = 0; i < this->getNumQueueFamilies(); ++i)
        {
            int numSelected = 1;
            if (chooseAllAvailableQueues)
                numSelected = this->getQueue(i).getNumAvailable();
            this->getQueue(i).setNumSelected(numSelected);
        }
        m_queuesSelected = true;
    }

    VknResult VknDevice::createDevice()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_queuesSelected)
            throw std::runtime_error("Queues not selected before trying to create device.");
        m_infos->fillDeviceExtensionNames(m_deviceIdx, m_extensions, m_extensionsSize);
        m_infos->fillDeviceFeatures(m_features);
        this->fillQueueCreateInfos();
        m_infos->fillDeviceCreateInfo(m_deviceIdx);
        VknResult res{
            vkCreateDevice(
                *(m_physicalDevice.getVkPhysicalDevice()),
                m_infos->getDeviceCreateInfo(m_deviceIdx),
                nullptr,
                &m_logicalDevice),
            "Create device"};
        if (!(res.isSuccess()))
            throw std::runtime_error(res.toErr("Error creating device."));
        m_resultArchive->store(res);
        m_vkDeviceCreated = true;
        return res;
    }

    void VknDevice::fillQueueCreateInfos()
    {
        if (m_filledQueueCreateInfos)
            throw std::runtime_error("Already filled queue create infos.");
        for (int i = 0; i < m_queues.size(); ++i)
            m_infos->fillDeviceQueueCreateInfo(m_deviceIdx, i, m_queues[i].getNumSelected());
        m_filledQueueCreateInfos = true;
    }

    void VknDevice::addRenderpass(uint32_t renderpassIdx)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (renderpassIdx != m_numRenderpasses)
            throw std::runtime_error("RenderpassIdx passed to addRenderpass is invalid. Should be next idx.");
        m_renderpasses.emplace_back(m_deviceIdx, m_numRenderpasses++, m_infos, m_resultArchive,
                                    &m_logicalDevice, &m_vkDeviceCreated);
    }
} // namespace vkn