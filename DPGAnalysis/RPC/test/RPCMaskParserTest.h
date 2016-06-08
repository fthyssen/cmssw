#ifndef DPGAnalysis_RPC_test_RPCMaskParserTest_h
#define DPGAnalysis_RPC_test_RPCMaskParserTest_h

#include "FWCore/Framework/interface/EDAnalyzer.h"

class RPCMaskParserTest
    : public edm::EDAnalyzer
{
public:
    RPCMaskParserTest(const edm::ParameterSet &);

private:
    void analyze(edm::Event const & _event, edm::EventSetup const & _setup);
};

#endif // DPGAnalysis_RPC_test_RPCMaskParserTest_h
