#ifndef DPGAnalysis_Tools_DivergingPalette_h
#define DPGAnalysis_Tools_DivergingPalette_h

#include <vector>

#include "TColor.h"
#include "TStyle.h"

/** http://colorbrewer2.org/?type=diverging&scheme=RdYlBu&n=10 **/
class DivergingPalette
{
public:
    DivergingPalette(int _offset = 10010, bool _inverted = false);
    ~DivergingPalette();

    std::vector<TColor *> const & getColors() const;
    int const * getPalette() const;

    void write() const;
    void applyPalette(TStyle * _tstyle = ::gStyle, float _alpha = 1.);

protected:
    std::vector<TColor *> colors_;
    std::vector<int> palette_;
};

#include "DPGAnalysis/Tools/interface/DivergingPalette.icc"

#endif // DPGAnalysis_Tools_DivergingPalette_h
