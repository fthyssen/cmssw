#ifndef DPGAnalysis_Tools_associator_h
#define DPGAnalysis_Tools_associator_h

#include <limits>

#include "DPGAnalysis/Tools/interface/associationvector.h"

namespace associator {

int const many_to_one_ = 1;
int const one_to_one_  = 2;
int const munkres_     = 2;

int const default_method_ = many_to_one_;

template<typename t_associationvector_type>
t_associationvector_type
associate(typename t_associationvector_type::key_container_ptr_type _key_container_ptr
          , typename t_associationvector_type::value_container_ptr_type _value_container_ptr
          , typename t_associationvector_type::distance_type (*_distance_fptr) (typename t_associationvector_type::key_value_type const &
                                                                                , typename t_associationvector_type::value_value_type const &)
          , int _method = associator::default_method_
          , typename t_associationvector_type::distance_type const & _max_distance = std::numeric_limits<typename t_associationvector_type::distance_type>::max());

template<typename t_associationvector_type>
t_associationvector_type
associate(typename t_associationvector_type::key_container_ptr_type _key_container_ptr
          , typename t_associationvector_type::key_iterator const & _key_begin
          , typename t_associationvector_type::key_iterator const & _key_end
          , typename t_associationvector_type::value_container_ptr_type _value_container_ptr
          , typename t_associationvector_type::distance_type (*_distance_fptr) (typename t_associationvector_type::key_value_type const &
                                                                                , typename t_associationvector_type::value_value_type const &)
          , int _method = associator::default_method_
          , typename t_associationvector_type::distance_type const & _max_distance = std::numeric_limits<typename t_associationvector_type::distance_type>::max());

template<typename t_associationvector_type>
t_associationvector_type
associate(typename t_associationvector_type::key_container_ptr_type _key_container_ptr
          , typename t_associationvector_type::value_container_ptr_type _value_container_ptr
          , typename t_associationvector_type::value_iterator const & _value_begin
          , typename t_associationvector_type::value_iterator const & _value_end
          , typename t_associationvector_type::distance_type (*_distance_fptr) (typename t_associationvector_type::key_value_type const &
                                                                                , typename t_associationvector_type::value_value_type const &)
          , int _method = associator::default_method_
          , typename t_associationvector_type::distance_type const & _max_distance = std::numeric_limits<typename t_associationvector_type::distance_type>::max());

template<typename t_associationvector_type>
t_associationvector_type
associate(typename t_associationvector_type::key_container_ptr_type _key_container_ptr
          , typename t_associationvector_type::key_iterator const & _key_begin
          , typename t_associationvector_type::key_iterator const & _key_end
          , typename t_associationvector_type::value_container_ptr_type _value_container_ptr
          , typename t_associationvector_type::value_iterator const & _value_begin
          , typename t_associationvector_type::value_iterator const & _value_end
          , typename t_associationvector_type::distance_type (*_distance_fptr) (typename t_associationvector_type::key_value_type const &
                                                                                , typename t_associationvector_type::value_value_type const &)
          , int _method = associator::default_method_
          , typename t_associationvector_type::distance_type const & _max_distance = std::numeric_limits<typename t_associationvector_type::distance_type>::max());

} // namespace associator

#include "DPGAnalysis/Tools/interface/associator.icc"

#endif // DPGAnalysis_Tools_associator_h
