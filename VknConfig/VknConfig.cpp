#include <iostream>

#include "VknConfig.hpp"

namespace vkn
{
    VknConfig::VknConfig()
    {
    }

    VknConfig::~VknConfig()
    {
        m_pipeline.destroy();
        m_device.destroy();
        vkDestroyInstance(m_instance, nullptr);
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

    void VknConfig::fillInstanceCreateInfo()
    {
        m_infos.fillInstanceCreateInfo(
            nullptr, 0U, std::vector<std::string>{}, m_instanceExtensions);
    }

    VknResult VknConfig::createInstance()
    {
        VknResult res{vkCreateInstance(m_infos.getInstanceCreateInfo(), nullptr, &m_instance),
                      "Create instance."};
        if (!(res.isSuccess()))
            throw std::runtime_error(res.toErr("Error creating instance."));
        m_resultArchive.store(res);

        return res;
    }

    VknResult VknConfig::createDevice(bool chooseAllAvailableQueues)
    {
        m_device = {&m_instance, &m_infos, &m_resultArchive};
        for (int i = 0; i < m_device.getQueues().size(); ++i)
        {
            int numSelected = 1;
            if (chooseAllAvailableQueues)
                numSelected = m_device.getQueue(i).getNumAvailable();
            m_infos.fillDeviceQueueCreateInfo(i, numSelected);
            m_device.getQueue(i).setNumSelected(numSelected);
        }

        VknResult res;
        if (!(res = m_device.createDevice()).isSuccess())
            throw std::runtime_error(res.toErr("Error creating device."));
        m_resultArchive.store(res);
        return res;
    }

    void VknConfig::archiveResult(VknResult res)
    {
        m_resultArchive.store(res);
    }
}