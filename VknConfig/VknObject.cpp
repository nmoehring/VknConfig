#include "include/VknObject.hpp"

namespace vkn
{
    VknEngine *VknObject::s_engine{nullptr};
    VknInfos *VknObject::s_infos{nullptr};

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
}