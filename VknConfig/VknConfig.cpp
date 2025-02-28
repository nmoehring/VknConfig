#include "VknConfig.hpp"

namespace vkn
{
    VknConfig::VknConfig()
        : m_resultArchive(VknResultArchive{}),
          m_infos(VknInfos{})
    {
        m_devices.push_back(VknDevice(&m_infos, &m_resultArchive, &m_instance, &m_instanceCreated));
    }

    VknConfig::~VknConfig()
    {
        this->destroy();
    }

    void VknConfig::destroy()
    {
        if (!m_destroyed)
        {
            for (auto device : m_devices)
                device.destroy();
            if (m_instanceCreated)
                vkDestroyInstance(m_instance, nullptr);
            m_destroyed = true;
        }
    }

    void VknConfig::enableExtensions(std::vector<std::string> extensions)
    {
        for (auto name : extensions)
            m_instanceExtensions.push_back(name);
    }

    void VknConfig::deviceInfo(uint32_t deviceIdx)
    {
        this->fillAppInfo(VK_API_VERSION_1_1, "DeviceInfo", "VknConfig");
        this->createInstance();
        this->createDevice(deviceIdx);
    }

    void VknConfig::fillAppInfo(uint32_t apiVersion, std::string appName,
                                std::string engineName, VkApplicationInfo *pNext,
                                uint32_t appVersion, uint32_t engineVersion)
    {
        m_infos.fillAppName(appName);
        m_infos.fillEngineName(engineName);
        m_infos.fillAppInfo(apiVersion, appVersion, engineVersion);
    }

    void VknConfig::fillInstanceCreateInfo(const char *const *enabledLayerNames,
                                           uint32_t enabledLayerNamesSize,
                                           const char *const *enabledExtensionNames,
                                           uint32_t enabledExtensionNamesSize,
                                           VkInstanceCreateFlags flags)
    {
        m_infos.fillEnabledLayerNames(enabledLayerNames, enabledLayerNamesSize);
        m_infos.fillInstanceExtensionNames(enabledExtensionNames, enabledExtensionNamesSize);
        m_infos.fillInstanceCreateInfo(flags);
    }

    VknResult VknConfig::createInstance()
    {
        VknResult res{vkCreateInstance(m_infos.getInstanceCreateInfo(), VK_NULL_HANDLE, &m_instance),
                      "Create instance."};
        if (!(res.isSuccess()))
            throw std::runtime_error(res.toErr("Error creating instance."));
        m_resultArchive.store(res);

        m_instanceCreated = true;
        return res;
    }

    void VknConfig::selectPhysicalDevice(uint32_t index)
    {
        if (!m_instanceCreated)
            throw std::runtime_error("Instance not created before trying to select a physical device.");
        else
        {
            m_devices[index].getPhysicalDevice()->selectPhysicalDevice();
            m_physicalDeviceSelected = true;
        }
    }

    void VknConfig::requestQueueFamilies(uint32_t index)
    {
        m_devices[index].requestQueueFamilyProperties();
        m_queueFamiliesRequested = true;
    }

    VknRenderPass *VknConfig::getRenderPass(uint32_t deviceIdx, uint32_t renderPassIdx)
    {
        VknRenderPass *renderPass{m_devices[deviceIdx].getRenderPass(renderPassIdx)};
        if (!(renderPass->getVkRenderPassCreated()))
            throw std::runtime_error("RenderPass not created before getting renderpass.");

        return renderPass;
    }

    void VknConfig::selectQueues(uint32_t deviceIdx, bool chooseAllAvailableQueues)
    {
        if (!m_queueFamiliesRequested)
            throw std::runtime_error("Queue families not requested before trying to select queues.");

        for (int i = 0; i < m_devices[deviceIdx].getNumQueueFamilies(); ++i)
        {
            int numSelected = 1;
            if (chooseAllAvailableQueues)
                numSelected = m_devices[deviceIdx].getQueue(i).getNumAvailable();
            // m_infos.fillDeviceQueueCreateInfo(deviceIdx, i, numSelected);
            m_devices[deviceIdx].getQueue(i).setNumSelected(numSelected);
        }
        m_queuesSelected = true;
    }

    // TODO: move more of setup for this to VknDevice, including the top half of this function
    VknResult VknConfig::createDevice(uint32_t deviceIdx, bool chooseAllAvailableQueues)
    {
        if (!m_queuesSelected)
            throw std::runtime_error("Queues not selected before trying to create device.");

        std::vector<VknQueueFamily> queues = m_devices[deviceIdx].getQueues();
        VknDevice &device = m_devices[deviceIdx];
        for (int i = 0; i < queues.size(); ++i)
            m_infos.fillDeviceQueueCreateInfo(deviceIdx, i, queues[i].getNumSelected());
        device.fillDeviceCreateInfo();
        VknResult res;
        if (!(res = device.createDevice()).isSuccess())
            throw std::runtime_error(res.toErr("Error creating device."));
        m_resultArchive.store(res);
        return res;
    }

    void VknConfig::archiveResult(VknResult res)
    {
        m_resultArchive.store(res);
    }
}