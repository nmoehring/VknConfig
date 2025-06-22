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

    protected:
        // Engine
        static VknEngine *s_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        static VknInfos *s_infos;
    };
}