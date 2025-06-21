#include "VknEngine.hpp"

namespace vkn
{
    class VknObject
    {
    public:
        VknObject();
        VknObject(VknIdxs relIdxs, VknIdxs absIdxs);
        ~VknObject() = default;

    protected:
        // Engine
        static VknEngine s_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        static VknInfos s_infos;
    };
}