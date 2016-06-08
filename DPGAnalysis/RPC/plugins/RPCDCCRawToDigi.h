#ifndef DPGAnalysis_RPC_RPCDCCRawToDigi_h
#define DPGAnalysis_RPC_RPCDCCRawToDigi_h

#include <vector>

#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "CondFormats/DataRecord/interface/RPCDCCLinkMapRcd.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"

#include "DPGAnalysis/Tools/interface/MergeMap.h"
#include "DPGAnalysis/Tools/interface/MergeRootHistogram.h"

namespace edm {
class ConfigurationDescriptions;
class Event;
class EventSetup;
class ParameterSet;
class Run;
} // namespace edm

class RPCDCCRawToDigiData
{
public:
    static unsigned int const dcc_record_cd_      = 0;
    static unsigned int const dcc_record_sbxd_    = 1;
    static unsigned int const dcc_record_sld_     = 2;
    static unsigned int const dcc_record_empty_   = 3;
    static unsigned int const dcc_record_rddm_    = 4;
    static unsigned int const dcc_record_sddm_    = 5;
    static unsigned int const dcc_record_rcdm_    = 6;
    static unsigned int const dcc_record_rdm_     = 7;
    static unsigned int const dcc_record_unknown_ = 8;

    static unsigned int const dcc_error_header_check_      = 0;
    static unsigned int const dcc_error_header_fed_        = 1;
    static unsigned int const dcc_error_trailer_check_     = 2;
    static unsigned int const dcc_error_trailer_length_    = 3;
    static unsigned int const dcc_error_trailer_crc_       = 4;
    static unsigned int const dcc_error_invalid_link_      = 5;
    static unsigned int const dcc_error_unknown_link_      = 6;
    static unsigned int const dcc_error_invalid_connector_ = 7;
    static unsigned int const dcc_error_missing_bx_        = 8;
    static unsigned int const dcc_error_missing_link_      = 9;
    static unsigned int const dcc_error_invalid_rddm_link_ = 10;
    static unsigned int const dcc_error_invalid_rcdm_link_ = 11;

    MergeTH2D record_fed_, error_fed_;
    MergeMap<::uint32_t, MergeTH2D> cd_fed_dcci_tbi_, invalid_fed_dcci_tbi_, eod_fed_dcci_tbi_
        , rddm_fed_dcci_tbi_, rcdm_fed_dcci_tbi_;
};

class RPCDCCRawToDigi
    : public edm::stream::EDProducer<edm::RunSummaryCache<RPCDCCRawToDigiData>
                                     , edm::EndRunProducer>
{
public:
    RPCDCCRawToDigi(edm::ParameterSet const & _config);
    ~RPCDCCRawToDigi();

    static void compute_crc_64bit(::uint16_t & _crc, ::uint64_t const & _word);

    static void fillDescriptions(edm::ConfigurationDescriptions & _descs);

    static std::shared_ptr<RPCDCCRawToDigiData> globalBeginRunSummary(edm::Run const & _run, edm::EventSetup const & _setup
                                                                      , RunContext const * _context);
    void beginRun(edm::Run const & _run, edm::EventSetup const & _setup) override;
    void produce(edm::Event & _event, edm::EventSetup const & _setup) override;
    void endRunSummary(edm::Run const & _run, edm::EventSetup const & _setup
                       , RPCDCCRawToDigiData * _run_data) const;
    static void globalEndRunSummary(edm::Run const & iRun, edm::EventSetup const &
                                    , RunContext const * _context
                                    , RPCDCCRawToDigiData const * _run_data) {}
    static void globalEndRunProduce(edm::Run & _run, edm::EventSetup const & _setup
                                    , RunContext const * _context
                                    , RPCDCCRawToDigiData const * _run_data);

protected:
    edm::EDGetTokenT<FEDRawDataCollection> raw_token_;

    bool calculate_crc_;

    edm::ESWatcher<RPCDCCLinkMapRcd> es_dcc_link_map_watcher_;
    std::vector<int> feds_;

    RPCDCCRawToDigiData stream_data_;
};

#endif // DPGAnalysis_RPC_RPCDCCRawToDigi_h
