#pragma once

#include <filesystem>
#include <list>
#include <fstream> //todo: some of my includes are only used in the cpp, so it may not be obvious, maybe comment as such
#include <string>

namespace vkn
{
    template <typename SizeType>
    struct PosSearchResult
    {
        SizeType pos{0};
        bool found{false};
    };

    // Forward declarations to avoid circular dependencies
    class VknIdxs;
    class VknInfos;

    // Forward declaration for template friend class
    template <typename DataType, typename SizeType>
    class VknVectorIterator;

    template <typename T>
    T *getListElement(uint32_t idx, std::list<T> &objList)
    {
        if (idx + 1u > objList.size())
            throw std::runtime_error("Index out of range.");
        typename std::list<T>::iterator it = objList.begin();
        std::advance(it, idx);
        return &(*it);
    }

    template <typename DataType, typename SizeType = uint32_t>
    class VknVector
    {
        SizeType *m_positions{nullptr};
        DataType *m_data{nullptr};
        SizeType *m_endIdx{nullptr};
        SizeType *m_endPos{nullptr};
        static constexpr SizeType s_maxPos = ~static_cast<SizeType>(0u);

        void resize(size_t newSize)
        {
            if (newSize > static_cast<size_t>(maxPos))
                throw std::runtime_error("Overflow error. newSize of VknVector is greater than the maximum allowed by the SizeType.");
            DataType *newData = new DataType[newSize];
            SizeType *newPositions = new SizeType[newSize];
            for (size_t i = 0; i < static_cast<size_t>(m_endIdx + 1); i = static_cast<SizeType>(i + 1u))
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
            SizeType result = endPosition;
            endPosition = static_cast<SizeType>(endPosition + 1u);
            return result;
        }

        SizeType findNextPosition(SizeType inputIdx)
        {
            SizeType minNext{maxPos};
            SizeType nextIdx{inputIdx};
            for (SizeType i = 0; i < size; ++i)
                if (positions[i] > positions[inputIdx] && positions[i] < minNext)
                {
                    minNext = positions[i];
                    nextIdx = i;
                }
            return nextIdx;
        }

        SizeType determineNextPosition()
        {
            for (SizeType i = 0; i < size; i = static_cast<SizeType>(i + 1u))
            {
                if (positions[i] > endPosition)
                    endPosition = positions[i];
            }
            return static_cast<SizeType>(endPosition + 1u);
        }

        void setPosition(SizeType index, SizeType position)
        {
            if (position >= endPosition)
                endPosition = static_cast<SizeType>(position + 1u);
            positions[index] = position;
        }

        void deleteArrays()
        {
            if (data)
                delete[] data;
            if (positions)
                delete[] positions;
        }

        PosSearchResult<SizeType> getIdxOfSmallestPos(size_t minPos)
        {
            PosSearchResult<SizeType> result{};

            if (m_endIdx == nullptr)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            if (minPos > m_endPos)
                throw std::runtime_error("Cannot get(). Position out of range.");
            SizeType smallestPos{s_maxPos};
            SizeType smallestPosIdx{0};
            for (size_t i = 0u; i < *m_endIdx + 1u; ++i)
                if (m_positions[i] > position && m_positions[i] < smallestPos)
                {
                    smallestPos = m_positions[i];
                    smallestPosIdx = i;
                    result.pos = true;
                }
                else if (m_positions[i] == position)
                {
                    result.pos = i;
                    result.found = true;
                    return result;
                }
            result.pos = smallestPosIdx;
            return result;
        }

        PosSearchResult<SizeType> getIdxOfLargestPos(size_t maxPos)
        {
            PosSearchResult<SizeType> result;

            if (m_endIdx == nullptr)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            SizeType largestPos{0};
            SizeType largestPosIdx{0};
            for (size_t i = *m_endIdx + 1u; i > 0; --i)
                if (m_positions[i - 1u] < position && m_positions[i - 1u] > largestPosIdx)
                {
                    largestPos = positions[i];
                    largestPosIdx = i;
                    result.found = true;
                }
                else if (m_positions[i - 1u] == position)
                {
                    result.pos = i;
                    result.found = true;
                    return result;
                }
            result.pos = largestPosIdx;
            return largestPosIdx;
        }

