#ifndef DPGAnalysis_RPC_RPCMaskParser_h
#define DPGAnalysis_RPC_RPCMaskParser_h

#include <string>

#include "DPGAnalysis/RPC/interface/RPCMaskDetId.h"

class RPCMaskParser
{
public:
    static RPCMaskDetId parse(std::string const & _name);

protected:
    RPCMaskParser();

    static bool parseRegionStationLayer(std::string const & _name
                                        , std::string::size_type & _pos, std::string::size_type _size
                                        , RPCMaskDetId & _id);
    static bool parseRing(std::string const & _name
                          , std::string::size_type & _pos, std::string::size_type _size
                          , RPCMaskDetId & _id);
    static bool parseSectorSubSectorSubSubSector(std::string const & _name
                                                 , std::string::size_type & _pos, std::string::size_type _size
                                                 , RPCMaskDetId & _id);
    static bool parseRoll(std::string const & _name
                          , std::string::size_type & _pos, std::string::size_type _size
                          , RPCMaskDetId & _id);
    static bool parseGap(std::string const & _name
                         , std::string::size_type & _pos, std::string::size_type _size
                         , RPCMaskDetId & _id);
};

#endif // DPGAnalysis_RPC_RPCMaskParser_h
