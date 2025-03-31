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

    template <typename VknT, typename VkT>
    VknT &addNewVknObject(uint32_t idx, std::list<VknT> &list, VknEngine *engine,
                          VknIdxs &relIdxs, VknIdxs &absIdxs, VknInfos *infos)
    {
        if (idx + 1 > list.size())
            throw std::runtime_error("List index out of range.");
        VknIdxs newRelIdxs = relIdxs;
        VknIdxs newAbsIdxs = absIdxs;
        newAbsIdxs.add<VkT>(m_engine->push_back(VkT{}));
        newRelIdxs.add<VkT>(m_devices.size());
        return list.emplace_back(engine, newRelIdxs, newAbsIdxs, infos);
    }
}