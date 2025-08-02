#pragma once
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <map>
#include <functional>

#include "VknTimer.hpp"

namespace vkn
{
    enum VknMessageType
    {
        VknThreadMessageType_Transfer,        // Transfer data between threads
        VknThreadMessageType_ReadyToReceive,  // Indicate that a thread is ready to receive data from another thread
        VknThreadMessageType_GetRegistration, // Get registration details for a thread
        VknThreadMessageType_StopDispatch,
        VknThreadMessageType_Tick,
        VknThreadMessageType_None // No message type
    };

    enum VknThreadName
    {
        MainThread,
        GpuThread,
        AppThread,
        NumThreadNames,
        NullThreadName
    };

    struct VknDispatchRegistration
    {
        void *sendPtr{nullptr};
        void *receivePtr{nullptr};
        std::atomic<uint32_t> receiveDataSize{0};
        std::atomic<bool> lastDataReceived{false};
        uint32_t registrationIdx{std::numeric_limits<uint32_t>::max()};
    };

    struct VknMessage
    {
        VknMessageType type{VknThreadMessageType_None};
        VknThreadName srcThreadName{NullThreadName};
        VknThreadName dstThreadName{NullThreadName};
        std::vector<void *> extraData{};
        uint32_t dataSize{0};
        uint32_t offset{0};
        uint32_t ticksToProcess{0};
        uint8_t srcDataIndex{std::numeric_limits<size_t>::max()}; // Index for data pointers in m_data
        uint8_t dstDataIndex{std::numeric_limits<size_t>::max()}; // Index for data pointers in m_data
        std::atomic<bool> processed{false};                       // Flag to indicate if the message has been processed
    };

    struct VknSharedQueue
    {
        std::queue<VknMessage *> dispatchQueue{}; // Thread-safe queue for transfer messages
        std::mutex queueMutex;                    // Mutex to protect access to the queue
        std::condition_variable queueCV{};        // Condition variable for signaling
    };

    class VknDispatch
    {
    public:
        VknDispatch();
        ~VknDispatch();
        void stopThread();
        void loop();
        VknSharedQueue *startThread();
        void completeTransfer(VknMessage *messageDetails);
        VknSharedQueue *getSharedQueue() { return &m_sharedQueue; }

    private:
        VknTimer m_timer{};
        std::thread m_thread;  // Thread to run the timer
        bool m_running{false}; // Flag to control the timer's running state
        // thread-safe queue for transfer details
        VknSharedQueue m_sharedQueue{};
        std::map<VknThreadName, std::vector<VknDispatchRegistration *>> m_registrar;
        std::map<VknThreadName, std::atomic<VknTickStats> *> m_tickStatsAtomics{
            {VknThreadName::MainThread, new std::atomic<VknTickStats>{VknTickStats{}}},
            {VknThreadName::GpuThread, new std::atomic<VknTickStats>{VknTickStats{}}},
            {VknThreadName::AppThread, new std::atomic<VknTickStats>{VknTickStats{}}},
        };
        std::list<VknMessage *> m_transferBacklog{};
        std::list<VknMessage *> m_receiveReadyBacklog{};
        uint32_t m_ticksSinceLastUpload{0};
    };
}