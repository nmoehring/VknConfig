#include "include/VknTimer.hpp"

namespace vkn
{
    void VknTimer::getTickStats(uint32_t tickNum, VknTickStats &acc)
    {
        if (tickNum == 47)
            acc.frequencyFlags |= Freq5Hz; // 5 Hz
        else if ((tickNum + 1u) % 16u == 0)
            acc.frequencyFlags |= Freq15Hz; // 15 Hz
        else if ((tickNum + 1u) % 8u == 0)
            acc.frequencyFlags |= Freq30Hz; // 30 Hz
        else if ((tickNum + 1u) % 4u == 0)
            acc.frequencyFlags |= Freq60Hz; // 60 Hz
        else if ((tickNum + 1u) % 2u == 0)
            acc.frequencyFlags |= Freq120Hz; // 120 Hz
        ++acc.numTicks;
    }

    VknTickStats VknTimer::tick(VknTickStats &acc)
    {
        m_lastNow = std::chrono::steady_clock::now();
        while (m_nextTick <= m_lastNow)
        {
            if (m_tickNum > m_highestTickNum)
                m_tickNum = 0;
            m_nextTick += m_basePeriod;
            this->getTickStats(m_tickNum++, acc);
        }
        return acc;
    }

    void VknTimer::reset()
    {
        m_nextTick = std::chrono::steady_clock::now() + m_basePeriod;
    }
}