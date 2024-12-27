#pragma once

#include <vector>
#include <stdexcept>

namespace voxigen
{
    struct Voxel
    {
        uint64_t id;
    };

    const uint8_t ONE = 1;

    class VoxTreeBranch
    {
        std::vector<VoxTreeBranch> m_branches;
        static uint8_t m_numBranches;
        Voxel m_data{};
        uint64_t m_state;
        VoxTreeBranch *m_parent;
        static const std::vector<std::vector<int8_t>> faceVectors;

    public:
        VoxTreeBranch(
            uint8_t numBranches = 8, uint8_t tierNum = 255, uint8_t branchNum = 0,
            uint64_t id = 0, VoxTreeBranch *parent = nullptr);

        void split();
        void merge();
        uint64_t tier();
        uint64_t branch();
        Voxel voxel();
        uint64_t id();
        uint64_t grounded();
        uint64_t isAir();
        bool isLeaf();
        void setIsLeaf(bool);
        void setGrounded(bool, bool);
        std::vector<VoxTreeBranch> getBranches();
    };
}