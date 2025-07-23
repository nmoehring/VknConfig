#pragma once
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <map>

namespace vkn
{
    enum VknMessageType
    {
        VknThreadMessageType_Transfer, // Transfer data between threads
        VknThreadMessageType_Register,
        VknThreadMessageType_Signal, // Signal a condition or event
        VknThreadMessageType_Custom, // Custom message type for user-defined actions
        VknThreadMessageType_StopDispatcher,
        VknThreadMessageType_None // No message type
    };

    enum VknThreadName
    {
        MainThread,
        RenderThread,
        AppThread,
        NumThreadNames,
        NullThreadName
    };

    struct VknDispatchRegistration
    {
        void *sendPtr{nullptr};
        void *receivePtr{nullptr};
        std::atomic<size_t> *receiveDataSize{nullptr};
    };

    struct VknMessage
    {
    public:
        VknMessageType type{VknThreadMessageType_None};
        VknThreadName srcThreadName{NullThreadName};
        VknThreadName dstThreadName{NullThreadName};
        size_t dataSize{0};
        size_t srcDataIndex{std::numeric_limits<size_t>::max()}; // Index for data pointers in m_data
        size_t dstDataIndex{std::numeric_limits<size_t>::max()}; // Index for data pointers in m_data
        void *extraData{nullptr};
    };

    struct VknSharedQueue
    {
        std::queue<VknMessage> dispatchQueue{}; // Thread-safe queue for transfer messages
        std::mutex queueMutex;                  // Mutex to protect access to the queue
        std::condition_variable queueCV{};      // Condition variable for signaling
    };

    class VknDispatch
    {
    public:
        VknDispatch();
        void dispatch();
        VknSharedQueue *startThread();

    private:
        std::thread m_thread;  // Thread to run the timer
        bool m_running{false}; // Flag to control the timer's running state
        // thread-safe queue for transfer details
        VknSharedQueue m_sharedQueue{};
        std::map<VknThreadName, std::vector<VknDispatchRegistration>> m_registrar; // Map of thread names to threads
    };
}