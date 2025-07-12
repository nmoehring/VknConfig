#include "../include/VknConfig.hpp"
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

    // Generates vertices and indices for a wavy grid mesh
    void generateWavyGrid(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
    {
        const int gridWidth = 50;
        const int gridHeight = 50;
        const float spacing = 0.1f;
        const float amplitude = 0.5f;
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
                vertex.pos[1] = sin(fx * frequency * 10.0f) * cos(fz * frequency * 10.0f) * amplitude;
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
        // Shallow Config members
        config.setAppName("CPU-Gen-Test");
        config.setEngineName("MinVknConfig");
        config.addWindow();
        config.createInstance();
        config.createSurface(0);

        // Config => Device
        auto *device = config.addDevice(0);
        device->addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        device->createDevice();
        VknSwapchain *swapchain{device->getSwapchain()};

        // --- Generate Mesh Data ---
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        generateWavyGrid(vertices, indices);

        // --- Create and Upload Buffers ---
        VknVertexBuffer *vertexBuffer = device->addVertexBuffer(vertices.size() * sizeof(Vertex));
        vertexBuffer->uploadData(vertices.data());

        VknIndexBuffer *indexBuffer = device->addIndexBuffer(indices.size() * sizeof(uint32_t));
        indexBuffer->uploadData(indices.data());

        // Config => Device => Renderpass
        auto *renderpass = device->addRenderpass(0);
        renderpass->addAttachment(0);
        renderpass->addAttachmentRef(0, 0);
        renderpass->addSubpass(0);
        renderpass->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        renderpass->addSubpassDependency(1, 0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0);
        renderpass->createRenderpass();
        renderpass->createFramebuffers(*swapchain);

        // Config => Device => Renderpass => Pipeline
        auto *pipeline = renderpass->getPipeline(0);
        pipeline->getRasterizationState()->setCullMode(VK_CULL_MODE_BACK_BIT);

        // --- Configure Vertex Input State ---
        VknVertexInputState *vertexInputState = pipeline->getVertexInputState();
        vertexInputState->fileVertexBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
        vertexInputState->fileVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
        vertexInputState->fileVertexAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));

        VknShaderStage *vertShader = pipeline->addShaderStage(0, vkn::VKN_VERTEX_STAGE, "triangle.vert.spv");
        vertShader->createShaderModule();
        VknShaderStage *fragShader = pipeline->addShaderStage(1, vkn::VKN_FRAGMENT_STAGE, "triangle.frag.spv");
        fragShader->createShaderModule();
        vkn::VknViewportState *viewportState = pipeline->getViewportState();
        viewportState->syncWithSwapchain(*swapchain, 0, 0);
        renderpass->createPipelines();

        device->addCommandPools();
        VknCommandPool *commandPool = device->getCommandPool(QueueType::PRESENT);
        commandPool->createCommandBuffers(swapchain->getNumImages());

        return true;
    }

    bool cpuGenTestCycle(VknCycle &cycle)
    {
    }
}