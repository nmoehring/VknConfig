#pragma once

#include <filesystem>
#include <list>
#include <fstream> //todo: some of my includes are only used in the cpp, so it may not be obvious, maybe comment as such
#include <string>

namespace vkn
{
    // Forward declarations to avoid circular dependencies
    class VknIdxs;
    class VknInfos;

    template <typename T>
    T *getListElement(uint32_t idx, std::list<T> &objList)
    {
        if (idx + 1 > objList.size())
            throw std::runtime_error("Index out of range.");
        typename std::list<T>::iterator it = objList.begin();
        std::advance(it, idx);
        return &(*it);
    }

    template <typename DataType, typename SizeType = std::uint8_t>
    class VknVector
    {
        SizeType *positions{nullptr};
        DataType *data{nullptr};
        SizeType size{0};
        bool isView{false};
        SizeType nextPosition{0};
        static constexpr SizeType maxSize = ~SizeType{0u};

        void resize(SizeType newSize)
        {
            if (isView)
                throw std::runtime_error("Cannot resize a span-view VknVector.");
            if (newSize > maxSize)
                throw std::runtime_error("Overflow error. newSize of VknVector is greater than the maximum allowed by the SizeType.");
            DataType *newData = new DataType[newSize];
            SizeType *newPositions = new SizeType[newSize];
            for (SizeType i = 0; i < size; ++i)
            {
                newData[i] = data[i];
                newPositions[i] = positions[i];
            }
            this->deleteArrays();
            data = newData;
            positions = newPositions;
        }

        SizeType getNextPosition()
        {
            return nextPosition++;
        }

        SizeType determineNextPosition()
        {
            for (SizeType i = 0; i < size; ++i)
            {
                if (positions[i] > nextPosition)
                    nextPosition = positions[i];
            }
            return ++nextPosition;
        }

        void setPosition(SizeType index, SizeType position)
        {
            if (position > nextPosition)
                nextPosition = position;
            positions[index] = position;
        }

        void deleteArrays()
        {
            if (data)
                delete[] data;
            if (positions)
                delete[] positions;
        }

    public:
        VknVector()
        {
            if (maxSize < 0)
                throw std::runtime_error("SizeType selected for VknVector should be unsigned.");
        }

        ~VknVector()
        {
            if (!isView)
            {
                this->deleteArrays();
            }
        }

        VknVector(const VknVector &other)
        {
            size = other.size;
            positions = new SizeType[size];
            data = new DataType[size];
            for (SizeType i = 0; i < size; ++i)
            {
                this->setPosition(i, other.positions[i]);
                data[i] = other.data[i];
            }
        }

        VknVector &operator=(const VknVector &other)
        {
            this->deleteArrays();
            size = other.size;
            positions = new SizeType[size];
            data = new DataType[size];
            for (SizeType i = 0; i < size; ++i)
            {
                this->setPosition(i, other.positions[i]);
                data[i] = other.data[i];
            }
        }

        VknVector(VknVector &&other)
        {
            size = other.size;
            positions = other.positions;
            data = other.data;
            nextPosition = other.nextPosition;
            isView = other.isView;

            other.deleteArrays();
            other.positions = nullptr;
            other.data = nullptr;
            other.size = 0;
        }

        VknVector &operator=(VknVector &&other)
        {

            this->deleteArrays();
            size = other.size;
            positions = other.positions;
            data = other.data;
            nextPosition = other.nextPosition;
            isView = other.isView;

            other.deleteArrays();
            other.positions = nullptr;
            other.data = nullptr;
            other.size = 0;
        }

        DataType &operator()(SizeType position)
        {
            DataType *result = this->getElement(position);
            if (!result)
                throw std::runtime_error("Element not found in vector!");
            return *result;
        }

        VknVector *getSlice(SizeType startPos, SizeType length)
        {
            if (!data)
                throw std::runtime_error("Cannot getSlice(). VknVector is empty!");
            if (length == 0)
                throw std::runtime_error("Cannot getSlice(). Length is 0");
            if (startPos + length > size) // Also catches if startPos is too large
                throw std::runtime_error("Slice range exceeds vector size.");

            VknVector *result = new VknVector();
            result->isView = true;
            result->data = data + startPos;
            result->positions = positions + startPos;
            result->size = length;
            result->determineNextPosition();
            return result;
        }

        DataType &append(DataType newElement)
        {
            this->resize(size + 1);
            positions[size] = this->getNextPosition();
            data[size] = newElement;
            return data[size++];
        }

        void append(VknVector<DataType, SizeType> &newElements)
        {
            SizeType newSize = size + newElements.size;
            this->resize(newSize);

            for (SizeType i = 0; i < newElements.size(); ++i)
            {
                positions[size + i] = this->getNextPosition();
                data[size + i] = newElements[i];
            }
            size = newSize;
        }

