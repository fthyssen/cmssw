#include "RecoLuminosity/LumiProducer/interface/LumiProducer.h"

#include <sstream>
#include <algorithm>
#include <memory>

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Luminosity/interface/LumiSummaryRunHeader.h"
#include "DataFormats/Luminosity/interface/LumiSummary.h"
#include "DataFormats/Luminosity/interface/LumiDetails.h"

#include "RecoLuminosity/LumiProducer/interface/LumiNames.h"

#include "CondCore/DBCommon/interface/DbSession.h"
#include "CondCore/DBCommon/interface/DbScopedTransaction.h"

#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/IQueryDefinition.h"
#include "RelationalAccess/ICursor.h"

#include "CoralBase/Exception.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"

LumiProducer::LumiProducer(edm::ParameterSet const & _config)
    : connect_(_config.getParameter<std::string>("connect"))
    , ncache_entries_(_config.getUntrackedParameter<unsigned int>("ncacheEntries"))
    , lumi_version_(_config.getParameter<std::string>("lumiVersion"))
    , produce_lumi_summary_run_header_(_config.getParameter<bool>("produceLumiSummaryRunHeader"))
    , produce_l1t_lumi_data_(_config.getParameter<bool>("produceL1TLumiData"))
    , produce_hlt_lumi_data_(_config.getParameter<bool>("produceHLTLumiData"))
    , produce_lumi_details_(_config.getParameter<bool>("produceLumiDetails"))
    , lumi_data_id_(0), l1t_data_id_(0), hlt_data_id_(0)
{
    if (produce_lumi_summary_run_header_)
        produces<LumiSummaryRunHeader, edm::InRun>();
    produces<LumiSummary, edm::InLumi>();
    if (produce_lumi_details_)
        produces<LumiDetails, edm::InLumi>();

    input_database_.configure(_config.getParameter<edm::ParameterSet>("DBParameters"));
}

LumiProducer::~LumiProducer()
{
    clearLumiCache();
    clearRunCache();
}

void LumiProducer::fillDescriptions(edm::ConfigurationDescriptions & _descs)
{
    edm::ParameterSetDescription _desc;
    _desc.addUntracked<unsigned int>("ncacheEntries", 5);
    _desc.add<std::string>("lumiVersion", "");
    _desc.add<bool>("produceLumiSummaryRunHeader", true);
    _desc.add<bool>("produceL1TLumiData", true);
    _desc.add<bool>("produceHLTLumiData", true);
    _desc.add<bool>("produceLumiDetails", true);

    _desc.add<std::string>("connect", std::string("frontier://LumiProd/CMS_LUMI_PROD"));
    // copy from DBCommon/python/CondDBCommon_cfi.py
    edm::ParameterSetDescription _dbparameters;
    _dbparameters.addUntracked<std::string>("authenticationPath", "");
    _dbparameters.addUntracked<int>("authenticationSystem", 0);
    _dbparameters.addUntracked<int>("connectionRetrialPeriod", 10);
    _dbparameters.addUntracked<int>("idleConnectionCleanupPeriod", 10);
    _dbparameters.addUntracked<int>("messageLevel", 0);
    _dbparameters.addUntracked<bool>("enablePoolAutomaticCleanUp", false);
    _dbparameters.addUntracked<bool>("enableConnectionSharing", true);
    _dbparameters.addUntracked<int>("connectionRetrialTimeOut", 60);
    _dbparameters.addUntracked<int>("connectionTimeOut", 60);
    _dbparameters.addUntracked<bool>("enableReadOnlySessionOnUpdateConnection", false);
    _desc.add<edm::ParameterSetDescription>("DBParameters", _dbparameters);

    _descs.add("LumiProducer", _desc);
}

