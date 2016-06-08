#include "DPGAnalysis/RPC/interface/RPCMaskDetId.h"

#include <ostream>
#include <iomanip>
#include <sstream>

#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DPGAnalysis/RPC/interface/RPCIdType.h"

RPCMaskDetId::RPCMaskDetId()
    : id_(0x0)
{
    setDetType().setSubDetType().setRPCIdType();
}

RPCMaskDetId::RPCMaskDetId(::uint32_t const & _id)
    : id_(_id)
{
    if (getDetType() != DetId::Muon || getSubDetType() != MuonSubdetId::RPC)
        throw cms::Exception("InvalidDetId")
            << "Invalid id in RPCMaskDetId ctor:"
            << " DetType " << getDetType() << ", SubDetType " << getSubDetType();
    if (getRPCIdType() == wildcard_) { // re-initialize
        setRPCIdType();
        set(RPCDetId(_id));
    } else if (getRPCIdType() != RPCIdType::RPCMaskDetId)
        throw cms::Exception("InvalidDetId")
            << "Invalid id in RPCMaskDetId ctor:"
            << " DetType " << getDetType() << ", SubDetType " << getSubDetType()
            << ", RPCIdType " << getRPCIdType();
}

RPCMaskDetId::RPCMaskDetId(DetId const & _detid)
    : id_(_detid.rawId())
{
    if (getDetType() != DetId::Muon || getSubDetType() != MuonSubdetId::RPC)
        throw cms::Exception("InvalidDetId")
            << "Invalid id in RPCMaskDetId ctor:"
            << " DetType " << getDetType() << ", SubDetType " << getSubDetType();
    if (getRPCIdType() == wildcard_) { // re-initialize
        setRPCIdType();
        set(RPCDetId(_detid));
    } else if (getRPCIdType() != RPCIdType::RPCMaskDetId)
        throw cms::Exception("InvalidDetId")
            << "Invalid id in RPCMaskDetId ctor:"
            << " DetType " << getDetType() << ", SubDetType " << getSubDetType()
            << ", RPCIdType " << getRPCIdType();
}

RPCMaskDetId::RPCMaskDetId(RPCDetId const & _rpcdetid)
    : id_(_rpcdetid.rawId())
{
    setRPCIdType();
    set(_rpcdetid);
}

RPCMaskDetId::RPCMaskDetId(int _region
                           , int _station
                           , int _layer
                           , int _ring
                           , int _sector
                           , int _subsector
                           , int _subsubsector
                           , int _roll
                           , int _gap)
    : id_(0x0)
{
    setDetType();
    setSubDetType();
    setRPCIdType();
    setRegion(_region);
    setStation(_station);
    setLayer(_layer);
    setRing(_ring);
    setSector(_sector);
    setSubSector(_subsector);
    setSubSubSector(_subsubsector);
    setRoll(_roll);
    setGap(_gap);
}

void RPCMaskDetId::set(RPCDetId const & _rpcdetid)
{
    reset();

    int _region(_rpcdetid.region());
    int _station(_rpcdetid.station());
    int _ring(_rpcdetid.ring());
    int _sector(_rpcdetid.sector());
    int _subsector(_rpcdetid.subsector());
    int _roll(_rpcdetid.roll());

    setRegion(_region);
    setStation(_station);
    setLayer(_rpcdetid.layer());
    setRing(_ring);
    setSector(_sector);
    setRoll(_roll > 0 ? _roll : wildcard_);

    if (_region == 0) {
        if (_station == 4 && _sector == 4) { // 1, 2, 3, 4 to 1.1, 1.0, 2.0, 2.2
            setSubSector(_subsector > 2 ? 2 : 1);
            if (_subsector == 2 || _subsector == 3)
                setSubSubSector(0);
            else if (_subsector == 1)
                setSubSubSector(1);
            else
                setSubSubSector(2);
        } else if (_station == 3
                   || (_station == 4 && _sector != 9 && _sector != 11)) { // 1, 2 to 1.0, 2.0
            setSubSector(_subsector);
            setSubSubSector(0);
        } else { // 1 to 0.0
            setSubSector(0);
            setSubSubSector(0);
        }
    } else if (_station != 1 && _ring == 1) { // 1, 2, 3 to 0.0, 1.0, 2.0
        setSubSector(_subsector - 1);
        setSubSubSector(0);
    } else { // 1..6 to 0.0, 0.1, 1.0, 1.1, 2.0, 2.1
        setSubSector((_subsector - 1) / 2);
        setSubSubSector((_subsector - 1) % 2);
    }
}

