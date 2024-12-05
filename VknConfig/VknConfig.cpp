#include <iostream>

#include "VknConfig.hpp"

namespace vkn
{
    VknConfig::VknConfig()
    {
        VknResult res(VK_SUCCESS, "null");
        if (!(res = this->createInstance()).isSuccess())
            throw std::runtime_error(res.toErr("Error creating instance."));

        if (!(res = this->selectPhysicalDevice()).isSuccess())
            throw std::runtime_error(res.toErr("Failed to get physical devices."));
    }
    /*
        VknConfig::~VknConfig()
        {
            vkDestroyInstance(m_instance, nullptr);
        }
    */
    VknResult VknConfig::createInstance()
    {
        VknResult res{vkCreateInstance(m_infos.getInstanceCreateInfo(), nullptr, &m_instance),
                      "Create instance."};
        return res;
    }

    VknResult VknConfig::selectPhysicalDevice()
    {
        uint32_t deviceCount{0};
        std::vector<VkPhysicalDevice> devices;

        VknResult res1{vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr),
                       "Enumerate physical devices."};
        this->archiveResult(res1);
        if (deviceCount == 0)
            throw std::runtime_error(res1.toErr("No GPU's supporting Vulkan found."));
        else if (deviceCount > 1)
            std::cerr << "Found more than one GPU supporting Vulkan. Selecting device at index 0." << std::endl;

        devices.resize(deviceCount);
        VknResult res2{vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data()),
                       "Enum physical devices and store."};
        m_device = VknDevice(devices[0], &m_infos);

        return res2;
    }

    void VknConfig::archiveResult(VknResult res)
    {
        m_resultArchive.push_back(res);
    }

    void VknConfig::createDevice(bool chooseAllAvailableQueues)
    {
        for (int i = 0; i < m_device.getQueues().size(); ++i)
        {
            int numSelected = 1;
            if (chooseAllAvailableQueues)
                numSelected = m_device.getQueue(i).getNumAvailable();
            m_infos.fillDeviceQueueCreateInfo(i, numSelected);
            m_device.getQueue(i).setNumSelected(numSelected);
        }
        VknResult res(VK_SUCCESS, "null");
        if (!(res = m_device.createDevice()).isSuccess())
            throw std::runtime_error(res.toErr("Error creating device."));
    }
}