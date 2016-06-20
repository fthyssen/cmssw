#include "DPGAnalysis/RPC/plugins/RPCRawToDigiAnalyser.h"

#include <memory>
#include <algorithm>

#include <set>
#include <cmath>

#include "TAxis.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCChamber.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"

#include "DPGAnalysis/RPC/interface/RPCMaskDetId.h"
#include "DPGAnalysis/RPC/interface/RPCMaskParser.h"
#include "DPGAnalysis/Tools/interface/associator.h"

float RPCRawToDigiAnalyser::DigiDistance(std::pair<RPCDetId, RPCDigi> const & _lhs, std::pair<RPCDetId, RPCDigi> const & _rhs)
{
    return (std::fabs(_lhs.second.strip() - _rhs.second.strip()) * 20
            + std::fabs(_lhs.second.bx() - _rhs.second.bx())
            + std::fabs(_lhs.first - _rhs.first) * 20);
}

std::unique_ptr<RPCRawToDigiAnalyserGlobalCache> RPCRawToDigiAnalyser::initializeGlobalCache(edm::ParameterSet const & _config)
{
    RPCRawToDigiAnalyserGlobalCache * _cache = new RPCRawToDigiAnalyserGlobalCache();
    std::vector<std::string> _roll_selection(_config.getParameter<std::vector<std::string> >("RollSelection"));
    for (std::vector<std::string>::const_iterator _roll = _roll_selection.begin()
             ; _roll != _roll_selection.end() ; ++_roll) {
        _cache->roll_selection_.push_back(RPCMaskParser::parse(*_roll));
        edm::LogInfo("RPCRawToDigiAnalyser") << "Selected roll " << _cache->roll_selection_.back();
    }
    return std::unique_ptr<RPCRawToDigiAnalyserGlobalCache>(_cache);
}

RPCRawToDigiAnalyser::RPCRawToDigiAnalyser(edm::ParameterSet const & _config, RPCRawToDigiAnalyserGlobalCache const *)
    : bx_min_(_config.getParameter<int>("bxMin"))
    , bx_max_(_config.getParameter<int>("bxMax"))
{
    lhs_digis_token_ = consumes<RPCDigiCollection>(_config.getParameter<edm::InputTag>("LHSDigiCollection"));
    rhs_digis_token_ = consumes<RPCDigiCollection>(_config.getParameter<edm::InputTag>("RHSDigiCollection"));

    produces<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>("RSRRollRoll");
    produces<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>("RSRStripStrip");
    produces<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>("RSRBXBX");
}

void RPCRawToDigiAnalyser::fillDescriptions(edm::ConfigurationDescriptions & _descs)
{
    edm::ParameterSetDescription _desc;
    _desc.add<edm::InputTag>("LHSDigiCollection", edm::InputTag("rpcunpacker", ""));
    _desc.add<edm::InputTag>("RHSDigiCollection", edm::InputTag("RPCDCCRawToDigi", ""));
    _desc.add<std::vector<std::string> >("RollSelection", std::vector<std::string>());
    _desc.add<int>("bxMin", -2);
    _desc.add<int>("bxMax", 2);

    _descs.add("RPCRawToDigiAnalyser", _desc);
}

