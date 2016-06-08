#ifndef DPGAnalysis_Tools_associationiterator_h
#define DPGAnalysis_Tools_associationiterator_h

#include <iterator>
#include <vector>

#include "boost/type_traits/remove_const.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/type_traits/is_const.hpp"
#include "boost/type_traits/conditional.hpp"
#include "boost/utility/enable_if.hpp"

#include "DPGAnalysis/Tools/interface/association.h"

template<typename t_association_type>
class associationiterator
    : public std::iterator<std::random_access_iterator_tag, t_association_type>
{
protected:
    typedef std::iterator<std::random_access_iterator_tag, t_association_type>
                                                            stl_base_type;

public:
    typedef typename boost::remove_const<t_association_type>::type
                                                            association_type;
    typedef typename boost::remove_const<t_association_type>::type const
                                                            const_association_type;
    typedef std::vector<association_type>                   association_container_type;

    typedef associationiterator<association_type>           iterator;
    typedef associationiterator<const_association_type>     const_iterator;

    typedef typename association_type::distance_type        distance_type;

    typedef typename association_type::key_container_type   key_container_type;
    typedef typename association_type::key_value_type       key_value_type;
    typedef typename association_type::key_iterator         key_iterator;
    typedef typename association_type::key_difference_type  key_difference_type;

    typedef typename association_type::value_container_type value_container_type;
    typedef typename association_type::value_value_type     value_value_type;
    typedef typename association_type::value_iterator       value_iterator;
    typedef typename association_type::value_difference_type
                                                            value_difference_type;

    typedef typename stl_base_type::value_type              value_type;
    typedef typename stl_base_type::difference_type         difference_type;
    typedef typename stl_base_type::pointer                 pointer;
    typedef typename stl_base_type::reference               reference;
    typedef typename stl_base_type::iterator_category       iterator_category;

    typedef typename boost::conditional<boost::is_const<t_association_type>::value
                                        , typename association_container_type::const_iterator
                                        , typename association_container_type::iterator>::type
                                                            base_type;

protected:
    friend class associationiterator<association_type>;
    friend class associationiterator<const_association_type>;

public:
    associationiterator();
    associationiterator(base_type const & _iterator
                        , key_iterator const & key_begin
                        , value_iterator const & value_begin);
    // enable construction from const_iterator only if this is a const_iterator
    template<typename t_iterator_type>
    associationiterator(typename boost::enable_if<boost::is_same<t_iterator_type, associationiterator>, t_iterator_type>::type const & _iterator);
    associationiterator(iterator const & _iterator);

    // enable assignment from const_iterator only if this is a const_iterator
    template<typename t_iterator_type>
    associationiterator & operator=(typename boost::enable_if<boost::is_same<t_iterator_type, associationiterator>, t_iterator_type>::type const & _iterator);
    associationiterator & operator=(iterator const & _iterator);

    bool valid() const;
    bool key_valid() const;
    bool value_valid() const;
    key_iterator key() const;
    value_iterator value() const;
    distance_type distance() const;

    base_type base() const;

    reference operator* () const;
    pointer   operator->() const;

    bool operator==(iterator const & _iterator) const;
    bool operator==(const_iterator const & _iterator) const;

    bool operator!=(iterator const & _iterator) const;
    bool operator!=(const_iterator const & _iterator) const;

    bool operator< (iterator const & _iterator) const;
    bool operator< (const_iterator const & _iterator) const;
    bool operator<=(iterator const & _iterator) const;
    bool operator<=(const_iterator const & _iterator) const;
    bool operator> (iterator const & _iterator) const;
    bool operator> (const_iterator const & _iterator) const;
    bool operator>=(iterator const & _iterator) const;
    bool operator>=(const_iterator const & _iterator) const;

    associationiterator & operator++();
    associationiterator   operator++(int);
    associationiterator & operator--();
    associationiterator   operator--(int);

    associationiterator   operator+(difference_type _n) const;
    associationiterator & operator+=(difference_type _n);
    associationiterator   operator-(difference_type _n) const;
    associationiterator & operator-=(difference_type _n);

    difference_type operator-(associationiterator const & _iterator) const;

    reference operator[] (difference_type _n) const;

protected:
    base_type iterator_;
    key_iterator key_begin_;
    value_iterator value_begin_;
};

#include "DPGAnalysis/Tools/interface/associationiterator.icc"

#endif // DPGAnalysis_Tools_associationiterator_h
