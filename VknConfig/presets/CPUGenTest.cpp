#include "../include/VknConfig.hpp"
#include "../include/VknCycle.hpp"
#include "../include/VknBuffer.hpp"
#include "../include/VknPipeline.hpp"
#include "../include/VknVertexInputState.hpp"
#include <vector>
#include <cmath>
#include <cstddef> // For offsetof

namespace vkn
{
    // A simple vertex structure with position and color
    struct Vertex
    {
        float pos[3];
        float color[3];
    };

    // 3rd test during development
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};

    const int VERTEX_BUFFER_IDX = 0;
    const int INDEX_BUFFER_IDX = 1;

    uint32_t totalTime{0};
    std::atomic<VknTickStats> *tickStats{nullptr};

    // Generates vertices and indices for a wavy grid mesh
    void generateWavyGrid(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, float time)
    {
        const int gridWidth = 50;
        const int gridHeight = 50;
        const float spacing = 0.1f;
        const float amplitude = 0.25f;
        const float frequency = 0.3f;

        vertices.clear();
        indices.clear();
        vertices.reserve(gridWidth * gridHeight);
        indices.reserve((gridWidth - 1) * (gridHeight - 1) * 6);

        // Generate vertices
        for (int z = 0; z < gridHeight; ++z)
        {
            for (int x = 0; x < gridWidth; ++x)
            {
                Vertex vertex;
                float fx = (x - gridWidth / 2.0f) * spacing;
                float fz = (z - gridHeight / 2.0f) * spacing;

                // Position
                vertex.pos[0] = fx;
                vertex.pos[1] = sin(fx * frequency * 10.0f + time) * cos(fz * frequency * 10.0f + time) * amplitude;
                vertex.pos[2] = fz;

                // Color based on height (y-position)
                vertex.color[0] = (vertex.pos[1] / amplitude) * 0.5f + 0.5f; // Red
                vertex.color[1] = 0.2f;                                      // Green
                vertex.color[2] = 1.0f - vertex.color[0];                    // Blue

                vertices.push_back(vertex);
            }
        }

        // Generate indices
        for (uint32_t z = 0; z < gridHeight - 1; ++z)
        {
            for (uint32_t x = 0; x < gridWidth - 1; ++x)
            {
                uint32_t topLeft = (z * gridWidth) + x;
                uint32_t topRight = topLeft + 1;
                uint32_t bottomLeft = ((z + 1) * gridWidth) + x;
                uint32_t bottomRight = bottomLeft + 1;

                // First triangle
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // Second triangle
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }
    }

    bool cpuGenTestConfig(VknConfig &config)
    {
        // Enable pipeline elements
        config.pipelineElements_graphicsEnabled = true;
        config.pipelineElements_graphicsUploadEnabled = true;
        config.pipelineElements_presentEnabled = true;

        // Set up the application name and engine name
        config.setAppName("CPU-Gen-Test");
        config.setEngineName("MinVknConfig");
        config.addWindow();
        config.createInstance();
        config.createSurface(0);

        // Set up the device
        auto *device = config.addDevice(0);
        device->addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        device->createDevice();
        VknSwapchain *swapchain{device->getSwapchain()};

        // Set up the renderpass
        auto *renderpass = device->addRenderpass(0);
        renderpass->addAttachment(0);
        renderpass->addAttachmentRef(0, 0);
        renderpass->addSubpass(0);
        renderpass->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        renderpass->addSubpassDependency(1, 0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0);
        renderpass->createRenderpass();
        renderpass->createFramebuffers(*swapchain);

        // Set up the pipeline
        auto *pipeline = renderpass->getPipeline(0);
        pipeline->getRasterizationState()->setCullMode(VK_CULL_MODE_BACK_BIT);
        // VknPipelineLayout *layout = pipeline->getPipelineLayout();
        // layout->addPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4); // For MVP matrix
        // VknDescriptorSetLayout *dsl0 = layout->addDescriptorSetLayout();
        // dsl0->addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
        // dsl0->createDescriptorSetLayout();
        // layout->createPipelineLayout();
        pipeline->getPipelineLayout()->_createPipelineLayout(); // Create a default empty layout

        // Set up vertex input state
        VknVertexInputState *vertexInputState = pipeline->getVertexInputState();
        vertexInputState->fileVertexBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
        vertexInputState->fileVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
        vertexInputState->fileVertexAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));

        // Add shader stages
        VknShaderStage *vertShader = pipeline->addShaderStage(0, vkn::VKN_VERTEX_STAGE, "triangle.vert.spv");
        vertShader->createShaderModule();
        VknShaderStage *fragShader = pipeline->addShaderStage(1, vkn::VKN_FRAGMENT_STAGE, "triangle.frag.spv");
        fragShader->createShaderModule();
        vkn::VknViewportState *viewportState = pipeline->getViewportState();
        viewportState->syncWithSwapchain(*swapchain, 0, 0);
        renderpass->createPipelines();

        // Create vertex and index buffers
        VknVertexBuffer *vertexBuffer = device->addVertexBuffer(75000, vertices.data(), tickStats);
        VknIndexBuffer *indexBuffer = device->addIndexBuffer(75000, indices.data(), tickStats);

        device->addCommandPools();

        return true;
    }

    bool cpuGenTestApp(std::stop_token stopToken, VknCycle &cycle)
    {
        while (!stopToken.stop_requested())
        {
            tickStats->wait(VknTickStats{});
            VknTickStats currentTick = tickStats->exchange(VknTickStats{});
            if (currentTick.frequencyFlags & VknFrequencyFlag::Freq0Hz)
                return false; // Exit if the tick frequency is 0
            else if (currentTick.frequencyFlags & VknFrequencyFlag::Freq30Hz)
                totalTime += 1; // Increment the time variable
            // Wait on the tick stats atomic with 30Hz to accumulate the time variable to be used in generateWavyGrid

            // --- Generate Mesh Data ---
            generateWavyGrid(vertices, indices, totalTime);

            // --- Upload Data ---
            cycle.transferUploadData(VERTEX_BUFFER_IDX, vertices.size() * sizeof(Vertex)); // Upload to the first vertex buffer
            cycle.transferUploadData(INDEX_BUFFER_IDX, indices.size() * sizeof(uint32_t)); // Upload to the first index

            // Record a graphics pass (draw call)
            cycle.setNumIndices(static_cast<uint32_t>(indices.size())); // Tell the cycle how many indices to draw
            return true;
        }
        return false;
    }
}