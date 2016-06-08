#include "DPGAnalysis/Tools/plugins/EventFilter.h"

#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

EventFilter::EventFilter(edm::ParameterSet const & _config)
{}

void EventFilter::fillDescriptions(edm::ConfigurationDescriptions & _descs)
{
    edm::ParameterSetDescription _desc;
    _descs.add("EventFilter", _desc);
}

bool EventFilter::filter(edm::Event &, edm::EventSetup const &)
{
    return false;
}

//define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(EventFilter);