        PosSearchResult<SizeType> getSmallestPos(size_t minPos)
        {
            PosSearchResult<SizeType> result{};

            if (m_endIdx == nullptr)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            if (minPos > m_endPos)
                throw std::runtime_error("Cannot get(). Position out of range.");
            SizeType smallestPos{s_maxPos};
            for (auto &position : m_positions)
                if (position < smallestPos)
                {
                    result.found = true;
                    smallestPos = position;
                }
                else if (position == minPos)
                {
                    result.pos = position;
                    result.found = true;
                    return result;
                }
            result.pos = smallestPos;
            return result;
        }

        PosSearchResult<SizeType> getLargestPos(size_t maxPos)
        {
            PosSearchResult<SizeType> result{};

            if (m_endIdx == nullptr)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            SizeType largestPos{0};
            bool anyPosFound{false};
            for (auto &position : m_positions)
                if (position > largestPos)
                {
                    largestPos = position;
                    result.found = true;
                }
                else if (position == minPos)
                {
                    result.found = true;
                    result.pos = position;
                    return result;
                }
            result.pos = largestPos;
            return largestPos;
        }

    public:
        friend class VknVectorIterator<DataType, SizeType>;

        VknVector()
        {
            if (maxPos < 0)
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
            for (SizeType i = 0; i < size; i = static_cast<SizeType>(i + 1u))
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
            for (SizeType i = 0; i < size; i = static_cast<SizeType>(i + 1u))
            {
                this->setPosition(i, other.positions[i]);
                data[i] = other.data[i];
            }
            return *this; // Return a reference to the current object
        }

        VknVector(VknVector &&other)
        {
            size = other.size;
            positions = other.positions;
            data = other.data;
            endPosition = other.endPosition;
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
            endPosition = other.endPosition;
            isView = other.isView;

            other.deleteArrays();
            other.positions = nullptr;
            other.data = nullptr;
            other.size = 0;
            return *this;
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
            if (static_cast<SizeType>(startPos + length) > size) // Also catches if startPos is too large
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
            this->resize(static_cast<SizeType>(size + 1u));
            positions[size] = this->getNextPosition();
            data[size] = newElement;
            return data[static_cast<SizeType>(size + 1u)];
        }

        void append(VknVector<DataType, SizeType> &newElements)
        {
            SizeType newSize = static_cast<SizeType>(size + newElements.size);
            this->resize(newSize);

            for (SizeType i = 0; i < newElements.size(); i = static_cast<SizeType>(i + 1u))
            {
                positions[static_cast<SizeType>(size + i)] = this->getNextPosition();
                data[static_cast<SizeType>(size + i)] = newElements[i];
            }
            size = newSize;
        }

        void append(DataType *arr, SizeType length)
        {
            this->resize(static_cast<SizeType>(size + length));

            for (SizeType i = 0; i < length; i = static_cast<SizeType>(i + 1u))
            {
                data[static_cast<SizeType>(size + i)] = arr[i];
                positions[static_cast<SizeType>(size + i)] = this->getNextPosition();
                size = static_cast<SizeType>(size + length);
            }
        }

