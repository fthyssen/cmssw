#include "DPGAnalysis/RPC/interface/RPCMaskParser.h"

#include <string>
#include <sstream>

RPCMaskParser::RPCMaskParser()
{}

RPCMaskDetId RPCMaskParser::parse(std::string const & _name)
{
    RPCMaskDetId _id;
    std::string::size_type _pos(0), _size(_name.size());

    bool _rsl = parseRegionStationLayer(_name, _pos, _size, _id);
    parseRing(_name, _pos, _size, _id);
    if (!_rsl)
        parseRegionStationLayer(_name, _pos, _size, _id);
    parseSectorSubSectorSubSubSector(_name, _pos, _size, _id);
    parseRoll(_name, _pos, _size, _id);
    parseGap(_name, _pos, _size, _id);

    return _id;
}

bool RPCMaskParser::parseRegionStationLayer(std::string const & _name
                                            , std::string::size_type & _pos, std::string::size_type _size
                                            , RPCMaskDetId & _id)
{
    if (_size - _pos < 2 || _name.at(_pos) != 'R')
        return false;

    int _region(RPCMaskDetId::wildcard_), _station(RPCMaskDetId::wildcard_);
    // region
    switch (_name.at(_pos + 1)) {
    case 'B':
        _pos += 2; _id.setRegion(_region = 0); break;
    case 'E':
        _pos += 2;
        _id.setLayer(1);
        if (_pos == _size)
            return true;
        switch (_name.at(_pos++)) {
        case '+': case 'P':
            _id.setRegion(_region = 1); break;
        case '-': case 'M': case 'N':
            _id.setRegion(_region = -1); break;
        case '*':
            --_pos; break;
        case '/':
            return true; break;
        case ':': case '_':
            --_pos; return true; break;
        default:
            throw cms::Exception("InvalidRPCName") << "Invalid RPC name for parsing: " << _name
                                                   << "; stopped after " << _name.substr(0, _pos)
                                                   << " (" << _id << ") ";
        }
        break;
    default:
        return false; break;
    }

    // station
    if (_pos == _size)
        return true;

    switch (_name.at(_pos++)) {
    case '1': case '2': case '3': case '4':
        _id.setStation(_station = (_name.at(_pos - 1) - '0'));
    case '*':
        break;
    case '/':
        return true; break;
    case ':': case '_':
        --_pos; return true; break;
    default:
        throw cms::Exception("InvalidRPCName") << "Invalid RPC name for parsing: " << _name
                                               << "; stopped after " << _name.substr(0, _pos)
                                               << " (" << _id << ") ";
    }

    if (_pos == _size)
        return true;

    // layer
    if (std::abs(_region) == 1 || (_station != RPCMaskDetId::wildcard_ && _station > 2))
        _id.setLayer(1);
    switch (_name.at(_pos++)) {
    case 'i':
        _id.setLayer(1); _pos += 1; break;
    case 'o':
        _id.setLayer(2); _pos += 2; break;
    case '*':
        _id.setLayer(RPCMaskDetId::wildcard_);
    case '/':
        return true; break;
    case ':': case '_':
        --_pos; return true; break;
    default:
        throw cms::Exception("InvalidRPCName") << "Invalid RPC name for parsing: " << _name
                                               << "; stopped after " << _name.substr(0, _pos)
                                               << " (" << _id << ") ";
    }

    if (_pos == _size)
        return true;

    if (_name.at(_pos) == '/')
        ++_pos;

    return true;
}

bool RPCMaskParser::parseRing(std::string const & _name
                              , std::string::size_type & _pos, std::string::size_type _size
                              , RPCMaskDetId & _id)
{
    if (_size - _pos < 2)
        return false;

    int _sign(1);

    switch (_name.at(_pos++)) {
    case 'W':
        _id.setRegion(0);
        switch (_name.at(_pos)) {
        case '+': case 'P':
            ++_pos; break;
        case '-': case 'M': case 'N':
            ++_pos; _sign = -1; break;
        }
        switch (_name.at(_pos++)) {
        case '0': case '1': case '2':
            _id.setRing(_sign * (_name.at(_pos - 1) - '0')); break;
        case '*': case '/':
            return true; break;
        case ':': case '_':
            --_pos; return true; break;
        default:
            throw cms::Exception("InvalidRPCName") << "Invalid RPC name for parsing: " << _name
                                                   << "; stopped after " << _name.substr(0, _pos)
                                                   << " (" << _id << ") ";
        }
        break;
    case 'R':
        switch (_name.at(_pos++)) {
        case '1': case '2': case '3':
            _id.setRing(_name.at(_pos - 1) - '0'); break;
        case '*': case '/':
            return true; break;
        case ':': case '_':
            --_pos; return true; break;
        default:
            throw cms::Exception("InvalidRPCName") << "Invalid RPC name for parsing: " << _name
                                                   << "; stopped after " << _name.substr(0, _pos)
                                                   << " (" << _id << ") ";
        }
        break;
    default:
        --_pos; return false; break;
    }

    if (_pos == _size)
        return true;

    if (_name.at(_pos) == '/')
        ++_pos;

    return true;
}

