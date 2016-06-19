#include "EventFilter/RPCRawToDigi/plugins/RPCTwinMuxRawToDigi.h"

#include <memory>

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/CRC16.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CondFormats/DataRecord/interface/RPCLBLinkMapRcd.h"
#include "CondFormats/RPCObjects/interface/RPCTwinMuxLink.h"
#include "DataFormats/FEDRawData/interface/FEDHeader.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"
#include "EventFilter/RPCRawToDigi/interface/RPCTwinMuxRecord.h"

RPCTwinMuxRawToDigi::RPCTwinMuxRawToDigi(edm::ParameterSet const & _config)
    : calculate_crc_(_config.getParameter<bool>("calculateCRC"))
    , bx_min_(_config.getParameter<int>("bxMin"))
    , bx_max_(_config.getParameter<int>("bxMax"))
{
    produces<RPCDigiCollection>();
    raw_token_ = consumes<FEDRawDataCollection>(_config.getParameter<edm::InputTag>("inputTag"));
}

RPCTwinMuxRawToDigi::~RPCTwinMuxRawToDigi()
{}

void RPCTwinMuxRawToDigi::compute_crc_64bit(::uint16_t & _crc, ::uint64_t const & _word)
{ // overcome constness problem evf::compute_crc_64bit
    unsigned char const * _uchars(reinterpret_cast<unsigned char const *>(&_word));
    for (unsigned char const * _uchar = _uchars + 7
             ; _uchar >= _uchars ; --_uchar)
        _crc = evf::compute_crc_8bit(_crc, *_uchar);
}

void RPCTwinMuxRawToDigi::fillDescriptions(edm::ConfigurationDescriptions & _descs)
{
    edm::ParameterSetDescription _desc;
    _desc.add<edm::InputTag>("inputTag", edm::InputTag("rawDataCollector", ""));
    _desc.add<bool>("calculateCRC", true);
    _desc.add<int>("bxMin", -2);
    _desc.add<int>("bxMax", 2);
    _descs.add("RPCTwinMuxRawToDigi", _desc);
}

void RPCTwinMuxRawToDigi::beginRun(edm::Run const & _run, edm::EventSetup const & _setup)
{
    if (es_rpc_tm_link_map_watcher_.check(_setup)) {
        _setup.get<RPCTwinMuxLinkMapRcd>().get(es_rpc_tm_link_map_);
        std::set<int> _feds;
        for (RPCTwinMuxLinkMap::map_type::const_iterator _tm_link = es_rpc_tm_link_map_->getMap().begin()
                 ; _tm_link != es_rpc_tm_link_map_->getMap().end() ; ++_tm_link)
            _feds.insert(_tm_link->first.getFED());
        feds_.assign(_feds.begin(), _feds.end());
    }
}