        DataType *append(DataType value, SizeType length)
        {
            SizeType oldSize{size};
            this->resize(size + length);
            for (SizeType i = oldSize; i < length; i = static_cast<SizeType>(i + 1u))
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
            for (SizeType i = 0; i < size; i = static_cast<SizeType>(i + 1u))
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
                this->setPosition(static_cast<SizeType>(size - 1u), position);
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
            for (SizeType i = 0; i < size; i = static_cast<SizeType>(i + 1u))
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

        DataType *getData(SizeType numNewElements = 0u)
        {
            if (maxPos - size < numNewElements)
                throw std::runtime_error("Resizing of internal array causes an overflow error.");
            if (numNewElements != 0u)
            {
                SizeType oldSize{size};
                SizeType newSize{static_cast<SizeType>(size + numNewElements)};
                this->resize(newSize);
                for (SizeType i = oldSize; i < newSize; i = static_cast<SizeType>(i + 1u))
                {
                    data[i] = DataType{};
                    this->setPosition(i, endPosition);
                }
                return static_cast<SizeType>(data + oldSize);
            }
            return data;
        }

        // TODO: Custom iterator class for better STL stuff but mainly
        //  need to iterate vector positions rather than array indices
        DataType *begin()
        {
            return VknVectorIterator{
                this, this->getIdxOfSmaller(0), this->getIdxOfLarger(s_maxPos)};
        }
        DataType *end()
        {
            return VknVectorIterator
            {
                this, this->getIdxOfSmaller(0), this->getIdxOfLarger(s_maxPos)
            }
        }
        const DataType *begin() const { return data; }
        const DataType *end() const { return data + size; }
        DataType *rbegin() { data + size - 1u; }
        DataType *rend() { return data - 1u; }
        const DataType *rbegin() const { return data + size - 1u; }
        const DataType *rend() const { return data - 1u; }

        bool isEmpty() { return size == 0; }
        bool isNotEmpty() { return size > 0; }
        SizeType getSize() { return size; }
        SizeType getNumPositions() { return endPosition; }
    };

#include <iterator>
#include <vector>

    template <typename DataType, typename SizeType>
    class VknVectorIterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag; // Or use bidirectional if needed
        using value_type = DataType;
        using difference_type = std::ptrdiff_t;
        using pointer = DataType *;
        using reference = DataType &;
        using vknVector = VknVector<DataType, SizeType> *;

    private:
        vknVector *m_vec;
        SizeType m_startPos;
        SizeType m_endPos;
        SizeType m_currentIdx;
        SizeType m_currentPos;

        bool m_isInvalid{false};
        bool m_atEnd{false};
        bool m_atBegin{false};
        bool m_iterFilled{true};

    public:
        // TODO: Find out what explicit keyword does
        explicit VknVectorIterator(vknVector *theVector, SizeType startPos, SizeType endPos, bool iterFilled = true)
            : m_vec{&theVector}, m_startPos{startPos}, m_endPos{endPos}
        {
            if (endPos > startPos)
            {
                m_currentIdx{theVector->getIdxOfSmallestPos(startPos)};
                if (m_iterFilled)
                    m_currentPos{m_vec->positions[m_currentIdx]};
                else
                    m_currentPos{startPos};
            }
            else if (startPos > endPos)
                throw std::runtime_error("Invalid VknVectorIterator range (startPos > endPos).");
            else // startPos == endPos
                throw std::runtime_error("Invalid VknVectorIterator range (startPos == endPos).");
        }

        reference operator*() const
        {
            if (m_currentPos != m_vec->m_positions[m_currentIdx])
                throw std::runtime_error("Trying to dereference a null element.");
            if (!m_atEnd && !m_atBegin)
                return m_vec->m_data[m_currentIdx]; // Access the current element directly
            throw std::runtime_error("Trying to dereference an out-of-range vector element.");
        }

        pointer operator->() const
        {
            if (m_currentPos != m_vec->m_positions[m_currentIdx])
                return nullptr;
            if (!m_atEnd && !m_atBegin)
                return &m_vec->m_data[m_currentIdx]; // Access the current element directly
            if (m_atEnd)
                return &m_vec->m_data[m_endIdx + 1u];
            if (m_atBegin)
                return &m_vec->m_data[m_endIdx - 1u];
        }