void LumiProducer::loadLumiVersion(coral::ISchema & _schema)
{
    std::auto_ptr<coral::IQuery> _query(_schema.newQuery());

    _query->addToTableList(lumi::LumiNames::tagsTableName());
    coral::IQueryDefinition & _subquery(_query->defineSubQuery("MAX_TAGID"));
    _query->addToTableList("MAX_TAGID");
    _query->addToOutputList("TAGNAME");

    _subquery.addToTableList(lumi::LumiNames::tagsTableName(), "SUBTABLE");
    _subquery.addToOutputList("MAX(SUBTABLE.TAGID)", "ID");

    std::string _condition_string = "TAGID = MAX_TAGID.ID";
    coral::AttributeList _condition_data;
    _query->setCondition(_condition_string, _condition_data);

    coral::AttributeList _output_data;
    _output_data.extend<std::string>("TAGNAME");
    _query->defineOutput(_output_data);

    coral::ICursor & _cursor(_query->execute());
    if (_cursor.next())
        lumi_version_ = _cursor.currentRow()["TAGNAME"].data<std::string>();
    _cursor.close();

    LogTrace("LumiProducer") << "LumiVersion loaded: " << lumi_version_;
}

unsigned long long LumiProducer::loadDataID(coral::ISchema & _schema, std::string const & _table
                                            , unsigned int _run)
{
    unsigned long long _data_id(0);

    std::auto_ptr<coral::IQuery> _query(_schema.newQuery());

    _query->addToTableList(_table);
    _query->addToOutputList("MAX(DATA_ID)","dataid");

    std::string _condition_string = "RUNNUM = :run";
    coral::AttributeList _condition_data;
    _condition_data.extend<unsigned int>("run");
    _condition_data["run"].data<unsigned int>() = _run;
    _query->setCondition(_condition_string, _condition_data);

    coral::AttributeList _output_data;
    _output_data.extend<unsigned long long>("dataid");
    _query->defineOutput(_output_data);

    coral::ICursor & _cursor(_query->execute());
    if (_cursor.next() && !(_cursor.currentRow()["dataid"].isNull()))
        _data_id = _cursor.currentRow()["dataid"].data<unsigned long long>();
    _cursor.close();

    LogTrace("LumiProducer") << "DataId loaded: " << _data_id;
    return _data_id;
}

void LumiProducer::loadRunData(coral::ISchema & _schema, unsigned int _run)
{}

void LumiProducer::loadRunL1TData(coral::ISchema & _schema, unsigned int _run)
{
    if (!produce_lumi_summary_run_header_)
        return;

    std::auto_ptr<coral::IQuery> _query(_schema.newQuery());

    _query->addToTableList(lumi::LumiNames::trgdataTableName());
    // _query->addToOutputList("BITZERONAME");
    _query->addToOutputList("BITNAMECLOB");

    std::string _condition_string = "DATA_ID = :dataid";
    coral::AttributeList _condition_data;
    _condition_data.extend<unsigned long long>("dataid");
    _condition_data["dataid"].data<unsigned long long>() = l1t_data_id_;
    _query->setCondition(_condition_string, _condition_data);

    coral::AttributeList _output_data;
    // _output_data.extend<std::string>("BITZERONAME");
    _output_data.extend<std::string>("BITNAMECLOB");
    _query->defineOutput(_output_data);

    coral::ICursor & _cursor(_query->execute());
    while (_cursor.next()) {
        const coral::AttributeList & _row(_cursor.currentRow());

        std::istringstream _l1t_names_ss(_row["BITNAMECLOB"].data<std::string>());
        std::string _l1t_name;
        while(std::getline(_l1t_names_ss, _l1t_name, ','))
            l1t_names_.push_back(_l1t_name);
    }
    _cursor.close();

    LogTrace("LumiProducer") << "RunL1TData loaded";
}

void LumiProducer::loadRunHLTData(coral::ISchema & _schema, unsigned int _run)
{
    if (!produce_lumi_summary_run_header_)
        return;

    std::auto_ptr<coral::IQuery> _query(_schema.newQuery());

    _query->addToTableList(lumi::LumiNames::hltdataTableName());
    _query->addToOutputList("PATHNAMECLOB");

    std::string _condition_string = "DATA_ID = :dataid";
    coral::AttributeList _condition_data;
    _condition_data.extend<unsigned long long>("dataid");
    _condition_data["dataid"].data<unsigned long long>() = hlt_data_id_;
    _query->setCondition(_condition_string, _condition_data);

    coral::AttributeList _output_data;
    _output_data.extend<std::string>("PATHNAMECLOB");
    _query->defineOutput(_output_data);

    coral::ICursor & _cursor(_query->execute());
    while (_cursor.next()) {
        const coral::AttributeList & _row(_cursor.currentRow());

        std::istringstream _hlt_names_ss(_row["PATHNAMECLOB"].data<std::string>());
        std::string _hlt_name;
        while(std::getline(_hlt_names_ss, _hlt_name, ','))
            hlt_names_.push_back(_hlt_name);
    }
    _cursor.close();

    LogTrace("LumiProducer") << "RunHLTData loaded";
}