void RPCTwinMuxRawToDigi::produce(edm::Event & _event, edm::EventSetup const & _setup)
{
    // Get EventSetup Electronics Maps
    _setup.get<RPCTwinMuxLinkMapRcd>().get(es_rpc_tm_link_map_);
    _setup.get<RPCLBLinkMapRcd>().get(es_rpc_lb_link_map_);

    // Get RAW Data
    edm::Handle<FEDRawDataCollection> _raw_data_collection;
    _event.getByToken(raw_token_, _raw_data_collection);

    std::set<std::pair<RPCDetId, RPCDigi> > _rpc_digis;

    // Loop over the FEDs
    for (std::vector<int>::const_iterator _fed = feds_.begin()
             ; _fed != feds_.end() ; ++_fed) {

        ::uint16_t _crc(0xffff);

        FEDRawData const & _raw_data = _raw_data_collection->FEDData(*_fed);
        unsigned int _nwords(_raw_data.size() / sizeof(::uint64_t));
        if (!_nwords)
            continue;
        ::uint64_t const * _word(reinterpret_cast<::uint64_t const *>(_raw_data.data()));
        ::uint64_t const * _word_end = _word + _nwords;

        int _bx_id(0);

        LogDebug("RPCTwinMuxRawToDigi") << "Handling FED " << *_fed << " with length " << _nwords;

        // Handle the CDF Headers
        if (!processCDFHeaders(*_fed
                               , _word, _word_end
                               , _crc, _bx_id))
            continue;

        // Handle the CDF Trailers
        if (!processCDFTrailers(*_fed, _nwords
                                , _word, _word_end
                                , _crc))
            continue;

        // Loop over the Blocks
        while (_word < _word_end)
            processBlock(*_fed
                         , _word, _word_end
                         , _crc, _rpc_digis);

        // Complete CRC check with trailer
        if (calculate_crc_) {
            _word = _word_end;
            _word_end = reinterpret_cast<::uint64_t const *>(_raw_data.data()) + _nwords - 1;
            for ( ; _word < _word_end ; ++_word)
                compute_crc_64bit(_crc, *_word);
            compute_crc_64bit(_crc, (*_word & 0xffffffff0000ffff)); // trailer excluding crc
            FEDTrailer _trailer(reinterpret_cast<unsigned char const *>(_word_end));
            if ((unsigned int)(_trailer.crc()) != _crc) {
                edm::LogWarning("RPCTwinMuxRawToDigi") << "FED Trailer CRC doesn't match for FED id " << *_fed;
            }
        }
    }

    putRPCDigis(_event, _rpc_digis);
}

bool RPCTwinMuxRawToDigi::processCDFHeaders(int _fed
                                            , ::uint64_t const * & _word, ::uint64_t const * & _word_end
                                            , ::uint16_t & _crc
                                            , int & _bx_id) const
{
    bool _more_headers(true);
    for ( ; _word < _word_end && _more_headers ; ++_word) {
        if (calculate_crc_)
            compute_crc_64bit(_crc, *_word);

        LogDebug("RPCTwinMuxRawToDigi") << "CDF Header " << std::hex << *_word << std::dec;
        FEDHeader _header(reinterpret_cast<unsigned char const *>(_word));
        if (!_header.check()) {
            edm::LogWarning("RPCTwinMuxRawToDigi") << "FED Header check failed for FED id " << _fed;
            ++_word;
            break;
        }
        if (_header.sourceID() != _fed) {
            edm::LogWarning("RPCTwinMuxRawToDigi") << "FED Header Source ID " << _header.sourceID()
                                                   << " does not match requested FED id " << _fed;
        }

        _bx_id = _header.bxID();
        // _more_headers = _header.moreHeaders();
        _more_headers = false;
        if (_header.moreHeaders())
            edm::LogInfo("RPCTwinMuxRawToDigi") << "Expected more headers";
    }

    return !_more_headers;
}

bool RPCTwinMuxRawToDigi::processCDFTrailers(int _fed, unsigned int _nwords
                                             , ::uint64_t const * & _word, ::uint64_t const * & _word_end
                                             , ::uint16_t & _crc) const
{
    bool _more_trailers(true);
    for (--_word_end ; _word_end > _word && _more_trailers ; --_word_end) {
        FEDTrailer _trailer(reinterpret_cast<unsigned char const *>(_word_end));
        LogDebug("RPCTwinMuxRawToDigi") << "CDF Trailer " << std::hex << *_word_end << std::dec
                                        << ", length " << _trailer.lenght();
        if (!_trailer.check()) {
            edm::LogWarning("RPCTwinMuxRawToDigi") << "FED Trailer check failed for FED id " << _fed;
            --_word_end;
            break;
        }
        if (_trailer.lenght() != (int)_nwords) {
            edm::LogWarning("RPCTwinMuxRawToDigi") << "FED Trailer length " << _trailer.lenght()
                                                   << " does not match actual data size " << _nwords
                                                   << " for FED id " << _fed;
            --_word_end;
            break;
        }
        // _more_trailers = _trailer.moreTrailers();
        _more_trailers = false;
        if (_trailer.moreTrailers())
            edm::LogInfo("RPCTwinMuxRawToDigi") << "Expected more trailers";
    }

    ++_word_end;

    return !_more_trailers;
}

