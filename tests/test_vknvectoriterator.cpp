// tests/test_vknvectoriterator.cpp
#include "gtest/gtest.h"
#include "../VknConfig/include/VknData.hpp" // Adjust path as necessary

// No need for friend class declaration if testing public API only.

class VknVectorIteratorTest : public ::testing::Test
{
protected:
    // Use a VknVector to create iterators from
    vkn::VknVector<int> vec_int;

    void SetUp() override
    {
        // Common setup if needed
    }

    void TearDown() override
    {
        // Common teardown if needed
    }

    // Helper to populate the vector with specific logical positions
    void PopulateVector(const std::vector<std::pair<uint32_t, int>> &elements)
    {
        for (const auto &pair : elements)
        {
            // Use insert to place elements at specific logical positions
            // This will determine the physical layout based on insertion order
            vec_int.insert(pair.first, pair.second);
        }
    }
};

// --- Constructor Tests ---

TEST_F(VknVectorIteratorTest, Constructor_EmptyVector_IsEmpty)
{
    // An iterator created from an empty vector with length 0 should be empty
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 0);
    ASSERT_TRUE(it.isEmpty());
    ASSERT_EQ(it.getSize(), 0);
    // Dereferencing/incrementing an empty iterator is undefined behavior,
    // but your iterator throws, which is a valid implementation choice.
    ASSERT_THROW(*it, std::runtime_error);
    ASSERT_THROW(++it, std::runtime_error);
}

TEST_F(VknVectorIteratorTest, Constructor_InvalidRange_Throws)
{
    PopulateVector({{0, 10}, {1, 20}}); // Vector has 2 elements (physical indices 0, 1)
    // Try to create a slice starting beyond the end
    ASSERT_NO_THROW(vec_int.getSlice(2, 1));
    ASSERT_TRUE(vec_int.getSlice(2, 1).isEmpty());
    // Try to create a slice that goes beyond the end
    ASSERT_NO_THROW(vec_int.getSlice(1, 2));
    ASSERT_EQ(vec_int.getSlice(1, 2).getSize(), 1);
    // Try to create a slice with length 0 but from an empty vector (your getSlice throws)
    ASSERT_NO_THROW(vkn::VknVector<int>().getSlice(0, 0)); // This tests getSlice itself
    ASSERT_TRUE(vkn::VknVector<int>().getSlice(0, 0).isEmpty());
}

TEST_F(VknVectorIteratorTest, Constructor_ZeroLengthSlice_IsEmpty)
{
    PopulateVector({{0, 10}, {1, 20}}); // Vector has 2 elements
    // A slice of length 0 should be empty, even from a non-empty vector
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 0);
    ASSERT_TRUE(it.isEmpty());
    ASSERT_EQ(it.getSize(), 0);
    ASSERT_THROW(*it, std::runtime_error);
    ASSERT_THROW(++it, std::runtime_error);
}

// --- Iteration Tests (Operator++, Operator*) ---

TEST_F(VknVectorIteratorTest, Iterate_PhysicallyContiguous_LogicallyContiguous)
{
    // Elements inserted in order, positions 0, 1, 2 -> physical indices 0, 1, 2
    PopulateVector({{0, 10}, {1, 20}, {2, 30}});             // m_positions = {0, 1, 2}
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 3); // Slice covers physical indices 0, 1, 2

    ASSERT_FALSE(it.isEmpty());
    ASSERT_EQ(it.getSize(), 3);

    // First element
    ASSERT_EQ(*it, 10);                    // Should be value at logical pos 0
    ASSERT_EQ(it.getLogicalPosition(), 0); // Assuming you add a getLogicalPosition() to iterator

    // Second element
    ++it;
    ASSERT_EQ(*it, 20); // Should be value at logical pos 1
    ASSERT_EQ(it.getLogicalPosition(), 1);

    // Third element
    ++it;
    ASSERT_EQ(*it, 30); // Should be value at logical pos 2
    ASSERT_EQ(it.getLogicalPosition(), 2);

    // Past the end
    ++it;
    ASSERT_TRUE(it.isAtEnd()); // Assuming you add isAtEnd()
    ASSERT_THROW(*it, std::runtime_error);
    ASSERT_THROW(++it, std::runtime_error);
}