void LumiProducer::loadLumiData(coral::ISchema & _schema, unsigned int _lumi)
{
    std::auto_ptr<coral::IQuery> _query(_schema.newQuery());

    _query->addToTableList(lumi::LumiNames::lumisummaryv2TableName());
    _query->addToOutputList("CMSLSNUM");
    _query->addToOutputList("INSTLUMI");
    _query->addToOutputList("STARTORBIT");
    _query->addToOutputList("NUMORBIT");
    if (produce_lumi_details_) {
        _query->addToOutputList("CMSBXINDEXBLOB");
        _query->addToOutputList("BEAMINTENSITYBLOB_1");
        _query->addToOutputList("BEAMINTENSITYBLOB_2");
        _query->addToOutputList("BXLUMIVALUE_OCC1");
        _query->addToOutputList("BXLUMIVALUE_OCC2");
        _query->addToOutputList("BXLUMIVALUE_ET");
    }

    std::string _condition_string = "CMSLSNUM >= :lsmin AND CMSLSNUM < :lsmax AND DATA_ID = :dataid";
    coral::AttributeList _condition_data;
    _condition_data.extend<unsigned int>("lsmin");
    _condition_data.extend<unsigned int>("lsmax");
    _condition_data.extend<unsigned long long>("dataid");
    _condition_data["lsmin"].data<unsigned int>() = _lumi;
    _condition_data["lsmax"].data<unsigned int>() = _lumi + ncache_entries_;
    _condition_data["dataid"].data<unsigned long long>() = lumi_data_id_;
    _query->setCondition(_condition_string, _condition_data);

    coral::AttributeList _output_data;
    _output_data.extend<unsigned int>("CMSLSNUM");
    _output_data.extend<float>("INSTLUMI");
    _output_data.extend<unsigned int>("STARTORBIT");
    _output_data.extend<unsigned int>("NUMORBIT");
    if (produce_lumi_details_) {
        _output_data.extend<coral::Blob>("CMSBXINDEXBLOB");
        _output_data.extend<coral::Blob>("BEAMINTENSITYBLOB_1");
        _output_data.extend<coral::Blob>("BEAMINTENSITYBLOB_2");
        _output_data.extend<coral::Blob>("BXLUMIVALUE_OCC1");
        _output_data.extend<coral::Blob>("BXLUMIVALUE_OCC2");
        _output_data.extend<coral::Blob>("BXLUMIVALUE_ET");
    }
    _query->defineOutput(_output_data);

    coral::ICursor & _cursor(_query->execute());
    while (_cursor.next()) {
        const coral::AttributeList & _row(_cursor.currentRow());

        _lumi = _row["CMSLSNUM"].data<unsigned int>();
        std::map<unsigned int, LumiSummary *>::iterator _lumi_summary_it = lumi_summary_cache_.find(_lumi);
        if (_lumi_summary_it != lumi_summary_cache_.end())
            continue;

        LumiSummary * _lumi_summary
            = lumi_summary_cache_.insert(std::pair<unsigned int, LumiSummary *>(_lumi, new LumiSummary())).first->second;
        LumiDetails * _lumi_details(0);
        if (produce_lumi_details_)
            _lumi_details = lumi_details_cache_.insert(std::pair<unsigned int, LumiDetails *>(_lumi, new LumiDetails())).first->second;

        _lumi_summary->setLumiVersion(lumi_version_);
        _lumi_summary->setLumiData(_row["INSTLUMI"].data<float>()
                                   , 0.0
                                   , 0);
        _lumi_summary->setOrbitData(_row["STARTORBIT"].data<unsigned int>()
                                    , _row["NUMORBIT"].data<unsigned int>());

        if(produce_lumi_details_ && !_row["CMSBXINDEXBLOB"].isNull() && !_row["BXLUMIVALUE_OCC1"].isNull())
            {
                { // Beam Intensity
                    coral::Blob const & _bx_idx_blob = _row["CMSBXINDEXBLOB"].data<coral::Blob>();
                    coral::Blob const & _beam1_intensity_blob = _row["BEAMINTENSITYBLOB_1"].data<coral::Blob>();
                    coral::Blob const & _beam2_intensity_blob = _row["BEAMINTENSITYBLOB_2"].data<coral::Blob>();

                    short const * _bx_idx = (short const *)(_bx_idx_blob.startingAddress());
                    float const * _beam1_intensity = (float const *)(_beam1_intensity_blob.startingAddress());
                    float const * _beam2_intensity = (float const *)(_beam2_intensity_blob.startingAddress());
                    unsigned int _bx_idx_size = _bx_idx_blob.size() / sizeof(short);
                    unsigned int _beam1_intensity_size = _beam1_intensity_blob.size() / sizeof(float);
                    unsigned int _beam2_intensity_size = _beam2_intensity_blob.size() / sizeof(float);

                    std::vector<float> _lumi_details_beam1_intensity(nbunches_, 0.0);
                    std::vector<float> _lumi_details_beam2_intensity(nbunches_, 0.0);

                    unsigned int _idx(0);
                    for (unsigned int _bin = 0 ; _bin < _bx_idx_size ; ++_bin) {
                        _idx = _bx_idx[_bin];
                        if (_bin < _beam1_intensity_size && _idx < _lumi_details_beam1_intensity.size())
                            _lumi_details_beam1_intensity.at(_idx) = _beam1_intensity[_bin];
                        if (_bin < _beam2_intensity_size && _idx < _lumi_details_beam2_intensity.size())
                            _lumi_details_beam2_intensity.at(_idx) = _beam2_intensity[_bin];
                    }

                    _lumi_details->fillBeamIntensities(_lumi_details_beam1_intensity, _lumi_details_beam2_intensity);
                }

                // BX Lumi Values

                { // OCC1
                    std::vector<float> _bx_lumi_val_occ1;
                    getVector(_row["BXLUMIVALUE_OCC1"].data<coral::Blob>(), _bx_lumi_val_occ1);
                    _bx_lumi_val_occ1.resize(nbunches_, 0.0);

                    _lumi_details->fill(LumiDetails::kOCC1, _bx_lumi_val_occ1
                                        , std::vector<float>(nbunches_, 0.0), std::vector<short>(nbunches_, 0));
                }

                { // OCC2
                    std::vector<float> _bx_lumi_val_occ2;
                    getVector(_row["BXLUMIVALUE_OCC2"].data<coral::Blob>(), _bx_lumi_val_occ2);
                    _bx_lumi_val_occ2.resize(nbunches_, 0.0);

                    _lumi_details->fill(LumiDetails::kOCC2, _bx_lumi_val_occ2
                                        , std::vector<float>(nbunches_, 0.0), std::vector<short>(nbunches_, 0));
                }

                { // ET
                    std::vector<float> _bx_lumi_val_et;
                    getVector(_row["BXLUMIVALUE_ET"].data<coral::Blob>(), _bx_lumi_val_et);
                    _bx_lumi_val_et.resize(nbunches_, 0.0);

                    _lumi_details->fill(LumiDetails::kET, _bx_lumi_val_et
                                        , std::vector<float>(nbunches_, 0.0), std::vector<short>(nbunches_, 0));
                }
            }
    }
    _cursor.close();

    LogTrace("LumiProducer") << "LumiData loaded";
}

