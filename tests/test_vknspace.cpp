// tests/test_vknspace.cpp
#include "gtest/gtest.h"
#include "../VknConfig/include/VknData.hpp" // Adjust path as necessary

// Helper getters for VknSpace - if these don't exist, add them to VknSpace
// or test these aspects indirectly.
/*
In VknSpace class:
public:
    uint32_t getDepth() const { return m_depth; }
    uint32_t getMaxDepth() const { return m_maxDepth; }
*/

class VknSpaceTest : public ::testing::Test
{
protected:
    vkn::VknSpace<int, uint32_t> space_int_default; // Default maxDepth = 255, depth = 0
    vkn::VknSpace<std::string, uint16_t> space_str_limited_depth;

    VknSpaceTest() : space_str_limited_depth(2, 0) {} // maxDepth = 2, depth = 0

    void SetUp() override
    {
        // Common setup if needed
    }

    void TearDown() override
    {
        // Common teardown if needed
    }
};

// --- Constructor Tests ---
TEST_F(VknSpaceTest, DefaultConstruction)
{
    ASSERT_EQ(space_int_default.getDepth(), 0);
    ASSERT_EQ(space_int_default.getMaxDepth(), 255);
    ASSERT_EQ(space_int_default.getDataSize(), 0);
    ASSERT_EQ(space_int_default.getNumSubspaces(), 0);
}

TEST_F(VknSpaceTest, ConstructionWithMaxDepth)
{
    vkn::VknSpace<int, uint32_t> space(5, 1); // maxDepth = 5, initial depth = 1
    ASSERT_EQ(space.getDepth(), 1);
    ASSERT_EQ(space.getMaxDepth(), 5);
}

// --- Copy Operations ---
TEST_F(VknSpaceTest, CopyConstruction_Empty)
{
    vkn::VknSpace<int, uint32_t> original(3, 1);
    vkn::VknSpace<int, uint32_t> copy(original);

    ASSERT_EQ(copy.getDepth(), 1);
    ASSERT_EQ(copy.getMaxDepth(), 3);
    ASSERT_EQ(copy.getDataSize(), 0);
    ASSERT_EQ(copy.getNumSubspaces(), 0);
}

TEST_F(VknSpaceTest, CopyConstruction_WithDataAndSubspaces)
{
    vkn::VknSpace<int, uint32_t> original(3, 0);
    original.append(100);
    original[0].append(200); // Creates subspace at [0]

    vkn::VknSpace<int, uint32_t> copy(original);
    ASSERT_EQ(copy.getDepth(), 0);
    ASSERT_EQ(copy.getMaxDepth(), 3);
    ASSERT_EQ(copy.getDataSize(), 1);
    ASSERT_EQ(copy(0), 100);
    ASSERT_EQ(copy.getNumSubspaces(), 1);
    ASSERT_TRUE(copy.getSubspaceVector().exists(0));
    ASSERT_EQ(copy[0].getDataSize(), 1);
    ASSERT_EQ(copy[0](0), 200);
    ASSERT_EQ(copy[0].getDepth(), 1);
    ASSERT_EQ(copy[0].getMaxDepth(), 3);
}

TEST_F(VknSpaceTest, CopyAssignment)
{
    vkn::VknSpace<int, uint32_t> original(3, 0);
    original.append(100);
    original[0].append(200);

    vkn::VknSpace<int, uint32_t> assigned_space(1, 1); // Different initial state
    assigned_space.append(999);

    assigned_space = original;

    ASSERT_EQ(assigned_space.getDepth(), 0);
    ASSERT_EQ(assigned_space.getMaxDepth(), 3);
    ASSERT_EQ(assigned_space.getDataSize(), 1);
    ASSERT_EQ(assigned_space(0), 100);
    ASSERT_EQ(assigned_space.getNumSubspaces(), 1);
    ASSERT_TRUE(assigned_space.getSubspaceVector().exists(0));
    ASSERT_EQ(assigned_space[0](0), 200);
}

// --- Move Operations ---
TEST_F(VknSpaceTest, MoveConstruction)
{
    vkn::VknSpace<int, uint32_t> original(3, 0);
    original.append(100);
    original[0].append(200);

    vkn::VknSpace<int, uint32_t> moved_space(std::move(original));

    ASSERT_EQ(moved_space.getDepth(), 0);
    ASSERT_EQ(moved_space.getMaxDepth(), 3);
    ASSERT_EQ(moved_space.getDataSize(), 1);
    ASSERT_EQ(moved_space(0), 100);
    ASSERT_EQ(moved_space.getNumSubspaces(), 1);
    ASSERT_TRUE(moved_space.getSubspaceVector().exists(0));
    ASSERT_EQ(moved_space[0](0), 200);

    // Original should be in a valid but unspecified (likely empty) state
    ASSERT_EQ(original.getDepth(), 0);      // As per your move constructor's reset
    ASSERT_EQ(original.getMaxDepth(), 255); // As per your move constructor's reset
    ASSERT_EQ(original.getDataSize(), 0);
    ASSERT_EQ(original.getNumSubspaces(), 0);
}

