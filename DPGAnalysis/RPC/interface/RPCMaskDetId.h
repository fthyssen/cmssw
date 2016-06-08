#ifndef DPGAnalysis_RPC_RPCMaskDetId_h
#define DPGAnalysis_RPC_RPCMaskDetId_h

#include <iosfwd>
#include <string>
#include <stdint.h>
#include <climits>

#include "DataFormats/MuonDetId/interface/RPCDetId.h"

class DetId;

class RPCMaskDetId
{
public:
    static int const wildcard_ = INT_MIN; ///< returned value if a field is not set

    /** @{ */
    /** field ranges */
    static int const min_dettype_ = 1; ///< 2 Muon
    static int const max_dettype_ = 15;
    static int const min_subdettype_ = 1; ///< 3 RPC
    static int const max_subdettype_ = 7;
    static int const min_rpcidtype_ = 1; ///< 0 RPCDetId, 1 RPCMaskDetId
    static int const max_rpcidtype_ = 3;
    static int const min_region_ = -1; ///< 0 brl, -1 RE-, 1 RE+
    static int const max_region_ = 1;
    static int const min_station_ = 1; ///< 1..4
    static int const max_station_ = 4;
    static int const min_layer_ = 1; ///< in, out
    static int const max_layer_ = 2;
    static int const min_ring_ = -2; ///< -2..2 brl, 1..3 fwd
    static int const max_ring_ = 3;
    static int const min_sector_ = 1; ///< 1..12 brl, 1..6 fwd
    static int const max_sector_ = 12;
    static int const min_subsector_ = 0; ///< "", -, +
    static int const max_subsector_ = 2;
    static int const min_subsubsector_ = 0; ///< "", -, +
    static int const max_subsubsector_ = 2;
    static int const min_roll_ = 1; ///< bwd/A, mid/B, fwd/C, D, E
    static int const max_roll_ = 5;
    static int const min_gap_ = 1; ///< DW/BOT, UP/TOP
    static int const max_gap_ = 2;
    /** @} */

    /** @{ */
    /** field positions, masks and usage wildcard */
    static int const pos_dettype_ = 28;
    static ::uint32_t const mask_dettype_ = 0xf0000000;
    static int const pos_subdettype_ = 25;
    static ::uint32_t const mask_subdettype_ = 0x0e000000;
    static int const pos_rpcidtype_ = 23;
    static ::uint32_t const mask_rpcidtype_ = 0x01800000;
    static int const pos_region_ = 21;
    static ::uint32_t const mask_region_ = 0x00600000;
    static int const pos_station_ = 18;
    static ::uint32_t const mask_station_ = 0x001c0000;
    static int const pos_layer_ = 16;
    static ::uint32_t const mask_layer_ = 0x00030000;
    static int const pos_ring_ = 13;
    static ::uint32_t const mask_ring_ = 0x0000e000;
    static int const pos_sector_ = 9;
    static ::uint32_t const mask_sector_ = 0x00001e00;
    static int const pos_subsector_ = 7;
    static ::uint32_t const mask_subsector_ = 0x00000180;
    static int const pos_subsubsector_ = 5;
    static ::uint32_t const mask_subsubsector_ = 0x00000060;
    static int const pos_roll_ = 2;
    static ::uint32_t const mask_roll_ = 0x0000001c;
    static int const pos_gap_ = 0;
    static ::uint32_t const mask_gap_ = 0x00000003;
    /** @} */

public:
    RPCMaskDetId();
    /** Construct from a packed id.
     * The id must have DetType Muon, SubDetType RPC
     * and RPCIdType missing (RPCDetId) or RPCMaskDetId.
     * A cms::Exception will be thrown otherwise.
     */
    RPCMaskDetId(::uint32_t const & _id);
    /** Construct from a DetId.
     * The id must have DetType Muon, SubDetType RPC
     * and RPCIdType missing (RPCDetId) or RPCMaskDetId.
     * A cms::Exception will be thrown otherwise.
     */
    RPCMaskDetId(DetId const & _detid);
    /** Construct from a RPCDetId.
     */
    RPCMaskDetId(RPCDetId const & _rpcdetid);
    /** Construct using the individual fields.
     * A cms::Exception will be thrown if a field is out of range.
     */
    RPCMaskDetId(int _region
                 , int _station
                 , int _layer = wildcard_
                 , int _ring = wildcard_
                 , int _sector = wildcard_
                 , int _subsector = wildcard_
                 , int _subsubsector = wildcard_
                 , int _roll = wildcard_
                 , int _gap = wildcard_);

