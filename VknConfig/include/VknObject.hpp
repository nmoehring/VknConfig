#pragma once

#include "VknEngine.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknObject
    {
    public:
        VknObject();
        VknObject(VknIdxs relIdxs, VknIdxs absIdxs);
        ~VknObject() = default;
        void init();
        void exit();
        static bool s_recordingGfxCommandBuffer;
        static bool s_recordingComputeCommandBuffer;
        static bool s_recordingTransferCommandBuffer;
        static VkCommandBuffer *s_transferCommandBuffer;

    protected:
        // Engine
        static VknEngine *s_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        static VknInfos *s_infos;

        // Params
        static uint32_t s_maxFramesInFlight;
    };
}