#include <vector>
#include <filesystem>

namespace vkn
{
    std::vector<char> readBinaryFile(std::filesystem::path filename);
}