#pragma once

#include <iterator>
#include <vector>
#include <filesystem>
#include <list>
#include <fstream>
#include <string>

namespace vkn
{
    struct PosSearchResult
    {
        size_t pos{0};
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
        size_t m_dataSize{0};
        size_t m_posSize{0};
        static constexpr size_t s_maxSizeTypeNum = ~static_cast<SizeType>(0u);

        void resize(size_t newSize)
        {
            if (newSize - 1 > s_maxSizeTypeNum)
                throw std::runtime_error("Overflow error. newSize of VknVector is greater than the maximum allowed by the SizeType.");
            DataType *newData = new DataType[newSize];
            SizeType *newPositions = new SizeType[newSize];
            if (m_data)
            {
                for (size_t i = 0; i < this->getSize(); ++i)
                {
                    newData[i] = m_data[i];
                    newPositions[i] = m_positions[i];
                }
                this->deleteArrays();
            }
            m_data = newData;
            m_positions = newPositions;
            m_dataSize = newSize;
        }

        size_t getNextPosition()
        {
            if (m_posSize > s_maxSizeTypeNum)
                throw std::runtime_error("Overflow error. VknVector is full.");
            return m_posSize++;
        }

        void setPosition(size_t index, size_t position)
        {
            if (position >= m_posSize)
                m_posSize = position + 1u;
            m_positions[index] = position;
        }

        void deleteArrays()
        {
            if (m_data)
            {
                delete[] m_data;
                m_data = nullptr;
            }
            if (m_positions)
            {
                delete[] m_positions;
                m_positions = nullptr;
            }
        }

        PosSearchResult getIdxOfSmallestPos(size_t minPos)
        {
            PosSearchResult result{};

            if (!m_data)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            if (minPos >= m_posSize)
                throw std::runtime_error("Cannot get(). Position out of range.");
            size_t smallestPos{s_maxSizeTypeNum};
            size_t smallestPosIdx{0};
            for (size_t i = 0u; i < this->getSize(); ++i)
                if (m_positions[i] == minPos)
                {
                    result.pos = i;
                    result.found = true;
                    return result;
                }
                else if (m_positions[i] > minPos && m_positions[i] < smallestPos)
                {
                    smallestPos = m_positions[i];
                    smallestPosIdx = i;
                    result.pos = true;
                }
            result.pos = smallestPosIdx;
            return result;
        }

        PosSearchResult getIdxOfLargestPos(size_t maxPos)
        {
            PosSearchResult result;

            if (!m_data)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            if (maxPos >= m_posSize)
                throw std::runtime_error("Cannot get(). Position out of range.");

            size_t largestPos{0};
            size_t largestPosIdx{0};
            for (size_t i = m_dataSize; i > 0; --i)
                if (m_positions[i - 1u] == maxPos)
                {
                    result.pos = i;
                    result.found = true;
                    return result;
                }
                else if (m_positions[i - 1u] < maxPos && m_positions[i - 1u] > largestPosIdx)
                {
                    largestPos = m_positions[i - 1u];
                    largestPosIdx = i - 1u;
                    result.found = true;
                }
            result.pos = largestPosIdx;
            return largestPosIdx;
        }

        PosSearchResult getSmallestPos(size_t minPos)
        {
            PosSearchResult result{};

            if (!m_data)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            if (minPos >= m_posSize)
                throw std::runtime_error("Cannot get(). Position out of range.");
            size_t smallestPos{s_maxSizeTypeNum};
            for (auto &position : m_positions)
                if (position == minPos)
                {
                    result.pos = position;
                    result.found = true;
                    return result;
                }
                else if (position < smallestPos)
                {
                    result.found = true;
                    smallestPos = position;
                }
            result.pos = smallestPos;
            return result;
        }

        PosSearchResult getLargestPos(size_t maxPos)
        {
            PosSearchResult result{};

            if (!m_data)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            if (maxPos >= m_posSize)
                throw std::runtime_error("Cannot get(). Position out of range.");

            size_t largestPos{0};
            for (auto &position : m_positions)
                if (position == maxPos)
                {
                    result.found = true;
                    result.pos = position;
                    return result;
                }
                else if (position > largestPos)
                {
                    largestPos = position;
                    result.found = true;
                }
            result.pos = largestPos;
            return largestPos;
        }

