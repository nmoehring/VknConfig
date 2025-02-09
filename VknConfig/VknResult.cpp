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
            break;
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
            break;
        default:
        {
            throw std::runtime_error("toString() error: Unknown result");
            return "FAILURE";
        }
        }
    }
}