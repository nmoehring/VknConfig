#include <iostream>
#include "../include/VknConfig.hpp"

namespace vkn
{

    bool deviceInfoConfig(VknConfig &config)
    {
        config.setNotPresentable();
        config.createInstance();

        auto device = config.addDevice(0);
        device->createDevice();

        // auto renderpass = device->addRenderpass(0);
        // auto pipeline = renderpass->getPipeline(0);

        auto limits = device->getPhysicalDevice()->getLimits();
        std::cout << "maxVertexInputBindings=" << limits->maxVertexInputBindings << std::endl;
        std::cout << "maxVertexInputAttributes=" << limits->maxVertexInputAttributes << std::endl;

        int idx = 0;
        std::cout << std::endl
                  << "Device 0 queues:" << std::endl;
        for (auto &queue : device->getPhysicalDevice()->getQueues())
        {
            std::cout << "============================================" << std::endl;
            std::cout << "x" << queue.getNumAvailable();
            if (queue.supportsGraphics())
                std::cout << " Graphics ";
            else if (queue.supportsCompute())
                std::cout << " Compute ";
            else if (queue.supportsTransfer())
                std::cout << " Transfer ";
            else if (queue.supportsSparseBinding())
                std::cout << " Sparse Binding ";
            else if (queue.supportsMemoryProtection())
                std::cout << " Memory Protection ";
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
}