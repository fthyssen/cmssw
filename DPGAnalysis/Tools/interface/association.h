#ifndef DPGAnalysis_Tools_association_h
#define DPGAnalysis_Tools_association_h

template<typename t_key_container_type
         , typename t_value_container_type
         , typename t_distance_type = float
         , typename t_key_difference_storage_type = typename t_key_container_type::difference_type
         , typename t_value_difference_storage_type = typename t_value_container_type::difference_type>
class association
{
protected:
    typedef t_key_difference_storage_type                   key_difference_storage_type;
    typedef t_value_difference_storage_type                 value_difference_storage_type;

public:
    typedef t_distance_type                                 distance_type;

    typedef t_key_container_type                            key_container_type;
    typedef typename key_container_type::value_type         key_value_type;
    typedef typename key_container_type::const_iterator     key_iterator;
    typedef typename key_container_type::difference_type    key_difference_type;

    typedef t_value_container_type                          value_container_type;
    typedef typename value_container_type::value_type       value_value_type;
    typedef typename value_container_type::const_iterator   value_iterator;
    typedef typename value_container_type::difference_type  value_difference_type;

public:
    association();
    association(key_difference_type _key_difference
                , value_difference_type _value_difference
                , distance_type _distance);

    distance_type distance() const;
    key_difference_type key_difference() const;
    value_difference_type value_difference() const;

    void distance(distance_type _distance);
    void key_difference(key_difference_type _key_difference);
    void value_difference(value_difference_type _value_difference);

    bool operator<(association const & _rhs) const;

protected:
    distance_type distance_;
    key_difference_storage_type key_difference_;
    value_difference_storage_type value_difference_;
};

#include "DPGAnalysis/Tools/interface/association.icc"

#endif // DPGAnalysis_Tools_association_h
