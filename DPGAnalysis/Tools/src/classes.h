#include "DataFormats/Common/interface/Wrapper.h"

#include <stdint.h>

#include "DPGAnalysis/Tools/interface/MergeRootHistogram.h"
#include "DPGAnalysis/Tools/interface/MergeMap.h"

namespace DPGAnalysis_Tools {

struct dictionary {

    MergeTH1C merge_th1c_;
    MergeTH2C merge_th2c_;
    MergeTH3C merge_th3c_;

    MergeTH1S merge_th1s_;
    MergeTH2S merge_th2s_;
    MergeTH3S merge_th3s_;

    MergeTH1I merge_th1i_;
    MergeTH2I merge_th2i_;
    MergeTH3I merge_th3i_;

    MergeTH1F merge_th1f_;
    MergeTH2F merge_th2f_;
    MergeTH3F merge_th3f_;

    MergeTH1D merge_th1d_;
    MergeTH2D merge_th2d_;
    MergeTH3D merge_th3d_;

    edm::Wrapper<MergeTH1C> wrapper_merge_th1c_;
    edm::Wrapper<MergeTH2C> wrapper_merge_th2c_;
    edm::Wrapper<MergeTH3C> wrapper_merge_th3c_;

    edm::Wrapper<MergeTH1S> wrapper_merge_th1s_;
    edm::Wrapper<MergeTH2S> wrapper_merge_th2s_;
    edm::Wrapper<MergeTH3S> wrapper_merge_th3s_;

    edm::Wrapper<MergeTH1I> wrapper_merge_th1i_;
    edm::Wrapper<MergeTH2I> wrapper_merge_th2i_;
    edm::Wrapper<MergeTH3I> wrapper_merge_th3i_;

    edm::Wrapper<MergeTH1F> wrapper_merge_th1f_;
    edm::Wrapper<MergeTH2F> wrapper_merge_th2f_;
    edm::Wrapper<MergeTH3F> wrapper_merge_th3f_;

    edm::Wrapper<MergeTH1D> wrapper_merge_th1d_;
    edm::Wrapper<MergeTH2D> wrapper_merge_th2d_;
    edm::Wrapper<MergeTH3D> wrapper_merge_th3d_;

    std::pair<::uint32_t, MergeTH1C> pair_detid_merge_th1c_;
    std::map<::uint32_t, MergeTH1C> map_detid_merge_th1c_;
    MergeMap<::uint32_t, MergeTH1C> merge_map_detid_merge_th1c_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH1C> > wrapper_merge_map_detid_merge_th1c_;

    std::pair<::uint32_t, MergeTH2C> pair_detid_merge_th2c_;
    std::map<::uint32_t, MergeTH2C> map_detid_merge_th2c_;
    MergeMap<::uint32_t, MergeTH2C> merge_map_detid_merge_th2c_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH2C> > wrapper_merge_map_detid_merge_th2c_;

    std::pair<::uint32_t, MergeTH3C> pair_detid_merge_th3c_;
    std::map<::uint32_t, MergeTH3C> map_detid_merge_th3c_;
    MergeMap<::uint32_t, MergeTH3C> merge_map_detid_merge_th3c_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH3C> > wrapper_merge_map_detid_merge_th3c_;

    std::pair<::uint32_t, MergeTH1S> pair_detid_merge_th1s_;
    std::map<::uint32_t, MergeTH1S> map_detid_merge_th1s_;
    MergeMap<::uint32_t, MergeTH1S> merge_map_detid_merge_th1s_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH1S> > wrapper_merge_map_detid_merge_th1s_;

    std::pair<::uint32_t, MergeTH2S> pair_detid_merge_th2s_;
    std::map<::uint32_t, MergeTH2S> map_detid_merge_th2s_;
    MergeMap<::uint32_t, MergeTH2S> merge_map_detid_merge_th2s_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH2S> > wrapper_merge_map_detid_merge_th2s_;

    std::pair<::uint32_t, MergeTH3S> pair_detid_merge_th3s_;
    std::map<::uint32_t, MergeTH3S> map_detid_merge_th3s_;
    MergeMap<::uint32_t, MergeTH3S> merge_map_detid_merge_th3s_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH3S> > wrapper_merge_map_detid_merge_th3s_;

    std::pair<::uint32_t, MergeTH1I> pair_detid_merge_th1i_;
    std::map<::uint32_t, MergeTH1I> map_detid_merge_th1i_;
    MergeMap<::uint32_t, MergeTH1I> merge_map_detid_merge_th1i_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH1I> > wrapper_merge_map_detid_merge_th1i_;

    std::pair<::uint32_t, MergeTH2I> pair_detid_merge_th2i_;
    std::map<::uint32_t, MergeTH2I> map_detid_merge_th2i_;
    MergeMap<::uint32_t, MergeTH2I> merge_map_detid_merge_th2i_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH2I> > wrapper_merge_map_detid_merge_th2i_;

    std::pair<::uint32_t, MergeTH3I> pair_detid_merge_th3i_;
    std::map<::uint32_t, MergeTH3I> map_detid_merge_th3i_;
    MergeMap<::uint32_t, MergeTH3I> merge_map_detid_merge_th3i_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH3I> > wrapper_merge_map_detid_merge_th3i_;

    std::pair<::uint32_t, MergeTH1F> pair_detid_merge_th1f_;
    std::map<::uint32_t, MergeTH1F> map_detid_merge_th1f_;
    MergeMap<::uint32_t, MergeTH1F> merge_map_detid_merge_th1f_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH1F> > wrapper_merge_map_detid_merge_th1f_;

    std::pair<::uint32_t, MergeTH2F> pair_detid_merge_th2f_;
    std::map<::uint32_t, MergeTH2F> map_detid_merge_th2f_;
    MergeMap<::uint32_t, MergeTH2F> merge_map_detid_merge_th2f_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH2F> > wrapper_merge_map_detid_merge_th2f_;

    std::pair<::uint32_t, MergeTH3F> pair_detid_merge_th3f_;
    std::map<::uint32_t, MergeTH3F> map_detid_merge_th3f_;
    MergeMap<::uint32_t, MergeTH3F> merge_map_detid_merge_th3f_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH3F> > wrapper_merge_map_detid_merge_th3f_;

    std::pair<::uint32_t, MergeTH1D> pair_detid_merge_th1d_;
    std::map<::uint32_t, MergeTH1D> map_detid_merge_th1d_;
    MergeMap<::uint32_t, MergeTH1D> merge_map_detid_merge_th1d_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH1D> > wrapper_merge_map_detid_merge_th1d_;

    std::pair<::uint32_t, MergeTH2D> pair_detid_merge_th2d_;
    std::map<::uint32_t, MergeTH2D> map_detid_merge_th2d_;
    MergeMap<::uint32_t, MergeTH2D> merge_map_detid_merge_th2d_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH2D> > wrapper_merge_map_detid_merge_th2d_;

    std::pair<::uint32_t, MergeTH3D> pair_detid_merge_th3d_;
    std::map<::uint32_t, MergeTH3D> map_detid_merge_th3d_;
    MergeMap<::uint32_t, MergeTH3D> merge_map_detid_merge_th3d_;
    edm::Wrapper<MergeMap<::uint32_t, MergeTH3D> > wrapper_merge_map_detid_merge_th3d_;

};

} // namespace DPGAnalysis_Tools
