#include <iostream>

#include "VknConfig.hpp"

namespace vkn
{
    VknConfig::VknConfig()
        : m_resultArchive(VknResultArchive{}),
          m_infos(VknInfos{})
    {
        m_device = VknDevice(&m_infos, &m_resultArchive);
        m_renderPass = VknRenderPass{&m_infos, &m_resultArchive};
    }

    VknConfig::~VknConfig()
    {
        this->destroy();
    }

    void VknConfig::destroy()
    {
        if (!m_destroyed)
        {
            m_renderPass.destroy();
            m_device.destroy();
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

    void VknConfig::deviceInfo()
    {
        this->fillAppInfo(VK_API_VERSION_1_1, "DeviceInfo", "VknConfig");
        this->createInstance();
        this->createDevice();
    }

    void VknConfig::fillAppInfo(uint32_t apiVersion, std::string appName,
                                std::string engineName, VkApplicationInfo *pNext,
                                uint32_t appVersion, uint32_t engineVersion)
    {
        m_infos.fillAppInfo(apiVersion, appName, engineName, pNext, appVersion, engineVersion);
    }

    void VknConfig::fillInstanceCreateInfo(std::vector<const char *> &enabledLayerNames,
                                           std::vector<const char *> &enabledExtensionNames,
                                           VkInstanceCreateInfo *pNext,
                                           VkInstanceCreateFlags flags)
    {
        m_infos.fillInstanceCreateInfo(
            enabledLayerNames, enabledExtensionNames, pNext, flags);
    }

    VknResult VknConfig::createInstance()
    {
        VknResult res{vkCreateInstance(m_infos.getInstanceCreateInfo(), nullptr, &m_instance),
                      "Create instance."};
        if (!(res.isSuccess()))
            throw std::runtime_error(res.toErr("Error creating instance."));
        m_resultArchive.store(res);
        m_device.addInstance(&m_instance);

        m_instanceCreated = true;
        return res;
    }

    void VknConfig::selectPhysicalDevice()
    {
        if (!m_instanceCreated)
            throw std::runtime_error("Instance not created before trying to select a physical device.");
        else
        {
            m_device.getPhysicalDevice()->selectPhysicalDevice();
            m_physicalDeviceSelected = true;
        }
    }

    void VknConfig::requestQueueFamilies()
    {
        if (!m_instanceCreated)
            throw std::runtime_error("Instance not created before trying to request queue families.");
        else if (!m_physicalDeviceSelected)
            throw std::runtime_error("Physical device not selected before trying to request queue families.");
        else
        {
            m_device.requestQueueFamilyProperties();
            m_queueFamiliesRequested = true;
        }
    }

    VknRenderPass *VknConfig::getRenderPass()
    {
        if (!m_instanceCreated && !m_renderPass.deviceAdded())
            throw std::runtime_error("Device not added to renderpass before retrieval.");
        return &m_renderPass;
    }

    VknResult VknConfig::createDevice(bool chooseAllAvailableQueues)
    {
        if (!m_instanceCreated)
            throw std::runtime_error("Instance not created before trying to create device.");
        for (int i = 0; i < m_device.getQueues().size(); ++i)
        {
            int numSelected = 1;
            if (chooseAllAvailableQueues)
                numSelected = m_device.getQueue(i).getNumAvailable();
            m_infos.fillDeviceQueueCreateInfo(i, numSelected);
            m_device.getQueue(i).setNumSelected(numSelected);
        }

        m_device.fillDeviceCreateInfo();
        VknResult res;
        if (!(res = m_device.createDevice()).isSuccess())
            throw std::runtime_error(res.toErr("Error creating device."));
        m_resultArchive.store(res);
        m_renderPass.addDevice(&m_device);
        return res;
    }

    void VknConfig::archiveResult(VknResult res)
    {
        m_resultArchive.store(res);
    }
}