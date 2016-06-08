#include "DPGAnalysis/RPC/plugins/RPCDCCRawToDigi.h"

#include <memory>
#include <set>
#include <sstream>
#include <utility>

#include "TAxis.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/CRC16.h"

#include "CondFormats/DataRecord/interface/RPCLBLinkMapRcd.h"
#include "CondFormats/RPCObjects/interface/RPCDCCLinkMap.h"
#include "CondFormats/RPCObjects/interface/RPCLBLinkMap.h"
#include "DataFormats/FEDRawData/interface/FEDHeader.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"
#include "DataFormats/RPCDigi/interface/RPCDigi.h"
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"
#include "DPGAnalysis/RPC/interface/RPCDCCRecord.h"

RPCDCCRawToDigi::RPCDCCRawToDigi(edm::ParameterSet const & _config)
    : calculate_crc_(_config.getParameter<bool>("calculateCRC"))
{
    produces<RPCDigiCollection>();
    raw_token_ = consumes<FEDRawDataCollection>(_config.getParameter<edm::InputTag>("inputTag"));

    produces<MergeTH2D, edm::InRun>("RecordFed");
    produces<MergeTH2D, edm::InRun>("ErrorFed");
    produces<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>("CdFedDcciTbi");
    produces<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>("InvalidFedDcciTbi");
    produces<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>("EodFedDcciTbi");
    produces<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>("RddmFedDcciTbi");
    produces<MergeMap<::uint32_t, MergeTH2D>, edm::InRun>("RcdmFedDcciTbi");
}

RPCDCCRawToDigi::~RPCDCCRawToDigi()
{}

void RPCDCCRawToDigi::compute_crc_64bit(::uint16_t & _crc, ::uint64_t const & _word)
{ // overcome constness problem evf::compute_crc_64bit
    unsigned char const * _uchars(reinterpret_cast<unsigned char const *>(&_word));
    for (unsigned char const * _uchar = _uchars + 7
             ; _uchar >= _uchars ; --_uchar)
        _crc = evf::compute_crc_8bit(_crc, *_uchar);
}

void RPCDCCRawToDigi::fillDescriptions(edm::ConfigurationDescriptions & _descs)
{
    edm::ParameterSetDescription _desc;
    _desc.add<edm::InputTag>("inputTag", edm::InputTag("rawDataCollector", ""));
    _desc.add<bool>("calculateCRC", true);
    _descs.add("RPCDCCRawToDigi", _desc);
}