TEST_F(VknSpaceTest, MoveAssignment)
{
    vkn::VknSpace<int, uint32_t> original(3, 0);
    original.append(100);
    original[0].append(200);

    vkn::VknSpace<int, uint32_t> assigned_space(1, 1);
    assigned_space.append(999);

    assigned_space = std::move(original);

    ASSERT_EQ(assigned_space.getDepth(), 0);
    ASSERT_EQ(assigned_space.getMaxDepth(), 3);
    ASSERT_EQ(assigned_space.getDataSize(), 1);
    ASSERT_EQ(assigned_space(0), 100);
    ASSERT_EQ(assigned_space.getNumSubspaces(), 1);
    ASSERT_TRUE(assigned_space.getSubspaceVector().exists(0));
    ASSERT_EQ(assigned_space[0](0), 200);

    ASSERT_EQ(original.getDepth(), 0);
    ASSERT_EQ(original.getMaxDepth(), 255);
    ASSERT_EQ(original.getDataSize(), 0);
    ASSERT_EQ(original.getNumSubspaces(), 0);
}

// --- GetSubspace / Operator[] / Dive ---
TEST_F(VknSpaceTest, GetSubspace_CreatesNew_AndReturnsExisting)
{
    ASSERT_EQ(space_int_default.getNumSubspaces(), 0);

    vkn::VknSpace<int, uint32_t> &sub0 = space_int_default.getSubspace(0);
    ASSERT_EQ(space_int_default.getNumSubspaces(), 1);
    ASSERT_EQ(sub0.getDepth(), 1);      // Assumes getter exists
    ASSERT_EQ(sub0.getMaxDepth(), 255); // Assumes getter exists

    vkn::VknSpace<int, uint32_t> &sub0_again = space_int_default[0]; // Using operator[]
    ASSERT_EQ(space_int_default.getNumSubspaces(), 1);               // No new subspace created
    ASSERT_EQ(&sub0, &sub0_again);                                   // Should be the same object
}

TEST_F(VknSpaceTest, GetSubspace_MultipleLevels)
{
    vkn::VknSpace<int, uint32_t> &sub0 = space_int_default[0];
    vkn::VknSpace<int, uint32_t> &sub0_1 = sub0[1];
    vkn::VknSpace<int, uint32_t> &sub0_1_2 = sub0_1[2];

    ASSERT_EQ(sub0.getDepth(), 1);
    ASSERT_EQ(sub0_1.getDepth(), 2);
    ASSERT_EQ(sub0_1_2.getDepth(), 3);

    ASSERT_EQ(sub0_1_2.getMaxDepth(), 255); // MaxDepth propagates
    sub0_1_2.append(123);
    ASSERT_EQ(space_int_default[0][1][2](0), 123);
}

TEST_F(VknSpaceTest, GetSubspace_ReachesMaxDepth_ThrowsOnNextDive)
{
    // space_str_limited_depth has maxDepth = 2, current depth = 0
    vkn::VknSpace<std::string, uint16_t> &sub_d0 = space_str_limited_depth[0]; // depth 1, OK
    ASSERT_EQ(sub_d0.getDepth(), 1);
    ASSERT_EQ(sub_d0.getMaxDepth(), 2);

    vkn::VknSpace<std::string, uint16_t> &sub_d1 = sub_d0[0]; // depth 2, OK (at maxDepth)
    ASSERT_EQ(sub_d1.getDepth(), 2);
    ASSERT_EQ(sub_d1.getMaxDepth(), 2);

    // Trying to get a subspace from sub_d1 would make its child depth 3
    ASSERT_THROW(sub_d1[0], std::runtime_error); // "Trying to dive too deep..."
}

TEST_F(VknSpaceTest, GetSubspace_PositionOutOfRange_Throws)
{
    // s_maxDimensions is 8, so valid positions are 0-7
    ASSERT_NO_THROW(space_int_default[7]);
    ASSERT_THROW(space_int_default[8], std::runtime_error); // "Position given is out of range."
}