    public:
        friend class VknVectorIterator<DataType, SizeType>;

        VknVector()
        {
            if (s_maxSizeTypeNum < 0)
                throw std::runtime_error("SizeType selected for VknVector should be unsigned.");
        }

        ~VknVector()
        {
            this->deleteArrays();
        }

        VknVector(const VknVector &other)
        {
            size_t otherSize{other.getSize()};
            m_dataSize = other.m_dataSize;
            m_posSize = other.m_posSize;
            m_positions = new SizeType[otherSize];
            m_data = new DataType[otherSize];
            for (size_t i = 0; i < this->getSize(); ++i)
            {
                this->setPosition(i, other.m_positions[i]);
                m_data[i] = other.m_data[i];
            }
        }

        VknVector &operator=(const VknVector &other)
        {
            size_t otherSize{other.getSize()};
            this->deleteArrays();
            m_dataSize = other.m_dataSize;
            m_posSize = other.m_posSize;
            m_positions = new SizeType[otherSize];
            m_data = new DataType[otherSize];
            for (size_t i = 0; i < otherSize; ++i)
            {
                m_positions[i] = other.m_positions[i];
                m_data[i] = other.m_data[i];
            }
            return *this; // Return a reference to the current object
        }

        // VknVector(VknVector &&other) - Move Constructor
        VknVector(VknVector &&other) noexcept // Add noexcept
        {
            // Steal resources
            m_positions = other.m_positions;
            m_data = other.m_data;
            m_dataSize = other.m_dataSize;
            m_posSize = other.m_posSize;

            // Null out other's pointers so its destructor does nothing
            other.m_positions = nullptr;
            other.m_data = nullptr;
            other.m_dataSize = 0; // Or appropriate default
            other.m_posSize = 0;  // Or appropriate default

            // DO NOT call other.deleteArrays(); here!
        }

        // VknVector &operator=(VknVector &&other) - Move Assignment
        VknVector &operator=(VknVector &&other) noexcept // Add noexcept
        {
            if (this != &other) // Protect against self-assignment
            {
                // Release existing resources
                this->deleteArrays();

                // Steal resources from other
                m_positions = other.m_positions;
                m_data = other.m_data;
                m_dataSize = other.m_dataSize;
                m_posSize = other.m_posSize;

                // Null out other's pointers
                other.m_positions = nullptr;
                other.m_data = nullptr;
                other.m_dataSize = 0;
                other.m_posSize = 0;

                // DO NOT call other.deleteArrays(); here!
            }
            return *this;
        }

        DataType &operator()(SizeType position)
        {
            DataType *result = this->getElement(position);
            if (!result)
                throw std::runtime_error("Element not found in vector!");
            return *result;
        }

        VknVectorIterator<DataType, SizeType> getSlice(size_t startPos, size_t length)
        {
            if (!m_data)
                throw std::runtime_error("Cannot getSlice(). VknVector is empty!");
            if (length == 0)
                throw std::runtime_error("Cannot getSlice(). Length is 0");
            if (startPos + length > m_dataSize) // Also catches if startPos is too large
                throw std::runtime_error("Slice range exceeds vector size.");

            return VknVectorIterator<DataType, SizeType>{
                this, startPos, startPos + length - 1u};
        }

        DataType &append(DataType newElement)
        {
            this->resize(this->getSize() + 1u);
            m_positions[m_dataSize - 1u] = this->getNextPosition();
            m_data[m_dataSize - 1u] = newElement;
            return m_data[m_dataSize - 1u];
        }

        VknVectorIterator<DataType, SizeType> append(VknVector<DataType, SizeType> &newElements)
        {
            size_t oldSize{this->getSize()};
            size_t otherSize{newElements.getSize()};
            size_t newSize = oldSize + otherSize;
            this->resize(newSize);

            for (size_t i = 0; i < otherSize; ++i)
            {
                m_positions[oldSize + i] = this->getNextPosition();
                m_data[oldSize + i] = newElements[i];
            }
            return VknVectorIterator(this, oldSize, newSize);
        }

        DataType *append(DataType *arr, size_t length)
        {
            size_t oldSize{this->getSize()};
            this->resize(oldSize + length);

            for (size_t i = 0; i < length; ++i)
            {
                m_data[oldSize + i] = arr[i];
                m_positions[oldSize + i] = this->getNextPosition();
            }
            return &m_data[oldSize];
        }