void LumiProducer::loadLumiL1TData(coral::ISchema & _schema, unsigned int _lumi)
{
    std::auto_ptr<coral::IQuery> _query(_schema.newQuery());

    _query->addToTableList(lumi::LumiNames::lstrgTableName());
    _query->addToOutputList("CMSLSNUM");
    _query->addToOutputList("DEADTIMECOUNT");
    _query->addToOutputList("BITZEROCOUNT");
    _query->addToOutputList("BITZEROPRESCALE");
    if (produce_l1t_lumi_data_) {
        _query->addToOutputList("PRESCALEBLOB");
        // _query->addToOutputList("TRGCOUNTBLOB");
    }

    std::string _condition_string = "CMSLSNUM >= :lsmin AND CMSLSNUM < :lsmax AND DATA_ID = :dataid";
    coral::AttributeList _condition_data;
    _condition_data.extend<unsigned int>("lsmin");
    _condition_data.extend<unsigned int>("lsmax");
    _condition_data.extend<unsigned long long>("dataid");
    _condition_data["lsmin"].data<unsigned int>() = _lumi;
    _condition_data["lsmax"].data<unsigned int>() = _lumi + ncache_entries_;
    _condition_data["dataid"].data<unsigned long long>() = l1t_data_id_;
    _query->setCondition(_condition_string, _condition_data);

    coral::AttributeList _output_data;
    _output_data.extend<unsigned int>("CMSLSNUM");
    _output_data.extend<unsigned long long>("DEADTIMECOUNT");
    _output_data.extend<unsigned int>("BITZEROCOUNT");
    _output_data.extend<unsigned int>("BITZEROPRESCALE");
    if (produce_l1t_lumi_data_) {
        _output_data.extend<coral::Blob>("PRESCALEBLOB");
        // _output_data.extend<coral::Blob>("TRGCOUNTBLOB");
    }
    _query->defineOutput(_output_data);

    coral::ICursor & _cursor(_query->execute());
    while (_cursor.next()) {
        const coral::AttributeList & _row(_cursor.currentRow());

        _lumi = _row["CMSLSNUM"].data<unsigned int>();
        std::map<unsigned int, LumiSummary *>::iterator _lumi_summary_it = lumi_summary_cache_.find(_lumi);
        if (_lumi_summary_it == lumi_summary_cache_.end())
            continue;

        LumiSummary * _lumi_summary = _lumi_summary_it->second;
        _lumi_summary->setDeadCount(_row["DEADTIMECOUNT"].data<unsigned long long>());
        _lumi_summary->setBitZeroCount(_row["BITZEROCOUNT"].data<unsigned int>()
                                       * _row["BITZEROPRESCALE"].data<unsigned int>());

        if (produce_l1t_lumi_data_ && ! _row["PRESCALEBLOB"].isNull()) {
            coral::Blob const & _l1t_prescale_blob = _row["PRESCALEBLOB"].data<coral::Blob>();
            // coral::Blob const & _l1t_count_blob = _row["TRGCOUNTBLOB"].data<coral::Blob>();
            unsigned int const * _l1t_prescale = (unsigned int const *)(_l1t_prescale_blob.startingAddress());
            // unsigned int const * _l1t_count = (unsigned int const *)(_l1t_count_blob.startingAddress());
            unsigned int _l1t_prescale_size = _l1t_prescale_blob.size() / sizeof(unsigned int);
            // unsigned int _l1t_count_size = _l1t_count_blob.size() / sizeof(unsigned int);

            std::vector<LumiSummary::L1> _lumi_summary_l1t_data;
            // unsigned int _size = std::min(_l1t_prescale_size, _l1t_count_size);
            unsigned int _size = _l1t_prescale_size;
            for (unsigned int _bin = 0 ; _bin < _size ; ++_bin) {
                LumiSummary::L1 _l1;
                _l1.triggernameidx = _bin;
                _l1.prescale = _l1t_prescale[_bin];
                _lumi_summary_l1t_data.push_back(_l1);
            }
            _lumi_summary->swapL1Data(_lumi_summary_l1t_data);
        }
    }
    _cursor.close();

    LogTrace("LumiProducer") << "LumiL1TData loaded";
}

