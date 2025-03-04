#include "VknConfig.hpp"

namespace vkn
{
    VknConfig::VknConfig()
        : m_resultArchive(VknResultArchive{}),
          m_infos(VknInfos{})
    {
        m_devices.push_back(VknDevice{m_numDevices++, &m_infos, &m_resultArchive, &m_instance, &m_instanceCreated});
    }

    VknConfig::~VknConfig()
    {
        this->destroy();
    }

    void VknConfig::destroy()
    {
        if (!m_destroyed)
        {
            for (auto &device : m_devices)
                device.destroy();
            if (m_instanceCreated)
                vkDestroyInstance(m_instance, nullptr);
            m_destroyed = true;
        }
        std::cout << "VknConfig DESTROYED." << std::endl;
    }

    void VknConfig::addDevice(uint32_t deviceIdx)
    {
        if (deviceIdx != m_numDevices)
            throw std::runtime_error("Device index should equal the current numDevices.");
        m_devices.push_back(VknDevice{m_numDevices++, &m_infos, &m_resultArchive, &m_instance, &m_instanceCreated});
    }

    void VknConfig::enableExtensions(std::vector<std::string> extensions)
    {
        for (auto &name : extensions)
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

    void VknConfig::selectPhysicalDevice(uint32_t deviceIdx)
    {
        if (!m_instanceCreated)
            throw std::runtime_error("Instance not created before trying to select a physical device.");
        else
        {
            this->getDevice(deviceIdx)->getPhysicalDevice()->selectPhysicalDevice();
            m_physicalDeviceSelected = true;
        }
    }

    void VknConfig::requestQueueFamilies(uint32_t deviceIdx)
    {
        this->getDevice(deviceIdx)->requestQueueFamilyProperties();
        m_queueFamiliesRequested = true;
    }

    VknRenderPass *VknConfig::getRenderPass(uint32_t deviceIdx, uint32_t renderPassIdx)
    {
        VknRenderPass *renderPass{this->getDevice(deviceIdx)->getRenderPass(renderPassIdx)};
        if (!(renderPass->getVkRenderPassCreated()))
            throw std::runtime_error("RenderPass not created before getting renderpass.");

        return renderPass;
    }

    void VknConfig::selectQueues(uint32_t deviceIdx, bool chooseAllAvailableQueues)
    {
        if (m_queuesSelected)
            throw std::runtime_error("Queues already selected.");
        if (!m_queueFamiliesRequested)
            throw std::runtime_error("Queue families not requested before trying to select queues.");

        for (int i = 0; i < this->getDevice(deviceIdx)->getNumQueueFamilies(); ++i)
        {
            int numSelected = 1;
            if (chooseAllAvailableQueues)
                numSelected = this->getDevice(deviceIdx)->getQueue(i).getNumAvailable();
            // m_infos.fillDeviceQueueCreateInfo(deviceIdx, i, numSelected);
            this->getDevice(deviceIdx)->getQueue(i).setNumSelected(numSelected);
        }
        m_queuesSelected = true;
    }

    // TODO: move more of setup for this to VknDevice, including the top half of this function
    VknResult VknConfig::createDevice(uint32_t deviceIdx, bool chooseAllAvailableQueues)
    {
        if (!m_queuesSelected)
            throw std::runtime_error("Queues not selected before trying to create device.");

        std::vector<VknQueueFamily> queues = this->getDevice(deviceIdx)->getQueues();
        VknDevice *device = this->getDevice(deviceIdx);
        for (int i = 0; i < queues.size(); ++i)
            m_infos.fillDeviceQueueCreateInfo(deviceIdx, i, queues[i].getNumSelected());
        device->fillDeviceCreateInfo();
        VknResult res;
        if (!(res = device->createDevice()).isSuccess())
            throw std::runtime_error(res.toErr("Error creating device."));
        m_resultArchive.store(res);
        return res;
    }

    void VknConfig::archiveResult(VknResult res)
    {
        m_resultArchive.store(res);
    }

    VknDevice *VknConfig::getDevice(uint32_t deviceIdx)
    {
        if (deviceIdx >= m_numDevices)
            throw std::runtime_error("Device index out of range.");
        std::list<VknDevice>::iterator it = m_devices.begin();
        std::advance(it, deviceIdx);
        return &(*it);
    }
}