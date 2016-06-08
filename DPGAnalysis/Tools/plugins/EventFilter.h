#ifndef DPGAnalysis_Tools_EventFilter_h
#define DPGAnalysis_Tools_EventFilter_h

#include "FWCore/Framework/interface/stream/EDFilter.h"

namespace edm {
class ParameterSet;
class ConfigurationDescriptions;
class EventSetup;
class Event;
} // namespace edm

class EventFilter : public edm::stream::EDFilter<>
{
public:
    explicit EventFilter(edm::ParameterSet const & _config);

    static void fillDescriptions(edm::ConfigurationDescriptions & _descs);

private:
    bool filter(edm::Event &, edm::EventSetup const &) override;
};

#endif // DPGAnalysis_Tools_EventFilter_h
