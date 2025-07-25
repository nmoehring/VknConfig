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
    bool cpuGenTestCycle(VknCycle &cycle);
    bool noInputCycle(VknCycle &cycle);

    class VknApp
    {
    public:
        VknApp(std::function<bool(VknConfig &)> configFunc, std::function<bool(VknCycle &)> func);

        // Setup
        void configureWithPreset(std::function<bool(VknConfig &)> func);
        void setAppMain(VknSharedQueue *m_dispatchQueue);
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
        VknCycle &getAppMain() { return m_appMain; }

    private:
        // Engine
        VknConfig m_config;
        VknCycle m_appMain;
        VknEngine *m_engine{nullptr};
        VknInfos *m_infos{nullptr};

        // Members
        std::function<bool(VknCycle &)> m_cycleFunction;
        VknDispatch m_dispatch{};
        VknSharedQueue *m_dispatchQueue;
        std::thread m_gpuThread;

        // State
        bool m_readyToRun{false};
        static uint32_t m_numApps;
        bool m_keepRunning{true};
    };

} // namespace vkn