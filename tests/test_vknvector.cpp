#include "gtest/gtest.h"
#include "VknCommon.hpp" // Adjust path as necessary

// Test fixture for VknVector tests
class VknVectorTest : public ::testing::Test
{
protected:
    vkn::VknVector<int, uint32_t> vec_int;
    vkn::VknVector<std::string, uint16_t> vec_str;

    // You can put common setup logic here if needed
    void SetUp() override
    {
        // Example: vec_int.append(10, 0); vec_int.append(20, 1);
    }

    // You can put common teardown logic here
    void TearDown() override
    {
    }
};

// --- Tests for getIdxOfSmallestPos ---
TEST_F(VknVectorTest, GetIdxOfSmallestPos_EmptyVector)
{
    ASSERT_THROW(vec_int.getIdxOfSmallestPos(0), std::runtime_error);
}

TEST_F(VknVectorTest, GetIdxOfSmallestPos_SingleElement_FoundAtMinPos)
{
    vec_int.insert(0, 100); // Insert 100 at logical position 0
    vkn::PosSearchResult result = vec_int.getIdxOfSmallestPos(0);
    ASSERT_TRUE(result.found);
    ASSERT_EQ(result.pos, 0); // Should be at physical index 0
    ASSERT_EQ(vec_int(0), 100);
}

TEST_F(VknVectorTest, GetIdxOfSmallestPos_SingleElement_MinPosTooLarge)
{
    vec_int.insert(0, 100);
    ASSERT_THROW(vec_int.getIdxOfSmallestPos(1), std::runtime_error);
}

TEST_F(VknVectorTest, GetIdxOfSmallestPos_MultipleElements_ExactMatch)
{
    vec_int.insert(1, 10); // pos 1, val 10
    vec_int.insert(3, 30); // pos 3, val 30
    vec_int.insert(5, 50); // pos 5, val 50

    vkn::PosSearchResult result = vec_int.getIdxOfSmallestPos(3);
    ASSERT_TRUE(result.found);
    // The physical index depends on insertion order if not sorted by position
    // For this specific insertion order, element at logical pos 3 is at physical index 1
    ASSERT_EQ(vec_int.m_data[result.pos], 30);
    ASSERT_EQ(vec_int.m_positions[result.pos], 3);
}

TEST_F(VknVectorTest, GetIdxOfSmallestPos_MultipleElements_NextSmallest)
{
    vec_int.insert(1, 10); // physical idx 0
    vec_int.insert(3, 30); // physical idx 1
    vec_int.insert(5, 50); // physical idx 2

    // Search for smallest position >= 2
    // Expected: logical position 3 (value 30) at physical index 1
    vkn::PosSearchResult result = vec_int.getIdxOfSmallestPos(2);
    ASSERT_TRUE(result.found);
    ASSERT_EQ(vec_int.m_data[result.pos], 30);
    ASSERT_EQ(vec_int.m_positions[result.pos], 3);
}

TEST_F(VknVectorTest, GetIdxOfSmallestPos_MultipleElements_NoElementGreaterOrEqual)
{
    vec_int.insert(1, 10);
    vec_int.insert(3, 30);
    // Search for smallest position >= 4. Should find logical pos 3 is not >= 4.
    // The current implementation of getIdxOfSmallestPos might return the largest if nothing is >= minPos.
    // This needs clarification or adjustment in VknVector if this behavior is not desired.
    // For now, let's assume it should not find anything strictly >= 4 if only 1 and 3 exist.
    // If it finds the "next smallest available after minPos", then it would find nothing.
    // If it finds "smallest available that is >= minPos", it would find nothing.
    // If it finds "smallest available if nothing is >= minPos", then it's different.

    // Based on current VknVector::getIdxOfSmallestPos:
    // if (m_positions[i] == minPos) -> found
    // else if (m_positions[i] > minPos && m_positions[i] < smallestPos) -> found
    // If no exact match and nothing > minPos, result.found might remain false or point to an irrelevant smallestPosIdx.
    // Let's test for a case where nothing should be found.
    vec_int.insert(0, 0);
    vec_int.insert(1, 1);
    vkn::PosSearchResult result = vec_int.getIdxOfSmallestPos(2); // minPos = 2
    // If only 0 and 1 exist, and we search for >=2, nothing should be found.
    // The loop for smallestPos will not update smallestPosIdx if nothing is > minPos.
    // smallestPosIdx remains 0. smallestPos remains s_maxSizeTypeNum.
    // result.found will be false if no exact match and nothing > minPos.
    ASSERT_FALSE(result.found); // This depends on precise behavior for "not found"
}

// --- Tests for getIdxOfLargestPos ---
TEST_F(VknVectorTest, GetIdxOfLargestPos_EmptyVector)
{
    ASSERT_THROW(vec_int.getIdxOfLargestPos(0), std::runtime_error);
}

TEST_F(VknVectorTest, GetIdxOfLargestPos_MultipleElements_ExactMatch)
{
    vec_int.insert(1, 10);
    vec_int.insert(3, 30);
    vec_int.insert(5, 50);

    vkn::PosSearchResult result = vec_int.getIdxOfLargestPos(3);
    ASSERT_TRUE(result.found);
    ASSERT_EQ(vec_int.m_data[result.pos], 30);
    ASSERT_EQ(vec_int.m_positions[result.pos], 3);
}

TEST_F(VknVectorTest, GetIdxOfLargestPos_MultipleElements_PrevLargest)
{
    vec_int.insert(1, 10); // phys 0
    vec_int.insert(3, 30); // phys 1
    vec_int.insert(5, 50); // phys 2

    // Search for largest position <= 4
    // Expected: logical position 3 (value 30) at physical index 1
    vkn::PosSearchResult result = vec_int.getIdxOfLargestPos(4);
    ASSERT_TRUE(result.found);
    ASSERT_EQ(vec_int.m_data[result.pos], 30);
    ASSERT_EQ(vec_int.m_positions[result.pos], 3);
}

// --- Tests for getSmallestPos ---
// (Similar structure to getIdxOfSmallestPos, but checks the returned logical position)
TEST_F(VknVectorTest, GetSmallestPos_EmptyVector)
{
    ASSERT_THROW(vec_int.getSmallestPos(0), std::runtime_error);
}

TEST_F(VknVectorTest, GetSmallestPos_MultipleElements_NextSmallest)
{
    vec_int.insert(1, 10);
    vec_int.insert(3, 30);
    vec_int.insert(5, 50);
    vkn::PosSearchResult result = vec_int.getSmallestPos(2); // minPos = 2
    ASSERT_TRUE(result.found);
    ASSERT_EQ(result.pos, 3); // Expect logical position 3
}

// --- Tests for getLargestPos ---
// (Similar structure to getIdxOfLargestPos, but checks the returned logical position)
TEST_F(VknVectorTest, GetLargestPos_EmptyVector)
{
    ASSERT_THROW(vec_int.getLargestPos(0), std::runtime_error);
}

TEST_F(VknVectorTest, GetLargestPos_MultipleElements_PrevLargest)
{
    vec_int.insert(1, 10);
    vec_int.insert(3, 30);
    vec_int.insert(5, 50);
    vkn::PosSearchResult result = vec_int.getLargestPos(4); // maxPos = 4
    ASSERT_TRUE(result.found);
    ASSERT_EQ(result.pos, 3); // Expect logical position 3
}

// Add more tests for edge cases:
// - minPos/maxPos being the actual smallest/largest in the vector.
// - minPos/maxPos outside the range of existing positions.
// - Sparse vectors where there are gaps.
// - Vectors with duplicate logical positions (if your design allows/handles this, though VknVector::insert prevents it).
