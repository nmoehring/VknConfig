#include "include/VknData.hpp"

namespace vkn
{
    VknVector<char> readBinaryFile(std::filesystem::path filename)
    {
        std::ifstream file(filename.string(), std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file!");
        }
        size_t fileSize = (size_t)file.tellg();
        VknVector<char> buffer{};
        file.seekg(0);
        file.read(buffer.getData(fileSize), fileSize);
        file.close();
        return buffer;
    }
}