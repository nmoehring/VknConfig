#pragma once

#include <chrono>
#include <cmath>

#include "VknObject.hpp"

namespace vkn
{
    enum VknFrequencyFlag
    {
        Freq120Hz = 128,
        Freq60Hz = 64,
        Freq30Hz = 32,
        Freq15Hz = 16,
        Freq5Hz = 8
    };

    struct VknTickStats
    {
        uint32_t frequencyFlags{0};
        uint32_t numTicks{0};
        uint32_t getActualFrequency()
        {
            if (!numTicks)
                return;
            else if (numTicks <= 240)
                return std::ceil(240.0 / numTicks);
            else
                return 240.0 / numTicks;
        }
    };

    class VknTimer
    {
    public:
        VknTickStats tick(VknTickStats &acc);
        void getTickStats(uint32_t tickNum, VknTickStats &acc);
        void reset();
        std::chrono::steady_clock::time_point getNextTickTime() { return m_nextTick; }

    private:
        VknSharedQueue *m_dispatchQueue{nullptr};
        double m_baseFrequency{240.0};
        double m_lowestFrequency{5.0};
        std::chrono::microseconds m_basePeriod{static_cast<uint64_t>(1000000.0 / m_baseFrequency)}; // 240 Hz
        uint32_t m_highestTickNum{static_cast<uint32_t>(m_baseFrequency / m_lowestFrequency)};
        std::chrono::steady_clock::time_point m_nextTick{std::chrono::steady_clock::now()};
        std::chrono::steady_clock::time_point m_lastNow{std::chrono::steady_clock::now()};
        uint32_t m_tickNum = 0;
    };
}