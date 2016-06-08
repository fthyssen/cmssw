#ifndef DPGAnalysis_RPC_RPCRawToDigiPlotter_h
#define DPGAnalysis_RPC_RPCRawToDigiPlotter_h

#include <stdint.h>

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DPGAnalysis/Tools/interface/MergeMap.h"
#include "DPGAnalysis/Tools/interface/MergeRootHistogram.h"

namespace edm {
class ParameterSet;
class ConfigurationDescriptions;
class Run;
class Event;
class EventSetup;
} // namespace edm

class RPCRawToDigiPlotter
    : public edm::one::EDAnalyzer<edm::one::WatchRuns>
{
public:
    explicit RPCRawToDigiPlotter(edm::ParameterSet const & _config);

    static void fillDescriptions(edm::ConfigurationDescriptions & _descs);

    void beginJob();
    void beginRun(edm::Run const & _run, edm::EventSetup const & _setup);
    void analyze(edm::Event const & _event, edm::EventSetup const & _setup);
    void endRun(edm::Run const & _run, edm::EventSetup const & _setup);
    void endJob();

protected:
    edm::Service<TFileService> fileservice_;

    bool first_run_;
    std::string output_pdf_;
    bool create_pdf_;

    // Tokens
    edm::EDGetTokenT<MergeMap<::uint32_t, MergeTH2D> > region_station_ring_roll_roll_token_
        , region_station_ring_strip_strip_token_
        , region_station_ring_bx_bx_token_;

    // Histograms
    MergeMap<::uint32_t, MergeTH2D> region_station_ring_roll_roll_
        , region_station_ring_strip_strip_
        , region_station_ring_bx_bx_;
};

#endif // DPGAnalysis_RPC_RPCRawToDigiPlotter_h