bool RPCTwinMuxRawToDigi::processBlock(int _fed
                                       , ::uint64_t const * & _word, ::uint64_t const * _word_end
                                       , ::uint16_t & _crc
                                       , std::set<std::pair<RPCDetId, RPCDigi> > & _digis) const
{
    // Block Header and Content
    rpctwinmux::BlockHeader _block_header(*_word);
    if (calculate_crc_)
        compute_crc_64bit(_crc, *_word);
    ++_word;

    unsigned int _n_amc(_block_header.getNAMC());
    if (_word + _n_amc + 1 >= _word_end) {
        edm::LogWarning("RPCTwinMuxRawToDigi") << "Block can not be complete for FED " << _fed;
        _word = _word_end;
        return false;
    }

    std::vector<std::pair<unsigned int, unsigned int> > _amc_size_map;
    for (unsigned int _amc = 0 ; _amc < _n_amc ; ++_amc) {
        LogDebug("RPCTwinMuxRawToDigi") << "Block AMC " << _amc;
        rpctwinmux::BlockAMCContent _amc_content(*_word);
        if (calculate_crc_)
            compute_crc_64bit(_crc, *_word);
        ++_word;

        _amc_size_map.push_back(std::make_pair(_amc_content.getAMCNumber(), _amc_content.getSize()));
        // to add: check BlockAMCContent errors
        // check blocknumber and amcnumber validity
    }

    for (std::vector<std::pair<unsigned int, unsigned int> >::const_iterator _amc_size = _amc_size_map.begin()
             ; _amc_size != _amc_size_map.end() ; ++_amc_size)
        processTwinMux(_fed, _amc_size->first, _amc_size->second
                       , _word, _word_end
                       , _crc
                       , _digis);

    if (_word < _word_end) {
        rpctwinmux::BlockTrailer _block_trailer(*_word);
        // check block number, event counter and bxid
        if (calculate_crc_)
            compute_crc_64bit(_crc, *_word);
        ++_word;
        return true;
    } else {
        return false;
    }
}

bool RPCTwinMuxRawToDigi::processTwinMux(int _fed, unsigned int _amc_number, unsigned int _size
                                         , ::uint64_t const * & _word, ::uint64_t const * _word_end
                                         , ::uint16_t & _crc
                                         , std::set<std::pair<RPCDetId, RPCDigi> > & _digis) const
{
    LogDebug("RPCTwinMuxRawToDigi") << "TwinMux AMC#" << _amc_number << ", size " << _size;
    if (!_size)
        return true;
    if (_word + _size >= _word_end || _size < 3) { // can not be complete
        edm::LogWarning("RPCTwinMuxRawToDigi") << "TwinMux Data can not be complete for FED " << _fed << " AMC #" << _amc_number;
        _word += _size;
        return false;
    }

    rpctwinmux::TwinMuxHeader _header(_word);
    if (calculate_crc_) {
        compute_crc_64bit(_crc, *_word); ++_word;
        compute_crc_64bit(_crc, *_word); ++_word;
    } else
        _word += 2;

    if (_amc_number != _header.getAMCNumber()) {
        edm::LogWarning("RPCTwinMuxRawToDigi") << "AMC Number inconsistent in TwinMuxHeader vs BlockAMCContent: " << _header.getAMCNumber()
                                               << " vs " << _amc_number;
        _word += (_size - 2);
        return false;
    }
    // also event counter, bx counter, data length, orbit number

    int _bx_min(bx_min_), _bx_max(bx_max_);
    if (_header.hasRPCBXWindow()) {
        _bx_min = std::max(_bx_min, _header.getRPCBXMin());
        _bx_max = std::min(_bx_max, _header.getRPCBXMax());
        LogDebug("RPCTwinMuxRawToDigi") << "BX range set to " << _bx_min << ", " << _bx_max;
    }

    bool _has_first_rpc(false);
    rpctwinmux::RPCRecord _rpc_record;
    for (unsigned int _word_count = 2 ; _word_count < (_size - 1) ; ++_word_count, ++_word) {
        if (calculate_crc_)
            compute_crc_64bit(_crc, *_word);
        unsigned int _type(rpctwinmux::TwinMuxRecord::getType(*_word));
        LogDebug("RPCTwinMuxRawToDigi") << "TwinMux data type " << std::hex << _type << std::dec;
        if (_type == rpctwinmux::TwinMuxRecord::rpc_first_type_) {
            if (_has_first_rpc)
                processRPCRecord(_fed, _amc_number, _rpc_record, _digis, _bx_min, _bx_max, 0, 1);
            _rpc_record.reset();
            _rpc_record.set(0, *_word);
            _has_first_rpc = true;
        } else if (_type == rpctwinmux::TwinMuxRecord::rpc_second_type_) {
            if (!_has_first_rpc) {
                edm::LogWarning("RPCTwinMuxRawToDigi") << "Received second RPC word without first";
                _rpc_record.set(1, *_word);
                processRPCRecord(_fed, _amc_number, _rpc_record, _digis, _bx_min, _bx_max, 2, 4);
                _has_first_rpc = false;
            } else {
                _rpc_record.set(1, *_word);
                processRPCRecord(_fed, _amc_number, _rpc_record, _digis, _bx_min, _bx_max, 0, 4);
                _has_first_rpc = false;
            }
        }
    }
    if (_has_first_rpc)
        processRPCRecord(_fed, _amc_number, _rpc_record, _digis, _bx_min, _bx_max, 0, 1);

    rpctwinmux::TwinMuxTrailer _trailer(*_word);
    LogDebug("RPCTwinMuxRawToDigi") << "TwinMux Trailer " << std::hex << *_word << std::dec;
    if (calculate_crc_)
        compute_crc_64bit(_crc, *_word);
    ++_word;
    // check datalength and eventcounter
    return true;
}

