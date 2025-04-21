#pragma once

#include <filesystem>
#include <list>
#include <fstream> //todo: some of my includes are only used in the cpp, so it may not be obvious, maybe comment as such
#include <string>

#include "VknEngine.hpp"

namespace vkn
{
    // Forward declarations to avoid circular dependencies
    class VknIdxs;
    class VknInfos;

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
        if (idx > objList.size())
            throw std::runtime_error("List index out of range.");
        VknIdxs newRelIdxs = relIdxs;
        VknIdxs newAbsIdxs = absIdxs;
        newAbsIdxs.add<VkT>(engine->push_back(VkT{}));
        newRelIdxs.add<VkT>(objList.size());
        return objList.emplace_back(engine, newRelIdxs, newAbsIdxs, infos);
    }

    template <typename DataType, typename SizeType = std::uint8_t>
    class VknVector
    {
        SizeType *positions{nullptr};
        DataType *data{nullptr};
        SizeType size{0};

        void resize(SizeType newSize)
        {
            DataType *newData = new DataType[newSize];
            SizeType *newPositions = new SizeType[newSize];
            for (SizeType i = 0; i < size; ++i)
            {
                newData[i] = data[i];
                newPositions[i] = positions[i];
            }
            delete[] data;
            delete[] positions;
            data = newData;
            positions = newPositions;
        }

        SizeType getNextPosition()
        {
            if (size == 0)
                return 0;
            if (size == 1)
                return positions[0] + 1;
            SizeType max = positions[0];
            for (SizeType i = 1; i < size; ++i)
                if (positions[i] > max)
                    max = positions[i];
            return max + 1;
        }

    public:
        VknVector() = default;

        ~VknVector()
        {
            delete[] positions;
            delete[] data;
        }

        VknVector(const VknVector &other)
        {
            size = other.size;
            positions = new SizeType[size];
            data = new DataType[size];
            for (SizeType i = 0; i < size; ++i)
            {
                positions[i] = other.positions[i];
                data[i] = other.data[i];
            }
        }

        VknVector &operator=(const VknVector &other)
        {
            delete[] positions;
            delete[] data;
            size = other.size;
            positions = new SizeType[size];
            data = new DataType[size];
            for (SizeType i = 0; i < size; ++i)
            {
                positions[i] = other.positions[i];
                data[i] = other.data[i];
            }
        }

        VknVector(const VknVector &&other)
        {
            size = other.size;
            positions = other.positions;
            data = other.data;

            delete[] other.positions;
            delete[] other.data;
            other.positions = nullptr;
            other.data = nullptr;
            other.size = 0;
        }

        VknVector &operator=(const VknVector &&other)
        {
            size = other.size;
            delete[] positions;
            delete[] data;
            positions = other.positions;
            data = other.data;

            delete[] other.positions;
            delete[] other.data;
            other.positions = nullptr;
            other.data = nullptr;
            others.size = 0;
        }

        DataType &operator()(SizeType position)
        {
            DataType *result = this->get(position);
            if (!result)
                throw std::runtime_error("Element not found in vector!");
            return *result;
        }

        DataType &append(DataType newElement)
        {
            if (data == nullptr)
            {
                data = new DataType[1];
                positions = new SizeType[1];
            }
            else
                this->resize(size + 1);
            positions[size] = this->getNextPosition();
            data[size] = newElement;
            return data[size++];
        }

        DataType append(VknVector<DataType> &newElements)
        {
            SizeType newSize = size + newElements.size;
            if (data == nullptr)
            {
                positions = new SizeType[newElements.size];
                data = new DataType[newElements.size];
            }
            else
                this->resize(newSize) for (int i = 0; i < newElements.size(); ++i)
                {
                    positions[size + i] = this->getNextPosition();
                    data[size + i] = newElements[i];
                }
            size = newSize;
        }

        DataType *get(SizeType position)
        {
            if (size == 0)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            SizeType idx{0};
            for (SizeType i = 0; i < size; ++i)
                if (positions[i] == position)
                    return &data[i];
            return nullptr;
        }

        DataType &insert(SizeType position, DataType newElement)
        {
            DataType *element = this->get(position);
            if (!element)
            {
                this->append(element);
                positions[size - 1] = position;
            }
            else
                (*element) = newElement;
            return *element;
        }

        void swap(SizeType position1, SizeType position2)
        {
            if (size == 0)
                throw std::runtime_error("Vector too small (0 or 1) for swap.");
            SizeType *idx1{nullptr};
            SizeType *idx2{nullptr};
            for (SizeType i = 0; i < size; ++i)
                if (positions[i] == position1)
                    idx1 = &positions[i];
            if (positions[i] == position2)
                idx2 = &positions[i];
            if (!idx1 || !idx2)
                throw std::runtime_error("Elements for swap not found!");
            SizeType temp{positions[*idx1]};
            positions[*idx1] = positions[*idx2];
            positions[*idx2] = temp;
        }

        DataType *getData()
        {
            return data;
        }

        bool isEmpty() { return size == 0; }
        bool isNotEmpty() { return size > 0; }
        SizeType getSize() { return size; }
    };

    template <typename DataType, typename SizeType = uint8_t>
    class VknSpace
    {
        VknVector<DataType, SizeType> data{};
        VknSpace<DataType, SizeType> subspaces{};
        uint8_t maxDimensions{8};
        uint8_t depth{0};

    public:
        VknSpace() = default;
        VknSpace(uint8_t depth) : depth = depth {}

        VknSpace &operator[](uint8_t position)
        {
            if (position >= maxDimensions)
                throw std::runtime_error("Position given is out of range.");
            VknSpace *subspace = subspaces.get(position);
            if (!subspace)
                subspaces.insert(position, VknSpace{depth + 1});
            return subspaces[position];
        }

        DataType &operator()(SizeType position) { return data(position); }
        DataType *getData() { return data.getData(); }
        DataType &append(DataType element) { return data.append(element); }
        DataType &insert(DataType element, SizeType position) { data.insert(position, element); }
        SizeType getDataSize() { return data.getSize(); }
    };
}