std::shared_ptr<RPCRawToDigiAnalyserRunCache> RPCRawToDigiAnalyser::globalBeginRun(edm::Run const & _run, edm::EventSetup const & _setup
                                                                                   , GlobalCache const * _cache)
{
    // Get the list of region_station_rings and their rolls
    std::map<RPCMaskDetId, std::vector<RPCMaskDetId> > _region_station_ring_rolls;

    edm::ESHandle<RPCGeometry> _es_rpc_geom;
    _setup.get<MuonGeometryRecord>().get(_es_rpc_geom);
    std::vector<RPCRoll const *> const & _rolls = (_es_rpc_geom->rolls());
    for (std::vector<RPCRoll const *>::const_iterator _roll = _rolls.begin()
             ; _roll != _rolls.end() ; ++_roll) {
        RPCMaskDetId _maskid((*_roll)->id());

        bool _keep(_cache->roll_selection_.empty());
        for (std::vector<RPCMaskDetId>::const_iterator _mask = _cache->roll_selection_.begin()
                 ; !_keep && _mask != _cache->roll_selection_.end() ; ++_mask)
            _keep = _maskid.matches(*_mask);
        if (!_keep)
            continue;

        RPCMaskDetId _region_station_ring(_maskid);
        _region_station_ring.setSector().setSubSector().setSubSubSector().setRoll().setGap();
        _region_station_ring_rolls[_region_station_ring].push_back(_maskid);
    }

    // fill the axes
    std::shared_ptr<RPCRawToDigiAnalyserRunCache> _axes(new RPCRawToDigiAnalyserRunCache());

    for (std::map<RPCMaskDetId, std::vector<RPCMaskDetId> >::iterator _it = _region_station_ring_rolls.begin()
             ; _it != _region_station_ring_rolls.end() ; ++_it) {
        std::sort(_it->second.begin(), _it->second.end());
        std::vector<double> _ranges(_it->second.begin(), _it->second.end());
        _ranges.push_back(RPCMaskDetId(1, 4, 2, 3, 12, 2, 2, 5, 2).getId());
        _ranges.insert(_ranges.begin(), 0.);
        TAxis _roll_axis(_ranges.size() - 1, &(_ranges.front()));
        _roll_axis.SetNameTitle("RPCRoll", "RPC Roll");
        _roll_axis.CenterLabels(true);
        int _bin(2);
        for (std::vector<RPCMaskDetId>::const_iterator _id = _it->second.begin()
                 ; _id != _it->second.end() ; ++_id, ++_bin)
            _roll_axis.SetBinLabel(_bin, _id->getName().c_str());

        _axes->region_station_ring_rolls_.insert(std::pair<RPCMaskDetId, TAxis>(_it->first, _roll_axis));
    }

    return _axes;
}

std::shared_ptr<RPCRawToDigiAnalyserData> RPCRawToDigiAnalyser::globalBeginRunSummary(edm::Run const & _run, edm::EventSetup const & _setup
                                                                                      , RunContext const * _context)
{
    std::shared_ptr<RPCRawToDigiAnalyserData> _data(new RPCRawToDigiAnalyserData());

    TAxis _bx_axis(10, -5.5, 4.5);
    _bx_axis.SetNameTitle("BX", "Bunch Crossing");
    TAxis _strip_axis(129, -.5, 128.5);
    _strip_axis.SetNameTitle("Strip", "Strip");
    std::map<RPCMaskDetId, TAxis> const & _region_station_ring_rolls = _context->run()->region_station_ring_rolls_;
    for (std::map<RPCMaskDetId, TAxis>::const_iterator _it = _region_station_ring_rolls.begin()
             ; _it != _region_station_ring_rolls.end() ; ++_it) {
        std::string _title(_it->first.getName());
        std::string _name(_title);
        std::replace(_name.begin(), _name.end(), '/', '_');
        std::replace(_name.begin(), _name.end(), '+', 'P');
        std::replace(_name.begin(), _name.end(), '-', 'N');

        _data->region_station_ring_roll_roll_->insert(std::pair<::uint32_t, MergeTH2D>(_it->first.getId()
                                                                                       , MergeTH2D(_name.c_str(), _title.c_str()
                                                                                                   , _it->second, _it->second)));
        _data->region_station_ring_strip_strip_->insert(std::pair<::uint32_t, MergeTH2D>(_it->first.getId()
                                                                                         , MergeTH2D(_name.c_str(), _title.c_str()
                                                                                                     , _strip_axis, _strip_axis)));
        _data->region_station_ring_bx_bx_->insert(std::pair<::uint32_t, MergeTH2D>(_it->first.getId()
                                                                                   , MergeTH2D(_name.c_str(), _title.c_str()
                                                                                               , _bx_axis, _bx_axis)));
    }

    return _data;
}

