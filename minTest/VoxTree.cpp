#include "VoxTree.hpp"
#include <iostream>

const uint8_t tierBitNum = 54;
const uint8_t tierBitMask = 31;
const uint8_t leafBitNum = 63;
const uint8_t leafBitMask = 1;
const uint8_t groundedBitNum = 61;
const uint8_t groundedBitMask = 3;
const uint8_t airBitNum = 59;
const uint8_t airBitMask = 3;
const uint8_t numBranchBits = 3;
const uint8_t branchBitMask = 7;
const uint8_t topTierNum = 19;
namespace voxigen
{
    uint8_t VoxTreeBranch::m_numBranches = 8;

    VoxTreeBranch::VoxTreeBranch(
        uint8_t numBranches, uint8_t tierNum, uint8_t branchNum, uint64_t id, VoxTreeBranch *parent)
    {
        m_parent = parent;
        m_numBranches = numBranches;
        m_data.id = (id & 0xF83FFFFFFFFFFFFF);      // Clear tierNum bits
        m_data.id = m_data.id | 0x8000000000000000; // Set leaf flag
        if (tierNum == 255)                         // Add new tier num
            m_data.id = m_data.id | (static_cast<uint64_t>(topTierNum) << tierBitNum);
        else
        {
            m_data.id = m_data.id | (static_cast<uint64_t>(tierNum) << tierBitNum);
            m_data.id = m_data.id | (static_cast<uint64_t>(branchNum) << ((tierNum * numBranchBits) - numBranchBits)); // Add new branch num at tier bits
        }
        if (m_parent == nullptr)
            this->split();
    }

    void VoxTreeBranch::split()
    {
        if (!this->isLeaf())
            throw std::logic_error("Node is already split.");
        for (uint8_t branch = 0; branch < m_numBranches; ++branch)
            m_branches.push_back(
                VoxTreeBranch(m_numBranches, static_cast<uint8_t>(this->tier() - ONE), branch, m_data.id, this));
        this->setIsLeaf(false);
    }

    void VoxTreeBranch::merge()
    {
        if (this->isLeaf())
            throw std::logic_error("Cannot merge leaf node.");
        std::vector<VoxTreeBranch>().swap(m_branches); // clear m_branches
        this->setIsLeaf(true);
    }

    uint64_t VoxTreeBranch::tier() { return (m_data.id >> tierBitNum) & tierBitMask; }

    uint64_t VoxTreeBranch::branch()
    {
        return (m_data.id >> ((this->tier() - 1) * numBranchBits)) & branchBitMask;
    }

    Voxel VoxTreeBranch::voxel() { return m_data; }

    uint64_t VoxTreeBranch::id() { return m_data.id; }

    bool VoxTreeBranch::isLeaf() { return (m_data.id >> leafBitNum) & leafBitMask; }

    void VoxTreeBranch::setIsLeaf(bool isLeaf)
    {
        if (isLeaf)
            m_data.id = m_data.id | (static_cast<uint64_t>(ONE) << leafBitNum);
        else
            m_data.id = m_data.id & 0x7FFFFFFFFFFFFFFF;
    }

    std::vector<VoxTreeBranch> VoxTreeBranch::getBranches() { return m_branches; }

    uint64_t VoxTreeBranch::grounded() { return (m_state >> groundedBitNum) & groundedBitMask; }

    uint64_t VoxTreeBranch::isAir() { return (m_state >> airBitNum) & airBitMask; }

    void VoxTreeBranch::setGrounded(bool isGrounded, bool isMix = false)
    {
        if (isGrounded)
            m_data.id = m_data.id & 0xBFFFFFFFFFFFFFFF;
        else
            m_data.id = m_data.id | 0x4000000000000000;
        if (isMix)
            m_data.id = m_data.id | 0x2000000000000000;
        else
            m_data.id = m_data.id & 0xDFFFFFFFFFFFFFFF;

        if (m_parent != nullptr)
            m_parent->setGrounded(isGrounded, true);
    }
}