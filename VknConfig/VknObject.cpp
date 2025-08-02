#include "include/VknObject.hpp"

namespace vkn
{
    VknEngine *VknObject::s_engine{nullptr};
    VknInfos *VknObject::s_infos{nullptr};
    uint32_t VknObject::s_maxFramesInFlight{2};
    bool VknObject::s_recordingGfxCommandBuffer{false};
    bool VknObject::s_recordingUploadCommandBuffer{false};
    bool VknObject::s_recordingDownloadCommandBuffer{false};
    bool VknObject::s_recordingPreComputeCommandBuffer{false};
    bool VknObject::s_recordingPostComputeCommandBuffer{false};
    VkCommandBuffer *VknObject::s_uploadCommandBuffer{nullptr};
    VkCommandBuffer *VknObject::s_downloadCommandBuffer{nullptr};
    VknSharedQueue *VknObject::s_dispatchQueue{nullptr};

    VknObject::VknObject() : m_relIdxs{}, m_absIdxs{}
    {
    }

    VknObject::VknObject(VknIdxs relIdxs, VknIdxs absIdxs) : m_relIdxs{relIdxs}, m_absIdxs{absIdxs}
    {
    }

    void VknObject::init()
    {
        s_engine = new VknEngine{};
        s_infos = new VknInfos{};
    }

    void VknObject::exit()
    {
        delete s_engine;
        delete s_infos;
        s_engine = nullptr;
        s_infos = nullptr;
    }

    void VknObject::sendMessage(VknMessage *msg)
    {
        {
            std::lock_guard<std::mutex> lock(s_dispatchQueue->queueMutex);
            s_dispatchQueue->dispatchQueue.push(msg);
        }
        s_dispatchQueue->queueCV.notify_one();
    }
}