TEST_F(VknVectorIteratorTest, Slice_TooSmall)
{
    // Elements inserted in order, positions 5, 10, 15 -> physical indices 0, 1, 2
    PopulateVector({{5, 50}, {10, 100}, {15, 150}});         // m_positions = {5, 10, 15}
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 3); // Slice covers physical indices 0, 1, 2

    ASSERT_TRUE(it.isEmpty()); // it slices logical positions, not indices. There's nothing there.
    ASSERT_EQ(it.getSize(), 0);
    ASSERT_TRUE(it.isAtBegin());
    ASSERT_TRUE(it.isAtEnd());
    ASSERT_THROW(++it, std::runtime_error);
}

TEST_F(VknVectorIteratorTest, Iterate_LogicallySparse)
{
    // Insert elements to create sparse physical indices for some logical positions
    vec_int.insert(0, 0);    // phys 0, pos 0
    vec_int.insert(10, 100); // phys 1, pos 10
    vec_int.insert(5, 50);   // phys 2, pos 5
    vec_int.insert(15, 150); // phys 3, pos 15
    // m_positions might be {0, 10, 5, 15} depending on insert implementation

    // Let's create a slice covering physical indices 1 and 2 (logical positions 10 and 5)
    vkn::VknVectorIterator<int> it = vec_int.getSlice(5, 6); // Slice covers physical indices 1, 2

    ASSERT_FALSE(it.isEmpty());
    ASSERT_EQ(it.getSize(), 2); // Should be 2 elements in the slice

    // The iterator should visit elements in logical order within the slice {10, 5}
    // Smallest logical pos in {10, 5} is 5.
    ASSERT_EQ(*it, 50); // Value at logical pos 5
    ASSERT_EQ(it.getLogicalPosition(), 5);

    // Next logical pos in {10, 5} is 10.
    ++it;
    ASSERT_EQ(*it, 100); // Value at logical pos 10
    ASSERT_EQ(it.getLogicalPosition(), 10);

    // Past the end of the slice
    ++it;
    ASSERT_TRUE(it.isAtEnd());
}

// --- Decrement Tests (Operator--) ---

TEST_F(VknVectorIteratorTest, Decrement_FromEnd_ToBeginning)
{
    PopulateVector({{0, 10}, {1, 20}, {2, 30}});
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 3);
    vkn::VknVectorIterator<int> end_it = vec_int.getSlice(0, 3); // Need an end iterator
    // How to get a valid end iterator? Your iterator doesn't provide begin/end methods.
    // Let's assume an iterator constructed with length 0 at the end position represents end().
    // This is tricky with your current iterator design.
    // A standard end iterator is one past the last element.
    // Let's manually advance an iterator to the end state.
    vkn::VknVectorIterator<int> it_at_end = vec_int.getSlice(0, 3);
    ++it_at_end;
    ++it_at_end;
    ++it_at_end; // Advance to end

    ASSERT_TRUE(it_at_end.isAtEnd());

    // Decrement from end
    --it_at_end;
    ASSERT_FALSE(it_at_end.isAtEnd());
    ASSERT_EQ(*it_at_end, 30); // Value at logical pos 2
    ASSERT_EQ(it_at_end.getLogicalPosition(), 2);

    --it_at_end;
    ASSERT_EQ(*it_at_end, 20); // Value at logical pos 1
    ASSERT_EQ(it_at_end.getLogicalPosition(), 1);

    --it_at_end;
    ASSERT_EQ(*it_at_end, 10); // Value at logical pos 0
    ASSERT_EQ(it_at_end.getLogicalPosition(), 0);

    // Before the beginning
    --it_at_end;
    ASSERT_TRUE(it_at_end.isAtBegin()); // Assuming you add isAtBegin()
    ASSERT_THROW(*it_at_end, std::runtime_error);
    ASSERT_THROW(--it_at_end, std::runtime_error);
}

// --- Comparison Tests ---

TEST_F(VknVectorIteratorTest, Comparison_Equal)
{
    PopulateVector({{0, 10}, {1, 20}});
    vkn::VknVectorIterator<int> it1 = vec_int.getSlice(0, 2); // Slice covering phys 0, 1
    vkn::VknVectorIterator<int> it2 = vec_int.getSlice(0, 2); // Another iterator for the same slice

    ASSERT_TRUE(it1 == it2);
    ASSERT_FALSE(it1 != it2);

    ++it1;
    ASSERT_FALSE(it1 == it2);
    ASSERT_TRUE(it1 != it2);

    ++it2;
    ASSERT_TRUE(it1 == it2); // Should be equal if they are at the same logical position
}

