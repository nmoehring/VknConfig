#include <string>

#include "VknResult.hpp"

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
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
            break;
        default:
        {
            return "Vk Err " + std::to_string(this->m_result);
        }
        }
    }
}