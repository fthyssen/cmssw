#ifndef CondFormats_RPCObjects_RPCTwinMuxLinkMap_h
#define CondFormats_RPCObjects_RPCTwinMuxLinkMap_h

#include <map>

#include "CondFormats/Serialization/interface/Serializable.h"

#include "CondFormats/RPCObjects/interface/RPCTwinMuxLink.h"
#include "CondFormats/RPCObjects/interface/RPCLBLink.h"

class RPCTwinMuxLinkMap
{
public:
    typedef std::map<RPCTwinMuxLink, RPCLBLink> map_type;

public:
    RPCTwinMuxLinkMap();

    map_type & getMap();
    map_type const & getMap() const;

protected:
    map_type map_;

    COND_SERIALIZABLE;
};

inline RPCTwinMuxLinkMap::map_type & RPCTwinMuxLinkMap::getMap()
{
    return map_;
}

inline RPCTwinMuxLinkMap::map_type const & RPCTwinMuxLinkMap::getMap() const
{
    return map_;
}

#endif // CondFormats_RPCObjects_RPCTwinMuxLinkMap_h
