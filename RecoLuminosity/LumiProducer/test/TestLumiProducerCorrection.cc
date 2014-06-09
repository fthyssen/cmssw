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
    unsigned int _first_run(_config.getUntrackedParameter<unsigned int>("firstRun", 1));
    unsigned int _last_run(_config.getUntrackedParameter<unsigned int>("lastRun", 1));

    std::vector<edm::LuminosityBlockRange>
        _lumiranges(_config.getUntrackedParameter<std::vector<edm::LuminosityBlockRange> >("lumisToProcess"));
    if (_lumiranges.empty()) {
        for (unsigned int _run = _first_run ; _run <= _last_run ; ++_run)
            run_lumis_[_run].push_back(std::pair<unsigned int, unsigned int>(_config.getUntrackedParameter<unsigned int>("firstLumi", 1)
                                                                             , _config.getUntrackedParameter<unsigned int>("lastLumi", 100)));
    } else {
        for (std::vector<edm::LuminosityBlockRange>::const_iterator _lumirange = _lumiranges.begin()
                 ; _lumirange != _lumiranges.end() ; ++_lumirange)
            if (_lumirange->startRun() >= _first_run && _lumirange->startRun() <= _last_run
                && _lumirange->startRun() == _lumirange->endRun())
                run_lumis_[_lumirange->startRun()].push_back(std::pair<unsigned int, unsigned int>(_lumirange->startLumi(), _lumirange->endLumi()));
        for (std::map<unsigned int, std::vector<std::pair<unsigned int, unsigned int> > >::iterator _lumis = run_lumis_.begin()
                 ; _lumis != run_lumis_.end() ; ++_lumis)
            std::sort(_lumis->second.begin(), _lumis->second.end());
    }
    std::cout << std::setw(8) << "Run"
              << "," << std::setw(8) << "LS"
              << "," << std::setw(8) << "Dur"
              << "," << std::setw(8) << "NColBun"
              << "," << std::setw(12) << "L1T0Cnt"
              << "," << std::setw(12) << "L1T0Dead"
              << "," << std::setw(12) << "InstLumi"
              << "," << std::setw(12) << "IntgLumi/evt"
              << "," << std::setw(12) << "Del"
              << "," << std::setw(12) << "Rec"
              << "," << std::setw(8) << "AvgPU"
              << std::endl;
}

void TestLumiProducerCorrection::analyze(edm::Event const &, edm::EventSetup const &)
{}

void TestLumiProducerCorrection::beginRun(edm::Run const & _run, edm::EventSetup const & _setup)
{
    _setup.get<LumiCorrectionParamRcd>().get(lumi_correction_);
    if (! lumi_correction_.isValid())
        std::cerr << "Could not get valid LumiCorrectionParam for run " << _run.run()
                  << "; skipping this run.";
    lumis_ = run_lumis_.find(_run.run());
}

void TestLumiProducerCorrection::beginLuminosityBlock(edm::LuminosityBlock const & _lumi, edm::EventSetup const &)
{
    if (! lumi_correction_.isValid())
        return;
    if (lumis_ == run_lumis_.end())
        return;

    std::vector<std::pair<unsigned int, unsigned int> >::const_iterator _lumis_it
        = std::upper_bound(lumis_->second.begin(), lumis_->second.end()
                           , std::pair<unsigned int, unsigned int>(_lumi.luminosityBlock(), std::numeric_limits<unsigned int>::max()));
    if (_lumis_it == lumis_->second.begin())
        return;
    if ((--_lumis_it)->second < _lumi.luminosityBlock())
        return;

    edm::Handle<LumiSummary> _lumi_summary;
    _lumi.getByLabel(edm::InputTag("lumiProducer", ""), _lumi_summary);
    if (! _lumi_summary.isValid()) {
        std::cerr<< "Could not get valid LumiSummary for LuminosityBlock " << _lumi.luminosityBlock()
                 << "; skipping this LuminosityBlock.";
        return;
    }
    double _instlumi = _lumi_summary->avgInsDelLumi();
    _instlumi *= lumi_correction_->getCorrection(_instlumi);
    double _duration = _lumi_summary->lumiSectionLength();
    double _avgpu = minbias_xsec_ * _instlumi * _duration / (double)(_lumi_summary->numOrbit() * lumi_correction_->ncollidingbunches());
    std::cout << std::setw(8) << _lumi.run()
              << "," << std::setw(8) << _lumi.luminosityBlock()
              << std::fixed << std::setprecision(2)
              << "," << std::setw(8) << _duration * _lumi_summary->liveFrac()
              << "," << std::setw(8) << lumi_correction_->ncollidingbunches()
              << "," << std::setw(12) << _lumi_summary->bitzerocount()
              << "," << std::setw(12) << _lumi_summary->deadcount()
              << "," << std::setw(12) << _instlumi
              << std::scientific
              << "," << std::setw(12) << _instlumi * _duration / (double)(_lumi_summary->numOrbit() * lumi_correction_->ncollidingbunches())
              << std::fixed << std::setprecision(2)
              << "," << std::setw(12) << _instlumi * _duration
              << "," << std::setw(12) << _instlumi * _duration * _lumi_summary->liveFrac()
              << "," << std::setw(8) << _avgpu
              << std::endl;
    std::cout.unsetf(std::ios_base::floatfield);
}

DEFINE_FWK_MODULE(TestLumiProducerCorrection);