        DataType *append(DataType value, size_t length)
        {
            size_t oldSize{this->getSize()};
            this->resize(oldSize + length);
            for (size_t i = oldSize; i < this->getSize(); ++i)
            {
                m_data[i] = value;
                m_positions[i] = this->getNextPosition();
            }
            return &m_data[oldSize];
        }

        DataType *getElement(size_t position)
        {
            if (this->getSize() == 0)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            for (size_t i = 0; i < this->getSize(); ++i)
            {
                if (m_positions[i] == position)
                    return &m_data[i];
            }
            return nullptr;
        }

        bool exists(size_t position)
        {
            if (!m_data)
                return false;
            for (size_t i{0}; i < this->getSize(); ++i)
                if (m_positions[i] == position)
                    return true;
            return false;
        }

        DataType &insert(size_t position, DataType newElement)
        {
            if (!m_data || !this->exists(position))
            {
                this->resize(m_dataSize + 1u);
                this->setPosition(m_dataSize - 1u, position);
                m_data[m_dataSize - 1u] = newElement;
            }
            else
                throw std::runtime_error("Tried to insert into a VknVector element that is already assigned.");
            return m_data[m_dataSize - 1u];
        }

        void swap(size_t position1, size_t position2)
        {
            if (m_dataSize <= 1)
                throw std::runtime_error("Vector too small (0 or 1) for swap.");
            SizeType *idx1{nullptr};
            SizeType *idx2{nullptr};
            for (size_t i = 0; i < this->getSize(); ++i)
            {
                if (m_positions[i] == position1)
                    idx1 = &m_positions[i];
                else if (m_positions[i] == position2)
                    idx2 = &m_positions[i];
            }
            if (!idx1 || !idx2)
                throw std::runtime_error("Elements for swap not found!");
            size_t temp{m_positions[*idx1]};
            m_positions[*idx1] = m_positions[*idx2];
            m_positions[*idx2] = temp;
        }

        DataType *getData(size_t numNewElements = 0u)
        {
            size_t oldSize{this->getSize()};
            size_t newSize{oldSize + numNewElements};
            if (newSize > s_maxSizeTypeNum)
                throw std::runtime_error("Resizing of internal array causes an overflow error.");
            if (numNewElements != 0u)
            {
                this->resize(newSize);
                for (size_t i = oldSize; i < newSize; ++i)
                {
                    m_data[i] = DataType{};
                    this->setPosition(i, this->getNextPosition());
                }
                return m_data + oldSize;
            }
            return m_data;
        }

        DataType *begin() { return m_data; }
        DataType *end() { return m_data + this->getSize(); }
        const DataType *begin() const { return m_data; }
        const DataType *end() const { return m_data + this->getSize(); }
        DataType *rbegin() { m_data + this->getSize() - 1u; }
        DataType *rend() { return m_data - 1u; }
        const DataType *rbegin() const { return m_data + this->getSize() - 1u; }
        const DataType *rend() const { return m_data - 1u; }

        bool isEmpty() { return !m_data; }
        bool isNotEmpty() { return m_data; }
        const size_t getSize() const { return m_dataSize; }
        size_t size() { return this->getSize(); }
        size_t getNumPositions() { return m_posSize; }
    };

    template <typename DataType, typename SizeType = uint32_t>
    class VknVectorIterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag; // Or use bidirectional if needed
        using value_type = DataType;
        using difference_type = std::ptrdiff_t;
        using pointer = DataType *;
        using reference = DataType &;

    private:
        VknVector<DataType, SizeType> *m_vec;
        SizeType m_firstPos{0};
        SizeType m_lastPos{0};
        SizeType m_currentIdx{0};
        SizeType m_currentPos{0};

        bool m_isInvalid{false};
        bool m_atEnd{false};
        bool m_atBegin{false};
        bool m_iterFilled{true};
        bool m_isEmpty{false};