void RPCTwinMuxRawToDigi::processRPCRecord(int _fed, unsigned int _amc_number
                                           , rpctwinmux::RPCRecord const & _record
                                           , std::set<std::pair<RPCDetId, RPCDigi> > & _digis
                                           , int _bx_min, int _bx_max
                                           , unsigned int _link_min, unsigned int _link_max) const
{
    LogDebug("RPCTwinMuxRawToDigi") << "RPCRecord " << std::hex << _record.getRecord()[0] << ", " << _record.getRecord()[1] << std::dec << std::endl;
    int _bx_offset(_record.getBXOffset());
    RPCTwinMuxLink _tm_link(_fed, _amc_number);
    for (unsigned int _link = _link_min ; _link <= _link_max ; ++_link) {
        _tm_link.setTMInput(_link);
        rpctwinmux::RPCLinkRecord _link_record(_record.getRPCLinkRecord(_link));
        if (_link_record.isError()) {
            LogDebug("RPCTwinMuxRawToDigi") << "Link in error for " << _tm_link;
            continue;
        } else if (!_link_record.isAcknowledge()) {
            LogDebug("RPCTwinMuxRawToDigi") << "Link without acknowledge for " << _tm_link;
            continue;
        }
        if (!_link_record.getData())
            continue;

        RPCTwinMuxLinkMap::map_type::const_iterator _tm_link_it = es_rpc_tm_link_map_->getMap().find(_tm_link);
        if (_tm_link_it == es_rpc_tm_link_map_->getMap().end()) {
            edm::LogWarning("RPCTwinMuxRawToDigi") << "Skipping unknown TwinMuxLink " << _tm_link;
            continue;
        }

        RPCLBLink _lb_link = _tm_link_it->second;

        if (_link_record.getLinkBoard() > RPCLBLink::max_linkboard_) {
            edm::LogWarning("RPCTwinMuxRawToDigi") << "Skipping invalid LinkBoard " << _link_record.getLinkBoard()
                                                   << " for record " << _link << " (" << std::hex << _link_record.getRecord()
                                                   << " in " << _record.getRecord()[0] << ':' << _record.getRecord()[1] << std::dec
                                                   << " from " << _tm_link;
            continue;
        }

        if (_link_record.getConnector() > RPCLBLink::max_connector_) {
            edm::LogWarning("RPCTwinMuxRawToDigi") << "Skipping invalid Connector " << _link_record.getConnector()
                                                   << " for record " << _link << " (" << std::hex << _link_record.getRecord()
                                                   << " in " << _record.getRecord()[0] << ':' << _record.getRecord()[1] << std::dec
                                                   << ") from " << _tm_link;
            continue;
        }

        _lb_link.setLinkBoard(_link_record.getLinkBoard());
        _lb_link.setConnector(_link_record.getConnector());

        RPCLBLinkMap::map_type::const_iterator _lb_link_it = es_rpc_lb_link_map_->getMap().find(_lb_link);
        if (_lb_link_it == es_rpc_lb_link_map_->getMap().end()) {
            edm::LogWarning("RPCTwinMuxRawToDigi") << "Could not find " << _lb_link
                                                   << " for record " << _link << " (" << std::hex << _link_record.getRecord()
                                                   << " in " << _record.getRecord()[0] << ':' << _record.getRecord()[1] << std::dec
                                                   << ") from " << _tm_link;
            continue;
        }

        int _bx(_bx_offset - (int)(_link_record.getDelay()));
        LogDebug("RPCTwinMuxRawToDigi") << "RPC BX " << _bx << " for offset " << _bx_offset;

        if (_bx < _bx_min || _bx > _bx_max)
            continue;

        RPCFebConnector const & _feb_connector(_lb_link_it->second);
        RPCDetId _det_id(_feb_connector.getRPCDetId());
        unsigned int _pin_offset(_link_record.getPartition() ? 9 : 1); // 1-16
        ::uint8_t _data(_link_record.getData());

        for (unsigned int _pin = 0 ; _pin < 8 ; ++_pin)
            if (_data & (0x1 << _pin)) {
                unsigned int _strip(_feb_connector.getStrip(_pin + _pin_offset));
                if (_strip) {
                    _digis.insert(std::pair<RPCDetId, RPCDigi>(_det_id, RPCDigi(_strip, _bx)));
                    LogDebug("RPCTwinMuxRawToDigi") << "RPCDigi " << _det_id.rawId() << ", " << _strip << ", " << _bx;
                }
            }

        rpctwinmux::RPCBXRecord _bx_record(_record.getRPCBXRecord(_link));
        // check bc0 and bcn, logwarning
    }
}