void LumiProducer::loadLumiHLTData(coral::ISchema & _schema, unsigned int _lumi)
{
    if (! produce_hlt_lumi_data_)
        return;

    std::auto_ptr<coral::IQuery> _query(_schema.newQuery());

    _query->addToTableList(lumi::LumiNames::lshltTableName());
    _query->addToOutputList("CMSLSNUM");
    _query->addToOutputList("PRESCALEBLOB");
    // _query->addToOutputList("HLTCOUNTBLOB");
    // _query->addToOutputList("HLTACCEPTBLOB");

    std::string _condition_string = "CMSLSNUM >= :lsmin AND CMSLSNUM < :lsmax AND DATA_ID = :dataid";
    coral::AttributeList _condition_data;
    _condition_data.extend<unsigned int>("lsmin");
    _condition_data.extend<unsigned int>("lsmax");
    _condition_data.extend<unsigned long long>("dataid");
    _condition_data["lsmin"].data<unsigned int>() = _lumi;
    _condition_data["lsmax"].data<unsigned int>() = _lumi + ncache_entries_;
    _condition_data["dataid"].data<unsigned long long>() = hlt_data_id_;
    _query->setCondition(_condition_string, _condition_data);

    coral::AttributeList _output_data;
    _output_data.extend<unsigned int>("CMSLSNUM");
    _output_data.extend<coral::Blob>("PRESCALEBLOB");
    // _output_data.extend<coral::Blob>("HLTCOUNTBLOB");
    // _output_data.extend<coral::Blob>("HLTACCEPTBLOB");
    _query->defineOutput(_output_data);

    coral::ICursor & _cursor(_query->execute());
    while (_cursor.next()) {
        const coral::AttributeList & _row(_cursor.currentRow());

        _lumi = _row["CMSLSNUM"].data<unsigned int>();
        std::map<unsigned int, LumiSummary *>::iterator _lumi_summary_it = lumi_summary_cache_.find(_lumi);
        if (_lumi_summary_it == lumi_summary_cache_.end())
            continue;

        LumiSummary * _lumi_summary = _lumi_summary_it->second;

        if (! _row["PRESCALEBLOB"].isNull()) {
            coral::Blob const & _hlt_prescale_blob = _row["PRESCALEBLOB"].data<coral::Blob>();
            // coral::Blob const & _hlt_count_blob = _row["HLTCOUNTBLOB"].data<coral::Blob>();
            // coral::Blob const & _hlt_accept_blob = _row["HLTACCEPTBLOB"].data<coral::Blob>();
            unsigned int const * _hlt_prescale = (unsigned int const *)(_hlt_prescale_blob.startingAddress());
            // unsigned int const * _hlt_count = (unsigned int const *)(_hlt_count_blob.startingAddress());
            // unsigned int const * _hlt_accept = (unsigned int const *)(_hlt_accept_blob.startingAddress());
            unsigned int _hlt_prescale_size = _hlt_prescale_blob.size() / sizeof(unsigned int);
            // unsigned int _hlt_count_size = _hlt_count_blob.size() / sizeof(unsigned int);
            // unsigned int _hlt_accept_size = _hlt_accept_blob.size() / sizeof(unsigned int);

            std::vector<LumiSummary::HLT> _lumi_summary_hlt_data;
            // unsigned int _size = std::min(std::min(_hlt_prescale_size, _hlt_count_size), _hlt_accept_size);
            unsigned int _size = _hlt_prescale_size;
            for (unsigned int _bin = 0 ; _bin < _size ; ++_bin) {
                LumiSummary::HLT _hlt;
                _hlt.pathnameidx = _bin;
                _hlt.prescale = _hlt_prescale[_bin];
                _lumi_summary_hlt_data.push_back(_hlt);
            }
            _lumi_summary->swapHLTData(_lumi_summary_hlt_data);
        }
    }
    _cursor.close();

    LogTrace("LumiProducer") << "LumiHLTData loaded";
}

