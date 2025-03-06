#include "VknImageView.hpp"

namespace vkn
{
    VknImageView::VknImageView() : m_placeholder{true} {}

    VknImageView::VknImageView(VknResultArchive *archive, VknInfos *infos)
        : m_placeholder{false}, m_archive{archive}, m_infos{infos}
    {
    }

    void VknImageView::fillImageViewCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Image view create info already filled.");
        m_infos->fillImageViewCreateInfo();
        m_filledCreateInfo = true;
    }
}