::uint32_t RPCMaskDetId::getMask() const
{
    ::uint32_t _mask = (mask_dettype_ | mask_subdettype_ | mask_rpcidtype_);
    if (id_ & mask_region_)
        _mask |= mask_region_;
    if (id_ & mask_station_)
        _mask |= mask_station_;
    if (id_ & mask_layer_)
        _mask |= mask_layer_;
    if (id_ & mask_ring_)
        _mask |= mask_ring_;
    if (id_ & mask_sector_)
        _mask |= mask_sector_;
    if (id_ & mask_subsector_)
        _mask |= mask_subsector_;
    if (id_ & mask_subsubsector_)
        _mask |= mask_subsubsector_;
    if (id_ & mask_roll_)
        _mask |= mask_roll_;
    if (id_ & mask_gap_)
        _mask |= mask_gap_;
    return _mask;
}

RPCDetId RPCMaskDetId::getRPCDetId() const
{
    int _region(getRegion())
        , _station(getStation())
        , _layer(getLayer())
        , _ring(getRing())
        , _sector(getSector())
        , _subsector(getSubSector())
        , _subsubsector(getSubSubSector())
        , _roll(getRoll());
    if (_region == wildcard_
        || _station == wildcard_
        || _layer == wildcard_
        || _ring == wildcard_
        || _sector == wildcard_
        || _subsector == wildcard_
        || _subsubsector == wildcard_) // is it a fully specified RPCDetId?
        return RPCDetId();

    if (_region == 0) {
        if (_sector == 4 && _station == 4) {
            if (_subsubsector == 0)
                ++_subsector;
            else
                _subsector *= _subsector;
        } else if (_subsector == 0)
            _subsector = 1;
    } else if (_station != 1 && _ring == 1)
        ++_subsector;
    else
        _subsector = _subsector * 2 + _subsubsector + 1;

    return RPCDetId(_region
                    , _ring
                    , _station
                    , _sector
                    , _layer
                    , _subsector
                    , (_roll == wildcard_ ? 0 : _roll) // 0 is the wildcard for a roll in RPCDetId
                    );
}

