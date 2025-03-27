#pragma once

#include <string>
#include <format>
#include <queue>

#include <vulkan/vulkan.h>

namespace vkn
{
    class VknResult
    {
    public:
        VknResult() = delete;
        VknResult(std::string opDesc);
        VknResult(VkResult result, std::string opDesc);

        void operator=(VkResult result) { this->evaluate(result); }
        void evaluate(VkResult result);

        std::string toString();
        std::string toErr(std::string msg = "test")
        {
            return std::format("{}: {}", msg, this->toString());
        }

        bool isSuccess() { return m_result == VK_SUCCESS; }
        bool isErrorStateEvaluated() { return m_errorStateEvaluated; }

    private:
        // Members
        VkResult m_result{};
        std::string m_opDesc{""};
        static VknResultArchive s_archive;

        // State
        bool m_errorStateEvaluated{false};
    };

    class VknResultArchive
    {
        std::queue<VknResult> results{};
        uint32_t maxResults{1000};

        void manage()
        {
            if (results.size() > maxResults)
                for (uint64_t count{results.size() - maxResults}; count > 0; --count)
                    results.pop();
        }

    public:
        void store(VknResult res)
        {
            if (!res.isErrorStateEvaluated())
                throw std::runtime_error("Target function not called, no result to store.");
            results.push(res);
            this->manage();
        }
    };
}
