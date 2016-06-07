#ifndef CondFormats_RPCObjects_RPCTwinMuxLink_h
#define CondFormats_RPCObjects_RPCTwinMuxLink_h

#include <stdint.h>
#include <string>
#include <iosfwd>
#include <climits>

#include "CondFormats/Serialization/interface/Serializable.h"

/** Identifier for RPC TwinMux-LB path */
class RPCTwinMuxLink
{
public:
    static int const wildcard_ = INT_MIN;

    /** @{ */
    /** field ranges */
    static int const min_fed_ = 0;
    static int const max_fed_ = 65534;
    static int const min_amcnumber_ = 0;
    static int const max_amcnumber_ = 12;
    static int const min_tminput_ = 0;
    static int const max_tminput_ = 4;
    /** @} */

protected:
    /** @{ */
    /** field positions and masks */
    static int const pos_fed_ = 16;
    static ::uint32_t const mask_fed_ = 0xffff0000;
    static int const pos_amcnumber_ = 8;
    static ::uint32_t const mask_amcnumber_ = 0x0000ff00;
    static int const pos_tminput_ = 0;
    static ::uint32_t const mask_tminput_ = 0x000000ff;
    /** @} */

public:
    RPCTwinMuxLink();
    RPCTwinMuxLink(::uint32_t const & _id);
    RPCTwinMuxLink(int _fed
                   , int _amcnumber
                   , int _tminput = wildcard_);

    ::uint32_t getId() const;
    operator ::uint32_t() const;
    ::uint32_t getMask() const;

    bool matches(RPCTwinMuxLink const & _rhs) const;

    void setId(::uint32_t const & _id);
    void reset();

    /** @{ */
    /** Field Getters */
    int getFED() const;
    int getAMCNumber() const;
    int getTMInput() const;
    /** @} */

    /** @{ */
    /** Field Setters
     * A cms::Exception("OutOfRange") is thrown for out-of-range input values.
     **/
    RPCTwinMuxLink & setFED(int _fed = wildcard_);
    RPCTwinMuxLink & setAMCNumber(int _amcnumber = wildcard_);
    RPCTwinMuxLink & setTMInput(int _tminput = wildcard_);
    /** @} */

    std::string getName() const;

    bool operator<(RPCTwinMuxLink const & _rhs) const;
    bool operator==(RPCTwinMuxLink const & _rhs) const;
    bool operator!=(RPCTwinMuxLink const & _rhs) const;
    bool operator<(::uint32_t const & _rhs) const;
    bool operator==(::uint32_t const & _rhs) const;
    bool operator!=(::uint32_t const & _rhs) const;

    RPCTwinMuxLink & operator++();
    RPCTwinMuxLink operator++(int);
    RPCTwinMuxLink & operator--();
    RPCTwinMuxLink operator--(int);

protected:
    int bf_get(int const _min, ::uint32_t const _mask, int const _pos) const;
    RPCTwinMuxLink & bf_set(int const _min, int const _max, ::uint32_t const _mask, int const _pos, int const _value);
    std::ostream & bf_stream(std::ostream & _ostream, int const _min, ::uint32_t const _mask, int const _pos) const;

protected:
    ::uint32_t id_;

    COND_SERIALIZABLE;
};

std::ostream & operator<<(std::ostream & _ostream, RPCTwinMuxLink const & _link);

#include "CondFormats/RPCObjects/interface/RPCTwinMuxLink.icc"

#endif // CondFormats_RPCObjects_RPCTwinMuxLink_h
