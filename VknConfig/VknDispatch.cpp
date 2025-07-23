#include "include/VknDispatch.hpp"

namespace vkn
{
    VknDispatch::VknDispatch() : m_sharedQueue{}
    {
    }

    void VknDispatch::dispatch()
    {
        // lock with mutex
        std::unique_lock<std::mutex> lock(m_sharedQueue.queueMutex);

        while (m_running)
        {
            m_sharedQueue.queueCV.wait(lock, [this]
                                       { return !m_sharedQueue.dispatchQueue.empty() || !m_running; });
            VknMessage messageDetails = m_sharedQueue.dispatchQueue.front();
            m_sharedQueue.dispatchQueue.pop();

            switch (messageDetails.type)
            {
            case VknThreadMessageType_Transfer:
                std::memcpy(m_registrar[messageDetails.srcThreadName][messageDetails.srcDataIndex].receivePtr,
                            m_registrar[messageDetails.dstThreadName][messageDetails.dstDataIndex].sendPtr, messageDetails.dataSize);
                m_registrar[messageDetails.dstThreadName][messageDetails.dstDataIndex].receiveDataSize->store(messageDetails.dataSize);
                break;
            case VknThreadMessageType_Register:
                m_registrar[messageDetails.srcThreadName].push_back(static_cast<VknDispatchRegistration>(messageDetails.extraData));
                break;
            default:
                throw std::runtime_error("Unknown or unhandled VknThreadMessageType in VknTimer::wait().");
            }
        }
    }

    VknSharedQueue *VknDispatch::startThread()
    {
        if (m_running)
            throw std::runtime_error("VknDispatch thread already running.");
        m_thread = std::thread(&VknDispatch::dispatch, this);
        return &m_sharedQueue;
    }
}