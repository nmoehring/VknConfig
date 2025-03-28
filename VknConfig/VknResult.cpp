#include <string>

#include "include/VknResult.hpp"

namespace vkn
{
    VknResult::VknResultArchive s_archive = VknResult::VknResultArchive{};

    void VknResult::VknResultArchive::manage()
    {
        if (results.size() > maxResults)
            for (uint64_t count{results.size() - maxResults}; count > 0; --count)
                results.pop();
    }

    void VknResult::VknResultArchive::store(VknResult res)
    {
        if (!res.isErrorStateEvaluated())
            throw std::runtime_error("Target function not called, no result to store.");
        results.push(res);
        this->manage();
    }

    VknResult::VknResult(std::string opDesc) : m_opDesc{opDesc}
    {
    }

    VknResult::VknResult(VkResult result, std::string opDesc)
        : m_result{result}, m_opDesc{opDesc}
    {
        if (m_result != VK_SUCCESS)
            throw std::runtime_error(this->toErr("Error: "));

        m_errorStateEvaluated = true;
        s_archive.store(*this);
    }

    void VknResult::evaluate(VkResult result)
    {
        m_result = result;
        m_errorStateEvaluated = true;
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