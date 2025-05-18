#include "include/VknApp.hpp"

namespace vkn
{
    VknApp::VknApp() : m_config{&m_engine, &m_infos}, m_cycle{&m_config, &m_engine}
    {
    }

    void VknApp::configureWithPreset(std::function<void(VknConfig *, VknEngine *, VknInfos *)> preset)
    {
        preset(&m_config, &m_engine, &m_infos);
    }
}