    public:
        // TODO: Find out what explicit keyword does
        explicit VknVectorIterator(VknVector<DataType, SizeType> *vknVector,
                                   size_t firstPos = 0, size_t endPos = 0, // Todo
                                   bool iterFilled = true)
            : m_vec{vknVector}, m_iterFilled{iterFilled}
        {
            if (!vknVector->m_data || firstPos == endPos)
            {
                m_isEmpty = true;
                m_firstPos = firstPos;
                m_lastPos = 0;
                m_currentIdx = 0;
                m_currentPos = firstPos;
                m_isInvalid = true;
                return;
            }
            else if (endPos > firstPos)
            {
                m_lastPos = endPos - 1u;
                m_firstPos = firstPos;
                m_isEmpty = false;
                m_isInvalid = false;

                PosSearchResult posSearch{vknVector->getIdxOfSmallestPos(firstPos)};
                if (posSearch.found)
                    m_currentIdx = posSearch.pos;
                else
                    m_isEmpty = true;

                if (!m_isEmpty && m_iterFilled)
                    m_currentPos = m_vec->m_positions[m_currentIdx];
                else
                    m_currentPos = firstPos;
            }
            else if (firstPos > endPos)
                throw std::runtime_error("Invalid VknVectorIterator range (startPos > endPos).");
            else //  ¯\_(ツ)_/¯
                throw std::runtime_error("Invalid VknVectorIterator range.");
        }

        reference operator*() const
        {
            if (m_currentPos != m_vec->m_positions[m_currentIdx])
                throw std::runtime_error("Trying to dereference a null element.");
            if (!m_atEnd && !m_atBegin)
                return m_vec->m_m_data[m_currentIdx]; // Access the current element directly
            throw std::runtime_error("Trying to dereference an out-of-range vector element.");
        }

