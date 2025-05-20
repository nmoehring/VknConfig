#include <iostream>
#include "../include/VknConfig.hpp"
#include "Presets.hpp"

using namespace vkn;

bool deviceInfo(VknConfig *config, VknEngine *engine, VknInfos *infos)
{
    config->fillAppInfo(VK_API_VERSION_1_1, "DeviceInfo", "VknConfig");
    config->createInstance();
    auto device = config->addDevice(0);
    device->createDevice();
    // config->getDevice(0)->createDevice(); Why did this work? Investigate.

    auto renderpass = device->getRenderpass(0);
    auto pipeline = renderpass->getPipeline(0);

    auto limits = device->getPhysicalDevice()->getLimits();
    std::cout << "maxVertexInputBindings=" << limits->maxVertexInputBindings << std::endl;
    std::cout << "maxVertexInputAttributes=" << limits->maxVertexInputAttributes << std::endl;

    int idx = 0;
    std::cout << std::endl
              << "Device 0 queues:" << std::endl;
    for (auto &queue : device->getPhysicalDevice()->getQueues())
    {
        std::cout << "============================================" << std::endl;
        std::cout << "Queue " << idx << ": " << std::endl;
        std::cout << "Graphics: " << queue.supportsGraphics() << std::endl;
        std::cout << "Compute: " << queue.supportsCompute() << std::endl;
        std::cout << "Transfer: " << queue.supportsTransfer() << std::endl;
        std::cout << "Sparse Binding: " << queue.supportsSparseBinding() << std::endl;
        std::cout << "Mem Protection: " << queue.supportsMemoryProtection() << std::endl;
        ++idx;
    }

    return false;
}