std::shared_ptr<RPCDCCRawToDigiData> RPCDCCRawToDigi::globalBeginRunSummary(edm::Run const & _run, edm::EventSetup const & _setup
                                                                            , RunContext const * _context)
{
    std::shared_ptr<RPCDCCRawToDigiData> _data(new RPCDCCRawToDigiData());

    // FED Axis
    edm::ESHandle<RPCDCCLinkMap> _es_dcc_link_map;
    _setup.get<RPCDCCLinkMapRcd>().get(_es_dcc_link_map);
    std::set<int> _fed_set;
    for (RPCDCCLinkMap::map_type::const_iterator _dcc_link = _es_dcc_link_map->getMap().begin()
             ; _dcc_link != _es_dcc_link_map->getMap().end() ; ++_dcc_link)
        _fed_set.insert(_dcc_link->first.getFED());

    std::vector<double> _feds(_fed_set.begin(), _fed_set.end());
    _feds.push_back(_feds.back() + 1.);

    TAxis _fed_axis(_feds.size() - 1, &(_feds.front()));
    _fed_axis.SetNameTitle("FED", "FED");
    _fed_axis.CenterLabels(true);
    std::ostringstream _label_oss;
    int _bin(1);
    for (std::set<int>::const_iterator _fed = _fed_set.begin() ; _fed != _fed_set.end() ; ++_fed, ++_bin) {
        _label_oss.clear(); _label_oss.str("");
        _label_oss << *_fed;
        _fed_axis.SetBinLabel(_bin, _label_oss.str().c_str());
    }

    // DCC Record Axis
    TAxis _dcc_record_axis(9, -.5, 8.5);
    _dcc_record_axis.SetNameTitle("DCCRecord", "DCC Record");
    _dcc_record_axis.CenterLabels(true);
    _dcc_record_axis.SetBinLabel(1, "CD");
    _dcc_record_axis.SetBinLabel(2, "SBXD");
    _dcc_record_axis.SetBinLabel(3, "SLD");
    _dcc_record_axis.SetBinLabel(4, "empty");
    _dcc_record_axis.SetBinLabel(5, "RDDM");
    _dcc_record_axis.SetBinLabel(6, "SDDM");
    _dcc_record_axis.SetBinLabel(7, "RCDM");
    _dcc_record_axis.SetBinLabel(8, "RDM");
    _dcc_record_axis.SetBinLabel(9, "unknown");

    // DCC Error Axis
    TAxis _dcc_error_axis(12, -.5, 11.5);
    _dcc_error_axis.SetNameTitle("DCCError", "DCC Error");
    _dcc_error_axis.CenterLabels(true);
    _dcc_error_axis.SetBinLabel(1, "Header Check Fail");
    _dcc_error_axis.SetBinLabel(2, "Inconsistent FED ID");
    _dcc_error_axis.SetBinLabel(3, "Trailer Check Fail");
    _dcc_error_axis.SetBinLabel(4, "Inconsistent Data Size");
    _dcc_error_axis.SetBinLabel(5, "Inconsistent CRC");
    _dcc_error_axis.SetBinLabel(6, "Invalid Link");
    _dcc_error_axis.SetBinLabel(7, "Unknown Link");
    _dcc_error_axis.SetBinLabel(8, "Invalid Connector");
    _dcc_error_axis.SetBinLabel(9, "Missing BX");
    _dcc_error_axis.SetBinLabel(10, "Missing Link");
    _dcc_error_axis.SetBinLabel(11, "Invalid Link RDDM");
    _dcc_error_axis.SetBinLabel(12, "Invalid Link RCDM");

    // DCC Input Axis
    TAxis _dcci_axis(37, -.5, 36.5);
    _dcci_axis.SetNameTitle("DCCInput", "DCC Input");
    // _dcci_axis.CenterLabels(true);

    // TB Input Axis
    TAxis _tbi_axis(19, -.5, 18.5);
    _tbi_axis.SetNameTitle("TBInput", "TB Input");
    // _tbi_axis.CenterLabels(true);

    _data->record_fed_ = MergeTH2D("RecordFed", "", _dcc_record_axis, _fed_axis);
    _data->error_fed_ = MergeTH2D("ErrorFed", "", _dcc_error_axis, _fed_axis);

    std::ostringstream _title_ss, _name_ss;

    for (std::set<int>::const_iterator _fed = _fed_set.begin() ; _fed != _fed_set.end() ; ++_fed) {
        _title_ss.clear(); _title_ss.str("");
        _name_ss.clear();  _name_ss.str("");
        _title_ss << "CD FED " << *_fed;
        _name_ss << "CDsFED" << *_fed;
        _data->cd_fed_dcci_tbi_->insert(std::pair<::uint32_t, MergeTH2D>(*_fed
                                                                         , MergeTH2D(_name_ss.str().c_str(), _title_ss.str().c_str()
                                                                                     , _dcci_axis, _tbi_axis)));

        _title_ss.clear(); _title_ss.str("");
        _name_ss.clear();  _name_ss.str("");
        _title_ss << "Invalid LinkBoard or Connector for FED " << *_fed;
        _name_ss << "InvalidCDsFED" << *_fed;
        _data->invalid_fed_dcci_tbi_->insert(std::pair<::uint32_t, MergeTH2D>(*_fed
                                                                              , MergeTH2D(_name_ss.str().c_str(), _title_ss.str().c_str()
                                                                                          , _dcci_axis, _tbi_axis)));

        _title_ss.clear(); _title_ss.str("");
        _name_ss.clear();  _name_ss.str("");
        _title_ss << "End-Of-Data Markers FED " << *_fed;
        _name_ss << "EODsFED" << *_fed;
        _data->eod_fed_dcci_tbi_->insert(std::pair<::uint32_t, MergeTH2D>(*_fed
                                                                          , MergeTH2D(_name_ss.str().c_str(), _title_ss.str().c_str()
                                                                                      , _dcci_axis, _tbi_axis)));

        _title_ss.clear(); _title_ss.str("");
        _name_ss.clear();  _name_ss.str("");
        _title_ss << "RMB Discarded Data Markers FED " << *_fed;
        _name_ss << "RDDMsFED" << *_fed;
        _data->rddm_fed_dcci_tbi_->insert(std::pair<::uint32_t, MergeTH2D>(*_fed
                                                                           , MergeTH2D(_name_ss.str().c_str(), _title_ss.str().c_str()
                                                                                       , _dcci_axis, _tbi_axis)));

        _title_ss.clear(); _title_ss.str("");
        _name_ss.clear();  _name_ss.str("");
        _title_ss << "RMB Corrupted Data Markers FED " << *_fed;
        _name_ss << "RCDMsFED" << *_fed;
        _data->rcdm_fed_dcci_tbi_->insert(std::pair<::uint32_t, MergeTH2D>(*_fed
                                                                           , MergeTH2D(_name_ss.str().c_str(), _title_ss.str().c_str()
                                                                                       , _dcci_axis, _tbi_axis)));
    }

    return _data;
}

