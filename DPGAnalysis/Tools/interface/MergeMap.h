#ifndef DPGAnalysis_Tools_MergeMap_h
#define DPGAnalysis_Tools_MergeMap_h

#include <map>

template<typename t_key_type, typename t_mapped_type>
class MergeMap
{
public:
    typedef std::map<t_key_type, t_mapped_type> map_type;

public:
    MergeMap();

    map_type & operator*();
    map_type const & operator*() const;
    map_type * operator->();
    map_type const * operator->() const;

    // Assumes mergeProduct exists for t_mapped_type
    bool mergeProduct(MergeMap const & _product);

protected:
    std::map<t_key_type, t_mapped_type> map_;
};

#include "DPGAnalysis/Tools/interface/MergeMap.icc"

#endif // DPGAnalysis_Tools_MergeMap_h
