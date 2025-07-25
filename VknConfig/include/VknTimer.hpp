#pragma once

#include <chrono>

#include "VknObject.hpp"

namespace vkn
{
    class VknTimer
    {
    public:
        VknTimer(std::function<void(uint32_t)> tickCallback);
        uint32_t tick();
        void getTickFrequency(uint32_t tickNum, uint32_t &acc);

    private:
        VknSharedQueue *m_dispatchQueue{nullptr};
        double m_baseFrequency{240.0};
        double m_lowestFrequency{5.0};
        std::chrono::microseconds m_basePeriod{static_cast<uint64_t>(1000000.0 / m_baseFrequency)}; // 240 Hz
        uint32_t m_highestTickNum{static_cast<uint32_t>(m_baseFrequency / m_lowestFrequency)};
        std::chrono::steady_clock::time_point m_next{std::chrono::steady_clock::now()};
        uint32_t m_tickNum = 0;
    };
}