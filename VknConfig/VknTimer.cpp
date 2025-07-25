#include "include/VknTimer.hpp"

namespace vkn
{
    VknTimer::VknTimer(std::function<void(uint32_t)> tickCallback)
    {
    }

    void VknTimer::getTickFrequency(uint32_t tickNum, uint32_t &acc)
    {
        if (tickNum == 47)
            acc |= 8; // 5 Hz
        else if ((tickNum + 1u) % 16u == 0)
            acc |= 16; // 15 Hz
        else if ((tickNum + 1u) % 8u == 0)
            acc |= 32; // 30 Hz
        else if ((tickNum + 1u) % 4u == 0)
            acc |= 64; // 60 Hz
        else if ((tickNum + 1u) % 2u == 0)
            acc |= 128; // 120 Hz
    }

    uint32_t VknTimer::tick()
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        if (now >= m_next)
        {
            uint32_t acc = 0;
            while (m_next <= now)
            {
                m_next += m_basePeriod;
                this->getTickFrequency(++m_tickNum, acc);
                if (m_tickNum > m_highestTickNum)
                    m_tickNum = 0;
            }
            return acc;
        }
        else
        {
            std::this_thread::sleep_until(m_next);
            return this->tick();
        }
    }
}