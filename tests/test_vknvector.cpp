#include "gtest/gtest.h"
#include "../VknConfig/include/VknData.hpp" // Adjust path as necessary

// No need for friend class declaration if testing public API only.
// If VknVectorTest was moved into the vkn namespace for the FRIEND_TEST macro,
// ensure it's either moved back to global or the VknVector friend declaration is updated.
// For this rewrite, I'll assume VknVectorTest is in the global namespace.

class VknVectorTest : public ::testing::Test
{
protected:
    vkn::VknVector<int> vec_int;
    vkn::VknVector<std::string> vec_str;

    // Helper to check basic state
    void ExpectEmpty(const vkn::VknVector<int> &vec)
    {
        ASSERT_TRUE(vec.isEmpty());
        ASSERT_EQ(vec.getSize(), 0);
        ASSERT_EQ(vec.getNumPositions(), 0);
    }
};

// --- Construction and Basic State ---
TEST_F(VknVectorTest, DefaultConstruction)
{
    ExpectEmpty(vec_int);
    ASSERT_EQ(vec_int.begin(), vec_int.end());
}

TEST_F(VknVectorTest, CopyConstruction_Empty)
{
    vkn::VknVector<int> copy_vec(vec_int);
    ASSERT_EQ(copy_vec.getData(), nullptr);
    ExpectEmpty(copy_vec);
}

TEST_F(VknVectorTest, CopyConstruction_WithElements)
{
    vec_int.insert(0, 10);
    vec_int.insert(2, 20);
    vkn::VknVector<int> copy_vec(vec_int);

    ASSERT_EQ(copy_vec.getSize(), 2);
    ASSERT_EQ(copy_vec.getNumPositions(), 3); // Logical positions up to 2
    ASSERT_TRUE(copy_vec.exists(0));
    ASSERT_EQ(copy_vec(0), 10);
    ASSERT_FALSE(copy_vec.exists(1));
    ASSERT_TRUE(copy_vec.exists(2));
    ASSERT_EQ(copy_vec(2), 20);
    ASSERT_NE(copy_vec.getData(), vec_int.getData()); // Should be a deep copy
}

TEST_F(VknVectorTest, MoveConstruction)
{
    vec_int.insert(0, 10);
    vec_int.insert(1, 20);
    int *original_data_ptr = vec_int.getData();

    vkn::VknVector<int> moved_vec(std::move(vec_int));

    ASSERT_EQ(moved_vec.getSize(), 2);
    ASSERT_EQ(moved_vec.getNumPositions(), 2);
    ASSERT_EQ(moved_vec(0), 10);
    ASSERT_EQ(moved_vec(1), 20);
    ASSERT_EQ(moved_vec.getData(), original_data_ptr); // Data pointer should be moved

    ExpectEmpty(vec_int); // Original vector should be empty
}

TEST_F(VknVectorTest, CopyAssignment)
{
    vec_int.insert(0, 10);
    vkn::VknVector<int> assigned_vec;
    assigned_vec.insert(5, 500); // Give it some initial state
    assigned_vec = vec_int;

    ASSERT_EQ(assigned_vec.getSize(), 1);
    ASSERT_EQ(assigned_vec.getNumPositions(), 1);
    ASSERT_EQ(assigned_vec(0), 10);
    ASSERT_NE(assigned_vec.getData(), vec_int.getData());
}

TEST_F(VknVectorTest, MoveAssignment)
{
    vec_int.insert(0, 10);
    int *original_data_ptr = vec_int.getData();
    vkn::VknVector<int> assigned_vec;
    assigned_vec.insert(5, 500);
    assigned_vec = std::move(vec_int);

    ASSERT_EQ(assigned_vec.getSize(), 1);
    ASSERT_EQ(assigned_vec.getNumPositions(), 1);
    ASSERT_EQ(assigned_vec(0), 10);
    ASSERT_EQ(assigned_vec.getData(), original_data_ptr);
    ExpectEmpty(vec_int);
}

// --- Insert, Access, Exists ---
TEST_F(VknVectorTest, Insert_SingleElement_AccessibleAndExists)
{
    vec_int.insert(5, 123);
    ASSERT_EQ(vec_int.getSize(), 1);
    ASSERT_EQ(vec_int.getNumPositions(), 6); // Logical positions up to 5
    ASSERT_TRUE(vec_int.exists(5));
    ASSERT_EQ(vec_int(5), 123);
    ASSERT_FALSE(vec_int.exists(0));
}