void LumiProducer::fillRunCache(unsigned int _run)
{
    cond::DbSession _session = input_database_.createSession();
    try {
        _session.open(connect_, true); // read-only
        std::auto_ptr<cond::DbScopedTransaction> _transaction(new cond::DbScopedTransaction(_session));
        _transaction->start(true);

        coral::ISchema & _schema(_session.nominalSchema());

        if (lumi_version_.empty())
            loadLumiVersion(_schema);

        lumi_data_id_ = loadDataID(_schema, lumi::LumiNames::lumidataTableName(), _run);
        l1t_data_id_ = loadDataID(_schema, lumi::LumiNames::trgdataTableName(), _run);
        if (produce_hlt_lumi_data_ || produce_lumi_summary_run_header_)
            hlt_data_id_ = loadDataID(_schema, lumi::LumiNames::hltdataTableName(), _run);

        if (lumi_data_id_)
            loadRunData(_schema, _run);
        if (l1t_data_id_)
            loadRunL1TData(_schema, _run);
        if (hlt_data_id_)
            loadRunHLTData(_schema, _run);

        _transaction->rollback();
    } catch (coral::Exception const & _coral_exception) {
        throw cms::Exception("CoralException ") << _coral_exception.what();
    }
}

void LumiProducer::fillLumiCache(unsigned int _run, unsigned int _lumi)
{
    cond::DbSession _session = input_database_.createSession();
    try {
        _session.open(connect_, true); // read-only
        std::auto_ptr<cond::DbScopedTransaction> _transaction(new cond::DbScopedTransaction(_session));
        _transaction->start(true);

        coral::ISchema & _schema(_session.nominalSchema());

        if (lumi_data_id_)
            loadLumiData(_schema, _lumi);
        if (l1t_data_id_ && lumi_summary_cache_.size())
            loadLumiL1TData(_schema, _lumi);
        if (hlt_data_id_ && lumi_summary_cache_.size())
            loadLumiHLTData(_schema, _lumi);

        _transaction->rollback();
    } catch (coral::Exception const & _coral_exception) {
        throw cms::Exception("CoralException ") << _coral_exception.what();
    }
}

