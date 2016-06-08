#ifndef DPGAnalysis_RPC_RPCDCCRecord_h
#define DPGAnalysis_RPC_RPCDCCRecord_h

#include <stdint.h>

namespace rpcdcc {

class Record
{
public:
    /** Types as defined in ReadoutFormats_v106.pdf **/
    static unsigned int const cd_type_      = 0;
    static unsigned int const sbxd_type_    = 1;
    static unsigned int const sld_type_     = 2;
    static unsigned int const empty_type_   = 3;
    static unsigned int const rddm_type_    = 4;
    static unsigned int const sddm_type_    = 5;
    static unsigned int const rcdm_type_    = 6;
    static unsigned int const rdm_type_     = 7;
    static unsigned int const unknown_type_ = 8;

    static ::uint16_t const not_cd_identifier_mask_ = 0xc000;
    static ::uint16_t const not_cd_identifier_      = 0xc000;

    static ::uint16_t const empty_identifier_mask_ = 0xffff;
    static ::uint16_t const empty_identifier_      = 0xe800;
    static ::uint16_t const sddm_identifier_mask_  = 0xffff;
    static ::uint16_t const sddm_identifier_       = 0xe801;

    static ::uint16_t const rdm_identifier_mask_   = 0xffc0;
    static ::uint16_t const rdm_identifier_        = 0xe840;

    static ::uint16_t const sld_identifier_mask_   = 0xf800;
    static ::uint16_t const sld_identifier_        = 0xf800;
    static ::uint16_t const rddm_identifier_mask_  = 0xf800;
    static ::uint16_t const rddm_identifier_       = 0xf000;
    static ::uint16_t const rcdm_identifier_mask_  = 0xf800;
    static ::uint16_t const rcdm_identifier_       = 0xe000;

    static ::uint16_t const sbxd_identifier_mask_  = 0xf000;
    static ::uint16_t const sbxd_identifier_       = 0xd000;

public:
    Record(::uint16_t const _record = 0x0);

    static unsigned int getType(::uint16_t const _record);
    unsigned int getType() const;

    void set(::uint16_t const _record);
    void reset();

    ::uint16_t const & getRecord() const;

protected:
    ::uint16_t record_;
};

/** Start of Bunch Crossing Data **/
class SBXDRecord
{
protected:
    static ::uint16_t const bx_mask_ = 0x0fff;
    static unsigned int const bx_offset_ = 0;

public:
    SBXDRecord(::uint16_t const _record = Record::sbxd_identifier_);

    void set(::uint16_t const _record);
    void reset();

    ::uint16_t const & getRecord() const;

    unsigned int getBX() const;

    void setBX(unsigned int _bx);

protected:
    ::uint16_t record_;
};

/** Start of Link Input Data **/
class SLDRecord
{
protected:
    static ::uint16_t const dcc_input_mask_ = 0x07e0;
    static ::uint16_t const tb_input_mask_  = 0x001f;
    static unsigned int const dcc_input_offset_ = 5;
    static unsigned int const tb_input_offset_  = 0;

public:
    SLDRecord(::uint16_t const _record = Record::sld_identifier_);

    void set(::uint16_t const _record);
    void reset();

    ::uint16_t const & getRecord() const;

    unsigned int getDCCInput() const;
    unsigned int getTBInput() const;

    void setDCCInput(unsigned int _dcc_input);
    void setTBInput(unsigned int _tb_input);

protected:
    ::uint16_t record_;
};

/** Start of Chamber Data **/
class CDRecord
{
protected:
    static ::uint16_t const link_board_mask_ = 0xc000;
    static ::uint16_t const connector_mask_  = 0x3800;
    static ::uint16_t const partition_mask_  = 0x0400;
    static ::uint16_t const eod_mask_        = 0x0200;
    static ::uint16_t const hp_mask_         = 0x0100;
    static ::uint16_t const data_mask_       = 0x00ff;
    static unsigned int const link_board_offset_ = 14;
    static unsigned int const connector_offset_  = 11;
    static unsigned int const partition_offset_  = 10;
    static unsigned int const data_offset_       = 0;

public:
    CDRecord(::uint16_t const _record = 0x0);

    void set(::uint16_t const _record);
    void reset();

    ::uint16_t const & getRecord() const;

    unsigned int getLinkBoard() const;
    unsigned int getConnector() const;
    unsigned int getPartition() const;
    ::uint8_t    getData() const;
    bool isEOD() const;
    bool isHalfPartition() const;

    void setLinkBoard(unsigned int _link_board);
    void setConnector(unsigned int _connector);
    void setPartition(unsigned int _partition);
    void setData     (::uint8_t _data);
    void setEOD(bool _eod);
    void setHalfPartition(bool _hp);

protected:
    ::uint16_t record_;
};

/** RMB Discarded Data Marker **/
class RDDMRecord
{
protected:
    static ::uint16_t const dcc_input_mask_ = 0x07e0;
    static ::uint16_t const tb_input_mask_  = 0x001f;
    static unsigned int const dcc_input_offset_ = 5;
    static unsigned int const tb_input_offset_  = 0;

public:
    RDDMRecord(::uint16_t const _record = Record::rddm_identifier_);

    void set(::uint16_t const _record);
    void reset();

    ::uint16_t const & getRecord() const;

    unsigned int getDCCInput() const;
    unsigned int getTBInput() const;

    void setDCCInput(unsigned int _dcc_input);
    void setTBInput(unsigned int _tb_input);

protected:
    ::uint16_t record_;
};

/** RMB Corrupted Data Marker **/
class RCDMRecord
{
protected:
    static ::uint16_t const dcc_input_mask_ = 0x07e0;
    static ::uint16_t const tb_input_mask_  = 0x001f;
    static unsigned int const dcc_input_offset_ = 5;
    static unsigned int const tb_input_offset_  = 0;

public:
    RCDMRecord(::uint16_t const _record = Record::rcdm_identifier_);

    void set(::uint16_t const _record);
    void reset();

    ::uint16_t const & getRecord() const;

    unsigned int getDCCInput() const;
    unsigned int getTBInput() const;

    void setDCCInput(unsigned int _dcc_input);
    void setTBInput(unsigned int _tb_input);

protected:
    ::uint16_t record_;
};

/** RMB Disabled Marker **/
class RDMRecord
{
protected:
    static ::uint16_t const dcc_input_mask_ = 0x003f;
    static unsigned int const dcc_input_offset_ = 0;

public:
    RDMRecord(::uint16_t const _record = Record::rdm_identifier_);

    void set(::uint16_t const _record);
    void reset();

    ::uint16_t const & getRecord() const;

    unsigned int getDCCInput() const;

    void setDCCInput(unsigned int _dcc_input);

protected:
    ::uint16_t record_;
};

} // namespace rpcdcc

#include "DPGAnalysis/RPC/interface/RPCDCCRecord.icc"

#endif // DPGAnalysis_RPC_RPCDCCRecord_h