bool RPCMaskParser::parseSectorSubSectorSubSubSector(std::string const & _name
                                                     , std::string::size_type & _pos, std::string::size_type _size
                                                     , RPCMaskDetId & _id)
{
    if (_size - _pos < 2)
        return false;

    std::stringstream _parser;
    int _sector(0);

    switch (_name.at(_pos++)) {
    case 'S':
        _id.setRegion(0);
        if (_name.at(_pos) == '*')
            ++_pos;
        else if (_name.at(_pos) >= '0' && _name.at(_pos) <= '9') {
            _parser << _name.at(_pos++);
            if (_pos < _size && _name.at(_pos) >= '0' && _name.at(_pos) <= '9')
                _parser << _name.at(_pos++);
            _parser >> _sector;
            _id.setSector(_sector);
        }

        _id.setSubSector(0); _id.setSubSubSector(0);

        if (_pos == _size)
            return true;

        switch (_name.at(_pos++)) {
        case '+':
            _id.setSubSector(2); _id.setSubSubSector(0); break;
        case '-':
            _id.setSubSector(1); _id.setSubSubSector(0); break;
        case '*':
            _id.setSubSector(RPCMaskDetId::wildcard_); _id.setSubSubSector(RPCMaskDetId::wildcard_);
            return true; break;
        case ':': case '_':
            --_pos; return true; break;
        default:
            throw cms::Exception("InvalidRPCName") << "Invalid RPC name for parsing: " << _name
                                                   << "; stopped after " << _name.substr(0, _pos)
                                                   << " (" << _id << ") ";
        }

        if (_pos == _size)
            return true;

        switch (_name.at(_pos++)) {
        case '+':
            _id.setSubSubSector(2); break;
        case '-':
            _id.setSubSubSector(1); break;
        case '*':
            _id.setSubSubSector(RPCMaskDetId::wildcard_);
            return true; break;
        case ':': case '_':
            --_pos; return true; break;
        default:
            throw cms::Exception("InvalidRPCName") << "Invalid RPC name for parsing: " << _name
                                                   << "; stopped after " << _name.substr(0, _pos)
                                                   << " (" << _id << ") ";
        }
        break;
    case 'C':
        if (_name.at(_pos) == '*')
            ++_pos;
        else if (_name.at(_pos) >= '0' && _name.at(_pos) <= '9') {
            _parser << _name.at(_pos++);
            if (_pos < _size && _name.at(_pos) >= '0' && _name.at(_pos) <= '9')
                _parser << _name.at(_pos++);
            _parser >> _sector;
            _id.setEndcapSector(_sector);
        }
        break;
    default:
        --_pos; return false; break;
    }

    return true;
}

bool RPCMaskParser::parseRoll(std::string const & _name
                              , std::string::size_type & _pos, std::string::size_type _size
                              , RPCMaskDetId & _id)
{
    if (_size - _pos < 2 || _name.at(_pos) != ':')
        return false;

    ++_pos;
    switch (_name.at(_pos++)) {
    case 'b': // bwd
        _pos += 2; _id.setRoll(1); break;
    case 'm': // mid
        _pos += 2; _id.setRoll(2); break;
    case 'f': // fwd
        _pos += 2; _id.setRoll(3); break;
    case 'A': case 'B':  case 'C': case 'D': case 'E':
        _id.setRoll(_name.at(_pos - 1) - 'A' + 1); break;
    case '*':
        return true; break;
    case '_':
        --_pos; return true; break;
    default:
        throw cms::Exception("InvalidRPCName") << "Invalid RPC name for parsing: " << _name
                                               << "; stopped after " << _name.substr(0, _pos)
                                               << " (" << _id << ") ";
    }

    return true;
}

bool RPCMaskParser::parseGap(std::string const & _name
                             , std::string::size_type & _pos, std::string::size_type _size
                             , RPCMaskDetId & _id)
{
    if (_size - _pos < 2 || _name.at(_pos) != '_')
        return false;

    ++_pos;
    switch (_name.at(_pos++)) {
    case 'T': // TOP
        ++_pos;
    case 'U': // UP
        ++_pos; _id.setGap(2); break;
    case 'B': // BOT
        ++_pos;
    case 'D': // DW
        ++_pos; _id.setGap(1); break;
    case '*':
        return true; break;
    default:
        throw cms::Exception("InvalidRPCName") << "Invalid RPC name for parsing: " << _name
                                               << "; stopped after " << _name.substr(0, _pos)
                                               << " (" << _id << ") ";
    }

    return true;
}