void LumiProducer::clearRunCache()
{
    lumi_data_id_ = 0;
    l1t_data_id_ = 0;
    hlt_data_id_ = 0;

    l1t_names_.clear();
    hlt_names_.clear();
}

void LumiProducer::clearLumiCache()
{
    for (std::map<unsigned int, LumiSummary *>::iterator _lumi_summary = lumi_summary_cache_.begin()
             ; _lumi_summary != lumi_summary_cache_.end() ; ++_lumi_summary)
        delete (_lumi_summary->second);
    lumi_summary_cache_.clear();

    if (produce_lumi_details_) {
        for (std::map<unsigned int, LumiDetails *>::iterator _lumi_details = lumi_details_cache_.begin()
                 ; _lumi_details != lumi_details_cache_.end() ; ++_lumi_details)
            delete (_lumi_details->second);
        lumi_details_cache_.clear();
    }
}

void LumiProducer::beginRun(edm::Run const & _run, edm::EventSetup const &)
{
    fillRunCache(_run.run());
}

void LumiProducer::beginRunProduce(edm::Run & _run, edm::EventSetup const &)
{
    if (produce_lumi_summary_run_header_) {
        std::auto_ptr<LumiSummaryRunHeader> _lumi_summary_run_header(new LumiSummaryRunHeader());
        _lumi_summary_run_header->swapL1Names(l1t_names_);
        _lumi_summary_run_header->swapHLTNames(hlt_names_);
        _run.put(_lumi_summary_run_header);
    }
}

void LumiProducer::beginLuminosityBlockProduce(edm::LuminosityBlock & _lumi
                                               , edm::EventSetup const & _setup)
{
    unsigned int _lumi_number(_lumi.luminosityBlock());

    std::map<unsigned int, LumiSummary *>::iterator _lumi_summary_it
        = lumi_summary_cache_.find(_lumi_number);
    if (_lumi_summary_it == lumi_summary_cache_.end()) {
        clearLumiCache();
        fillLumiCache(_lumi.run(), _lumi_number);
        _lumi_summary_it = lumi_summary_cache_.find(_lumi_number);
        if (_lumi_summary_it == lumi_summary_cache_.end()) {
            //edm::LogWarning("LumiProducer")
            //    << "Missing LumiSummary for " << _lumi.run() << ":" << _lumi.luminosityBlock();
            return;
        }
    }

    std::auto_ptr<LumiSummary> _lumi_summary(_lumi_summary_it->second);
    _lumi.put(_lumi_summary);
    lumi_summary_cache_.erase(_lumi_summary_it);

    if (produce_lumi_details_) {
        std::map<unsigned int, LumiDetails *>::iterator _lumi_details_it
            = lumi_details_cache_.find(_lumi_number);
        std::auto_ptr<LumiDetails> _lumi_details(_lumi_details_it->second);
        _lumi.put(_lumi_details);
        lumi_details_cache_.erase(_lumi_details_it);
    }
}

void LumiProducer::produce(edm::Event &, edm::EventSetup const &)
{}

void LumiProducer::endRun(edm::Run const &, edm::EventSetup const &)
{
    clearLumiCache();
    clearRunCache();
}

//define this as a plug-in
DEFINE_FWK_MODULE(LumiProducer);