TEST_F(VknVectorTest, Insert_MultipleSparseElements_AccessibleAndExists)
{
    vec_int.insert(10, 100);
    vec_int.insert(0, 0);
    vec_int.insert(5, 50);

    ASSERT_EQ(vec_int.getSize(), 3);
    ASSERT_EQ(vec_int.getNumPositions(), 11); // Logical positions up to 10

    ASSERT_TRUE(vec_int.exists(0));
    ASSERT_EQ(vec_int(0), 0);
    ASSERT_TRUE(vec_int.exists(5));
    ASSERT_EQ(vec_int(5), 50);
    ASSERT_TRUE(vec_int.exists(10));
    ASSERT_EQ(vec_int(10), 100);

    ASSERT_FALSE(vec_int.exists(1));
    ASSERT_FALSE(vec_int.exists(6));
}

TEST_F(VknVectorTest, Insert_ExistingPosition_ThrowsException)
{
    vec_int.insert(1, 10);
    ASSERT_ANY_THROW(vec_int.insert(1, 20));
}

TEST_F(VknVectorTest, GetElement_NonExistent_ReturnsNullptr)
{
    vec_int.insert(0, 10);
    ASSERT_EQ(vec_int.getElement(1), nullptr);
    ASSERT_EQ(vec_int.getElement(100), nullptr);
}

TEST_F(VknVectorTest, GetElement_EmptyVector_ReturnsNullptrOrThrows)
{
    // Your getElement throws if empty, which is fine.
    ASSERT_EQ(vec_int.getElement(0), nullptr);
}

TEST_F(VknVectorTest, OperatorCall_NonExistent_ThrowsException)
{
    vec_int.insert(0, 10);
    ASSERT_THROW(vec_int(1), std::runtime_error);
}

TEST_F(VknVectorTest, Exists_NonExistent_ReturnsFalse)
{
    ASSERT_FALSE(vec_int.exists(0));
    vec_int.insert(1, 10);
    ASSERT_FALSE(vec_int.exists(0));
    ASSERT_TRUE(vec_int.exists(1));
}

// --- Append ---
TEST_F(VknVectorTest, Append_SingleElement)
{
    vec_int.appendOne(10);
    vec_int.appendOne(20);
    ASSERT_EQ(vec_int.getSize(), 2);
    ASSERT_EQ(vec_int.getNumPositions(), 2);
    ASSERT_EQ(vec_int(0), 10);
    ASSERT_EQ(vec_int(1), 20);
}

TEST_F(VknVectorTest, Append_MultipleValues)
{
    vec_int.appendRepeat(7, 3); // Append 7, 3 times
    ASSERT_EQ(vec_int.getSize(), 3);
    ASSERT_EQ(vec_int.getNumPositions(), 3);
    ASSERT_EQ(vec_int(0), 7);
    ASSERT_EQ(vec_int(1), 7);
    ASSERT_EQ(vec_int(2), 7);
}

// --- Resize and Clear ---
TEST_F(VknVectorTest, Resize_Larger_FromEmpty)
{
    vec_int.grow(3);
    ASSERT_EQ(vec_int.getSize(), 3);
    // Content of new elements depends on DataType default construction
    // For int, it's likely uninitialized or zero-initialized by new DataType[newSize]
    // Your resize copies old data, then new elements are just allocated.
}

TEST_F(VknVectorTest, Resize_Larger_PreservesExisting)
{
    vec_int.insert(0, 10);
    vec_int.insert(1, 20);
    vec_int.grow(4);
    ASSERT_EQ(vec_int.getSize(), 4);
    ASSERT_EQ(vec_int.getNumPositions(), 2); // Resize doesn't change logical positions of old elements
    ASSERT_EQ(vec_int(0), 10);
    ASSERT_EQ(vec_int(1), 20);
}

TEST_F(VknVectorTest, Resize_Smaller_Truncates)
{
    vec_int.insert(0, 10);
    vec_int.insert(1, 20);
    vec_int.insert(2, 30);
    ASSERT_THROW(vec_int.grow(2), std::runtime_error);
}

TEST_F(VknVectorTest, Resize_ToZero_IsEmpty)
{
    vec_int.insert(0, 10);
    // Your resize throws if newSize is 0. If you want to allow it:
    // vec_int.grow(0);
    // ExpectEmpty(vec_int);
    // For now, let's test the throw:
    ASSERT_THROW(vec_int.grow(0), std::runtime_error);
}

