#include <stdexcept>
#include <iostream>
#include <functional>

#include "VknConfig.hpp"
#include "VknInfos.hpp"
#include "VknResult.hpp"
#include <vulkan/vulkan.h>

namespace vkn
{
    VknConfig::VknConfig() : m_resultArchive{}, m_physicalDevices{1}, m_idxSelectedPhysicalDevice{0},
                             m_device{nullptr}, m_instance{nullptr}
    {
        VknResult res(VK_SUCCESS, "null");
        if (!(res = this->createInstance()).isSuccess())
            throw std::runtime_error(res.toErr("Error creating instance."));

        if (!(res = this->selectPhysicalDevice()).isSuccess())
            throw std::runtime_error(res.toErr("Error retrieving selected physical device data."));

        if (!(res = this->getQueueFamilyProperties()).isSuccess())
            throw std::runtime_error(res.toErr("Error retrieving queue family properties."));

        if (!(res = this->createDevice()).isSuccess())
            throw std::runtime_error(res.toErr("Error creating device."));
    }

    void VknConfig::runVkFunction(std::function<VknResult()> func, std::string errMsg)
    {
        VknResult res(VK_SUCCESS, "null");
        if (!(res = func()).isSuccess())
            throw std::runtime_error(res.toErr(errMsg));
        this->archiveResult(res);
    }

    VknConfig::~VknConfig()
    {
        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    void VknConfig::archiveResult(VknResult res)
    {
        m_resultArchive.push_back(res);
    }

    VknResult VknConfig::createInstance()
    {
        VknResult res{vkCreateInstance(m_infos.getInstanceCreateInfo(), nullptr, &m_instance),
                      "Create instance."};
        return res;
    }

    VknResult VknConfig::selectPhysicalDevice()
    {
        uint32_t deviceCount{0};

        VknResult res1{vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr),
                       "Enumerate physical devices."};
        this->archiveResult(res1);
        if (deviceCount == 0)
            throw std::runtime_error(res1.toErr("No GPU's supporting Vulkan found."));
        m_physicalDevices.resize(deviceCount);

        VknResult res2{vkEnumeratePhysicalDevices(m_instance, &deviceCount, m_physicalDevices.data()),
                       "Enum physical devices and store."};
        m_idxSelectedPhysicalDevice = 0; // TODO
        return res2;
    }

    VknResult VknConfig::getQueueFamilyProperties()
    {
        uint32_t propertyCount = 0;
        VknResult res1{
            vkGetPhysicalDeviceQueueFamilyProperties(
                m_physicalDevices[m_idxSelectedPhysicalDevice],
                &propertyCount,
                nullptr),
            "Get available queue family count."};
        if (propertyCount == 0)
            throw std::runtime_error(res1.toErr("No available queue families found."));
        this->archiveResult(res1);

        VknResult res2{
            vkGetPhysicalDeviceQueueFamilyProperties(
                m_physicalDevices[m_idxSelectedPhysicalDevice],
                &propertyCount,
                &m_queueFamilyProperties),
            "Retrieve queue family properties."};

        return res2;
    }

    VknResult VknConfig::createDevice()
    {
        VknResult res{
            vkCreateDevice(
                m_physicalDevices[m_idxSelectedPhysicalDevice],
                m_infos.getDeviceCreateInfo(),
                nullptr,
                &m_device),
            "Create device"};
        return res;
    }
} // namespace vkn