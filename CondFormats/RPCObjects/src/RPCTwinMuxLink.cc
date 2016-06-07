#include "CondFormats/RPCObjects/interface/RPCTwinMuxLink.h"

#include <ostream>
#include <sstream>

RPCTwinMuxLink::RPCTwinMuxLink()
    : id_(0x0)
{}

RPCTwinMuxLink::RPCTwinMuxLink(::uint32_t const & _id)
    : id_(_id)
{}

RPCTwinMuxLink::RPCTwinMuxLink(int _fed
                               , int _amcnumber
                               , int _tminput)
    : id_(0x0)
{
    setFED(_fed);
    setAMCNumber(_amcnumber);
    setTMInput(_tminput);
}

::uint32_t RPCTwinMuxLink::getMask() const
{
    ::uint32_t _mask(0x0);
    if (id_ & mask_fed_)
        _mask |= mask_fed_;
    if (id_ & mask_amcnumber_)
        _mask |= mask_amcnumber_;
    if (id_ & mask_tminput_)
        _mask |= mask_tminput_;
    return _mask;
}

std::string RPCTwinMuxLink::getName() const
{
    std::ostringstream _oss;
    _oss << "RPCTwinMuxLink_";
    bf_stream(_oss, min_fed_, mask_fed_, pos_fed_);
    bf_stream(_oss << '_', min_amcnumber_, mask_amcnumber_, pos_amcnumber_);
    bf_stream(_oss << '_', min_tminput_, mask_tminput_, pos_tminput_);
    return _oss.str();
}

std::ostream & operator<<(std::ostream & _ostream, RPCTwinMuxLink const & _link)
{
    return (_ostream << _link.getName());
}
