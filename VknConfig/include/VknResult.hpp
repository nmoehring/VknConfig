#pragma once

#include <string>
#include <format>
#include <queue>

#include <vulkan/vulkan.h>

namespace vkn
{
    template <typename T>
    class VknArchive
    {
        std::queue<T> m_data{};
        uint32_t m_capacity{1000};

        void manage()
        {
            if (m_data.size() > m_capacity)
                for (uint64_t count{m_data.size() - m_capacity}; count > 0; --count)
                    m_data.pop();
        }

    public:
        void store(T res)
        {
            if (!res.isErrorStateEvaluated())
                throw std::runtime_error("Target function not called, no result to store.");
            m_data.push(res);
            this->manage();
        }
    };

    class VknResult
    {
    public:
        VknResult() = delete;
        VknResult(std::string opDesc);
        VknResult(VkResult result, std::string opDesc);

        void operator=(VkResult result) { this->evaluate(result); }
        void evaluate(VkResult result);

        std::string toString();
        std::string toErr(std::string msg);
        bool isErrorStateEvaluated() { return m_errorStateEvaluated; }

    private:
        // Members
        VkResult m_result{};
        std::string m_opDesc{""};
        static VknArchive<VknResult> s_archive;

        // State
        bool m_errorStateEvaluated{false};
    };
}
