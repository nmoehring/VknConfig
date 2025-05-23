#include "include/VknResult.hpp"

namespace vkn
{
    VknArchive<VknResult> VknResult::s_archive = VknArchive<VknResult>{};

    VknResult::VknResult(std::string opDesc) : m_opDesc{opDesc}
    {
    }

    VknResult::VknResult(VkResult result, std::string opDesc)
        : m_opDesc{opDesc}
    {
        this->evaluate(result);
    }

    void VknResult::evaluate(VkResult result)
    {
        if (result != VK_SUCCESS)
            throw std::runtime_error(this->toErr("Error: "));
        m_result = result;
        m_evaluatedErrorState = true;
        s_archive.store(*this);
    }

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
            return "Vk Err " + std::to_string(m_result);
        }
        } // switch
    }

    std::string VknResult::toErr(std::string msg)
    {
        return std::format("{}: {}", msg, this->toString());
    }
}