        // Pre-increment
        VknVectorIterator &operator++()
        {
            if (m_atEnd)
                throw std::runtime_error("Accessing out-of-range vector element.");
            else if (m_atBegin)
            {
                m_atBegin = false;
                return *this;
            }

            if (m_currentPos == m_endPos)
            {
                m_atEnd = true;
                return *this;
            }
            else if (m_currentPos != m_vec->m_positions[m_currentIdx])
            {
                ++m_currentPos;
                return *this;
            }

            PosSearchResult<SizeType> nextIdxRes{m_currentIdx};
            if (m_currentIdx == VknVector<DataType, SizeType>.s_maxPos)
                nextIdxRes = m_vec->getIdxOfSmallestPos(currentPos + 1u);
            else if (m_vec->positions[m_currentIdx + 1u] == static_cast<SizeType>(currentPos + 1u))
            {
                nextIdxRes.pos = static_cast<SizeType>(m_currentIdx + 1u);
                nextIdxRes.found = true;
            }
            else
                nextIdxRes = vec->findIdxOfSmallestPos(currentPos + 1u);

            if (nextIdxRes.found)
            {
                m_currentIdx = nextIdxRes.pos;
                if (m_iterFilled)
                    m_currentPos = m_vec->positions[m_currentIdx];
                else
                    ++m_currentPos;
            }
            else // nextIdx not found
                if (m_iterFilled)
                    m_atEnd = true;
                else
                    ++m_currentPos;

            return *this;
        }

        // Post-increment
        VknVectorIterator
        operator++(int)
        {
            VknVectorIterator temp = *this;
            ++(*this);
            return temp;
        }

        // Pre-decrement
        VknVectorIterator &operator--()
        {
            if (m_atBegin)
                throw std::runtime_error("Accessing out-of-range vector element.");
            else if (m_atEnd)
            {
                m_atEnd = false;
                return *this;
            }

            if (m_position == m_beginPos)
            {
                m_atBegin = true;
                return *this;
            }
            else if (m_currentPos != m_vec->m_positions[m_currentIdx])
            {
                --m_currentPos;
                return *this;
            }

            PosSearchResult<SizeType> nextIdxRes{m_currentIdx};
            if (m_currentIdx == VknVector<DataType, SizeType>.s_maxPos)
                nextIdxRes = m_vec->getIdxOfLargestPos(currentPos - 1u);
            else if (m_vec->positions[m_currentIdx - 1u] == static_cast<SizeType>(currentPos - 1u))
            {
                nextIdxRes.pos = static_cast<SizeType>(m_currentIdx - 1u);
                nextIdxRes.found = true;
            }
            else
                nextIdxRes = vec->findIdxOfLargestPos(currentPos - 1u);

            if (nextIdxRes.found)
            {
                m_currentIdx = nextIdxRes.pos;
                if (m_iterFilled)
                    m_currentPos = m_vec->positions[m_currentIdx];
                else
                    --m_currentPos;
            }
            else // nextIdx not found
                if (m_iterFilled)
                    m_atEnd = true;
                else
                    --m_currentPos;

            return *this;
        }

        // Post-decrement
        VknVectorIterator operator--(int)
        {
            DynamicOrderIterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const VknVectorIterator &other) const
        {
            return m_currentPos == other.m_currentPos;
        }

        bool operator!=(const VknVectorIterator &other) const
        {
            return !(*this == other);
        }

        bool operator<(const VknVectorIterator &other) const
        {
            return m_currentPos < other.m_currentPos;
        }

        bool operator>(const VknVectorIterator &other) const
        {
            return m_currentPos > other.m_currentPos;
        }

        bool operator<=(const VknVectorIterator &other) const
        {
            return m_currentPos <= other.m_currentPos;
        }

        bool operator>=(const VknVectorIterator &other) const
        {
            return m_currentPos >= other.m_currentPos;
        }

        difference_type operator-(const VknVectorIterator &other) const
        {
            return m_currentPos - other.m_currentPos;
        }

