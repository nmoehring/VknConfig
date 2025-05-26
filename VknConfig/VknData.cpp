#include "include/VknData.hpp"

namespace vkn
{
    std::vector<char> readBinaryFile(std::filesystem::path filename)
    {
        std::ifstream file(filename.string(), std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file!");
        }
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer{};
        file.seekg(0);
        buffer.resize(fileSize);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }
}