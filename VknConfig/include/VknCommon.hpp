#include <vector>
#include <filesystem>
#include <list>
#include "VknEngine.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    std::vector<char> readBinaryFile(std::filesystem::path filename);

    template <typename T>
    T *getListElement(uint32_t idx, std::list<T> &objList)
    {
        if (idx + 1 > objList.size())
            throw std::runtime_error("Index out of range.");
        typename std::list<T>::iterator it = objList.begin();
        std::advance(it, idx);
        return &(*it);
    }

    template <typename VknT, typename VkT>
    VknT &addNewVknObject(uint32_t idx, std::list<VknT> &objList, VknEngine *engine,
                          VknIdxs &relIdxs, VknIdxs &absIdxs, VknInfos *infos)
    {
        if (idx + 1 > objList.size())
            throw std::runtime_error("List index out of range.");
        VknIdxs newRelIdxs = relIdxs;
        VknIdxs newAbsIdxs = absIdxs;
        newAbsIdxs.add<VkT>(engine->push_back(VkT{}));
        newRelIdxs.add<VkT>(objList.size());
        return objList.emplace_back(engine, newRelIdxs, newAbsIdxs, infos);
    }
}