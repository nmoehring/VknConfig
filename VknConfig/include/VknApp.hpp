#pragma once

#include <functional>

#include "VknConfig.hpp"
#include "VknCycle.hpp"
#include "VknWindow.hpp"
#include "VknDispatch.hpp"

namespace vkn
{
    // Forward declarations for the presets
    bool deviceInfoConfig(VknConfig &config);
    bool noInputConfig(VknConfig &config);
    bool cpuGenTestConfig(VknConfig &config);
    bool cpuGenTestApp(VknCycle &cycle);
    bool noInputApp(VknCycle &cycle);

    class VknApp
    {
    public:
        VknApp(std::function<bool(VknConfig &)> configFunc, std::function<bool(VknCycle &)> func);

        // Setup
        void configureWithPreset(std::function<bool(VknConfig &)> func);
        void setAppMain(std::function<bool(VknCycle &)> appMain);
        void enableValidationLayer();

        // Execute
        bool executePipeline();
        void loop();
        bool cycleOnce();
        bool preComputeUpload(void *data, size_t size);
        bool preComputeDownload(void *data, size_t *size, BufferType type);
        bool graphicsUpload(void *data, size_t size);
        bool graphicsDownload(void *data, size_t *size, BufferType type);
        bool postComputeDownload(void *data, size_t *size, BufferType type);
        void exit();

        VknConfig &getConfig() { return m_config; }

    private:
        // Engine
        VknConfig m_config;
        VknCycle m_cycle;
        VknEngine *m_engine{nullptr};
        VknInfos *m_infos{nullptr};

        // Members
        std::function<bool(VknCycle &)> m_appMain;
        VknDispatch m_dispatch{};
        VknSharedQueue *m_dispatchQueue;
        std::thread m_gpuThread;
        std::thread m_appThread;

        // State
        bool m_readyToRun{false};
        static uint32_t m_numApps;
        bool m_keepRunning{true};
        std::atomic<VknTickStats> m_tickStats{VknTickStats{}};
    };

} // namespace vkn