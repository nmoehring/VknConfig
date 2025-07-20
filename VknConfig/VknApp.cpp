#include "include/VknApp.hpp"

namespace vkn
{
    uint32_t VknApp::m_numApps{0};

    VknApp::VknApp() : m_config{}, m_cycle{}
    {
        m_engine = m_config.getEngine();
        m_infos = m_config.getInfos();
        if (m_numApps > 0)
            throw std::runtime_error("Previous VknApp() was not exited via VknApp::exit().");
        else
            ++m_numApps;
    }

    void VknApp::run()
    {
        bool keepRunning{true};
        while (keepRunning)
        {
            keepRunning = m_config.getWindow()->update();
            if (keepRunning && m_config.getWindow()->isActive())
                this->cycleEngine();
        }
    }

    bool VknApp::cycle()
    {
        bool keepRunning{true};
        if (keepRunning)
        {
            keepRunning = m_config.getWindow()->update();
            if (keepRunning && m_config.getWindow()->isActive())
                keepRunning = this->cycleEngine();
        }
    }

    bool VknApp::preComputeUpload(void *data, size_t size)
    {
        m_cycle.setUploadData(data, size);
    }

    bool VknApp::preComputeDownload(void *data, size_t *size, BufferType type)
    {
    }

    bool VknApp::graphicsUpload(void *data, size_t size)
    {
        m_cycle.setUploadData(data, size);
    }

    bool VknApp::graphicsDownload(void *data, size_t *size, BufferType type)
    {
    }

    bool VknApp::postComputeDownload(void *data, size_t *size, BufferType type)
    {
    }

    void VknApp::exit()
    {
        m_engine->shutdown();
        m_config.demolish();
        --m_numApps;
    }

    void VknApp::configureWithPreset(std::function<bool(VknConfig &)> preset)
    {
        m_readyToRun = preset(m_config);

        if (!m_config.pipelineElements_preComputeEnabled && !m_config.pipelineElements_graphicsEnabled)
            throw std::runtime_error("No main pipeline elements enabled. At least one of preCompute or graphics must be enabled.");
        if (m_config.pipelineElements_postComputeEnabled && !m_config.pipelineElements_graphicsEnabled)
            throw std::runtime_error("Cannot enable postCompute stage without graphics. Enable precompute for lone compute stage.");
        if (m_config.pipelineElements_postComputeDownloadEnabled && !m_config.pipelineElements_postComputeEnabled)
            throw std::runtime_error("Postcompute download enabled but postcompute stage not enabled.");
        if (m_config.pipelineElements_graphicsDownloadEnabled && !m_config.pipelineElements_graphicsEnabled)
            throw std::runtime_error("Graphics download enabled but graphics stage not enabled.");
        if (m_config.pipelineElements_preComputeDownloadEnabled && !m_config.pipelineElements_preComputeEnabled)
            throw std::runtime_error("Precompute download enabled but precompute stage not enabled.");
        if (m_config.pipelineElements_presentEnabled && !m_config.pipelineElements_graphicsEnabled)
            throw std::runtime_error("Present enabled but graphics stage not enabled.");
        if (m_config.pipelineElements_graphicsEnabled && !m_config.isRenderingGraphics())
            throw std::runtime_error("Graphics stage enabled but renderpass is not configured.");
        if (m_config.pipelineElements_presentEnabled && !m_config.isPresentable())
            throw std::runtime_error("Present enabled but surface not configured.");

        if (m_readyToRun)
            m_cycle.loadBasicConfig(&m_config, m_engine);
        if (m_config.isRenderingGraphics())
            m_cycle.loadGraphicsConfig(&m_config, m_engine);
        if (m_config.isComputing()) // isComputing() needs work
            m_cycle.loadComputeConfig(&m_config, m_engine);
    }

    void VknApp::setCycleFunction(std::function<bool(VknCycle &)> func)
    {
        if (!m_readyToRun)
            throw std::runtime_error("App Cycle not configured before being run.");
        m_cycleFunction = std::move(func);
    }

    void VknApp::enableValidationLayer()
    {
        std::string debugExtName = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        m_config.addInstanceExtension(debugExtName);

        std::string name = "VK_LAYER_KHRONOS_validation";
        m_config.addLayer(name);
        m_config.setValidationEnabled();
    }

    bool VknApp::cycleEngine()
    {
        if (!m_readyToRun)
            throw std::runtime_error("App Cycle not configured before being run.");
        if (!m_cycleFunction)
            throw std::runtime_error("No cycle function set for VknApp. Use VknApp::setCycleFunction().");

        m_cycle.wait();
        // Acquire the next available image from the swap chain.
        // If the swapchain is out of date or the window is minimized,
        // acquireImage will return false and handle recovery internally.
        // In that case, we skip rendering for this frame.
        // First, use short-circuit evaluation to confirm rendering is enabled.
        if (m_config.pipelineElements_graphicsEnabled && !m_cycle.acquireImage())
            return true; // Continue the app loop, but don't render this frame.

        m_cycle.beginFrameRecording(); // Starts all the command buffers
        if (m_config.pipelineElements_uploadEnabled && !m_cycle.uploadData())
        // Do something?
        {
        }

        m_cycleFunction(m_cycle); // Record commands for the frame.
        m_cycle.submitCommandBuffers();

        // Present the image. This also handles swapchain errors.
        // The return value of presentImage is only for the app loop's immediate control,
        // which we don't need here as window->update() handles it.
        if (m_config.pipelineElements_presentEnabled)
            return m_cycle.presentImage();
        return true;
    }
}