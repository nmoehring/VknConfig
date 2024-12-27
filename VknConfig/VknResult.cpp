#include "VknResult.hpp"
#include <string>
#include <vulkan/vulkan.h>

namespace vkn
{
    VknResult::VknResult() {}

    std::string VknResult::toString()
    {
        switch (m_result)
        {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        default:
        {
            throw std::range_error("toString() error: Unknown result");
            return "FAILURE";
        }
        }
    }
}