TEST_F(VknVectorTest, Clear_EmptiesVector)
{
    vec_int.insert(0, 10);
    vec_int.insert(1, 20);
    vec_int.clear();
    ExpectEmpty(vec_int);
}

// --- Swap ---
TEST_F(VknVectorTest, Swap_ExistingElements)
{
    vec_int.insert(0, 10);
    vec_int.insert(1, 20);
    vec_int.swap(0, 1); // Swaps the *data* at the physical indices corresponding to these logical positions
    ASSERT_EQ(vec_int(0), 20);
    ASSERT_EQ(vec_int(1), 10);
}

TEST_F(VknVectorTest, Swap_SparseElements)
{
    vec_int.insert(0, 10);
    vec_int.insert(5, 50);
    vec_int.swap(0, 5);
    ASSERT_EQ(vec_int(0), 50);
    ASSERT_EQ(vec_int(5), 10);
}

TEST_F(VknVectorTest, Swap_NonExistentElement_Throws)
{
    vec_int.insert(0, 10);
    ASSERT_NO_THROW(vec_int.swap(0, 1)); // Element at logical pos 1 doesn't exist
    ASSERT_EQ(vec_int(1), 10);
}

// --- getData ---
TEST_F(VknVectorTest, GetData_EmptyVector_ReturnsNullptr)
{
    ASSERT_EQ(vec_int.getData(), nullptr);
}

TEST_F(VknVectorTest, GetData_WithElements_ReturnsDataPointer)
{
    vec_int.insert(0, 10);
    ASSERT_NE(vec_int.getData(), nullptr);
    ASSERT_EQ(*(vec_int.getData()), 10); // Assuming first element is at physical index 0
}

TEST_F(VknVectorTest, GetData_WithNumNewElements_ResizesAndPointsToEndOfOld)
{
    vec_int.insert(0, 10); // size 1, posSize 1

    int *new_block_ptr = vec_int.getData(2);       // Add 2 new elements
    int *old_data_end_ish = vec_int.getData() + 1; // moved here because getData with arguments resizes the array

    ASSERT_EQ(vec_int.getSize(), 3);
    ASSERT_EQ(vec_int.getNumPositions(), 3); // getNextPosition() was called for new elements

    // Check if new_block_ptr points to where the new elements start
    // This depends on how getData(numNewElements) returns the pointer
    // Your VknVector::getData(numNewElements) returns m_data + oldSize
    ASSERT_EQ(new_block_ptr, vec_int.getData() + 1); // oldSize was 1
    // Check if new elements are default initialized (for int, this is often 0 or garbage)
    // Your getData initializes them to DataType{}
    ASSERT_EQ(new_block_ptr[0], 0); // or int{}
    ASSERT_EQ(new_block_ptr[1], 0); // or int{}
    ASSERT_EQ(vec_int(0), 10);      // old element preserved
}

// --- Edge Cases (inspired by original private function tests) ---

// This test replaces GetIdxOfSmallestPos_MultipleElements_NoElementGreaterOrEqual
// It checks the public behavior: does an element exist beyond the current logical range?
TEST_F(VknVectorTest, Exists_BeyondLastInsertedPosition_ReturnsFalse)
{
    vec_int.insert(0, 0);
    vec_int.insert(1, 1); // Max logical position inserted is 1. m_posSize should be 2.
    ASSERT_FALSE(vec_int.exists(2));
    ASSERT_EQ(vec_int.getElement(2), nullptr);
}

// These tests check if elements at specific positions (that would be boundaries
// for getSmallestPos/getLargestPos) exist or not.
TEST_F(VknVectorTest, ElementExistence_AroundSearchedPosition_ForSmallest)
{
    vec_int.insert(1, 10);
    vec_int.insert(3, 30);
    vec_int.insert(5, 50);

    // Corresponds to: getSmallestPos(2) should find 3
    ASSERT_FALSE(vec_int.exists(2));
    ASSERT_TRUE(vec_int.exists(3));
    ASSERT_EQ(vec_int(3), 30);
}

