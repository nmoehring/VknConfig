#pragma once

namespace vkn
{
    class VknQueueFamily
    {
    public:
        VknQueueFamily();
        VknQueueFamily(VkQueueFamilyProperties properties);

        bool supportsGraphics();
        bool supportsCompute();
        bool supportsTransfer();
        bool supportsSparseBinding();
        bool supportsMemoryProtection();
        int getNumSelected();
        void setNumSelected(int num);
        uint32_t getNumAvailable();

    private:
        int m_numSelected{0};
        VkQueueFamilyProperties m_properties{};
    };
}