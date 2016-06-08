#ifndef DPGAnalysis_Tools_associationvector_h
#define DPGAnalysis_Tools_associationvector_h

#include <vector>
#include <utility> // std::pair

#include "DPGAnalysis/Tools/interface/association.h"
#include "DPGAnalysis/Tools/interface/associationiterator.h"

template<typename t_association_type
         , typename t_key_container_ptr_type = typename t_association_type::key_container_type const *
         , typename t_value_container_ptr_type = typename t_association_type::value_container_type const *>
class associationvector
{
public:
    typedef t_association_type                              association_type;
    typedef std::vector<association_type>                   container_type;

    typedef typename container_type::value_type             value_type;
    typedef typename container_type::allocator_type         allocator_type;
    typedef typename container_type::size_type              size_type;
    typedef typename container_type::difference_type        difference_type;
    typedef typename container_type::reference              reference;
    typedef typename container_type::const_reference        const_reference;
    typedef typename container_type::pointer                pointer;
    typedef typename container_type::const_pointer          const_pointer;

    typedef associationiterator<value_type>                 iterator;
    typedef associationiterator<value_type const>           const_iterator;
    typedef std::reverse_iterator<iterator>                 reverse_iterator;
    typedef std::reverse_iterator<const_iterator>           const_reverse_iterator;

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

    typedef t_key_container_ptr_type                        key_container_ptr_type;
    typedef t_value_container_ptr_type                      value_container_ptr_type;

protected:
    typedef std::pair<iterator, iterator>                   range_type;
    typedef std::pair<const_iterator, const_iterator>       const_range_type;

protected:
    key_iterator key_begin() const;
    value_iterator value_begin() const;
    key_iterator key_end() const;
    value_iterator value_end() const;

public:
    associationvector();
    associationvector(key_container_ptr_type _key_container_ptr
                      , value_container_ptr_type _value_container_ptr);
    associationvector(key_container_ptr_type _key_container_ptr
                      , value_container_ptr_type _value_container_ptr
                      , container_type const & _associations);
    associationvector(associationvector const & _associationvector);

    associationvector & operator=(associationvector const & _associationvector);
    allocator_type get_allocator() const;

    key_container_ptr_type key_container_ptr() const;
    value_container_ptr_type value_container_ptr() const;

    void key_container_ptr(key_container_ptr_type _key_container_ptr);
    void value_container_ptr(value_container_ptr_type _value_container_ptr);

    const_reference at(size_type _n) const;
    const_reference operator[](size_type _n) const;
    const_reference front() const;
    const_reference back() const;
    container_type const & data() const;

    reference at(size_type _n);
    reference operator[](size_type _n);
    reference front();
    reference back();
    container_type & data();

    iterator begin();
    iterator end();
    reverse_iterator rbegin();
    reverse_iterator rend();
    std::pair<iterator, iterator> equal_range();

    const_iterator begin() const;
    const_iterator cbegin() const;
    const_iterator end() const;
    const_iterator cend() const;
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;
    std::pair<const_iterator, const_iterator> equal_range() const;

    bool empty() const;
    size_type size() const;
    size_type max_size() const;
    void reserve(size_type _size);
    size_type capacity() const;

    void clear();
    iterator insert(iterator _position, value_type const & _value);
    template <typename i_iterator>
    void insert(iterator _position, i_iterator _first, i_iterator _last);
    iterator erase(iterator _position);
    iterator erase(iterator _first, iterator _last);
    void push_back(value_type const & _value);
    void pop_back();
    void swap(associationvector & _associationvector);
    void swap(container_type & _container);

    iterator find_key(key_iterator _key_iterator);
    iterator find_value(value_iterator _value_iterator);

    const_iterator find_key(key_iterator _key_iterator) const;
    const_iterator find_value(value_iterator _value_iterator) const;

protected:
    container_type data_;

    key_container_ptr_type key_container_ptr_;
    value_container_ptr_type value_container_ptr_;
};

template<typename t_association_type
         , typename t_key_container_ptr_type
         , typename t_value_container_ptr_type>
bool operator<(associationvector<t_association_type, t_key_container_ptr_type, t_value_container_ptr_type> const & _lhs, associationvector<t_association_type, t_key_container_ptr_type, t_value_container_ptr_type> const & _rhs);

template<typename t_association_type
         , typename t_key_container_ptr_type
         , typename t_value_container_ptr_type>
bool operator==(associationvector<t_association_type, t_key_container_ptr_type, t_value_container_ptr_type> const & _lhs, associationvector<t_association_type, t_key_container_ptr_type, t_value_container_ptr_type> const & _rhs);

#include "DPGAnalysis/Tools/interface/associationvector.icc"

#endif // DPGAnalysis_Tools_associationvector_h