TEST_F(VknVectorIteratorTest, Comparison_LessThan)
{
    PopulateVector({{0, 10}, {1, 20}, {2, 30}});
    vkn::VknVectorIterator<int> it1 = vec_int.getSlice(0, 3); // At logical pos 0
    vkn::VknVectorIterator<int> it2 = vec_int.getSlice(0, 3);
    ++it2; // At logical pos 1

    ASSERT_TRUE(it1 < it2);
    ASSERT_FALSE(it2 < it1);
    ASSERT_FALSE(it1 < it1);
}

// Add tests for >, <=, >= similarly

// --- Arithmetic Tests ---

TEST_F(VknVectorIteratorTest, Arithmetic_Add)
{
    PopulateVector({{0, 10}, {1, 20}, {2, 30}, {3, 40}, {4, 50}});
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 5); // At logical pos 0

    vkn::VknVectorIterator<int> it_plus_2 = it + 2; // Should move 2 logical steps
    ASSERT_EQ(*it_plus_2, 30);                      // Value at logical pos 2
    ASSERT_EQ(it_plus_2.getLogicalPosition(), 2);

    vkn::VknVectorIterator<int> it_plus_4 = it + 4; // Should move 4 logical steps
    ASSERT_EQ(*it_plus_4, 50);                      // Value at logical pos 4
    ASSERT_EQ(it_plus_4.getLogicalPosition(), 4);

    // Test adding past the end (should result in end state)
    vkn::VknVectorIterator<int> it_plus_5 = it + 5;
    ASSERT_TRUE(it_plus_5.isAtEnd());
}

TEST_F(VknVectorIteratorTest, Arithmetic_Subtract)
{
    PopulateVector({{0, 10}, {1, 20}, {2, 30}, {3, 40}, {4, 50}});
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 5);
    // Advance to logical pos 3
    vkn::VknVectorIterator<int> it_at_3 = it + 3;

    vkn::VknVectorIterator<int> it_minus_2 = it_at_3 - 2; // Should move back 2 logical steps
    ASSERT_EQ(*it_minus_2, 20);                           // Value at logical pos 1
    ASSERT_EQ(it_minus_2.getLogicalPosition(), 1);

    // Test subtracting before the beginning (should throw or result in begin state)
    // Your operator- throws if the target logical position is before the slice's logical range
    vkn::VknVectorIterator<int> it_minus_4 = it_at_3 - 4;
    ASSERT_TRUE(it_minus_4.isAtBegin()); // Target logical pos -1
}

TEST_F(VknVectorIteratorTest, Arithmetic_Difference)
{
    PopulateVector({{0, 10}, {1, 20}, {2, 30}, {3, 40}, {4, 50}});
    vkn::VknVectorIterator<int> it1 = vec_int.getSlice(0, 5); // At logical pos 0
    vkn::VknVectorIterator<int> it2 = it1 + 3;                // At logical pos 3

    ASSERT_EQ(it2 - it1, 3); // Difference in logical positions
    ASSERT_THROW(it1 - it2, std::runtime_error);
}

// --- getData() Test ---

TEST_F(VknVectorIteratorTest, GetData_LogicallyContiguousSlice_ReturnsPointer)
{
    // Elements inserted contiguously in logical positions 0, 1, 2, 3, 4
    PopulateVector({{0, 10}, {1, 20}, {2, 30}, {3, 40}, {4, 50}}); // m_positions = {0, 1, 2, 3, 4}
    // Slice covering physical indices 1, 2, 3 (logical positions 1, 2, 3)
    vkn::VknVectorIterator<int> it = vec_int.getSlice(1, 3);

    ASSERT_FALSE(it.isEmpty());
    ASSERT_EQ(it.getSize(), 3);

    int *data_ptr = it.getData();
    ASSERT_NE(data_ptr, nullptr);
    ASSERT_EQ(data_ptr[0], 20); // Value at logical pos 1
    ASSERT_EQ(data_ptr[1], 30); // Value at logical pos 2
    ASSERT_EQ(data_ptr[2], 40); // Value at logical pos 3
}