std::string RPCMaskDetId::getName() const
{
    // region.station.layer/ring/sector.subsector.subsubsector:roll_gap = chamber(r, eta, phi)/(inside chamber)
    // R*.*/(R,W)*/(C,S)*.*.*
    // RE-1/R2/C09:A, RB1in/W-2/S07+:fwd

    std::ostringstream _oss;
    _oss.fill('0');

    ::uint32_t _remaining = id_ & ~(mask_dettype_ | mask_subdettype_ | mask_rpcidtype_);

    if (!_remaining) {
        _oss << "R*";
        return _oss.str();
    }

    int _region(getRegion())
        , _station(getStation())
        , _layer(getLayer())
        , _ring(getRing())
        , _sector(getSector())
        , _subsector(getSubSector())
        , _subsubsector(getSubSubSector())
        , _roll(getRoll())
        , _gap(getGap());


    bool _previous(false);

    // pre-handle remaining for region-station-layer
    _remaining &= ~(mask_region_ | mask_station_ | mask_layer_);

    if (_region == 0 && _subsector == 0 && _subsubsector == 0)
        _remaining &= ~(mask_subsector_ | mask_subsubsector_);

    if (_station != wildcard_ || _layer != wildcard_ || (_region != wildcard_ && (std::abs(_region) == 1 || _ring == wildcard_))) {
        _oss << 'R';

        // RB1in, RE-1
        switch (_region) {
        case -1: _oss << "E-"; break;
        case 0:  _oss << "B";  break;
        case 1:  _oss << "E+"; break;
        default: _oss << '*';  break;
        }

        switch (_station) {
        case wildcard_: (_region == wildcard_ ? _oss : _oss << '*'); break;
        default:        _oss << _station; break;
        }

        if (_station < 3 && (_region == 0 || _region == wildcard_))
            switch (_layer) {
            case 1: _oss << "in";  break;
            case 2: _oss << "out"; break;
            } else if (_layer > 1)
            _oss << ".?";

        if (!_remaining)
            return _oss.str();
        _previous = true;
    }

    // W-2, R2
    if (_ring != wildcard_) {
        if (_previous)
            _oss << '/';
        switch (_region) {
        case 0:  _oss << 'W';  break;
        case -1:
        case 1:  _oss << 'R'; break;
        }
        if (_region == 0 && _ring < 3)
            _oss << std::showpos << _ring << std::noshowpos;
        else if (std::abs(_region) == 1 && _ring > 0)
            _oss << _ring;
        else if (_region == wildcard_)
            _oss << _ring;
        else
            _oss << '?';
        _remaining &= ~mask_ring_;
        _previous = true;
    }

    if (!_remaining)
        return _oss.str();

    if ((_remaining & mask_roll_) != _remaining) {
        if (_previous)
            _oss << '/';

        // S07+, C09
        if (_region == 0) {
            _oss << 'S';
            if (_sector != wildcard_)
                _oss << std::setw(2) << _sector;
            else
                _oss << '*';
            if (_subsector != wildcard_ || _subsubsector != wildcard_) {
                switch (_subsector) {
                case wildcard_: _oss << "*"; break;
                case 2: _oss << '+'; break;
                case 1: _oss << '-'; break;
                case 0:
                    if (_subsubsector != 0)
                        _oss << "?";
                    break;
                }
                if (_subsector != 0)
                    switch (_subsubsector) {
                    case wildcard_: _oss << "*"; break;
                    case 2: _oss << '+'; break;
                    case 1: _oss << '-'; break;
                    }
            }
        } else if (_region != wildcard_ // endcap
                   && (_sector != wildcard_
                       && _subsector != wildcard_
                       && (_subsubsector != wildcard_ || (_station != 1 && _ring == 1))
                       )
                   ) {
            if (_station != 1 && _ring == 1)
                _oss << 'C' << std::setw(2) << ((_sector - 1) * 3 + _subsector + 1);
            else
                _oss << 'C' << std::setw(2)
                     << ((_sector - 1) * 6 + _subsector * 2 + _subsubsector + 1);
        } else {
            _oss << 'C';
            if (_sector == wildcard_)
                _oss << '*';
            else
                _oss << std::setw(2) << _sector;
            if (_subsector == wildcard_) {
                if (_sector != wildcard_ || _subsubsector != wildcard_)
                    _oss << ".*";
            } else
                _oss << '.' << _subsector;
            if (_subsubsector == wildcard_) {
                if (_sector != wildcard_ || _subsector != wildcard_)
                    _oss << ".*";
            } else
                _oss << '.' << _subsubsector;
        }
        _remaining &= ~(mask_sector_ | mask_subsector_ | mask_subsubsector_);

        if (!_remaining)
            return _oss.str();
    }

    // roll
    if (_roll != wildcard_) {
        if (_region == wildcard_)
            _oss << ':' << _roll;
        else if (_region == 0)
            switch (_roll) {
            case 1:  _oss << ":bwd"; break;
            case 2:  _oss << ":mid"; break;
            case 3:  _oss << ":fwd"; break;
            default: _oss << ":?";   break;
            }
        else
            switch (_roll) {
            case 1:  _oss << ":A"; break;
            case 2:  _oss << ":B"; break;
            case 3:  _oss << ":C"; break;
            case 4:  _oss << ":D"; break;
            case 5:  _oss << ":E"; break;
            default: _oss << ":?"; break;
            }
    }

    if (_gap != wildcard_) {
        if (std::abs(_region) == 1)
            switch (_gap) {
            case 1: _oss << "_BOT"; break;
            case 2: _oss << "_TOP"; break;
            }
        else
            switch (_gap) {
            case 1: _oss << "_DW"; break;
            case 2: _oss << "_UP"; break;
            }

    }

    return _oss.str();
}

std::ostream & operator<<(std::ostream & _ostream, RPCMaskDetId const & _rpccompdetid)
{
    return _ostream << _rpccompdetid.getName();
}
