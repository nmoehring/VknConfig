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
        uint_least32_t pos{0u};
        bool found{false};
    };

    // Forward declarations to avoid circular dependencies
    class VknIdxs;
    class VknInfos;

    // Forward declaration for template friend class
    template <typename IterDataType>
    class VknVectorIterator;

    const uint_least16_t MAX_DATA_SIZE = 256u;
    const uint_least16_t MAX_POS = 255u;

    template <typename T>
    T *getListElement(uint_least32_t idx, std::list<T> &objList)
    {
        if (idx + 1u > objList.size())
            throw std::runtime_error("Index out of range.");
        typename std::list<T>::iterator it = objList.begin();
        std::advance(it, idx);
        return &(*it);
    }

    template <typename VecDataType>
    class VknVector
    {
        uint_least8_t *m_positions{nullptr};
        VecDataType *m_data{nullptr};
        uint_least16_t m_dataSize{0u};
        uint_least16_t m_posSize{0u};

        uint_least32_t getNextPosition()
        {
            if (m_posSize > MAX_DATA_SIZE)
                throw std::runtime_error("Overflow error. VknVector is full.");
            return m_posSize++;
        }

        void setPosition(uint_least32_t index, uint_least32_t position)
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

        PosSearchResult getIdxOfSmallestPos(uint_least32_t minPos, uint_least32_t maxPos = MAX_POS)
        {
            PosSearchResult result{};
            if (!m_data || minPos >= m_posSize)
                return result;

            uint_least32_t i{0u};
            uint_least32_t smallestPos{MAX_POS};
            uint_least32_t smallestPosIdx{0u};
            result.found = false;

            for (uint_least32_t i = 0u; i < this->getSize(); ++i)
                if (m_positions[i] == minPos)
                {
                    result.pos = i;
                    result.found = true;
                    return result;
                }
                else if (m_positions[i] > minPos && m_positions[i] <= maxPos && m_positions[i] <= smallestPos)
                {
                    smallestPos = m_positions[i];
                    smallestPosIdx = i;
                    result.found = true;
                }
            result.pos = smallestPosIdx;
            return result;
        }

        PosSearchResult getIdxOfLargestPos(uint_least32_t maxPos, uint_least32_t minPos = MAX_POS)
        {
            PosSearchResult result;
            if (!m_data || maxPos >= m_posSize)
                return result;

            uint_least32_t i{0u};
            result.found = false;
            uint_least32_t largestPos{0u};
            uint_least32_t largestPosIdx{0u};

            for (i = m_dataSize; i > 0u; --i)
                if (m_positions[i - 1u] == maxPos)
                {
                    result.pos = i - 1u;
                    result.found = true;
                    return result;
                }
                else if (m_positions[i - 1u] < maxPos && m_positions[i - 1u] >= minPos && m_positions[i - 1u] >= largestPos)
                {
                    largestPos = m_positions[i - 1u];
                    largestPosIdx = i - 1u;
                    result.found = true;
                }
            result.pos = largestPosIdx;
            return result;
        }

        PosSearchResult getSmallestPos(uint_least32_t minPos, uint_least32_t maxPos = MAX_POS)
        {
            PosSearchResult result{};
            result.found = false;

            if (!m_data || minPos >= m_posSize)
                return result;
            uint_least32_t smallestPos{MAX_POS};
            for (uint_least32_t i = 0u; i < m_dataSize; ++i)
            {
                uint_least32_t position = m_positions[i];
                if (position == minPos)
                {
                    result.pos = position;
                    result.found = true;
                    return result;
                }
                else if (position > minPos && position <= maxPos && position <= smallestPos)
                {
                    result.found = true;
                    smallestPos = position;
                }
            }
            result.pos = smallestPos;
            return result;
        }

        PosSearchResult getLargestPos(uint_least32_t maxPos, uint_least32_t minPos = MAX_POS)
        {
            PosSearchResult result{};
            if (!m_data || maxPos >= m_posSize)
                return result;

            uint_least32_t i{0u};
            uint_least32_t largestPos{0u};
            uint_least32_t position{maxPos};
            result.found = false;

            for (i = 0u; i < m_dataSize; ++i)
            {
                position = m_positions[i];
                if (position == maxPos)
                {
                    result.found = true;
                    result.pos = position;
                    return result;
                }
                else if (position < maxPos && position >= minPos && position >= largestPos)
                {
                    largestPos = position;
                    result.found = true;
                }
            }
            result.pos = largestPos;
            return result;
        }

        VecDataType &_insert(uint32_t pos, VecDataType newElement)
        {
            this->grow(m_dataSize + 1u);
            m_data[m_dataSize - 1u] = newElement;
            m_positions[m_dataSize - 1u] = pos;
            return m_data[m_dataSize - 1u];
        }

        VecDataType *_insert(uint_fast8_t startPos, VecDataType *newElements, uint_least32_t length)
        {
            uint32_t oldSize = m_dataSize;
            this->grow(oldSize + length);
            for (uint_least32_t i{0}; i < length; ++i)
            {
                m_data[i] = newElements[i];
                m_positions[i] = startPos + i;
            }
            return &m_data[oldSize];
        }

    public:
        friend class VknVectorIterator<VecDataType>;

        uint_fast8_t getDefragPos(uint_fast32_t length)
        {
            if (m_dataSize == 0u)
                return 0u;
            else if (m_dataSize == 1u)
                return m_positions[0u];
            else if (this->getSize() == this->getNumPositions())
                return this->getSize();

            uint_fast32_t i{0u};
            PosSearchResult smallestPosRes{};

            for (i = 0u; i < this->getNumPositions(); ++i)
            {
                smallestPosRes = this->getSmallestPos(i);
                if (!smallestPosRes.found && (smallestPosRes.pos - i) + 1u >= length)
                    return i;
                else if (!smallestPosRes.found && (this->getNumPositions() - i) >= length)
                    return i;
                else if (!smallestPosRes.found)
                    break;
                else
                    i = smallestPosRes.pos;
            }
            throw std::runtime_error("Due to fragmentation and/or limited capacity, there is no room in the vector for the specified contiguous data.");
        }

        void remove(uint_fast8_t position)
        {
            if (this->isEmpty())
                throw std::runtime_error("Cannot remove from an empty vector.");
            VecDataType *newData = this->getSize() > 1u ? new VecDataType[this->getSize() - 1u] : nullptr;
            uint_least8_t *newPositions = this->getSize() > 1u ? new uint_least8_t[this->getSize() - 1u] : nullptr;
            for (uint_fast8_t i = 0u; i < this->getSize(); ++i)
            {
                if (m_positions[i] == position)
                    continue;
                newData[i] = m_data[i];
                newPositions[i] = m_positions[i];
            }
            this->deleteArrays();
            m_data = newData;
            m_positions = newPositions;
            --m_dataSize;
            --m_posSize;
        }

        void remove(uint_fast8_t startPos, uint_fast32_t length)
        {
            if (this->isEmpty())
                throw std::runtime_error("Cannot remove from an empty vector.");
            VecDataType *newData = this->getSize() > length ? new VecDataType[this->getSize() - length] : nullptr;
            uint_least8_t *newPositions = this->getSize() > length ? new uint_least8_t[this->getSize() - length] : nullptr;
            for (uint32_t i = 0u; i < this->getSize(); ++i)
            {
                if (m_positions[i] >= startPos && m_positions[i] < startPos + length)
                    continue;
                newData[i] = m_data[i];
                newPositions[i] = m_positions[i];
            }
            this->deleteArrays();
            m_data = newData;
            m_positions = newPositions;
            m_dataSize -= length;
            m_posSize -= length;
        }

        void grow(uint_least32_t newSize)
        {
            if (newSize < this->getSize() || newSize > MAX_DATA_SIZE)
                throw std::runtime_error("Invalid size for VknVector.");
            VecDataType *newData = new VecDataType[newSize];
            uint_least8_t *newPositions = new uint_least8_t[newSize];
            for (uint_least32_t i = 0u; i < this->getSize(); ++i)
            {
                newData[i] = m_data[i];
                newPositions[i] = m_positions[i];
            }
            for (uint_least32_t i = this->getSize(); i < newSize; ++i)
            {
                newData[i] = VecDataType{};
                newPositions[i] = 0;
            }
            this->deleteArrays();
            m_data = newData;
            m_positions = newPositions;
            m_dataSize = newSize;
        }

        void clear()
        {
            this->deleteArrays();
            m_dataSize = 0u;
            m_posSize = 0u;
        }

        VknVector()
        {
        }

        ~VknVector()
        {
            this->deleteArrays();
        }

        VknVector(const VknVector &other)
        {
            m_dataSize = other.m_dataSize;
            m_posSize = other.m_posSize;
            if (!other.m_data)
            {
                m_data = nullptr;
                m_positions = nullptr;
            }
            else
            {
                m_positions = new uint_least8_t[m_dataSize];
                m_data = new VecDataType[m_dataSize];

                for (uint_least32_t i = 0u; i < this->getSize(); ++i)
                {
                    this->setPosition(i, other.m_positions[i]);
                    m_data[i] = other.m_data[i];
                }
            }
        }

        VknVector &operator=(const VknVector &other)
        {
            uint_least32_t otherSize{other.getSize()};
            this->deleteArrays();
            m_dataSize = other.m_dataSize;
            m_posSize = other.m_posSize;
            if (!other.m_data)
            {
                m_data = nullptr;
                m_positions = nullptr;
            }
            else
            {
                m_positions = new uint_least8_t[otherSize];
                m_data = new VecDataType[otherSize];
                for (uint_least32_t i = 0u; i < otherSize; ++i)
                {
                    m_positions[i] = other.m_positions[i];
                    m_data[i] = other.m_data[i];
                }
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
            other.m_dataSize = 0u; // Or appropriate default
            other.m_posSize = 0u;  // Or appropriate default

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
                other.m_dataSize = 0u;
                other.m_posSize = 0u;

                // DO NOT call other.deleteArrays(); here!
            }
            return *this;
        }

        VecDataType &operator()(uint8_t position)
        {
            VecDataType *result = this->getElement(position);
            if (!result)
                throw std::runtime_error("Element not found in vector!");
            return *result;
        }

        VknVectorIterator<VecDataType> getSlice(uint_least32_t startPos, uint_least32_t length)
        {
            if (startPos + length > m_posSize) // Also catches if startPos is too large
                throw std::runtime_error("Slice range exceeds logical vector size.");

            return VknVectorIterator<VecDataType>{
                this, startPos, length};
        }

        VecDataType &append(VecDataType newElement)
        {
            this->grow(this->getSize() + 1u);
            m_positions[m_dataSize - 1u] = this->getNextPosition();
            m_data[m_dataSize - 1u] = newElement;
            return m_data[m_dataSize - 1u];
        }

        VknVectorIterator<VecDataType> append(VknVector<VecDataType> &newElements)
        {
            uint_least32_t i = 0u;
            uint_least32_t oldSize{this->getSize()};
            this->grow(newElements.getSize() + m_dataSize);

            for (i = 0u; i < newElements.m_dataSize; ++i)
            {
                m_data[oldSize + i] = newElements[i];
                m_positions[oldSize + i] = this->getNextPosition();
            }
            return VknVectorIterator(this, oldSize, m_dataSize - oldSize);
        }

        VecDataType *append(VecDataType *arr, uint_least32_t length)
        {
            uint_least32_t i = 0u;
            uint_least32_t oldSize{this->getSize()};
            this->grow(oldSize + length);

            for (i = 0u; i < length; ++i)
            {
                m_data[oldSize + i] = arr[i];
                m_positions[oldSize + i] = this->getNextPosition();
            }
            return &m_data[oldSize];
        }

        VecDataType *append(VecDataType value, uint_least32_t length)
        {
            uint_least32_t i = 0;
            uint_least32_t oldSize{this->getSize()};
            this->grow(oldSize + length);
            for (i = oldSize; i < this->getSize(); ++i)
            {
                m_data[i] = value;
                m_positions[i] = this->getNextPosition();
            }
            return &m_data[oldSize];
        }

        VecDataType *getElement(uint32_t position)
        {
            if (this->getSize() == 0u)
                throw std::runtime_error("Cannot get(). Vector is empty!");
            for (uint32_t i = 0u; i < this->getSize(); ++i)
            {
                if (m_positions[i] == position)
                    return &m_data[i];
            }
            return nullptr;
        }

        uint32_t defragInsert(VecDataType element)
        {
            uint32_t pos = this->getDefragPos(1);
            this->_insert(pos, element);
            return pos;
        }

        uint32_t defragInsert(VecDataType *elements, uint32_t length)
        {
            uint32_t pos = this->getDefragPos(length);
            this->_insert(pos, elements, length);
            return pos;
        }

        bool exists(uint32_t position)
        {
            if (!m_data)
                return false;
            for (uint_least32_t i{0u}; i < this->getSize(); ++i)
                if (m_positions[i] == position)
                    return true;
            return false;
        }

        VecDataType &insert(uint32_t position, VecDataType newElement)
        {
            if (!m_data || !this->exists(position))
            {
                this->grow(m_dataSize + 1u);
                this->setPosition(m_dataSize - 1u, position);
                m_data[m_dataSize - 1u] = newElement;
            }
            else
                throw std::runtime_error("Tried to insert into a VknVector element that is already assigned.");
            return m_data[m_dataSize - 1u];
        }

        VecDataType *insert(uint32_t startPos, VecDataType *newElements, uint32_t length)
        {
            for (uint32_t i{0u}; i < length; ++i)
                if (this->exists(startPos + i))
                    throw std::runtime_error("Tried to insert into a VknVector element that is already assigned.");
            return this->_insert(startPos, newElements, length);
        }

        void swap(uint_least32_t position1, uint_least32_t position2)
        {
            if (m_dataSize <= 1u)
                throw std::runtime_error("Vector too small (0 or 1) for swap.");
            std::optional<uint_least32_t> idx1{};
            std::optional<uint_least32_t> idx2{};
            for (uint_least32_t i = 0u; i < this->getSize(); ++i)
            {
                if (m_positions[i] == position1)
                    idx1 = i;
                else if (m_positions[i] == position2)
                    idx2 = i;
            }
            if (!idx1.has_value() || !idx2.has_value())
                throw std::runtime_error("Elements for swap not found!");
            uint_least32_t temp{m_positions[idx1.value()]};
            m_positions[idx1.value()] = m_positions[idx2.value()];
            m_positions[idx2.value()] = temp;
        }

        VecDataType *getData(uint_least32_t numNewElements = 0u)
        {
            if (numNewElements != 0u)
            {
                if (m_dataSize + numNewElements > MAX_DATA_SIZE)
                    throw std::runtime_error("Resizing of internal array causes an overflow error.");

                uint_least32_t i = 0;
                uint_least32_t oldSize{this->getSize()};
                this->grow(oldSize + numNewElements);
                uint32_t startPos = this->getDefragPos(numNewElements);
                for (i = oldSize; i < m_dataSize; ++i)
                {
                    if (startPos + i >= m_posSize)
                        m_positions[i] = this->getNextPosition();
                    else
                        m_positions[i] = startPos + i;
                }
                return m_data + oldSize;
            }
            return m_data;
        }

        VecDataType *begin() { return m_data; }
        VecDataType *end() { return m_data + this->getSize(); }
        const VecDataType *begin() const { return m_data; }
        const VecDataType *end() const { return m_data + this->getSize(); }
        VecDataType *rbegin() { m_data + this->getSize() - 1u; }
        VecDataType *rend() { return m_data - 1u; }
        const VecDataType *rbegin() const { return m_data + this->getSize() - 1u; }
        const VecDataType *rend() const { return m_data - 1u; }

        bool isEmpty() const { return !m_data; }
        const uint_least32_t getSize() const { return m_dataSize; }
        uint_least32_t size() const { return this->getSize(); }
        uint_least32_t getNumPositions() const { return m_posSize; }
    };

    template <typename IterDataType>
    class VknVectorIterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag; // Or use bidirectional if needed
        using value_type = IterDataType;
        using difference_type = std::ptrdiff_t;
        using pointer = IterDataType *;
        using reference = IterDataType &;

    private:
        VknVector<IterDataType> *m_vec;
        uint_least8_t m_firstPos{0u};
        uint_least8_t m_lastPos{0u};
        uint_least8_t m_currentIdx{0u};
        uint_least8_t m_currentPos{0u};

        bool m_isInvalid{false};
        bool m_atEnd{false};
        bool m_atBegin{false};
        bool m_iterFilledElements{true};
        bool m_isEmpty{false};

    public:
        // TODO: Find out what explicit keyword does
        explicit VknVectorIterator(VknVector<IterDataType> *vknVector,
                                   uint_least32_t firstPos = 0u, uint_least32_t length = 0u, // Todo
                                   bool iterateFilledElementsOnly = true)
            : m_vec(vknVector),
              m_iterFilledElements{iterateFilledElementsOnly}
        {
            if (!vknVector->m_data || length == 0u)
            {
                m_isEmpty = true;
                m_firstPos = firstPos;
                m_lastPos = firstPos;
                m_currentIdx = 0u;
                m_currentPos = firstPos;
                m_atBegin = true;
                m_atEnd = true;
                return;
            }
            else
            {
                m_lastPos = firstPos + length - 1u;
                m_firstPos = firstPos;
                m_isEmpty = false;
                m_isInvalid = false;

                PosSearchResult posSearch{vknVector->getIdxOfSmallestPos(m_firstPos, m_lastPos)};
                if (posSearch.found)
                    m_currentIdx = posSearch.pos;
                else
                {
                    m_isEmpty = true;
                    m_atBegin = true;
                    m_atEnd = true;
                    m_currentIdx = 0u;
                    m_currentPos = firstPos;
                }

                if (!m_isEmpty && m_iterFilledElements)
                    m_currentPos = m_vec->m_positions[m_currentIdx];
                else
                    m_currentPos = firstPos;
            }
        }

        reference operator*() const
        {
            if (m_isEmpty)
                throw std::runtime_error("Trying to dereference an element of an empty vector.");
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
            if (m_currentIdx == MAX_POS)
                nextIdxResult = m_vec->getIdxOfSmallestPos(m_currentPos + 1u, m_lastPos);
            else if (m_vec->m_positions[m_currentIdx + 1u] == m_currentPos + 1u)
            {
                nextIdxResult.pos = m_currentIdx + 1u;
                nextIdxResult.found = true;
            }
            else
                nextIdxResult = m_vec->getIdxOfSmallestPos(m_currentPos + 1u, m_lastPos);

            if (nextIdxResult.found)
            {
                m_currentIdx = nextIdxResult.pos;
                if (m_iterFilledElements)
                    m_currentPos = m_vec->m_positions[m_currentIdx];
                else
                    ++m_currentPos;
            }
            else // nextIdx not found
                if (m_iterFilledElements)
                    m_atEnd = true;
                else
                    ++m_currentPos;

            return *this;
        }

        // Post-increment
        VknVectorIterator
        operator++(int)
        {
            throw std::runtime_error("No post-increment operator defined for VknVectorIterator.");
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
            if (m_currentIdx == MAX_POS)
                nextIdxResult = m_vec->getIdxOfLargestPos(m_currentPos - 1u, m_firstPos);
            else if (m_vec->m_positions[m_currentIdx - 1u] == m_currentPos - 1u)
            {
                nextIdxResult.pos = m_currentIdx - 1u;
                nextIdxResult.found = true;
            }
            else
                nextIdxResult = m_vec->getIdxOfLargestPos(m_currentPos - 1u, m_firstPos);

            if (nextIdxResult.found)
            {
                m_currentIdx = nextIdxResult.pos;
                if (m_iterFilledElements)
                    m_currentPos = m_vec->m_positions[m_currentIdx];
                else
                    --m_currentPos;
            }
            else // nextIdx not found
                if (m_iterFilledElements)
                    m_atEnd = true;
                else
                    --m_currentPos;

            return *this;
        }

        // Post-decrement
        VknVectorIterator operator--(int)
        {
            throw std::runtime_error("No post-decrement operator defined for VknVectorIterator.");
        }

        bool operator==(const VknVectorIterator &other) const
        {
            if (m_vec != other.m_vec)
                return false;
            else if (!m_atBegin && !m_atEnd)
                return m_currentPos == other.m_currentPos;
            else if (m_atBegin)
                return m_atBegin == other.m_atBegin;
            else if (m_atEnd)
                return m_atEnd == other.m_atEnd;
            else
                return m_currentPos == other.m_currentPos;
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

        difference_type operator-(const VknVectorIterator &other)
        {
            int diff = m_currentPos - other.m_currentPos;
            if (diff < 0)
                throw std::runtime_error("Accessing out-of-range vector element.");
            return diff;
        }

        difference_type operator+(const VknVectorIterator &other)
        {
            difference_type max = static_cast<difference_type>(0u) - 1u;
            if (m_currentPos > max - other.m_currentPos)
                throw std::runtime_error("Accessing out-of-range vector element.");
            return m_currentPos + other.m_currentPos;
        }

        VknVectorIterator operator+(difference_type diff)
        {
            VknVectorIterator<IterDataType> temp{*this};
            if (m_atEnd)
                throw std::runtime_error("Accessing out-of-range vector element.");
            if (m_currentPos > MAX_POS - diff) // same as m_currentPos + diff > maxSizeTypeNum
            {
                temp.m_currentPos = MAX_POS;
                temp.m_atEnd = true;
            }
            else
                temp.m_currentPos = m_currentPos + diff;

            if (m_atBegin)
            {
                temp.m_atBegin = false;
                temp.m_currentPos -= 1u;
            }
            if (temp.m_currentPos > temp.m_lastPos || temp.m_atEnd)
            {
                temp.m_atEnd = true;
                temp.m_currentPos = temp.m_lastPos;
                PosSearchResult previousIdxRes = temp.m_vec->getIdxOfLargestPos(temp.m_currentPos, temp.m_firstPos);
                temp.m_currentIdx = previousIdxRes.pos; // Not empty, there has to be something
                return temp;
            }
            else
            {
                PosSearchResult searchResult = temp.m_vec->getIdxOfSmallestPos(temp.m_currentPos, temp.m_lastPos);
                if (!searchResult.found)
                    searchResult = temp.m_vec->getIdxOfLargestPos(temp.m_currentPos, temp.m_firstPos);
                temp.m_currentIdx = searchResult.pos;
                return temp;
            }
        }

        VknVectorIterator operator-(difference_type diff)
        {
            VknVectorIterator<IterDataType> temp{*this};
            if (m_atBegin)
                throw std::runtime_error("Accessing out-of-range vector element.");
            if (m_currentPos < diff) // same as m_currentPos - diff < 0
            {
                temp.m_currentPos = 0u;
                temp.m_atBegin = true;
            }
            else
                temp.m_currentPos -= diff;

            if (m_atEnd)
            {
                temp.m_atEnd = false;
                temp.m_currentPos -= 1u;
            }

            if (temp.m_currentPos < temp.m_firstPos || temp.m_atBegin)
            {
                temp.m_atBegin = true;
                temp.m_currentPos = temp.m_firstPos;
                PosSearchResult previousIdxResult = temp.m_vec->getIdxOfSmallestPos(temp.m_currentPos, temp.m_lastPos);
                temp.m_currentIdx = previousIdxResult.pos; // Not empty, there has to be something
                return temp;
            }
            else
            {
                PosSearchResult nextIdxResult = temp.m_vec->getIdxOfLargestPos(temp.m_currentPos, temp.m_firstPos);
                if (!nextIdxResult.found)
                    nextIdxResult = temp.m_vec->getIdxOfSmallestPos(temp.m_currentPos, temp.m_lastPos);
                temp.m_currentIdx = nextIdxResult.pos;
                return temp;
            }
        }

        uint_least32_t getSize()
        {
            if (!m_iterFilledElements) // Size is number of positions, even if not filled
                return (m_lastPos - m_firstPos) + 1u;
            else if (m_isEmpty)
                return 0u;
            else // Size is number of filled positions, which may be out of order
            {
                uint_least32_t size{0u};
                for (uint_least32_t idx{0u}; idx < m_vec->getSize(); ++idx)
                    if (m_vec->m_positions[idx] >= m_firstPos && m_vec->m_positions[idx] <= m_lastPos)
                        ++size;
                return size;
            }
        }

        bool isEmpty() { return m_isEmpty; }

        IterDataType *getData()
        {
            if (m_isEmpty)
                return nullptr;
            PosSearchResult posSearch{m_vec->getIdxOfSmallestPos(m_firstPos)};
            uint_least32_t idx{};
            if (posSearch.found)
                idx = posSearch.pos;
            else
                return nullptr;

            if (m_vec->m_positions[idx] != m_firstPos)
                throw std::runtime_error("Cannot getData(). Iterator range is not obviously contiguous.");
            for (uint_least32_t i = idx; m_vec->m_positions[i] < m_lastPos; ++i)
                if (m_vec->m_positions[i + 1u] != m_vec->m_positions[i] + 1u)
                    throw std::runtime_error("Cannot getData(). Iterator range is not obviously contiguous.");
            return &m_vec->m_data[idx];
        }

        uint_least32_t getLogicalPosition()
        {
            return m_currentPos;
        }

        bool isAtBegin() { return m_atBegin; }
        bool isAtEnd() { return m_atEnd; }
        bool iteratesFilledElements() { return m_iterFilledElements; }
        bool isInvalid() { return m_isInvalid; }
    };

    template <typename SpaceDataType>
    class VknSpace
    {
        VknVector<SpaceDataType> m_data{};
        VknVector<VknSpace<SpaceDataType>> m_subspaces{};
        static constexpr uint_least32_t s_maxDimensions{8};
        uint_least32_t m_depth{0u};
        uint_least32_t m_maxDepth{0u};
        bool m_dataOnLeafsOnly{true};

    public:
        // VknSpace() = default;
        VknSpace(uint_least32_t maxDepth = MAX_POS, uint_least32_t depth = 0u, bool dataOnLeafsOnly = true)
            : m_depth(depth), m_maxDepth(maxDepth), m_dataOnLeafsOnly{dataOnLeafsOnly} {}
        ~VknSpace() = default;

        // Copy Constructor
        VknSpace(const VknSpace &other) : m_data(other.m_data),           // Relies on VknVector's copy constructor
                                          m_subspaces(other.m_subspaces), // Relies on VknVector's copy constructor
                                          m_depth(other.m_depth),
                                          m_maxDepth(other.m_maxDepth),
                                          m_dataOnLeafsOnly{other.m_dataOnLeafsOnly}
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
                m_dataOnLeafsOnly = other.m_dataOnLeafsOnly;
            }
            return *this;
        }

        // Move Constructor
        VknSpace(VknSpace &&other) noexcept : m_data(std::move(other.m_data)),           // Relies on VknVector's move constructor
                                              m_subspaces(std::move(other.m_subspaces)), // Relies on VknVector's move constructor
                                              m_maxDepth(other.m_maxDepth),
                                              m_dataOnLeafsOnly(other.m_dataOnLeafsOnly)
        {
            // Optional: Reset other's state if necessary, though VknVector move should handle it
            other.m_depth = 0u;
            other.m_maxDepth = MAX_POS;
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
                m_dataOnLeafsOnly = other.m_dataOnLeafsOnly;

                // Optional: Reset other's state
                other.m_depth = 0u;
                other.m_maxDepth = MAX_POS;
            }
            return *this;
        }

        VknSpace<SpaceDataType> &getSubspace(uint8_t position)
        {
            if (position >= s_maxDimensions)
                throw std::runtime_error("Position given is out of range.");
            if (!m_subspaces.exists(position))
                this->dive(position);
            return *m_subspaces.getElement(position);
        }

        void dive(uint8_t position)
        {
            if (m_depth + 1u > m_maxDepth)
                throw std::runtime_error("Trying to dive too deep into VknSpace. New depth > max depth.");
            m_subspaces.insert(position, VknSpace<SpaceDataType>{m_maxDepth, m_depth + 1u, m_dataOnLeafsOnly});
        }

        VknVectorIterator<VknSpace<SpaceDataType>> getSubspaceSlice(
            uint8_t startPos, uint8_t length)
        {
            return m_subspaces.getSlice(startPos, length);
        }

        VknSpace<SpaceDataType> &operator[](uint_least32_t position)
        {
            return this->getSubspace(position);
        }

        void dataLeafTest()
        {
            if (m_dataOnLeafsOnly && m_depth != m_maxDepth)
                throw std::runtime_error("Data On Leafs Only is set. Cannot access data members at any depth lower than maxDepth.");
        }

        void dataExistsTest()
        {
            if (m_data.isEmpty())
                throw std::runtime_error("No data is set at this depth.");
        }

        uint_least32_t getNumSubspaces() { return m_subspaces.getSize(); }
        SpaceDataType &operator()(uint8_t position)
        {
            this->dataLeafTest();
            this->dataExistsTest();
            return m_data(position);
        }
        SpaceDataType *getData(uint8_t newSize = 0u)
        {
            this->dataLeafTest();
            return m_data.getData(newSize);
        }
        VknVector<SpaceDataType> &getDataVector()
        {
            this->dataLeafTest();
            return m_data;
        }
        VknVector<VknSpace<SpaceDataType>> &getSubspaceVector() { return m_subspaces; }
        SpaceDataType &append(SpaceDataType element)
        {
            this->dataLeafTest();
            return m_data.append(element);
        }
        SpaceDataType &insert(SpaceDataType element, uint8_t position)
        {
            this->dataLeafTest();
            return m_data.insert(position, element);
        }
        uint8_t getDataSize()
        {
            this->dataLeafTest();
            return m_data.getSize();
        }
        uint8_t getDepth() { return m_depth; }
        uint8_t getMaxDepth() { return m_maxDepth; }
    };

    std::vector<char> readBinaryFile(std::filesystem::path filename);
#ifdef __ANDROID__
    std::vector<char> readAssetFile(const std::string &assetPath);
#endif
} // namespace vkn