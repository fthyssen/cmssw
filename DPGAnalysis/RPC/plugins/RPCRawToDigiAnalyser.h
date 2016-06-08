#ifndef DPGAnalysis_RPC_RPCRawToDigiAnalyser_h
#define DPGAnalysis_RPC_RPCRawToDigiAnalyser_h

#include <memory>
#include <vector>
#include <stdint.h>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"

#include "DPGAnalysis/Tools/interface/MergeMap.h"
#include "DPGAnalysis/Tools/interface/MergeRootHistogram.h"
#include "DPGAnalysis/RPC/interface/RPCMaskDetId.h"

namespace edm {
class ParameterSet;
class ConfigurationDescriptions;
class Run;
class Event;
class EventSetup;
} // namespace edm

class RPCRawToDigiAnalyserGlobalCache
{
public:
    std::vector<RPCMaskDetId> roll_selection_;
};

class RPCRawToDigiAnalyserRunCache
{
public:
    std::map<RPCMaskDetId, TAxis> region_station_ring_rolls_;
};

class RPCRawToDigiAnalyserData
{
public:
    MergeMap<::uint32_t, MergeTH2D> region_station_ring_roll_roll_
        , region_station_ring_strip_strip_
        , region_station_ring_bx_bx_;
};

class RPCRawToDigiAnalyser
    : public edm::stream::EDProducer<edm::GlobalCache<RPCRawToDigiAnalyserGlobalCache>
                                     , edm::RunCache<RPCRawToDigiAnalyserRunCache>
                                     , edm::RunSummaryCache<RPCRawToDigiAnalyserData>
                                     , edm::EndRunProducer>
{
public:
    static std::unique_ptr<RPCRawToDigiAnalyserGlobalCache> initializeGlobalCache(edm::ParameterSet const & _config);
    explicit RPCRawToDigiAnalyser(edm::ParameterSet const & _config, RPCRawToDigiAnalyserGlobalCache const *);

    static float DigiDistance(std::pair<RPCDetId, RPCDigi> const & _lhs
                              , std::pair<RPCDetId, RPCDigi> const & _rhs);

    static void fillDescriptions(edm::ConfigurationDescriptions & _descs);

    static std::shared_ptr<RPCRawToDigiAnalyserRunCache> globalBeginRun(edm::Run const & _run, edm::EventSetup const & _setup
                                                                        , GlobalCache const * _cache);
    static std::shared_ptr<RPCRawToDigiAnalyserData> globalBeginRunSummary(edm::Run const & _run, edm::EventSetup const & _setup
                                                                           , RunContext const * _context);
    void beginRun(edm::Run const & _run, edm::EventSetup const & _setup);
    void produce(edm::Event & _event, edm::EventSetup const & _setup);
    void endRunSummary(edm::Run const & _run, edm::EventSetup const & _setup
                       , RPCRawToDigiAnalyserData * _run_data) const;
    static void globalEndRun(edm::Run const & iRun, edm::EventSetup const &, RunContext const *) {}
    static void globalEndRunSummary(edm::Run const & iRun, edm::EventSetup const &
                                    , RunContext const * _context
                                    , RPCRawToDigiAnalyserData const * _run_data) {}
    static void globalEndRunProduce(edm::Run & _run, edm::EventSetup const & _setup
                                    , RunContext const * _context
                                    , RPCRawToDigiAnalyserData const * _run_data);
    static void globalEndJob(RPCRawToDigiAnalyserGlobalCache *) {}
protected:
    // tokens
    edm::EDGetTokenT<RPCDigiCollection> lhs_digis_token_, rhs_digis_token_;
    // stream-specific histograms
    RPCRawToDigiAnalyserData stream_data_;
};

#endif // DPGAnalysis_RPC_RPCRawToDigiAnalyser_h
