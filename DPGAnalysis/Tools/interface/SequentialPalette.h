#ifndef DPGAnalysis_Tools_SequentialPalette_h
#define DPGAnalysis_Tools_SequentialPalette_h

#include <vector>

#include "TColor.h"
#include "TStyle.h"

/** http://colorbrewer2.org/?type=sequential&scheme=YlGn&n=9 **/
class SequentialPalette
{
public:
    SequentialPalette(int _offset = 10000, bool _inverted = false);
    ~SequentialPalette();

    std::vector<TColor *> const & getColors() const;
    int const * getPalette() const;

    void write() const;
    void applyPalette(TStyle * _tstyle = ::gStyle, float _alpha = 1.);

protected:
    std::vector<TColor *> colors_;
    std::vector<int> palette_;
};

#include "DPGAnalysis/Tools/interface/SequentialPalette.icc"

#endif // DPGAnalysis_Tools_SequentialPalette_h
