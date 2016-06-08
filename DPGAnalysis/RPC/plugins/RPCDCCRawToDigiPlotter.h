#ifndef DPGAnalysis_RPC_RPCDCCRawToDigiPlotter_h
#define DPGAnalysis_RPC_RPCDCCRawToDigiPlotter_h

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

class RPCDCCRawToDigiPlotter
    : public edm::one::EDAnalyzer<edm::one::WatchRuns>
{
public:
    explicit RPCDCCRawToDigiPlotter(edm::ParameterSet const & _config);

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
    edm::EDGetTokenT<MergeTH2D> record_fed_token_, error_fed_token_;
    edm::EDGetTokenT<MergeMap<::uint32_t, MergeTH2D> > cd_fed_dcci_tbi_token_, invalid_fed_dcci_tbi_token_, eod_fed_dcci_tbi_token_
        , rddm_fed_dcci_tbi_token_, rcdm_fed_dcci_tbi_token_;

    // Histograms
    MergeTH2D record_fed_, error_fed_;
    MergeMap<::uint32_t, MergeTH2D> cd_fed_dcci_tbi_, invalid_fed_dcci_tbi_, eod_fed_dcci_tbi_
        , rddm_fed_dcci_tbi_, rcdm_fed_dcci_tbi_;
};

#endif // DPGAnalysis_RPC_RPCDCCRawToDigiPlotter_h