    void set(RPCDetId const & _rpcdetid);

    ::uint32_t getId() const;
    operator ::uint32_t() const;
    ::uint32_t getMask() const;

    bool matches(RPCMaskDetId const & _in) const;

    void setId(::uint32_t const & _in);
    void reset();

    RPCDetId getRPCDetId() const;

    /** @{ */
    /** Field Getters */
    int getDetType() const;
    int getSubDetType() const;
    int getRPCIdType() const;
    int getRegion() const;
    int getStation() const;
    int getLayer() const;
    int getRing() const;
    int getSector() const;
    int getSubSector() const;
    int getSubSubSector() const;
    int getRoll() const;
    int getGap() const;
    /** @} */

    /** @{ */
    /** Derived Getters */
    bool isBarrel() const;
    bool isEndcap() const;
    int getWheel() const;
    int getDisk() const;
    int getEndcapSector() const;

    RPCMaskDetId getWheelId() const;
    RPCMaskDetId getDiskId() const;
    RPCMaskDetId getChamberId() const;
    RPCMaskDetId getRollId() const;
    /** @} */

    /** @{ */
    /** The field setters will throw a cms::Exception("OutOfRange") when the input is out-of-range */
    RPCMaskDetId & setDetType();
    RPCMaskDetId & setSubDetType();
    RPCMaskDetId & setRPCIdType();
    RPCMaskDetId & setRegion(int _region = wildcard_);
    RPCMaskDetId & setStation(int _station = wildcard_);
    RPCMaskDetId & setLayer(int _layer = wildcard_);
    RPCMaskDetId & setRing(int _ring = wildcard_);
    RPCMaskDetId & setSector(int _sector = wildcard_);
    RPCMaskDetId & setSubSector(int _subsector = wildcard_);
    RPCMaskDetId & setSubSubSector(int _subsubsector = wildcard_);
    RPCMaskDetId & setRoll(int _roll = wildcard_);
    RPCMaskDetId & setGap(int _gap = wildcard_);
    /** @} */

    /** @{ */
    /** Derived Setters */
    RPCMaskDetId & setWheel(int _wheel = wildcard_);
    RPCMaskDetId & setDisk(int _disk = wildcard_);
    RPCMaskDetId & setEndcapSector(int _endcap_sector = wildcard_);
    /** @} */
    
    std::string getName() const;

    bool operator<(RPCMaskDetId const & _rhs) const;
    bool operator==(RPCMaskDetId const & _rhs) const;
    bool operator!=(RPCMaskDetId const & _rhs) const;
    bool operator<(::uint32_t const & _rhs) const;
    bool operator==(::uint32_t const & _rhs) const;
    bool operator!=(::uint32_t const & _rhs) const;

    RPCMaskDetId & operator++();
    RPCMaskDetId operator++(int);
    RPCMaskDetId & operator--();
    RPCMaskDetId operator--(int);

protected:
    int bf_get(int const _min, ::uint32_t const _mask, int const _pos) const;
    RPCMaskDetId & bf_set(int const _min, int const _max, ::uint32_t const _mask, int const _pos, int const _value);
    std::ostream & bf_stream(std::ostream & _ostream, int const _min, ::uint32_t const _mask, int const _pos) const;

protected:
    ::uint32_t id_;
};

std::ostream & operator<<(std::ostream & _ostream, RPCMaskDetId const & _rpccompdetid);

#include "DPGAnalysis/RPC/interface/RPCMaskDetId.icc"

#endif // DPGAnalysis_RPC_RPCMaskDetId_h
