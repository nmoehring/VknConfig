#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <format>

namespace vkn
{
    class VknResult
    {
    public:
        VknResult();
        VknResult(VkResult result, std::string opDesc) : m_result{result}, m_opDesc{opDesc} {};

        std::string toString();
        std::string toErr(std::string msg = "test")
        {
            return std::format("Error: {}: {}", msg, this->toString());
        }

        bool isSuccess() { return m_result == VK_SUCCESS; }

    private:
        VkResult m_result;
        std::string m_opDesc{""};
    };
}
