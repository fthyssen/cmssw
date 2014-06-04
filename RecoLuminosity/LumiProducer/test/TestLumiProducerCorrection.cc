#include "RecoLuminosity/LumiProducer/test/TestLumiProducerCorrection.h"

#include <iostream>
#include <algorithm>

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Luminosity/interface/LumiSummary.h"
#include "DataFormats/Provenance/interface/LuminosityBlockRange.h"

#include "RecoLuminosity/LumiProducer/interface/LumiCorrectionParam.h"
#include "RecoLuminosity/LumiProducer/interface/LumiCorrectionParamRcd.h"

TestLumiProducerCorrection::TestLumiProducerCorrection(edm::ParameterSet const & _config)
    : minbias_xsec_(_config.getUntrackedParameter<double>("minBiasXsec", 69400.))
{
    unsigned int _run(_config.getUntrackedParameter<unsigned int>("run", 1));
    std::vector<edm::LuminosityBlockRange>
        _lumiranges(_config.getUntrackedParameter<std::vector<edm::LuminosityBlockRange> >("lumisToProcess"));
    for (std::vector<edm::LuminosityBlockRange>::const_iterator _lumirange = _lumiranges.begin()
             ; _lumirange != _lumiranges.end() ; ++_lumirange)
        if (_lumirange->startRun() == _run && _lumirange->endRun() == _run)
            lumis_.push_back(std::pair<unsigned int, unsigned int>(_lumirange->startLumi(), _lumirange->endLumi()));
    std::sort(lumis_.begin(), lumis_.end());

    if (lumis_.empty()) {
        std::cerr << "No valid LuminosityBlocks found; the full given range will be assumed (firstLumi-lastLumi)." << std::endl;
        lumis_.push_back(std::pair<unsigned int, unsigned int>(_config.getUntrackedParameter<unsigned int>("firstLumi", 1)
                                                               , _config.getUntrackedParameter<unsigned int>("lastLumi", 100)));
    }
}

void TestLumiProducerCorrection::analyze(edm::Event const &, edm::EventSetup const &)
{}

void TestLumiProducerCorrection::beginRun(edm::Run const & _run, edm::EventSetup const & _setup)
{
    _setup.get<LumiCorrectionParamRcd>().get(lumi_correction_);
    if (! lumi_correction_.isValid())
        std::cerr << "Could not get valid LumiCorrectionParam for run " << _run.run()
                  << "; skipping this run.";
    std::cout << std::setw(8) << "Run"
              << "," << std::setw(8) << "LS"
              << "," << std::setw(8) << "Dur"
              << "," << std::setw(8) << "NColBun"
              << "," << std::setw(12) << "InstLumi"
              << "," << std::setw(12) << "IntgLumi/evt"
              << "," << std::setw(12) << "Del"
              << "," << std::setw(12) << "Rec"
              << "," << std::setw(8) << "AvgPU"
              << std::endl;
}

void TestLumiProducerCorrection::beginLuminosityBlock(edm::LuminosityBlock const & _lumi, edm::EventSetup const &)
{
    std::vector<std::pair<unsigned int, unsigned int> >::const_iterator _lumis_it
        = std::upper_bound(lumis_.begin(), lumis_.end()
                           , std::pair<unsigned int, unsigned int>(_lumi.luminosityBlock(), std::numeric_limits<unsigned int>::max()));
    if (_lumis_it == lumis_.begin())
        return;
    if ((--_lumis_it)->second < _lumi.luminosityBlock())
        return;

    if (lumi_correction_.isValid()) {
        edm::Handle<LumiSummary> _lumisummary;
        _lumi.getByLabel(edm::InputTag("lumiProducer", ""), _lumisummary);
        if (! _lumisummary.isValid()) {
            std::cerr<< "Could not get valid LumiSummary for LuminosityBlock " << _lumi.luminosityBlock()
                     << "; skipping this LuminosityBlock.";
            return;
        }
        double _instlumi = _lumisummary->avgInsDelLumi();
        _instlumi *= lumi_correction_->getCorrection(_instlumi);
        double _duration = _lumisummary->lumiSectionLength();
        double _avgpu = minbias_xsec_ * _instlumi * _duration / (double)(_lumisummary->numOrbit() * lumi_correction_->ncollidingbunches());
        std::cout << std::setw(8) << _lumi.run()
                  << "," << std::setw(8) << _lumi.luminosityBlock()
                  << std::fixed << std::setprecision(2)
                  << "," << std::setw(8) << _duration * _lumisummary->liveFrac()
                  << "," << std::setw(8) << lumi_correction_->ncollidingbunches()
                  << "," << std::setw(12) << _instlumi
                  << std::scientific
                  << "," << std::setw(12) << _instlumi * _duration / (double)(_lumisummary->numOrbit() * lumi_correction_->ncollidingbunches())
                  << std::fixed << std::setprecision(2)
                  << "," << std::setw(12) << _instlumi * _duration
                  << "," << std::setw(12) << _instlumi * _duration * _lumisummary->liveFrac()
                  << "," << std::setw(8) << _avgpu
                  << std::endl;
        std::cout.unsetf(std::ios_base::floatfield);
    }
}

DEFINE_FWK_MODULE(TestLumiProducerCorrection);
