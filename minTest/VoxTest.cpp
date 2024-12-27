#include <iostream>
#include "../VknConfig/VknConfig.hpp"
#include "VoxTree.hpp"

int main()
{
    voxigen::VoxTreeBranch trunk{};
    std::vector<voxigen::Voxel> drawPile;
    std::vector<voxigen::VoxTreeBranch> branches = trunk.getBranches();
    for (voxigen::VoxTreeBranch leaf : branches)
        drawPile.push_back(leaf.voxel());
}