void RPCRawToDigiAnalyser::beginRun(edm::Run const & _run, edm::EventSetup const & _setup)
{
    stream_data_.region_station_ring_roll_roll_->clear();
    stream_data_.region_station_ring_strip_strip_->clear();
    stream_data_.region_station_ring_bx_bx_->clear();

    // RunSummaryCache not available?
    // other solution would be to put Data in runCache, and add mutex to mergeProduct at endRun
    // but runCache is const, also in runcontext

    TAxis _bx_axis(10, -5.5, 4.5);
    _bx_axis.SetNameTitle("BX", "Bunch Crossing");
    _bx_axis.CenterLabels(true);
    TAxis _strip_axis(129, -.5, 128.5);
    _strip_axis.SetNameTitle("Strip", "Strip");
    _strip_axis.CenterLabels(true);
    std::map<RPCMaskDetId, TAxis> const & _region_station_ring_rolls = runCache()->region_station_ring_rolls_;
    for (std::map<RPCMaskDetId, TAxis>::const_iterator _it = _region_station_ring_rolls.begin()
             ; _it != _region_station_ring_rolls.end() ; ++_it) {
        std::string _title(_it->first.getName());
        std::string _name(_title);
        std::replace(_name.begin(), _name.end(), '/', '_');
        std::replace(_name.begin(), _name.end(), '+', 'P');
        std::replace(_name.begin(), _name.end(), '-', 'N');

        stream_data_.region_station_ring_roll_roll_->insert(std::pair<::uint32_t, MergeTH2D>(_it->first.getId()
                                                                                             , MergeTH2D(_name.c_str(), _title.c_str()
                                                                                                         , _it->second, _it->second)));
        stream_data_.region_station_ring_strip_strip_->insert(std::pair<::uint32_t, MergeTH2D>(_it->first.getId()
                                                                                               , MergeTH2D(_name.c_str(), _title.c_str()
                                                                                                           , _strip_axis, _strip_axis)));
        stream_data_.region_station_ring_bx_bx_->insert(std::pair<::uint32_t, MergeTH2D>(_it->first.getId()
                                                                                         , MergeTH2D(_name.c_str(), _title.c_str()
                                                                                                     , _bx_axis, _bx_axis)));
    }
}

void RPCRawToDigiAnalyser::produce(edm::Event & _event, edm::EventSetup const & _setup)
{
    edm::Handle<RPCDigiCollection> _lhs_digis_handle, _rhs_digis_handle;
    _event.getByToken(lhs_digis_token_, _lhs_digis_handle);
    _event.getByToken(rhs_digis_token_, _rhs_digis_handle);

    std::set<std::pair<RPCDetId, RPCDigi> > _lhs_digis, _rhs_digis;

    for (RPCDigiCollection::DigiRangeIterator _detid_digis = _lhs_digis_handle->begin()
             ; _detid_digis != _lhs_digis_handle->end() ; ++_detid_digis) {
        RPCDetId _detid((*_detid_digis).first);

        RPCMaskDetId _maskid(_detid);
        bool _keep(globalCache()->roll_selection_.empty());
        for (std::vector<RPCMaskDetId>::const_iterator _mask = globalCache()->roll_selection_.begin()
                 ; !_keep && _mask != globalCache()->roll_selection_.end() ; ++_mask)
            _keep = _maskid.matches(*_mask);
        if (!_keep)
            continue;

        RPCDigiCollection::DigiRangeIterator::DigiRangeIterator _digi_end = (*_detid_digis).second.second;
        for (RPCDigiCollection::DigiRangeIterator::DigiRangeIterator _digi = (*_detid_digis).second.first
                 ; _digi != _digi_end ; ++_digi)
            if (_digi->bx() >= bx_min_ && _digi->bx() <= bx_max_)
                _lhs_digis.insert(std::pair<RPCDetId, RPCDigi>(_detid, *_digi));
    }

    for (RPCDigiCollection::DigiRangeIterator _detid_digis = _rhs_digis_handle->begin()
             ; _detid_digis != _rhs_digis_handle->end() ; ++_detid_digis) {
        RPCDetId _detid((*_detid_digis).first);

        RPCMaskDetId _maskid(_detid);
        bool _keep(globalCache()->roll_selection_.empty());
        for (std::vector<RPCMaskDetId>::const_iterator _mask = globalCache()->roll_selection_.begin()
                 ; !_keep && _mask != globalCache()->roll_selection_.end() ; ++_mask)
            _keep = _maskid.matches(*_mask);
        if (!_keep)
            continue;

        RPCDigiCollection::DigiRangeIterator::DigiRangeIterator _digi_end = (*_detid_digis).second.second;
        for (RPCDigiCollection::DigiRangeIterator::DigiRangeIterator _digi = (*_detid_digis).second.first
                 ; _digi != _digi_end ; ++_digi)
            if (_digi->bx() >= bx_min_ && _digi->bx() <= bx_max_)
                _rhs_digis.insert(std::pair<RPCDetId, RPCDigi>(_detid, *_digi));
    }

    typedef association<std::set<std::pair<RPCDetId, RPCDigi> >, std::set<std::pair<RPCDetId, RPCDigi> > >
        association_type;
    associationvector<association_type> _associations
        = associator::associate<associationvector<association_type> >(&_lhs_digis, &_rhs_digis, &RPCRawToDigiAnalyser::DigiDistance
                                                                      , associator::munkres_
                                                                      , 10.);

    bool _report(false);
    for (associationvector<association_type>::const_iterator _association = _associations.begin()
             ; _association != _associations.end() ; ++_association) {
        RPCMaskDetId _id(_association.key_valid() ? _association.key()->first : _association.value()->first);
        RPCMaskDetId _region_station_ring(_id);
        _region_station_ring.setSector().setSubSector().setSubSubSector().setRoll().setGap();

        if (_association.distance() > 0)
            _report = true;

        if (!_association.value_valid()) {
            stream_data_.region_station_ring_roll_roll_->at(_region_station_ring.getId())->Fill(.5 + _id.getId(), 0.);
            stream_data_.region_station_ring_strip_strip_->at(_region_station_ring.getId())->Fill(_association.key()->second.strip(), 0.);
            stream_data_.region_station_ring_bx_bx_->at(_region_station_ring.getId())->Fill(_association.key()->second.bx(), -5.);
            _report = true;
        } else if (!_association.key_valid()) {
            stream_data_.region_station_ring_roll_roll_->at(_region_station_ring.getId())->Fill(0., .5 + _id.getId());
            stream_data_.region_station_ring_strip_strip_->at(_region_station_ring.getId())->Fill(0., _association.value()->second.strip());
            stream_data_.region_station_ring_bx_bx_->at(_region_station_ring.getId())->Fill(-5., _association.value()->second.bx());
            _report = true;
        } else {
            RPCMaskDetId _rhs(_association.value()->first);
            stream_data_.region_station_ring_roll_roll_->at(_region_station_ring.getId())->Fill(.5 + _id.getId(), .5 + _rhs.getId());
            stream_data_.region_station_ring_strip_strip_->at(_region_station_ring.getId())->Fill(_association.key()->second.strip(), _association.value()->second.strip());
            stream_data_.region_station_ring_bx_bx_->at(_region_station_ring.getId())->Fill(_association.key()->second.bx(), _association.value()->second.bx());
        }
    }
    if (_report)
        LogDebug("RPCRawToDigiAnalyser") << "Report difference";
}

