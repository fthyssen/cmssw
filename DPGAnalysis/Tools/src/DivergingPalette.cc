#include "DPGAnalysis/Tools/interface/DivergingPalette.h"

DivergingPalette::DivergingPalette(int _offset, bool _inverted)
{
    colors_.reserve(10);
    colors_.push_back(new TColor(_offset, 49./255., 54./255., 149./255.));
    colors_.push_back(new TColor(_offset+1, 69./255., 117./255., 180./255.));
    colors_.push_back(new TColor(_offset+2, 116./255., 173./255., 209./255.));
    colors_.push_back(new TColor(_offset+3, 171./255., 217./255., 233./255.));
    colors_.push_back(new TColor(_offset+4, 224./255., 243./255., 248./255.));
    colors_.push_back(new TColor(_offset+5, 254./255., 224./255., 144./255.));
    colors_.push_back(new TColor(_offset+6, 253./255., 174./255., 97./255.));
    colors_.push_back(new TColor(_offset+7, 244./255., 109./255., 67./255.));
    colors_.push_back(new TColor(_offset+8, 215./255., 48./255., 39./255.));
    colors_.push_back(new TColor(_offset+9, 165./255., 0./255., 38./255.));

    palette_.reserve(colors_.size());
    for (std::vector<TColor *>::const_iterator _color = colors_.begin()
             ; _color != colors_.end() ; ++_color)
        palette_.push_back((*_color)->GetNumber());
    if (_inverted)
        std::reverse(palette_.begin(), palette_.end());
}

DivergingPalette::~DivergingPalette()
{
    // TColor owned by gRoot
}

void DivergingPalette::write() const
{
    for (std::vector<TColor *>::const_iterator _color = colors_.begin()
             ; _color != colors_.end() ; ++_color)
        (*_color)->Write();
}

void DivergingPalette::applyPalette(TStyle * _tstyle, float _alpha)
{
    _tstyle->SetPalette(palette_.size(), (int *)(&(palette_.at(0))), _alpha);
    _tstyle->SetNumberContours(palette_.size());
}