void RPCTwinMuxRawToDigi::putRPCDigis(edm::Event & _event
                                      , std::set<std::pair<RPCDetId, RPCDigi> > const & _digis)
{
    std::unique_ptr<RPCDigiCollection> _rpc_digi_collection(new RPCDigiCollection());
    RPCDetId _rpc_det_id;
    std::vector<RPCDigi> _local_digis;
    for (std::set<std::pair<RPCDetId, RPCDigi> >::const_iterator _rpc_digi = _digis.begin()
             ; _rpc_digi != _digis.end() ; ++_rpc_digi) {
        LogDebug("RPCTwinMuxRawToDigi") << "RPCDigi " << _rpc_digi->first.rawId() << ", " << _rpc_digi->second.strip() << ", " << _rpc_digi->second.bx();
        if (_rpc_digi->first != _rpc_det_id) {
            if (!_local_digis.empty()) {
                _rpc_digi_collection->put(RPCDigiCollection::Range(_local_digis.begin(), _local_digis.end()), _rpc_det_id);
                _local_digis.clear();
            }
            _rpc_det_id = _rpc_digi->first;
        }
        _local_digis.push_back(_rpc_digi->second);
    }
    if (!_local_digis.empty())
        _rpc_digi_collection->put(RPCDigiCollection::Range(_local_digis.begin(), _local_digis.end()), _rpc_det_id);

    _event.put(std::move(_rpc_digi_collection));
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(RPCTwinMuxRawToDigi);
