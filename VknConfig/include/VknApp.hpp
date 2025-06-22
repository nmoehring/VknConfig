#pragma once

#include <functional>

#include "VknConfig.hpp"
#include "VknCycle.hpp"
#include "VknWindow.hpp"

namespace vkn
{
    // Forward declarations for the presets
    bool deviceInfoConfig(VknConfig &config);
    bool noInputConfig(VknConfig &config);

    class VknApp
    {
    public:
        VknApp();

        // Setup
        void configureWithPreset(std::function<bool(VknConfig &)> func);
        void enableValidationLayer();

        // Execute
        bool cycleEngine();
        void run();
        void exit();

        VknConfig &getConfig() { return m_config; }
        VknCycle &getCycle() { return m_cycle; }

    private:
        // Engine
        VknConfig m_config;
        VknCycle m_cycle;
        VknEngine *m_engine{nullptr};
        VknInfos *m_infos{nullptr};

        // State
        bool m_readyToRun{false};
        static uint32_t m_numApps;
    };

} // namespace vkn