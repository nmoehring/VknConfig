#include <vector>
#include <filesystem>
#include <list>
#include "VknEngine.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    std::vector<char> readBinaryFile(std::filesystem::path filename);

    template <typename T>
    T *getListElement(uint32_t idx, std::list<T> &list)
    {
        if (swapchainIdx + 1 > list.size())
            throw std::runtime_error("Index out of range.");
        std::list<T>::iterator it = m_swapchains.begin();
        std::advance(it, swapchainIdx);
        return &(*it);
    }

    template <typename T>
    T *addNewVknObject(uint32_t idx, std::list<T> &list, VknEngine *engine,
                       VknIdxs &relIdxs, VknIdxs &absIdxs, VknInfos *infos)
    {
        VknIdxs newRelIdxs = relIdxs;
        VknIdxs newAbsIdxs = absIdxs;
        newAbsIdxs.add<T>(m_engine->push_back(VkDevice{}));
        newRelIdxs.add<T>(m_devices.size());
    }
}