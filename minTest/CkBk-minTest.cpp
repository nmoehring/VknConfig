#include <vector>
#include <iostream>

#include "VknConfig/VknConfig.hpp"

void printDevProps(std::vector<VkPhysicalDeviceProperties> devProps);
void printFamProps(std::vector<VkQueueFamilyProperties> famProps);

int main()
{
    vkn::VknConfig vknConfig{};
    vknConfig.createDevice();
    int idx = 0;
    for (auto queue : vknConfig.getDevice().getQueues())
    {
        std::cout << "Queue " << idx << ": " << std::endl;
        std::cout << "Compute: " << queue.supportsCompute() << std::endl;
        std::cout << "Graphics: " << queue.supportsGraphics() << std::endl;
        std::cout << "Transfer: " << queue.supportsTransfer() << std::endl;
        std::cout << "Sparse Binding: " << queue.supportsSparseBinding() << std::endl;
        std::cout << "Mem Protection: " << queue.supportsMemoryProtection() << std::endl;
    }
}