TEST_F(VknVectorIteratorTest, GetData_LogicallySparseSlice_Throws)
{
    // Elements inserted in order, positions 5, 10, 15 -> physical indices 0, 1, 2
    PopulateVector({{5, 50}, {10, 100}, {15, 150}}); // m_positions = {5, 10, 15}
    // Slice covering physical indices 0, 1 (logical positions 5, 10)
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 11);

    ASSERT_FALSE(it.isEmpty());
    ASSERT_EQ(it.getSize(), 2); // Slice has 2 elements

    // Logical positions in slice are 5 and 10. These are NOT contiguous (10 != 5 + 1).
    // getData() should throw based on its check.
    // getData should throw if the actual
    ASSERT_THROW(it.getData(), std::runtime_error);
}

TEST_F(VknVectorIteratorTest, GetData_EmptySlice_ReturnsNullptr)
{
    PopulateVector({{0, 10}});
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 0); // Empty slice
    ASSERT_TRUE(it.isEmpty());
    ASSERT_EQ(it.getData(), nullptr);
}

// --- Helper Method Tests ---

TEST_F(VknVectorIteratorTest, GetSize_VariousSlices)
{
    PopulateVector({{0, 10}, {2, 20}, {5, 50}, {6, 60}}); // phys: {0,1,2,3}, logical: {0,2,5,6}

    // Slice covering physical indices 0, 1, 2, 3 (all elements)
    vkn::VknVectorIterator<int> it_all = vec_int.getSlice(0, 7);
    ASSERT_EQ(it_all.getSize(), 4);

    // Slice covering physical indices 1, 2 (logical positions 2, 5)
    vkn::VknVectorIterator<int> it_partial = vec_int.getSlice(1, 5);
    ASSERT_EQ(it_partial.getSize(), 2);

    // Empty slice
    vkn::VknVectorIterator<int> it_empty = vec_int.getSlice(0, 0);
    ASSERT_EQ(it_empty.getSize(), 0);
}

// Add tests for isEmpty(), isAtBegin(), isAtEnd() if you add those public methods.

// --- Additional Edge Cases ---

TEST_F(VknVectorIteratorTest, Iterate_SingleElementSlice)
{
    PopulateVector({{10, 100}});                              // phys 0, pos 10
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 11); // Slice phys 0, length 1

    ASSERT_FALSE(it.isEmpty());
    ASSERT_EQ(it.getSize(), 1);
    ASSERT_EQ(*it, 100);
    ASSERT_EQ(it.getLogicalPosition(), 10);

    ++it;
    ASSERT_TRUE(it.isAtEnd());
}

// Test iteration over a slice that includes the highest logical position
TEST_F(VknVectorIteratorTest, Iterate_SliceIncludesMaxLogicalPosition)
{
    PopulateVector({{0, 0}, {10, 100}, {5, 50}}); // phys: {0,1,2}, logical: {0,10,5}
    // Max logical position is 10.
    // Let's get a slice that includes the element at logical position 10.
    // Element at logical pos 10 is at physical index 1.
    vkn::VknVectorIterator<int> it = vec_int.getSlice(5, 6); // Slice covers physical indices 1, 2 (logical pos 10, 5)

    // Iteration order should be 5, 10 (logical order)
    ASSERT_EQ(*it, 50); // Logical pos 5
    ++it;
    ASSERT_EQ(*it, 100); // Logical pos 10
    ++it;
    ASSERT_TRUE(it.isAtEnd());
}

// Test iteration over a slice that includes the lowest logical position
TEST_F(VknVectorIteratorTest, Iterate_SliceIncludesMinLogicalPosition)
{
    PopulateVector({{0, 0}, {10, 100}, {5, 50}}); // phys: {0,1,2}, logical: {0,10,5}
    // Min logical position is 0.
    // Element at logical pos 0 is at physical index 0.
    vkn::VknVectorIterator<int> it = vec_int.getSlice(0, 11); // Slice covers physical indices 0, 1 (logical pos 0, 10)

    // Iteration order should be 0, 10 (logical order)
    ASSERT_EQ(*it, 0); // Logical pos 0
    ++it;
    ASSERT_EQ(*it, 50); // Logical pos 5
    ++it;
    ASSERT_EQ(*it, 100); // Logical pos 10
    ++it;
    ASSERT_TRUE(it.isAtEnd());
}

// --- Iterator Helper Methods to Consider Adding (Public) ---
// These would make testing and using the iterator easier:
// bool isAtBegin() const;
// bool isAtEnd() const;
// SizeType getLogicalPosition() const; // Get the logical position of the current element
// size_t getPhysicalIndex() const; // Get the physical index of the current element (less common for logical iterators)
