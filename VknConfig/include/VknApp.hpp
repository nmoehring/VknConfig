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
    bool cpuGenTestConfig(VknConfig &config);
    void cpuGenTestCycle(VknCycle &cycle);
    void noInputCycle(VknCycle &cycle);
    void deviceInfoCycle(VknCycle &cycle);

    class VknApp
    {
    public:
        VknApp();

        // Setup
        void configureWithPreset(std::function<bool(VknConfig &)> func);
        void setCycleFunction(std::function<bool(VknCycle &)> func);
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

        // Members
        std::function<bool(VknCycle &)> m_cycleFunction;

        // State
        bool m_readyToRun{false};
        static uint32_t m_numApps;
    };

} // namespace vkn