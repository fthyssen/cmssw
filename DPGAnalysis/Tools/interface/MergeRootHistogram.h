#ifndef DPGAnalysis_Tools_MergeRootHistogram_h
#define DPGAnalysis_Tools_MergeRootHistogram_h

#include <string>

#include "TAxis.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include "TH1C.h"
#include "TH2C.h"
#include "TH3C.h"

#include "TH1S.h"
#include "TH2S.h"
#include "TH3S.h"

#include "TH1I.h"
#include "TH2I.h"
#include "TH3I.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

template<typename t_value_type, unsigned int t_dimensions>
struct RootHistogram
{
    typedef ::TH1F type;
};

template<>
struct RootHistogram<char, 1>
{
    typedef ::TH1C type;
};

template<>
struct RootHistogram<char, 2>
{
    typedef ::TH2C type;
};

template<>
struct RootHistogram<char, 3>
{
    typedef ::TH3C type;
};

template<>
struct RootHistogram<short, 1>
{
    typedef ::TH1S type;
};

template<>
struct RootHistogram<short, 2>
{
    typedef ::TH2S type;
};

template<>
struct RootHistogram<short, 3>
{
    typedef ::TH3S type;
};

template<>
struct RootHistogram<int, 1>
{
    typedef ::TH1I type;
};

template<>
struct RootHistogram<int, 2>
{
    typedef ::TH2I type;
};

template<>
struct RootHistogram<int, 3>
{
    typedef ::TH3I type;
};

template<>
struct RootHistogram<float, 1>
{
    typedef ::TH1F type;
};

template<>
struct RootHistogram<float, 2>
{
    typedef ::TH2F type;
};

template<>
struct RootHistogram<float, 3>
{
    typedef ::TH3F type;
};

template<>
struct RootHistogram<double, 1>
{
    typedef ::TH1D type;
};

template<>
struct RootHistogram<double, 2>
{
    typedef ::TH2D type;
};

template<>
struct RootHistogram<double, 3>
{
    typedef ::TH3D type;
};

template<typename t_value_type = double, unsigned int t_dimensions = 1>
class MergeRootHistogram
{
public:
    typedef typename RootHistogram<t_value_type, t_dimensions>::type
    histogram_type;

public:
    MergeRootHistogram(std::string const & _name = std::string("empty")
                       , std::string const & _title = std::string("empty")
                       , ::TAxis const & _x_axis = ::TAxis()
                       , ::TAxis const & _y_axis = ::TAxis()
                       , ::TAxis const & _z_axis = ::TAxis());

    histogram_type & operator*();
    histogram_type const & operator*() const;
    histogram_type * operator->();
    histogram_type const * operator->() const;

    bool mergeProduct(MergeRootHistogram const & _product);

protected:
    histogram_type histogram_;
};

typedef MergeRootHistogram<char, 1> MergeTH1C;
typedef MergeRootHistogram<char, 2> MergeTH2C;
typedef MergeRootHistogram<char, 3> MergeTH3C;

typedef MergeRootHistogram<short, 1> MergeTH1S;
typedef MergeRootHistogram<short, 2> MergeTH2S;
typedef MergeRootHistogram<short, 3> MergeTH3S;

typedef MergeRootHistogram<int, 1> MergeTH1I;
typedef MergeRootHistogram<int, 2> MergeTH2I;
typedef MergeRootHistogram<int, 3> MergeTH3I;

typedef MergeRootHistogram<float, 1> MergeTH1F;
typedef MergeRootHistogram<float, 2> MergeTH2F;
typedef MergeRootHistogram<float, 3> MergeTH3F;

typedef MergeRootHistogram<double, 1> MergeTH1D;
typedef MergeRootHistogram<double, 2> MergeTH2D;
typedef MergeRootHistogram<double, 3> MergeTH3D;

#include "DPGAnalysis/Tools/interface/MergeRootHistogram.icc"

#endif // DPGAnalysis_Tools_MergeRootHistogram_h
