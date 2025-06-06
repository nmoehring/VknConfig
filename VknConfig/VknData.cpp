#include "include/VknData.hpp"
#ifdef __ANDROID__
#include <android/asset_manager.h>
// Declare g_assetManager if it's not accessible otherwise.
extern AAssetManager *g_assetManager; // Ensure this is accessible
#endif
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

#ifdef __ANDROID__
    std::vector<char> readAssetFile(const std::string &assetPath)
    {
        if (!g_assetManager)
        {
            throw std::runtime_error("Asset manager not set for reading asset: " + assetPath);
        }

        AAsset *asset = AAssetManager_open(g_assetManager, assetPath.c_str(), AASSET_MODE_BUFFER);
        if (!asset)
        {
            throw std::runtime_error("Failed to open asset: " + assetPath);
        }

        off_t assetSize = AAsset_getLength(asset);
        std::vector<char> buffer(assetSize);

        int bytesRead = AAsset_read(asset, buffer.data(), assetSize);
        AAsset_close(asset);

        if (bytesRead < assetSize)
        {
            throw std::runtime_error("Failed to read entire asset: " + assetPath);
        }
        return buffer;
    }
#endif

}