void RPCDCCRawToDigi::beginRun(edm::Run const & _run, edm::EventSetup const & _setup)
{
    if (es_dcc_link_map_watcher_.check(_setup)) {
        edm::ESHandle<RPCDCCLinkMap> _es_dcc_link_map;
        _setup.get<RPCDCCLinkMapRcd>().get(_es_dcc_link_map);
        std::set<int> _feds;
        for (RPCDCCLinkMap::map_type::const_iterator _dcc_link = _es_dcc_link_map->getMap().begin()
                 ; _dcc_link != _es_dcc_link_map->getMap().end() ; ++_dcc_link)
            _feds.insert(_dcc_link->first.getFED());
        feds_.assign(_feds.begin(), _feds.end());
    }

    std::shared_ptr<RPCDCCRawToDigiData> _data = globalBeginRunSummary(_run, _setup, 0); // recycling code
    stream_data_.record_fed_ = _data->record_fed_;
    stream_data_.error_fed_ = _data->error_fed_;
    stream_data_.cd_fed_dcci_tbi_ = _data->cd_fed_dcci_tbi_;
    stream_data_.invalid_fed_dcci_tbi_ = _data->invalid_fed_dcci_tbi_;
    stream_data_.eod_fed_dcci_tbi_ = _data->eod_fed_dcci_tbi_;
    stream_data_.rddm_fed_dcci_tbi_ = _data->rddm_fed_dcci_tbi_;
    stream_data_.rcdm_fed_dcci_tbi_ = _data->rcdm_fed_dcci_tbi_;
}

