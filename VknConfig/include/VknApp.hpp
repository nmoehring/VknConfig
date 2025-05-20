#pragma once
#include <functional>
#include "VknConfig.hpp"
#include "VknCycle.hpp"

namespace vkn
{
    // Forward declarations for the presets
    bool deviceInfo(VknConfig *config, VknEngine *engine, VknInfos *infos);
    bool noInputConfig(VknConfig *config, VknEngine *engine, VknInfos *infos);

    class VknApp
    {
    public:
        VknApp();

        // Setup
        void configureWithPreset(std::function<bool(VknConfig *, VknEngine *, VknInfos *)> func);
        void addWindow_GLFW(GLFWwindow *window);
        void enableValidationLayer();

        // Execute
        void cycleEngine();
        void exit();

    private:
        // Engine
        VknConfig m_config;
        VknCycle m_cycle;
        VknEngine m_engine{};
        VknInfos m_infos{};

        // State
        bool m_configured{false};
    };

} // namespace vkn