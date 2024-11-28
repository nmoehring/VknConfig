#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

int main()
{
  // 1. CREATE INSTANCE
  VkInstance instance;
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
  {
    std::cout << "Failed to create Vulkan instance" << std::endl;
    return -1;
  }

  // 2. SELECT PHYSICAL DEVICE
  VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
  uint32_t deviceCount{0};
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0)
  {
    std::cout << "Failed to find GPUs with Vulkan support" << std::endl;
    return -1;
  }

  std::vector<VkPhysicalDevice> devices{deviceCount};
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
  physicalDevice = devices[0];

  // 3. CREATE DEVICE
  VkDevice device;
  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
  {
    std::cout << "Failed to create Vulkan device" << std::endl;
    return -1;
  }

  // END: Destroy Vk stuff
  std::cout << "Vulkan device created successfully" << std::endl;
  vkDestroyDevice(device, NULL);
  vkDestroyInstance(instance, NULL);
  return 0;
}