        pointer operator->() const
        {
            if (m_currentPos != m_vec->m_positions[m_currentIdx])
                return nullptr;
            if (!m_atEnd && !m_atBegin)
                return &m_vec->m_data[m_currentIdx]; // Access the current element directly
            if (m_atEnd)
                return &m_vec->m_data[m_vec->m_dataSize];
            if (m_atBegin)
                return &m_vec->m_data[m_vec->m_data - 1u];
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

            if (m_currentPos == m_lastPos)
            {
                m_atEnd = true;
                return *this;
            }
            else if (m_currentPos != m_vec->m_positions[m_currentIdx])
            {
                ++m_currentPos;
                return *this;
            }

            PosSearchResult nextIdxResult{};
            if (m_currentIdx == VknVector<DataType, SizeType>::s_maxSizeTypeNum)
                nextIdxResult = m_vec->getIdxOfSmallestPos(m_currentPos + 1u);
            else if (m_vec->m_positions[m_currentIdx + 1u] == m_currentPos + 1u)
            {
                nextIdxResult.pos = m_currentIdx + 1u;
                nextIdxResult.found = true;
            }
            else
                nextIdxResult = m_vec->findIdxOfSmallestPos(m_currentPos + 1u);

            if (nextIdxResult.found)
            {
                m_currentIdx = nextIdxResult.pos;
                if (m_iterFilled)
                    m_currentPos = m_vec->m_positions[m_currentIdx];
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

            if (m_currentPos == m_firstPos)
            {
                m_atBegin = true;
                return *this;
            }
            else if (m_currentPos != m_vec->m_positions[m_currentIdx])
            {
                --m_currentPos;
                return *this;
            }

            PosSearchResult nextIdxResult{};
            if (m_currentIdx == VknVector<DataType, SizeType>::s_maxSizeTypeNum)
                nextIdxResult = m_vec->getIdxOfLargestPos(m_currentPos - 1u);
            else if (m_vec->m_positions[m_currentIdx - 1u] == m_currentPos - 1u)
            {
                nextIdxResult.pos = m_currentIdx - 1u;
                nextIdxResult.found = true;
            }
            else
                nextIdxResult = m_vec->findIdxOfLargestPos(m_currentPos - 1u);

            if (nextIdxResult.found)
            {
                m_currentIdx = nextIdxResult.pos;
                if (m_iterFilled)
                    m_currentPos = m_vec->m_positions[m_currentIdx];
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
            VknVectorIterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const VknVectorIterator &other) const
        {
            if (!m_atBegin && !m_atEnd)
                return m_currentPos == other.m_currentPos;
            else if (m_atBegin)
                return m_atBegin == other.m_atBegin;
            else if (m_atEnd)
                return m_atEnd == other.m_atEnd;
        }

        bool operator!=(const VknVectorIterator &other) const
        {
            return !(*this == other);
        }

        bool operator<(const VknVectorIterator &other) const
        {
            if (m_atBegin)
                return !other.m_atBegin;
            else if (other.m_atEnd)
                return !m_atEnd;
            else if (m_atEnd || other.m_atBegin)
                return false;
            else
                return m_currentPos < other.m_currentPos;
        }

        bool operator>(const VknVectorIterator &other) const
        {
            return !(*this < other);
        }

        bool operator<=(const VknVectorIterator &other) const
        {
            if (m_atBegin || other.m_atEnd)
                return true;
            else if (m_atEnd)
                return other.m_atEnd;
            else if (other.m_atBegin)
                return m_atBegin;
            else
                return m_currentPos <= other.m_currentPos;
        }

        bool operator>=(const VknVectorIterator &other) const
        {
            if (m_atEnd || other.m_atBegin)
                return true;
            else if (m_atBegin)
                return other.m_atBegin;
            else if (other.m_atEnd)
                return m_atEnd;
            else
                return m_currentPos >= other.m_currentPos;
        }

        difference_type operator-(const VknVectorIterator &other) const
        {
            return *this - other.m_currentPos;
        }

        difference_type operator+(const VknVectorIterator &other) const
        {
            return *this + other.m_currentPos;
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
            if (m_lastPos - m_currentPos < n)
            {
                m_atEnd = true;
                m_currentPos = m_lastPos;
                PosSearchResult previousIdxRes = m_vec->findIdxOfLargestPos(m_currentPos);
                m_currentIdx = previousIdxRes.pos;
                return *this;
            }
            else if (m_vec->m_positions[m_currentIdx + n] == m_currentPos + n)
            {
                m_currentIdx = m_currentIdx + n;
                m_currentPos = m_currentPos + n;
                return *this;
            }
            else
            {
                m_currentPos = m_currentPos + n;
                PosSearchResult nextIdxResult = m_vec->findIdxOfSmallestPos(m_currentPos);
                if (!nextIdxResult.found)
                    nextIdxResult = m_vec->findIdxOfLargestPos(m_currentPos);
                m_currentIdx = nextIdxResult.pos;
                return *this;
            }
        }

        VknVectorIterator operator-(difference_type nArg) const
        {
            if (m_atBegin)
                throw std::runtime_error("Accessing out-of-range vector element.");
            difference_type n{nArg};
            if (m_atEnd)
            {
                m_atEnd = false;
                n -= 1u;
            }

            if (m_currentPos - m_firstPos < n)
            {
                m_atBegin = true;
                m_currentPos = m_firstPos;
                PosSearchResult previousIdxResult = m_vec->findIdxOfSmallestPos(m_currentPos);
                m_currentIdx = previousIdxResult.pos;
                return *this;
            }
            else if (m_vec->m_positions[m_currentIdx - n] == m_currentPos - n)
            {
                m_currentIdx = m_currentIdx - n;
                m_currentPos = m_currentPos - n;
                return *this;
            }
            else
            {
                m_currentPos = m_currentPos - n;
                PosSearchResult nextIdxResult = m_vec->findIdxOfLargestPos(m_currentPos);
                if (!nextIdxResult.found)
                    nextIdxResult = m_vec->findIdxOfSmallestPos(m_currentPos);
                m_currentIdx = nextIdxResult.pos;
                return *this;
            }
        }

        size_t getSize()
        {
            if (m_isEmpty)
                return 0;
            size_t size{0};
            for (size_t posIdx{0}; posIdx < m_vec->getSize(); ++posIdx)
                if (m_vec->m_positions[posIdx] >= m_firstPos && m_vec->m_positions[posIdx] <= m_lastPos)
                    ++size;
            return size;
        }

        bool isEmpty() { return m_isEmpty; }

        DataType *getData()
        {
            if (m_isEmpty)
                return nullptr;
            PosSearchResult posSearch{m_vec->getIdxOfSmallestPos(m_firstPos)};
            size_t idx{};
            if (posSearch.found)
                idx = posSearch.pos;
            else
                return nullptr;

            if (m_vec->m_positions[idx] != m_firstPos)
                throw std::runtime_error("Cannot getData(). Iterator range is not obviously contiguous.");
            for (size_t i = idx; m_vec->m_positions[i] < m_lastPos; ++i)
                if (m_vec->m_positions[i + 1u] != m_vec->m_positions[i] + 1)
                    throw std::runtime_error("Cannot getData(). Iterator range is not obviously contiguous.");
            return &m_vec->m_data[idx];
        }
    };

    template <typename DataType, typename SizeType = uint32_t>
    class VknSpace
    {
        VknVector<DataType, SizeType> m_data{};
        VknVector<VknSpace<DataType, SizeType>, SizeType> m_subspaces{};
        static constexpr uint32_t s_maxDimensions{8};
        uint32_t m_depth{0};
        uint32_t m_maxDepth{0};

    public:
        // VknSpace() = default;
        VknSpace(uint32_t maxDepth = 255, uint32_t depth = 0) : m_depth(depth), m_maxDepth(maxDepth) {}
        ~VknSpace() = default;

        // Copy Constructor
        VknSpace(const VknSpace &other) : m_data(other.m_data),           // Relies on VknVector's copy constructor
                                          m_subspaces(other.m_subspaces), // Relies on VknVector's copy constructor
                                          m_depth(other.m_depth),
                                          m_maxDepth(other.m_maxDepth)
        {
        }

        // Copy Assignment Operator
        VknSpace &operator=(const VknSpace &other)
        {
            if (this != &other) // Protect against self-assignment
            {
                m_data = other.m_data;           // Relies on VknVector's copy assignment
                m_subspaces = other.m_subspaces; // Relies on VknVector's copy assignment
                m_depth = other.m_depth;
                m_maxDepth = other.m_maxDepth;
            }
            return *this;
        }

        // Move Constructor
        VknSpace(VknSpace &&other) noexcept : m_data(std::move(other.m_data)),           // Relies on VknVector's move constructor
                                              m_subspaces(std::move(other.m_subspaces)), // Relies on VknVector's move constructor
                                              m_depth(other.m_depth)
        {
            // Optional: Reset other's state if necessary, though VknVector move should handle it
            other.m_depth = 0;
            other.m_maxDepth = 255;
        }

        // Move Assignment Operator
        VknSpace &operator=(VknSpace &&other) noexcept
        {
            if (this != &other)
            {
                m_data = std::move(other.m_data);           // Relies on VknVector's move assignment
                m_subspaces = std::move(other.m_subspaces); // Relies on VknVector's move assignment
                m_depth = other.m_depth;
                m_maxDepth = other.m_maxDepth;

                // Optional: Reset other's state
                other.m_depth = 0;
                other.m_maxDepth = 255;
            }
            return *this;
        }

        VknSpace<DataType, SizeType> &getSubspace(SizeType position)
        {
            if (position >= s_maxDimensions)
                throw std::runtime_error("Position given is out of range.");
            if (!m_subspaces.exists(position))
                this->dive(position);
            return *m_subspaces.getElement(position);
        }

        void dive(SizeType position)
        {
            if (m_depth + 1u > m_maxDepth)
                throw std::runtime_error("Trying to dive too deep into VknSpace. New depth > max depth.");
            m_subspaces.insert(position, VknSpace<DataType, SizeType>{
                                             m_maxDepth,
                                             m_depth + 1u});
        }

        VknVector<DataType, SizeType> getSubspaceSlice(SizeType startPos, SizeType length)
        {
            return m_subspaces.getSlice(startPos, length);
        }

        VknSpace<DataType, SizeType> &operator[](uint8_t position)
        {
            return this->getSubspace(position);
        }

        uint8_t getNumSubspaces() { return m_subspaces.getSize(); }
        DataType &operator()(SizeType position) { return m_data(position); }
        DataType *getData(SizeType newSize = 0) { return m_data.getData(newSize); }
        VknVector<DataType, SizeType> &getDataVector() { return m_data; }
        VknVector<VknSpace<DataType, SizeType>, SizeType> &getSubspaceVector() { return m_subspaces; }
        DataType &append(DataType element) { return m_data.append(element); }
        DataType &insert(DataType element, SizeType position) { return m_data.insert(position, element); }
        SizeType getDataSize() { return m_data.getSize(); }
    };

    VknVector<char> readBinaryFile(std::filesystem::path filename);
} // namespace vkn