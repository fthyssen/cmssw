#ifndef RecoLuminosity_LumiProducer_LumiProducer_h
#define RecoLuminosity_LumiProducer_LumiProducer_h

#include <string>
#include <vector>
#include <map>

#include "FWCore/Framework/interface/one/EDProducer.h"

#include "CondCore/DBCommon/interface/DbConnection.h"

#include "CoralBase/Blob.h"

namespace coral {
class ISchema;
} // namespace coral

namespace edm {
class ParameterSet;
class ConfigurationDescriptions;
class EventSetup;
class Event;
class Run;
} // namespace edm

class LumiSummary;
class LumiDetails;

class LumiProducer : public edm::one::EDProducer<edm::one::WatchRuns
                                                 , edm::BeginLuminosityBlockProducer
                                                 , edm::EndRunProducer>
{
protected:
    static unsigned int const nbunches_ = 3564;

public:
    explicit LumiProducer(edm::ParameterSet const & _config);
    ~LumiProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions & _descs);

private:
    template<typename t_type>
    static void getVector(coral::Blob const & _blob, std::vector<t_type> & _vector);

    void loadLumiVersion(coral::ISchema & _schema);
    unsigned long long loadDataID(coral::ISchema & _schema, std::string const & _table
                                  , unsigned int _run);

    void loadRunData(coral::ISchema & _schema, unsigned int _run);
    void loadRunL1TData(coral::ISchema & _schema, unsigned int _run);
    void loadRunHLTData(coral::ISchema & _schema, unsigned int _run);

    void loadLumiData(coral::ISchema & _schema, unsigned int _lumi);
    void loadLumiL1TData(coral::ISchema & _schema, unsigned int _lumi);
    void loadLumiHLTData(coral::ISchema & _schema, unsigned int _lumi);

    void fillRunCache(unsigned int _run);
    void fillLumiCache(unsigned int _run, unsigned int _lumi);

    void clearRunCache();
    void clearLumiCache();

    virtual void beginRun(edm::Run const & _run, edm::EventSetup const &) override final;

    virtual void beginLuminosityBlockProduce(edm::LuminosityBlock & _lumi
                                             , edm::EventSetup const & _setup) override final;

    virtual void produce(edm::Event &, edm::EventSetup const &) override final;

    virtual void endRun(edm::Run const &, edm::EventSetup const &) override final;
    virtual void endRunProduce(edm::Run & _run, edm::EventSetup const &) override final;

protected:
    cond::DbConnection input_database_;
    std::string connect_;

    unsigned ncache_entries_;
    std::string lumi_version_;
    bool produce_lumi_summary_run_header_;
    bool produce_l1t_lumi_data_, produce_hlt_lumi_data_;
    bool produce_lumi_details_;

    unsigned long long lumi_data_id_, l1t_data_id_, hlt_data_id_;
    std::vector<std::string> l1t_names_, hlt_names_;
    std::map<unsigned int, LumiSummary *> lumi_summary_cache_;
    std::map<unsigned int, LumiDetails *> lumi_details_cache_;
};

template<typename t_type>
inline void LumiProducer::getVector(coral::Blob const & _blob, std::vector<t_type> & _vector)
{
    _vector.resize(_blob.size() / sizeof(t_type));
    std::copy((t_type *)(_blob.startingAddress())
              , (t_type *)(_blob.startingAddress()) + _vector.size()
              , _vector.begin());
}

#endif // RecoLuminosity_LumiProducer_LumiProducer_h
