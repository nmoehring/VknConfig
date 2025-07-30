#include "include/VknDispatch.hpp"

namespace vkn
{
    VknDispatch::VknDispatch() : m_sharedQueue{}
    {
    }

    VknDispatch::~VknDispatch()
    {
        stopThread();
    }

    void VknDispatch::stopThread()
    {
        if (m_running)
        {
            m_running = false;
            m_thread.join();
        }
        for (auto &reg : m_registrations)
        {
            VknTickStats endTick{};
            endTick.frequencyFlags = VknFrequencyFlag::Freq0Hz;
            reg->tickStats.store(endTick);
            reg->tickStats.notify_all();
            reg->receiveDataSize.notify_all();
            delete reg;
        }
        m_registrations.clear();
    }

    void VknDispatch::loop()
    {
        // lock with mutex
        std::unique_lock<std::mutex> lock(m_sharedQueue.queueMutex);

        m_timer.reset();
        while (m_running)
        {
            VknTickStats tickStats{};
            m_sharedQueue.queueCV.wait_until(lock, m_timer.getNextTickTime(), [this, &tickStats]
                                             { return m_timer.tick(tickStats).frequencyFlags || !m_sharedQueue.dispatchQueue.empty(); });

            VknMessage messageDetails{};
            if (tickStats.numTicks > 0 && m_sharedQueue.dispatchQueue.back().type != VknThreadMessageType_StopDispatch)
                messageDetails.type = VknThreadMessageType_Tick; // Handle tick, then loop back around for messages, if necessary
            else if (!m_sharedQueue.dispatchQueue.empty())
            {
                messageDetails = m_sharedQueue.dispatchQueue.front();
                m_sharedQueue.dispatchQueue.pop();
            }

            switch (messageDetails.type)
            {
            case VknThreadMessageType_Tick:
                for (auto message : m_transferBacklog)
                    ++message.ticksToProcess;
                for (auto message : m_receiveReadyBacklog)
                    ++message.ticksToProcess;
                VknTickStats temp = m_tickStatsAtomics[VknThreadName::AppThread].load();
                m_tickStatsAtomics[VknThreadName::AppThread].store(temp + tickStats);
                temp = m_tickStatsAtomics[VknThreadName::GpuThread].load();
                m_tickStatsAtomics[VknThreadName::GpuThread].store(temp + tickStats);
                break;
            case VknThreadMessageType_Transfer:
                for (auto iter = m_receiveReadyBacklog.begin(); iter != m_receiveReadyBacklog.end(); ++iter)
                    if (iter->srcThreadName == messageDetails.dstThreadName && iter->srcDataIndex == messageDetails.dstDataIndex)
                    {
                        m_receiveReadyBacklog.erase(iter);      // Remove the receiveReady message from the backlog
                        this->completeTransfer(messageDetails); // Use the transfer details to complete the transfer
                        break;
                    }
                m_transferBacklog.push_back(messageDetails);
                break;
            case VknThreadMessageType_ReadyToReceive:
                for (auto iter = m_transferBacklog.begin(); iter != m_transferBacklog.end(); ++iter)
                    if (iter->dstThreadName == messageDetails.srcThreadName && iter->dstDataIndex == messageDetails.srcDataIndex)
                    {
                        this->completeTransfer(*iter); // Use the transfer details to complete the transfer
                        m_transferBacklog.erase(iter); // Remove the transfer from the backlog
                        break;
                    }
                m_receiveReadyBacklog.push_back(messageDetails);
                break;
            case VknThreadMessageType_Register:
                for (auto &reg : messageDetails.extraData)
                    m_registrar[messageDetails.srcThreadName].push_back(static_cast<VknDispatchRegistration *>(reg));
                break;
            case VknThreadMessageType_None:
                continue;
            case VknThreadMessageType_StopDispatch:
                this->stopThread();
            default:
                throw std::runtime_error("Unknown or unhandled VknThreadMessageType in VknTimer::wait().");
            } // Switch

            if (messageDetails.finishedCallback)
                messageDetails.finishedCallback();
        }
    }

    VknSharedQueue *VknDispatch::startThread()
    {
        if (m_running)
            throw std::runtime_error("VknDispatch thread already running.");
        m_thread = std::thread(&VknDispatch::loop, this);
        return &m_sharedQueue;
    }

    void VknDispatch::completeTransfer(VknMessage messageDetails)
    {
        std::memcpy(m_registrar[messageDetails.srcThreadName][messageDetails.srcDataIndex]->receivePtr,
                    m_registrar[messageDetails.dstThreadName][messageDetails.dstDataIndex]->sendPtr, messageDetails.dataSize);
        m_registrar[messageDetails.dstThreadName][messageDetails.dstDataIndex]->receiveDataSize.store(messageDetails.dataSize);
    }

    VknDispatchRegistration *VknDispatch::getRegistration()
    {
        m_registrations.push_back(new VknDispatchRegistration());
        return m_registrations.back();
    }
}