void RPCDCCRawToDigi::produce(edm::Event & _event, edm::EventSetup const & _setup)
{
    // Get EventSetup Electronics Maps
    edm::ESHandle<RPCDCCLinkMap> _es_dcc_link_map;
    _setup.get<RPCDCCLinkMapRcd>().get(_es_dcc_link_map);
    RPCDCCLinkMap::map_type const & _dcc_link_map(_es_dcc_link_map->getMap());

    edm::ESHandle<RPCLBLinkMap> _es_lb_link_map;
    _setup.get<RPCLBLinkMapRcd>().get(_es_lb_link_map);
    RPCLBLinkMap::map_type const & _lb_link_map(_es_lb_link_map->getMap());

    // Get RAW Data
    edm::Handle<FEDRawDataCollection> _raw_data_collection;
    _event.getByToken(raw_token_, _raw_data_collection);

    std::set<std::pair<RPCDetId, RPCDigi> > _digis;

    // Loop over the FEDs
    for (std::vector<int>::const_iterator _fed = feds_.begin()
             ; _fed != feds_.end() ; ++_fed) {

        ::uint16_t _crc(0xffff);

        RPCDCCLink _dcc_link;
        _dcc_link.setFED(*_fed);

        FEDRawData const & _raw_data = _raw_data_collection->FEDData(*_fed);

        unsigned int _nwords(_raw_data.size() / sizeof(::uint64_t));
        if (!_nwords)
            continue;

        ::uint64_t const * _word(reinterpret_cast<::uint64_t const *>(_raw_data.data()));
        ::uint64_t const * _word_end = _word + _nwords;

        int _bx_id(0);

        // Handle the CDF Headers
        bool _more_headers(true);
        for ( ; _word < _word_end && _more_headers ; ++_word) {
            LogDebug("RPCDCCRawToDigi") << "Header " << std::hex << *_word << std::dec;
            FEDHeader _header(reinterpret_cast<unsigned char const *>(_word));
            if (!_header.check()) {
                edm::LogWarning("RPCDCCRawToDigi") << "FED Header check failed for FED id " << *_fed;
                stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_header_check_, _dcc_link.getFED());
                break;
            }
            if (_header.sourceID() != *_fed) {
                edm::LogWarning("RPCDCCRawToDigi") << "FED Header Source ID " << _header.sourceID()
                                                   << " does not match requested FED id " << *_fed;
                stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_header_fed_, _dcc_link.getFED());
            }

            if (calculate_crc_)
                compute_crc_64bit(_crc, *_word);
            // _header.lvl1ID();
            _bx_id = _header.bxID();
            _more_headers = _header.moreHeaders();
        }

        if (_more_headers)
            continue;

        // Handle the CDF Trailers
        bool _more_trailers(true);
        for (--_word_end ; _word_end > _word && _more_trailers ; --_word_end) {
            FEDTrailer _trailer(reinterpret_cast<unsigned char const *>(_word_end));
            LogDebug("RPCDCCRawToDigi") << "Trailer " << std::hex << *_word_end << std::dec
                                        << ", length " << _trailer.lenght();
            if (!_trailer.check()) {
                edm::LogWarning("RPCDCCRawToDigi") << "FED Trailer check failed for FED id " << *_fed;
                stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_trailer_check_, _dcc_link.getFED());
                --_word_end;
                break;
            }
            if (_trailer.lenght() != (int)_nwords) {
                edm::LogWarning("RPCDCCRawToDigi") << "FED Trailer length " << _trailer.lenght()
                                                   << " does not match actual data size " << _nwords
                                                   << " for FED id " << *_fed;
                stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_trailer_length_, _dcc_link.getFED());
                --_word_end;
                break;
            }
            _more_trailers = _trailer.moreTrailers();
        }

        if (_more_trailers)
            continue;

        ++_word_end;

        // Loop over the words/records to find the digis
        // fill DCC Link and BX on the fly
        int _bx(0);
        RPCDCCLinkMap::map_type::const_iterator _dcc_link_it(_dcc_link_map.end());

        bool _has_bx(false);
        bool _has_link(false);

        for (; _word < _word_end ; ++_word) {
            if (calculate_crc_)
                compute_crc_64bit(_crc, *_word);

            ::uint16_t const * _record = reinterpret_cast<::uint16_t const *>(_word + 1) - 1;
            ::uint16_t const * _record_end = reinterpret_cast<::uint16_t const *>(_word) - 1;

            LogDebug("RPCDCCRawToDigi") << "Word " << std::hex << *_word << std::dec;

            for ( ; _record > _record_end ; --_record) {
                unsigned int _record_type(rpcdcc::Record::getType(*_record));
                if (_record_type == rpcdcc::Record::sbxd_type_) {
                    LogDebug("RPCDCCRawToDigi") << "SBXD: " << std::hex << *_record << std::dec;
                    stream_data_.record_fed_->Fill(RPCDCCRawToDigiData::dcc_record_sbxd_, _dcc_link.getFED());
                    _has_bx = true;
                    _has_link = false;
                    rpcdcc::SBXDRecord _sbxd_record(*_record);
                    _bx = ((_sbxd_record.getBX() - _bx_id + 1782) % 3564) - 1782;
                    // _dcc_link.setDCCInput().setTBInput();
                } else if (_record_type == rpcdcc::Record::sld_type_) {
                    LogDebug("RPCDCCRawToDigi") << "SLD: " << std::hex << *_record << std::dec;
                    stream_data_.record_fed_->Fill(RPCDCCRawToDigiData::dcc_record_sld_, _dcc_link.getFED());
                    if (_has_bx) {
                        rpcdcc::SLDRecord _sld_record(*_record);
                        if (_sld_record.getDCCInput() > RPCDCCLink::max_dccinput_) {
                            edm::LogWarning("RPCDCCRawToDigi") << "Skipping invalid DCC Input " << _sld_record.getDCCInput()
                                                               << " from FED " << _dcc_link.getFED();
                            stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_invalid_link_, _dcc_link.getFED());
                            continue;
                        }
                        if (_sld_record.getTBInput() > RPCDCCLink::max_tbinput_) {
                            edm::LogWarning("RPCDCCRawToDigi") << "Skipping invalid TB Input " << _sld_record.getTBInput()
                                                               << " from FED " << _dcc_link.getFED();
                            stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_invalid_link_, _dcc_link.getFED());
                            continue;
                        }
                        _has_link = true;
                        _dcc_link.setDCCInput(_sld_record.getDCCInput());
                        _dcc_link.setTBInput(_sld_record.getTBInput());
                        _dcc_link_it = _dcc_link_map.find(_dcc_link);
                    }
                } else if (_record_type == rpcdcc::Record::cd_type_) {
                    LogDebug("RPCDCCRawToDigi") << "CD: " << std::hex << *_record << std::dec;
                    stream_data_.record_fed_->Fill(RPCDCCRawToDigiData::dcc_record_cd_, _dcc_link.getFED());
                    if (_has_bx && _has_link) {
                        stream_data_.cd_fed_dcci_tbi_->at(_dcc_link.getFED())->Fill(_dcc_link.getDCCInput(), _dcc_link.getTBInput());
                        rpcdcc::CDRecord _cd_record(*_record);
                        if (_cd_record.getConnector() > RPCLBLink::max_connector_) {
                            LogDebug("RPCDCCRawToDigi") << "Skipping invalid Connector " << _cd_record.getConnector()
                                                        << " for CD " << std::hex << *_record << std::dec
                                                        << " from " << _dcc_link;
                            stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_invalid_connector_, _dcc_link.getFED());
                            stream_data_.invalid_fed_dcci_tbi_->at(_dcc_link.getFED())->Fill(_dcc_link.getDCCInput(), _dcc_link.getTBInput());
                            continue;
                        }
                        if (_cd_record.isEOD()) {
                            LogDebug("RPCDCCRawToDigi") << "EOD from " << _dcc_link;
                            stream_data_.eod_fed_dcci_tbi_->at(_dcc_link.getFED())->Fill(_dcc_link.getDCCInput(), _dcc_link.getTBInput());
                        }
                        // Add the digis
                        if (_dcc_link_it != _dcc_link_map.end()) {
                            RPCLBLink _lb_link = _dcc_link_it->second;
                            _lb_link.setLinkBoard(_cd_record.getLinkBoard());
                            _lb_link.setConnector(_cd_record.getConnector());
                            RPCLBLinkMap::map_type::const_iterator _lb_link_it = _lb_link_map.find(_lb_link);
                            if (_lb_link_it != _lb_link_map.end()) {
                                RPCFebConnector const & _feb_connector(_lb_link_it->second);
                                RPCDetId _det_id(_feb_connector.getRPCDetId());
                                unsigned int _pin_offset(_cd_record.getPartition() ? 9 : 1); // 1-16
                                ::uint8_t _data(_cd_record.getData());
                                for (unsigned int _pin = 0 ; _pin < 8 ; ++_pin)
                                    if (_data & (0x1 << _pin)) {
                                        unsigned int _strip(_feb_connector.getStrip(_pin + _pin_offset));
                                        if (_strip)
                                            _digis.insert(std::pair<RPCDetId, RPCDigi>(_det_id, RPCDigi(_strip, _bx)));
                                    }
                            } else {
                                LogDebug("RPCDCCRawToDigi") << "Skipping unknown Link " << _lb_link;
                                stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_unknown_link_, _dcc_link.getFED());
                                stream_data_.invalid_fed_dcci_tbi_->at(_dcc_link.getFED())->Fill(_dcc_link.getDCCInput(), _dcc_link.getTBInput());
                            }
                        } else {
                            LogDebug("RPCDCCRawToDigi") << "Skipping unknown DCCLink " << _dcc_link;
                            stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_unknown_link_, _dcc_link.getFED());
                            stream_data_.invalid_fed_dcci_tbi_->at(_dcc_link.getFED())->Fill(_dcc_link.getDCCInput(), _dcc_link.getTBInput());
                        }
                    } else {
                        if (!_has_bx)
                            stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_missing_bx_, _dcc_link.getFED());
                        if (!_has_link)
                            stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_missing_link_, _dcc_link.getFED());
                        LogDebug("RPCDCCRawToDigi") << "Skipping Chamber Data, missing BX or Link "
                                                    << std::hex << *_record << std::dec
                                                    << " from " << _dcc_link;
                    }
                } else if (_record_type == rpcdcc::Record::empty_type_) {
                    stream_data_.record_fed_->Fill(RPCDCCRawToDigiData::dcc_record_empty_, _dcc_link.getFED());
                    // do nothing
                } else if (_record_type == rpcdcc::Record::rddm_type_) {
                    stream_data_.record_fed_->Fill(RPCDCCRawToDigiData::dcc_record_rddm_, _dcc_link.getFED());
                    rpcdcc::RDDMRecord _rddm_record(*_record);
                    if (_rddm_record.getDCCInput() > RPCDCCLink::max_dccinput_) {
                        edm::LogWarning("RPCDCCRawToDigi") << "Skipping invalid DCC Input " << _rddm_record.getDCCInput()
                                                           << " from FED " << _dcc_link.getFED()
                                                           << " in RDDM record";
                        stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_invalid_rddm_link_, _dcc_link.getFED());
                        continue;
                    }
                    if (_rddm_record.getTBInput() > RPCDCCLink::max_tbinput_) {
                        edm::LogWarning("RPCDCCRawToDigi") << "Skipping invalid TB Input " << _rddm_record.getTBInput()
                                                           << " from FED" << _dcc_link.getFED()
                                                           << " in RDDM record";
                        stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_invalid_rddm_link_, _dcc_link.getFED());
                        continue;
                    }
                    RPCDCCLink _temp_dcc_link(_dcc_link);
                    _temp_dcc_link.setDCCInput(_rddm_record.getDCCInput());
                    _temp_dcc_link.setTBInput(_rddm_record.getTBInput());
                    LogDebug("RPCDCCRawToDigi") << "RMB Discarded Data Marker for " << _temp_dcc_link;
                    stream_data_.rddm_fed_dcci_tbi_->at(_dcc_link.getFED())->Fill(_temp_dcc_link.getDCCInput(), _temp_dcc_link.getTBInput());
                } else if (_record_type == rpcdcc::Record::sddm_type_) {
                    LogDebug("RPCDCCRawToDigi") << "S-Link Discarded Data Marker for " << _dcc_link;
                    stream_data_.record_fed_->Fill(RPCDCCRawToDigiData::dcc_record_sddm_, _dcc_link.getFED());
                } else if (_record_type == rpcdcc::Record::rcdm_type_) {
                    stream_data_.record_fed_->Fill(RPCDCCRawToDigiData::dcc_record_rcdm_, _dcc_link.getFED());
                    rpcdcc::RCDMRecord _rcdm_record(*_record);
                    if (_rcdm_record.getDCCInput() > RPCDCCLink::max_dccinput_) {
                        edm::LogWarning("RPCDCCRawToDigi") << "Skipping invalid DCC Input " << _rcdm_record.getDCCInput()
                                                           << " from FED " << _dcc_link.getFED()
                                                           << " in RCDM record";
                        stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_invalid_rcdm_link_, _dcc_link.getFED());
                        continue;
                    }
                    if (_rcdm_record.getTBInput() > RPCDCCLink::max_tbinput_) {
                        edm::LogWarning("RPCDCCRawToDigi") << "Skipping invalid TB Input " << _rcdm_record.getTBInput()
                                                           << " from FED " << _dcc_link.getFED()
                                                           << " in RCDM record";
                        stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_invalid_rcdm_link_, _dcc_link.getFED());
                        continue;
                    }
                    RPCDCCLink _temp_dcc_link(_dcc_link);
                    _temp_dcc_link.setDCCInput(_rcdm_record.getDCCInput());
                    _temp_dcc_link.setTBInput(_rcdm_record.getTBInput());
                    LogDebug("RPCDCCRawToDigi") << "RMB Corrupted Data Marker for " << _temp_dcc_link;
                    stream_data_.rcdm_fed_dcci_tbi_->at(_dcc_link.getFED())->Fill(_temp_dcc_link.getDCCInput(), _temp_dcc_link.getTBInput());
                } else if (_record_type == rpcdcc::Record::rdm_type_) {
                    rpcdcc::RDMRecord _rdm_record(*_record);
                    LogDebug("RPCDCCRawToDigi") << "RMB Disabled Marker for " << _dcc_link
                                                << " input " << _rdm_record.getDCCInput();
                    stream_data_.record_fed_->Fill(RPCDCCRawToDigiData::dcc_record_rdm_, _dcc_link.getFED());
                } else {
                    LogDebug("RPCDCCRawToDigi") << "Unknown Record type for record "
                                                << std::hex << *_record << std::dec
                                                << " from " << _dcc_link;;
                    stream_data_.record_fed_->Fill(RPCDCCRawToDigiData::dcc_record_unknown_, _dcc_link.getFED());
                }
            }
        }

        // Complete CRC check
        if (calculate_crc_) {
            _word_end = reinterpret_cast<::uint64_t const *>(_raw_data.data()) + _nwords - 1;
            for ( ; _word < _word_end ; ++_word)
                compute_crc_64bit(_crc, *_word);
            compute_crc_64bit(_crc, (*_word & 0xffffffff0000ffff)); // trailer excluding crc
            FEDTrailer _trailer(reinterpret_cast<unsigned char const *>(_word_end));
            if ((unsigned int)(_trailer.crc()) != _crc) {
                edm::LogWarning("RPCDCCRawToDigi") << "FED Trailer CRC doesn't match for FED id " << *_fed;
                stream_data_.error_fed_->Fill(RPCDCCRawToDigiData::dcc_error_trailer_crc_, _dcc_link.getFED());
            }
        }
    }

    // Create collection without duplicates
    std::unique_ptr<RPCDigiCollection> _digi_collection(new RPCDigiCollection());
    RPCDetId _det_id;
    std::vector<RPCDigi> _local_digis;
    for (std::set<std::pair<RPCDetId, RPCDigi> >::const_iterator _digi = _digis.begin()
             ; _digi != _digis.end() ; ++_digi) {
        LogDebug("RPCDCCRawToDigi") << "RPCDigi " << _digi->first.rawId() << ", " << _digi->second.strip() << ", " << _digi->second.bx();
        if (_digi->first != _det_id) {
            if (!_local_digis.empty()) {
                _digi_collection->put(RPCDigiCollection::Range(_local_digis.begin(), _local_digis.end()), _det_id);
                _local_digis.clear();
            }
            _det_id = _digi->first;
        }
        _local_digis.push_back(_digi->second);
    }
    if (!_local_digis.empty())
        _digi_collection->put(RPCDigiCollection::Range(_local_digis.begin(), _local_digis.end()), _det_id);

    _event.put(std::move(_digi_collection));
}

