#ifndef RecoLuminosity_LumiProducer_test_TestLumiProducerCorrection_h
#define RecoLuminosity_LumiProducer_test_TestLumiProducerCorrection_h

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include <map>
#include <vector>
#include <utility>

#include "FWCore/Framework/interface/ESHandle.h"

class LumiCorrectionParam;

class TestLumiProducerCorrection
    : public edm::EDAnalyzer
{
public:
    explicit TestLumiProducerCorrection(edm::ParameterSet const & _config);

private:
    void analyze(edm::Event const &, edm::EventSetup const &);
    void beginRun(edm::Run const & _run, edm::EventSetup const & _setup);
    void beginLuminosityBlock(edm::LuminosityBlock const & _lumi, edm::EventSetup const &);

protected:
    double minbias_xsec_;
    edm::ESHandle<LumiCorrectionParam> lumi_correction_;

    std::map<unsigned int, std::vector<std::pair<unsigned int, unsigned int> > > run_lumis_;
    std::map<unsigned int, std::vector<std::pair<unsigned int, unsigned int> > >::const_iterator lumis_;
};

#endif // RecoLuminosity_LumiProducer_test_TestLumiProducerCorrection_h