void RPCRawToDigiAnalyser::endRunSummary(edm::Run const & _run, edm::EventSetup const & _setup
                                         , RPCRawToDigiAnalyserData * _run_data) const
{
    _run_data->region_station_ring_roll_roll_.mergeProduct(stream_data_.region_station_ring_roll_roll_);
    _run_data->region_station_ring_strip_strip_.mergeProduct(stream_data_.region_station_ring_strip_strip_);
    _run_data->region_station_ring_bx_bx_.mergeProduct(stream_data_.region_station_ring_bx_bx_);
}

void RPCRawToDigiAnalyser::globalEndRunProduce(edm::Run & _run, edm::EventSetup const & _setup
                                               , RunContext const * _context
                                               , RPCRawToDigiAnalyserData const * _run_data)
{
    std::auto_ptr<MergeMap<::uint32_t, MergeTH2D> > _region_station_ring_roll_roll(new MergeMap<::uint32_t, MergeTH2D>(_run_data->region_station_ring_roll_roll_));
    std::auto_ptr<MergeMap<::uint32_t, MergeTH2D> > _region_station_ring_strip_strip(new MergeMap<::uint32_t, MergeTH2D>(_run_data->region_station_ring_strip_strip_));
    std::auto_ptr<MergeMap<::uint32_t, MergeTH2D> > _region_station_ring_bx_bx(new MergeMap<::uint32_t, MergeTH2D>(_run_data->region_station_ring_bx_bx_));

    _run.put(_region_station_ring_roll_roll, "RSRRollRoll");
    _run.put(_region_station_ring_strip_strip, "RSRStripStrip");
    _run.put(_region_station_ring_bx_bx, "RSRBXBX");
}

//define this as a module
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(RPCRawToDigiAnalyser);