void RPCDCCRawToDigi::endRunSummary(edm::Run const & _run, edm::EventSetup const & _setup
                                    , RPCDCCRawToDigiData * _run_data) const
{
    _run_data->record_fed_.mergeProduct(stream_data_.record_fed_);
    _run_data->error_fed_.mergeProduct(stream_data_.error_fed_);
    _run_data->cd_fed_dcci_tbi_.mergeProduct(stream_data_.cd_fed_dcci_tbi_);
    _run_data->invalid_fed_dcci_tbi_.mergeProduct(stream_data_.invalid_fed_dcci_tbi_);
    _run_data->eod_fed_dcci_tbi_.mergeProduct(stream_data_.eod_fed_dcci_tbi_);
    _run_data->rddm_fed_dcci_tbi_.mergeProduct(stream_data_.rddm_fed_dcci_tbi_);
    _run_data->rcdm_fed_dcci_tbi_.mergeProduct(stream_data_.rcdm_fed_dcci_tbi_);
}

void RPCDCCRawToDigi::globalEndRunProduce(edm::Run & _run, edm::EventSetup const & _setup
                                          , RunContext const * _context
                                          , RPCDCCRawToDigiData const * _run_data)
{
    std::unique_ptr<MergeTH2D> _record_fed(new MergeTH2D(_run_data->record_fed_));
    std::unique_ptr<MergeTH2D> _error_fed(new MergeTH2D(_run_data->error_fed_));
    std::unique_ptr<MergeMap<::uint32_t, MergeTH2D> > _cd_fed_dcci_tbi(new MergeMap<::uint32_t, MergeTH2D>(_run_data->cd_fed_dcci_tbi_));
    std::unique_ptr<MergeMap<::uint32_t, MergeTH2D> > _invalid_fed_dcci_tbi(new MergeMap<::uint32_t, MergeTH2D>(_run_data->invalid_fed_dcci_tbi_));
    std::unique_ptr<MergeMap<::uint32_t, MergeTH2D> > _eod_fed_dcci_tbi(new MergeMap<::uint32_t, MergeTH2D>(_run_data->eod_fed_dcci_tbi_));
    std::unique_ptr<MergeMap<::uint32_t, MergeTH2D> > _rddm_fed_dcci_tbi(new MergeMap<::uint32_t, MergeTH2D>(_run_data->rddm_fed_dcci_tbi_));
    std::unique_ptr<MergeMap<::uint32_t, MergeTH2D> > _rcdm_fed_dcci_tbi(new MergeMap<::uint32_t, MergeTH2D>(_run_data->rcdm_fed_dcci_tbi_));

    _run.put(std::move(_record_fed), "RecordFed");
    _run.put(std::move(_error_fed), "ErrorFed");
    _run.put(std::move(_cd_fed_dcci_tbi), "CdFedDcciTbi");
    _run.put(std::move(_invalid_fed_dcci_tbi), "InvalidFedDcciTbi");
    _run.put(std::move(_eod_fed_dcci_tbi), "EodFedDcciTbi");
    _run.put(std::move(_rddm_fed_dcci_tbi), "RddmFedDcciTbi");
    _run.put(std::move(_rcdm_fed_dcci_tbi), "RcdmFedDcciTbi");
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(RPCDCCRawToDigi);
