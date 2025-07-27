#include "include/VknTimer.hpp"

namespace vkn
{
    void VknTimer::getTickStats(uint32_t tickNum, VknTickStats &acc)
    {
        if (tickNum == 47)
            acc.frequencyFlags |= 8; // 5 Hz
        else if ((tickNum + 1u) % 16u == 0)
            acc.frequencyFlags |= 16; // 15 Hz
        else if ((tickNum + 1u) % 8u == 0)
            acc.frequencyFlags |= 32; // 30 Hz
        else if ((tickNum + 1u) % 4u == 0)
            acc.frequencyFlags |= 64; // 60 Hz
        else if ((tickNum + 1u) % 2u == 0)
            acc.frequencyFlags |= 128; // 120 Hz
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