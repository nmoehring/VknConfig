#include "include/VknObject.hpp"

namespace vkn
{
    VknEngine VknObject::s_engine{};
    VknInfos VknObject::s_infos{};

    VknObject::VknObject() : m_relIdxs{}, m_absIdxs{}
    {
    }

    VknObject::VknObject(VknIdxs relIdxs, VknIdxs absIdxs) : m_relIdxs{relIdxs}, m_absIdxs{absIdxs}
    {
    }
}