// --- Data Operations ---
TEST_F(VknSpaceTest, Data_AppendAndAccess)
{
    space_int_default.append(10);
    space_int_default.append(20);
    ASSERT_EQ(space_int_default.getDataSize(), 2);
    ASSERT_EQ(space_int_default(0), 10);
    ASSERT_EQ(space_int_default(1), 20);
}

TEST_F(VknSpaceTest, Data_InsertAndAccess)
{
    space_int_default.insert(100, 5); // Insert 100 at logical position 5
    ASSERT_EQ(space_int_default.getDataSize(), 1);
    ASSERT_TRUE(space_int_default.getDataVector().exists(5));
    ASSERT_EQ(space_int_default(5), 100);
}

TEST_F(VknSpaceTest, Data_AccessNonExistent_Throws)
{
    ASSERT_THROW(space_int_default(0), std::runtime_error); // Empty
    space_int_default.append(10);
    ASSERT_THROW(space_int_default(1), std::runtime_error); // Position 1 doesn't exist
}

TEST_F(VknSpaceTest, Data_GetDataWithNewSize)
{
    int *data_ptr = space_int_default.getData(3); // Should grow data vector to size 3
    ASSERT_NE(data_ptr, nullptr);
    ASSERT_EQ(space_int_default.getDataSize(), 3);
    data_ptr[0] = 1;
    data_ptr[1] = 2;
    data_ptr[2] = 3;
    ASSERT_EQ(space_int_default(0), 1); // Assumes getData also sets positions
}

TEST_F(VknSpaceTest, Data_GetDataVector_Modify)
{
    space_int_default.append(10);
    vkn::VknVector<int, uint32_t> &data_vec = space_int_default.getDataVector();
    data_vec(0) = 111; // Modify through the reference
    ASSERT_EQ(space_int_default(0), 111);
}

// --- Subspace Vector and Slice ---
TEST_F(VknSpaceTest, GetSubspaceVector_CheckSize)
{
    ASSERT_EQ(space_int_default.getSubspaceVector().getSize(), 0);
    space_int_default[0];
    space_int_default[1];
    ASSERT_EQ(space_int_default.getSubspaceVector().getSize(), 2);
    ASSERT_EQ(space_int_default.getNumSubspaces(), 2);
}

TEST_F(VknSpaceTest, GetSubspaceSlice_Iterate)
{
    space_int_default[0].append(10); // Subspace at logical pos 0, depth 1
    space_int_default[2].append(20); // Subspace at logical pos 2, depth 1
    space_int_default[5].append(30); // Subspace at logical pos 5, depth 1

    // Get a slice of the subspaces. VknVector::getSlice uses physical indices.
    // After inserting at 0, 2, 5, the physical indices in m_subspaces are 0, 1, 2.
    // The logical positions stored in m_subspaces.m_positions are 0, 2, 5.
    // Let's get a slice covering physical indices 0 to 1 (logical positions 0 and 2).
    vkn::VknVectorIterator<vkn::VknSpace<int, uint32_t>, uint32_t> slice_it = space_int_default.getSubspaceSlice(0, 3); // Physical start 0, length 2

    ASSERT_FALSE(slice_it.isEmpty());
    ASSERT_EQ(slice_it.getSize(), 2);

    // Iterator should yield subspaces in their logical order within the slice
    // Slice contains subspaces originally at logical positions 0 and 2.
    // Smallest logical position is 0.
    ASSERT_EQ((*slice_it).getDepth(), 1);
    ASSERT_EQ((*slice_it)(0), 10); // Data in subspace at logical pos 0

    ++slice_it;
    ASSERT_EQ((*slice_it).getDepth(), 1);
    ASSERT_EQ((*slice_it)(0), 20); // Data in subspace at logical pos 2

    ++slice_it;
    ASSERT_TRUE(slice_it.isAtEnd());
}

TEST_F(VknSpaceTest, GetSubspaceSlice_Empty)
{
    vkn::VknVectorIterator<vkn::VknSpace<int, uint32_t>, uint32_t> slice_it =
        space_int_default.getSubspaceSlice(0, 0);
    ASSERT_TRUE(slice_it.isEmpty());
    ASSERT_EQ(slice_it.getSize(), 0);
}

TEST_F(VknSpaceTest, GetSubspaceSlice_OutOfBounds_Throws)
{
    space_int_default[0];                                                       // One subspace at physical index 0
    ASSERT_THROW(space_int_default.getSubspaceSlice(0, 2), std::runtime_error); // Length too long
    ASSERT_THROW(space_int_default.getSubspaceSlice(1, 1), std::runtime_error); // Start too far
}
