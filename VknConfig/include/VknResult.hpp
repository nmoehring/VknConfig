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
        std::string toErr(std::string msg);
        bool isErrorStateEvaluated() { return m_errorStateEvaluated; }

    private:
        class VknResultArchive
        {
            std::queue<VknResult> results{};
            uint32_t maxResults{1000};

            void manage();

        public:
            void store(VknResult res);
        };

        // Members
        VkResult m_result{};
        std::string m_opDesc{""};
        static VknResultArchive s_archive;

        // State
        bool m_errorStateEvaluated{false};
    };
}