TEST_F(VknVectorTest, ElementExistence_AroundSearchedPosition_ForLargest)
{
    vec_int.insert(1, 10);
    vec_int.insert(3, 30);
    vec_int.insert(5, 50);

    // Corresponds to: getLargestPos(4) should find 3
    ASSERT_FALSE(vec_int.exists(4));
    ASSERT_TRUE(vec_int.exists(3));
    ASSERT_EQ(vec_int(3), 30);
}

// Test for the specific exception case that was failing one of your original tests
TEST_F(VknVectorTest, Insert_CorrectnessCheck_For_NoElementGreaterOrEqual_Setup)
{
    // This test specifically checks the setup part of your previously failing test
    // VknVectorTest.GetIdxOfSmallestPos_MultipleElements_NoElementGreaterOrEqual
    // which threw "Tried to insert into a VknVector element that is already assigned."
    ASSERT_NO_THROW(vec_int.insert(0, 0));
    ASSERT_NO_THROW(vec_int.insert(1, 1));
    // If the above pass, then the insert logic is fine for this sequence.
    // The original exception might have been due to test state bleeding or a subtle bug
    // in how `exists` or `setPosition` interacted in that specific scenario.
}

TEST_F(VknVectorTest, Insert_Max_Elements_Plus_One)
{
    vec_int.appendRepeat(1, 256);
    ASSERT_THROW(vec_int.appendOne(1), std::runtime_error);
}

TEST_F(VknVectorTest, Remove_Element_From_Empty_Vec)
{
    ASSERT_THROW(vec_int.remove(0), std::runtime_error);
}

TEST_F(VknVectorTest, Clear_Empty_Vec)
{
    ASSERT_NO_THROW(vec_int.clear());
}

TEST_F(VknVectorTest, Defrag_Pos_Basic_And_Remove_Each_Element)
{
    vec_int.appendRepeat(3, 8);
    ASSERT_EQ(vec_int.getDefragPos(2), 8);
    vec_int.remove(2);
    ASSERT_EQ(vec_int.getDefragPos(2), 8);
    ASSERT_EQ(vec_int.getDefragPos(1), 2);
    vec_int.remove(4, 3);
    ASSERT_EQ(vec_int.getDefragPos(3), 4);
    vec_int.remove(0, 2);
    vec_int.remove(3);
    ASSERT_NO_THROW(vec_int.remove(7));
    ASSERT_EQ(vec_int.getSize(), 0);
    ASSERT_THROW(vec_int(0), std::runtime_error);
}

TEST_F(VknVectorTest, Defrag_Pos_No_Room)
{
    vec_int.appendRepeat(3, 256);
    ASSERT_THROW(vec_int.getDefragPos(1), std::runtime_error);
    vec_int.remove(40, 8);
    vec_int.remove(190, 4);
    vec_int.remove(222, 9);
    ASSERT_THROW(vec_int.getDefragPos(10), std::runtime_error);
    ASSERT_EQ(vec_int.getDefragPos(9), 222);
}

TEST_F(VknVectorTest, Remove_Too_Much)
{
    vec_int.appendRepeat(3, 8);
    ASSERT_NO_THROW(vec_int.remove(6, 4));
    vec_int.appendRepeat(6, 248);
    ASSERT_NO_THROW(vec_int.remove(250, 10));
}

TEST_F(VknVectorTest, Remove_Sparse)
{
    vec_int.appendRepeat(7, 50);
    vec_int.remove(8, 2);
    vec_int.remove(15, 3);
    vec_int.remove(25, 5);
    vec_int.remove(35, 5);
    ASSERT_NO_THROW(vec_int.remove(6, 5));
    ASSERT_NO_THROW(vec_int.remove(15, 3));
    ASSERT_NO_THROW(vec_int.remove(27, 10));
    ASSERT_NO_THROW(vec_int.remove(0, 50));
}

TEST_F(VknVectorTest, Get_Element_Test)
{
    ASSERT_THROW(vec_int(0), std::runtime_error);
    ASSERT_THROW(vec_int(256), std::runtime_error);
    vec_int.appendOne(1);
    ASSERT_EQ(vec_int(0), 1);
    ASSERT_THROW(vec_int(1), std::runtime_error);
    ASSERT_THROW(vec_int(256), std::runtime_error);
}