        VknVectorIterator operator+(difference_type nArg) const
        {
            if (m_atEnd)
                throw std::runtime_error("Accessing out-of-range vector element.");
            difference_type n{nArg};
            if (m_atBegin)
            {
                m_atBegin = false;
                n -= 1u;
            }
            if (m_endPos - m_currentPos < n)
            {
                m_atEnd = true;
                m_currentPos = m_endPos;
                PosSearchResult<SizeType> previousIdxRes = m_vec->findIdxOfLargestPos(m_currentPos);
                m_currentIdx = previousIdxRes.pos;
                return *this;
            }
            else if (m_vec->m_positions[m_currentIdx + n] == m_currentPos + n)
            {
                m_currentIdx = static_cast<SizeType>(m_currentIdx + n);
                m_currentPos = static_cast<SizeType>(m_currentPos + n);
                return *this;
            }
            else
            {
                m_currentPos = m_currentPos + n;
                PosSearchResult<SizeType> nextIdxRes = m_vec->findIdxOfSmallestPos(m_currentPos);
                if (!nextIdxRes.found)
                    nextIdxRes = m_vec->findIdxOfLargestPos(m_currentPos);
                m_currentIdx = nextIdxRes.pos;
                return *this;
            }
        }

        VknVectorIterator operator-(difference_type n) const
        {
            if (m_atBegin)
                throw std::runtime_error("Accessing out-of-range vector element.");
            difference_type n{nArg};
            if (m_atEnd)
            {
                m_atEnd = false;
                n -= 1u;
            }
            if (m_currentPos - m_beginPos < n)
            {
                m_atBegin = true;
                m_currentPos = m_beginPos;
                PosSearchResult<SizeType> previousIdxRes = m_vec->findIdxOfSmallestPos(m_currentPos);
                m_currentIdx = previousIdxRes.pos;
                return *this;
            }
            else if (m_vec->m_positions[m_currentIdx - n] == m_currentPos - n)
            {
                m_currentIdx = static_cast<SizeType>(m_currentIdx - n);
                m_currentPos = static_cast<SizeType>(m_currentPos - n);
                return *this;
            }
            else
            {
                m_currentPos = m_currentPos - n;
                PosSearchResult<SizeType> nextIdxRes = m_vec->findIdxOfLargestPos(m_currentPos);
                if (!nextIdxRes.found)
                    nextIdxRes = m_vec->findIdxOfSmallestPos(m_currentPos);
                m_currentIdx = nextIdxRes.pos;
                return *this;
            }
        }
    };

    template <typename DataType, typename SizeType = uint32_t>
    class VknSpace
    {
        VknVector<DataType, SizeType> data{};
        VknVector<VknSpace<DataType, SizeType>, SizeType> subspaces{};
        uint32_t maxDimensions{8};
        uint32_t depth{0};

    public:
        VknSpace() = default;
        VknSpace(uint32_t depth) : depth(depth) {}

        VknSpace<DataType, SizeType> &getSubspace(SizeType position)
        {
            if (position >= maxDimensions)
                throw std::runtime_error("Position given is out of range.");
            VknSpace *subspace = subspaces.getElement(position);
            if (!subspace)
                subspaces.insert(position, VknSpace{depth + static_cast<SizeType>(1u)});
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
            return result;
        }

        VknSpace<DataType, SizeType> &operator[](uint8_t position)
        {
            return this->getSubspace(position);
        }

        uint8_t getNumSubspaces() { return subspaces.getSize(); }
        DataType &operator()(SizeType position) { return data(position); }
        DataType *getData(SizeType newSize = 0) { return data.getData(newSize); }
        VknVector<DataType, SizeType> &getDataVector() { return data; }
        VknVector<VknSpace<DataType, SizeType>, SizeType> &getSubspaceVector() { return subspaces; }
        DataType &append(DataType element) { return data.append(element); }
        DataType &insert(DataType element, SizeType position) { return data.insert(position, element); }
        SizeType getDataSize() { return data.getSize(); }
    };

    VknVector<char> readBinaryFile(std::filesystem::path filename);
} // namespace vkn