        void append(DataType *arr, SizeType length)
        {
            this->resize(size + length);

            for (SizeType i = 0; i < length; ++i)
            {
                data[size + i] = arr[i];
                positions[size + i] = this->getNextPosition();
                size = size + length;
            }
        }

        void resizeView(SizeType newSize)
        {
            if (!isView)
                throw std::runtime_error("Cannot resize a non-view VknVector.");
            size = newSize;
        }

        DataType *append(DataType value, SizeType length)
        {
            SizeType oldSize{size};
            this->resize(size + length);
            for (SizeType i = oldSize; i < length; ++i)
            {
                data[i] = value;
                positions[i] = this->getNextPosition();
            }
            return &data[oldSize];
        }

        DataType *getElement(SizeType position)
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
            DataType *element = this->getElement(position);
            if (!element)
            {
                this->append(newElement);
                this->setPosition(size - 1, position);
            }
            else
                throw std::runtime_error("Tried to insert into a VknVector element that is already assigned.");
            return *element;
        }

        void swap(SizeType position1, SizeType position2)
        {
            if (size == 0)
                throw std::runtime_error("Vector too small (0 or 1) for swap.");
            SizeType *idx1{nullptr};
            SizeType *idx2{nullptr};
            for (SizeType i = 0; i < size; ++i)
            {
                if (positions[i] == position1)
                    idx1 = &positions[i];
                else if (positions[i] == position2)
                    idx2 = &positions[i];
            }
            if (!idx1 || !idx2)
                throw std::runtime_error("Elements for swap not found!");
            SizeType temp{positions[*idx1]};
            positions[*idx1] = positions[*idx2];
            positions[*idx2] = temp;
        }

        DataType *getData(SizeType numNewElements = 0U)
        {
            if (numNewElements != 0)
            {
                SizeType oldSize{size};
                SizeType newSize{size + numNewElements};
                this->resize(newSize);
                for (SizeType i = oldSize; i < newSize; ++i)
                {
                    data[i] = DataType{};
                    this->setPosition(i, nextPosition);
                }
                return data + oldSize;
            }
            return data;
        }

        // TODO: Custom iterator class for better STL stuff but mainly
        //  need to iterate vector positions rather than array indices
        DataType *begin() { return data; }
        DataType *end() { return data + size; }
        const DataType *begin() const { return data; }
        const DataType *end() const { return data + size; }
        DataType *rbegin() { return data + size - 1; }
        DataType *rend() { return data - 1; }
        const DataType *rbegin() const { return data + size - 1; }
        const DataType *rend() const { return data - 1; }

        bool isEmpty() { return size == 0; }
        bool isNotEmpty() { return size > 0; }
        SizeType getSize() { return size; }
    };

    template <typename DataType, typename SizeType = uint8_t>
    class VknSpace
    {
        VknVector<DataType, SizeType> data{};
        VknVector<VknSpace<DataType, SizeType>, SizeType> subspaces{};
        uint8_t maxDimensions{8};
        uint8_t depth{0};

    public:
        VknSpace() = default;
        VknSpace(uint8_t depth) : depth(depth) {}

        VknSpace<DataType, SizeType> &getSubspace(uint8_t position)
        {
            if (position >= maxDimensions)
                throw std::runtime_error("Position given is out of range.");
            VknSpace *subspace = subspaces.getElement(position);
            if (!subspace)
                subspaces.insert(position, VknSpace{depth + 1u});
            return *subspace;
        }

        VknVector<DataType, SizeType> getDataSlice(SizeType startPos, SizeType length)
        {
            return data.getSlice(startPos, length);
        }

        VknSpace<DataType, SizeType> getSubspaceSlice(SizeType startPos, SizeType length)
        {
            VknVector slice = subspaces.getSlice(startPos, length);
            VknSpace<DataType, SizeType> result{};
            result.subspaces = slice;
        }

        VknSpace<DataType, SizeType> &operator[](uint8_t position)
        {
            this->getSubspace(position);
        }

        uint8_t getNumSubspaces() { return subspaces.getSize(); }
        DataType &operator()(SizeType position) { return data(position); }
        DataType *getData(SizeType newSize = 0) { return data.getData(newSize); }
        VknVector<DataType, SizeType> &getDataVector() { return data; }
        VknVector<VknSpace<DataType, SizeType>, SizeType> &getSubspaceVector() { return subspaces; }
        DataType &append(DataType element) { return data.append(element); }
        DataType &insert(DataType element, SizeType position) { data.insert(position, element); }
        SizeType getDataSize() { return data.getSize(); }
    };

    VknVector<char> readBinaryFile(std::filesystem::path filename);
} // namespace vkn