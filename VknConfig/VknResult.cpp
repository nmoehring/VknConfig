#include "VknResult.hpp"
#include <string>
#include <vulkan/vulkan.h>

namespace vkn
{
    std::string VknResult::toString()
    {
        switch (m_result)
        {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        }
    }
}