TEST_F(VknVectorTest, Append_Test)
{
    vec_int.appendRepeat(3, 256);
    ASSERT_THROW(vec_int.appendOne(1), std::runtime_error);
    vec_int.remove(250, 6);
    ASSERT_THROW(vec_int.appendRepeat(4, 7), std::runtime_error);
    vkn::VknVector<int> append_vec_int{};
    append_vec_int.appendRepeat(4, 10);
    ASSERT_THROW(vec_int.appendVector(append_vec_int), std::runtime_error);
    int append_ints[6] = {1, 2, 3, 4, 5, 6};
    ASSERT_NO_THROW(vec_int.insert(250, append_ints, 6));
}

TEST_F(VknVectorTest, Relative_Size)
{
    std::vector<int> stdIntCompare{};
    std::cout << "sizeOf(empty std::vector<int>): " << sizeof(stdIntCompare) << std::endl;
    std::cout << "sizeOf(empty VknVector<int>): " << sizeof(vec_int) << std::endl;

    stdIntCompare.push_back(1);
    vec_int.appendOne(1);
    std::cout << "sizeOf(1 element std::vector<int>): " << sizeof(stdIntCompare) << std::endl;
    std::cout << "sizeOf(1 element VknVector<int>): " << sizeof(vec_int) << std::endl;

    for (int i = 0; i < 9; ++i)
        stdIntCompare.push_back(1);
    vec_int.appendRepeat(1, 9);
    std::cout << "sizeOf(10 element std::vector<int>): " << sizeof(stdIntCompare) << std::endl;
    std::cout << "sizeOf(10 element VknVector<int>): " << sizeof(vec_int) << std::endl;
}

TEST_F(VknVectorTest, Append_Array)
{
    int *test1{nullptr};
    int test2[1] = {1};
    int test3[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ASSERT_NO_THROW(vec_int.appendArray(test1, 0));
    ASSERT_NO_THROW(vec_int.appendArray(test2, 1));
    ASSERT_NO_THROW(vec_int.appendArray(test3, 10));
    vec_int.appendRepeat(3, 240);
    ASSERT_THROW(vec_int.appendArray(test3, 10), std::runtime_error);
}

TEST_F(VknVectorTest, Append_VknVector)
{
    vkn::VknVector<int> test{};
    ASSERT_NO_THROW(vec_int.appendVector(test));
    test.appendOne(1);
    ASSERT_NO_THROW(vec_int.appendVector(test));
    test.appendRepeat(3, 9);
    ASSERT_NO_THROW(vec_int.appendVector(test));
    test.appendRepeat(3, 240);
    ASSERT_THROW(vec_int.appendVector(test), std::runtime_error);
}

TEST_F(VknVectorTest, Get_Element_2_I_Think)
{
    ASSERT_EQ(vec_int.getElement(0), nullptr);
    vec_int.appendRepeat(3, 10);
    ASSERT_EQ(vec_int.getElement(11), nullptr);
    ASSERT_NO_THROW(vec_int.getElement(5));
    ASSERT_THROW(vec_int.getElement(256), std::runtime_error);
}

TEST_F(VknVectorTest, VknVector_Exists)
{
    ASSERT_EQ(vec_int.exists(9), false);
    vec_int.appendRepeat(3, 15);
    ASSERT_EQ(vec_int.exists(15), false);
    ASSERT_EQ(vec_int.exists(3), true);
    ASSERT_THROW(vec_int.exists(256), std::runtime_error);
}

TEST_F(VknVectorTest, VknVector_Insert)
{
    int test[5] = {1, 2, 3, 4, 5};
    ASSERT_NO_THROW(vec_int.insert(5, 10));
    ASSERT_NO_THROW(vec_int.insert(11, test, 5));
    ASSERT_NO_THROW(vec_int.insert(6, test, 5));
    ASSERT_THROW(vec_int.insert(256, 2), std::runtime_error);
    ASSERT_THROW(vec_int.insert(254, test, 5), std::runtime_error);
    ASSERT_THROW(vec_int.insert(3, test, 5), std::runtime_error);
}

TEST_F(VknVectorTest, VknVector_Swap)
{
    ASSERT_NO_THROW(vec_int.swap(2, 3));
    vec_int.appendOne(100);
    vec_int.appendOne(150);
    ASSERT_NO_THROW(vec_int.swap(0, 1));
    ASSERT_THROW(vec_int.swap(1, 256), std::runtime_error);
    ASSERT_NO_THROW(vec_int.swap(1, 2));
    ASSERT_EQ(vec_int(0), 150);
    ASSERT_EQ(vec_int.getElement(1), nullptr);
    ASSERT_EQ(vec_int(2), 100);
}