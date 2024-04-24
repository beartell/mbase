#ifndef MBASE_SEQUENCE_ITERATOR_H
#define MBASE_SEQUENCE_ITERATOR_H

#include <mbase/common.h>

MBASE_STD_BEGIN

//template<typename T>
//class legacy_sequence_iterator {
//public:
//    using value_type = T;
//    using pointer = T*;
//    using const_pointer = const T*;
//    using reference = T&;
//    using const_reference = const T&;
//    using size_type = SIZE_T;
//    using difference_type = PTRDIFF;
//
//    legacy_sequence_iterator(pointer in_ptr) noexcept;
//    legacy_sequence_iterator(const legacy_sequence_iterator in_rhs) noexcept;
//
//    legacy_sequence_iterator& operator=(const legacy_sequence_iterator) noexcept;
//
//    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE reference operator*() const noexcept;
//
//    MBASE_INLINE legacy_sequence_iterator& operator++() noexcept;
//
//    MBASE_INLINE GENERIC swap(legacy_sequence_iterator& in_rhs);
//private:
//    pointer _ptr;
//};
//
//template<typename T>
//class const_legacy_sequence_iterator {
//public:
//    using value_type = T;
//    using pointer = T*;
//    using const_pointer = const T*;
//    using reference = T&;
//    using const_reference = const T&;
//    using size_type = SIZE_T;
//    using difference_type = PTRDIFF;
//
//    const_legacy_sequence_iterator(pointer in_ptr) noexcept;
//    const_legacy_sequence_iterator(const const_legacy_sequence_iterator in_rhs) noexcept;
//
//    const_legacy_sequence_iterator& operator=(const const_legacy_sequence_iterator) noexcept;
//
//    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE reference operator*() const noexcept;
//
//    MBASE_INLINE const_legacy_sequence_iterator& operator++() noexcept;
//
//    MBASE_INLINE GENERIC swap(const_legacy_sequence_iterator& in_rhs);
//private:
//    const_pointer _ptr;
//};
//
//template<typename T>
//class legacy_input_sequence_iterator : public legacy_sequence_iterator<T> {
//public:
//    using value_type = typename legacy_sequence_iterator<T>::value_type;
//    using pointer = typename legacy_sequence_iterator<T>::pointer;
//    using const_pointer = typename legacy_sequence_iterator<T>::const_pointer;
//    using reference = typename legacy_sequence_iterator<T>::reference;
//    using const_reference = typename legacy_sequence_iterator<T>::const_reference;
//    using size_type = typename legacy_sequence_iterator<T>::size_type;
//    using difference_type = typename legacy_sequence_iterator<T>::difference_type;
//    using iterator_category = std::input_iterator_tag;
//    
//    MBASE_INLINE pointer operator->() const noexcept;
//
//    MBASE_INLINE legacy_input_sequence_iterator operator++(I32) noexcept;
//
//    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const legacy_input_sequence_iterator& in_rhs) const noexcept;
//    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const legacy_input_sequence_iterator& in_rhs) const noexcept;
//};
//
//template<typename T>
//class const_legacy_input_sequence_iterator : public const_legacy_sequence_iterator<T> {
//public:
//    using value_type = typename const_legacy_sequence_iterator<T>::value_type;
//    using pointer = typename const_legacy_sequence_iterator<T>::pointer;
//    using const_pointer = typename const_legacy_sequence_iterator<T>::const_pointer;
//    using reference = typename const_legacy_sequence_iterator<T>::reference;
//    using const_reference = typename const_legacy_sequence_iterator<T>::const_reference;
//    using size_type = typename const_legacy_sequence_iterator<T>::size_type;
//    using difference_type = typename const_legacy_sequence_iterator<T>::difference_type;
//    using iterator_category = std::input_iterator_tag;
//
//    MBASE_INLINE pointer operator->() const noexcept;
//
//    MBASE_INLINE const_legacy_input_sequence_iterator operator++(I32) noexcept;
//
//    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const const_legacy_input_sequence_iterator& in_rhs) const noexcept;
//    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_legacy_input_sequence_iterator& in_rhs) const noexcept;
//    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const legacy_input_sequence_iterator<T>& in_rhs) const noexcept;
//    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const legacy_input_sequence_iterator<T>& in_rhs) const noexcept;
//};
//

template<typename T>
class sequence_iterator;

template<typename T>
class const_sequence_iterator;

template<typename T>
class reverse_sequence_iterator;

template<typename T>
class const_reverse_sequence_iterator;

template<typename T>
class sequence_iterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    sequence_iterator(pointer in_ptr) noexcept;
    sequence_iterator(const sequence_iterator& in_rhs) noexcept;

    sequence_iterator& operator=(const sequence_iterator& in_rhs) noexcept;

    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE pointer get() noexcept;

    MBASE_INLINE pointer operator->() const noexcept;
    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE reference operator*() const noexcept;
    MBASE_INLINE reference operator[](difference_type in_index);

    MBASE_ND("ignoring iterator index") MBASE_INLINE sequence_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE sequence_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE sequence_iterator& operator++() noexcept;
    MBASE_INLINE const sequence_iterator operator++(int) noexcept;
    MBASE_INLINE sequence_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE sequence_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE sequence_iterator& operator--() noexcept;
    MBASE_INLINE const sequence_iterator operator--(int) noexcept;
    MBASE_INLINE difference_type operator-(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE difference_type operator-(const const_sequence_iterator<T>& in_rhs) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const sequence_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const sequence_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const const_sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_INLINE bool operator<(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<(const const_sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const const_sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const const_sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const const_sequence_iterator<T>& in_rhs) noexcept;

protected:
    pointer _ptr;
};

template<typename T>
class const_sequence_iterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    const_sequence_iterator(pointer in_ptr) noexcept;
    const_sequence_iterator(const const_sequence_iterator& in_rhs) noexcept;
    const_sequence_iterator(sequence_iterator<T> in_rhs) noexcept;

    const_sequence_iterator& operator=(const const_sequence_iterator& in_rhs) noexcept;
    const_sequence_iterator& operator=(sequence_iterator<T> in_rhs) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE const_pointer get() noexcept;
    
    MBASE_INLINE const_pointer operator->() const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE const_reference operator*() const noexcept;
    MBASE_INLINE const_reference operator[](difference_type in_index);

    MBASE_ND("ignoring equality comparison") MBASE_INLINE const_sequence_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE const_sequence_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_sequence_iterator& operator++() noexcept;
    MBASE_INLINE const const_sequence_iterator operator++(int) noexcept;
    MBASE_INLINE const_sequence_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE const_sequence_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_sequence_iterator& operator--() noexcept;
    MBASE_INLINE const const_sequence_iterator operator--(int) noexcept;
    MBASE_INLINE difference_type operator-(const sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE difference_type operator-(const const_sequence_iterator& in_rhs) noexcept;
    
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const const_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_INLINE bool operator<(const const_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const const_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const const_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const const_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<(const sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const sequence_iterator<T>& in_rhs) noexcept;

protected:
    pointer _ptr;
};

template<typename T>
class reverse_sequence_iterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    reverse_sequence_iterator(pointer in_ptr) noexcept;
    reverse_sequence_iterator(const reverse_sequence_iterator& in_rhs) noexcept;

    reverse_sequence_iterator& operator=(const reverse_sequence_iterator& in_rhs) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE pointer get() noexcept;

    MBASE_INLINE pointer operator->() const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE reference operator*() const noexcept;
    MBASE_INLINE reference operator[](difference_type in_index);

    MBASE_ND("ignoring equality comparison") MBASE_INLINE reverse_sequence_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_sequence_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_sequence_iterator& operator++() noexcept;
    MBASE_INLINE const reverse_sequence_iterator operator++(int) noexcept;
    MBASE_INLINE reverse_sequence_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_sequence_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_sequence_iterator& operator--() noexcept;
    MBASE_INLINE const reverse_sequence_iterator operator--(int) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const reverse_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const reverse_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept;

protected:
    pointer _ptr;
};

template<typename T>
class const_reverse_sequence_iterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    const_reverse_sequence_iterator(pointer in_ptr) noexcept;
    const_reverse_sequence_iterator(const const_reverse_sequence_iterator& in_rhs) noexcept;
    const_reverse_sequence_iterator(reverse_sequence_iterator<T> in_rhs) noexcept;

    const_reverse_sequence_iterator& operator=(const const_reverse_sequence_iterator& in_rhs) noexcept;
    const_reverse_sequence_iterator& operator=(reverse_sequence_iterator<T> in_rhs) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE const_pointer get() noexcept;
    
    MBASE_INLINE const_pointer operator->() const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE const_reference operator*() const noexcept;
    MBASE_INLINE const_reference operator[](difference_type in_index);

    MBASE_ND("ignoring equality comparison") MBASE_INLINE const_reverse_sequence_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator& operator++() noexcept;
    MBASE_INLINE const const_reverse_sequence_iterator operator++(int) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator& operator--() noexcept;
    MBASE_INLINE const const_reverse_sequence_iterator operator--(int) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const const_reverse_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_reverse_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const reverse_sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const reverse_sequence_iterator<T>& in_rhs) const noexcept;

protected:
    pointer _ptr;
};

/* <-- SEQUENCE ITERATOR IMPLEMENTATION --> */
template<typename T>
sequence_iterator<T>::sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T>
sequence_iterator<T>::sequence_iterator(const sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) 
{
}

template<typename T>
sequence_iterator<T>& sequence_iterator<T>::operator=(const sequence_iterator& in_rhs) noexcept {
    _ptr = in_rhs._ptr;
    return *this;
}

template<typename T>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE typename sequence_iterator<T>::pointer sequence_iterator<T>::get() noexcept {
    return _ptr;
}

template<typename T>
MBASE_INLINE  typename sequence_iterator<T>::pointer sequence_iterator<T>::operator->() const noexcept {
    return _ptr;
}

template<typename T>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE typename sequence_iterator<T>::reference sequence_iterator<T>::operator*() const noexcept {
    return *_ptr;
}

template<typename T>
MBASE_INLINE typename sequence_iterator<T>::reference sequence_iterator<T>::operator[](difference_type in_index) {
    return *(_ptr + in_index);
}

template<typename T>
MBASE_ND("ignoring iterator index") MBASE_INLINE sequence_iterator<T> sequence_iterator<T>::operator+(difference_type in_rhs) noexcept {
    sequence_iterator sit(_ptr);
    sit += in_rhs;
    return sit;
}

template<typename T>
MBASE_INLINE sequence_iterator<T>& sequence_iterator<T>::operator+=(difference_type in_rhs) noexcept {
    _ptr += in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE sequence_iterator<T>& sequence_iterator<T>::operator++() noexcept {
    ++_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const sequence_iterator<T> sequence_iterator<T>::operator++(int) noexcept {
    sequence_iterator si(*this);
    ++(*this);
    return si;
}

template<typename T>
MBASE_INLINE sequence_iterator<T> sequence_iterator<T>::operator-(difference_type in_rhs) noexcept {
    sequence_iterator sit(_ptr);
    sit -= in_rhs;
    return sit;
}

template<typename T>
MBASE_INLINE sequence_iterator<T>& sequence_iterator<T>::operator-=(difference_type in_rhs) noexcept {
    _ptr -= in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE sequence_iterator<T>& sequence_iterator<T>::operator--() noexcept {
    --_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const sequence_iterator<T> sequence_iterator<T>::operator--(int) noexcept {
    sequence_iterator si(*this);
    --(*this);
    return si;
}

template<typename T>
MBASE_INLINE typename sequence_iterator<T>::difference_type sequence_iterator<T>::operator-(const sequence_iterator& in_rhs) noexcept {
    return static_cast<difference_type>(_ptr - in_rhs._ptr);
}

template<typename T>
MBASE_INLINE typename sequence_iterator<T>::difference_type sequence_iterator<T>::operator-(const const_sequence_iterator<T>& in_rhs) noexcept {
    return static_cast<difference_type>(_ptr - in_rhs.get());
}


template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool sequence_iterator<T>::operator==(const sequence_iterator& in_rhs) const noexcept {
    return _ptr == in_rhs._ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool sequence_iterator<T>::operator!=(const sequence_iterator& in_rhs) const noexcept {
    return _ptr != in_rhs._ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool sequence_iterator<T>::operator==(const const_sequence_iterator<T>& in_rhs) const noexcept {
    return _ptr == in_rhs.get();
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool sequence_iterator<T>::operator!=(const const_sequence_iterator<T>& in_rhs) const noexcept {
    return _ptr != in_rhs.get();
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator<(const sequence_iterator& in_rhs) noexcept {
    return (in_rhs._ptr - _ptr) > 0;
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator>(const sequence_iterator& in_rhs) noexcept {
    return !((in_rhs._ptr - _ptr) > 0);
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator>=(const sequence_iterator& in_rhs) noexcept {
    return (in_rhs._ptr - _ptr) >= 0;
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator<=(const sequence_iterator& in_rhs) noexcept {
    return !((in_rhs._ptr - _ptr) >= 0);
}

/* <-- CONST SEQUENCE ITERATOR IMPLEMENTATION --> */

template<typename T>
const_sequence_iterator<T>::const_sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T>
const_sequence_iterator<T>::const_sequence_iterator(const const_sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) 
{
}

template<typename T>
const_sequence_iterator<T>::const_sequence_iterator(sequence_iterator<T> in_rhs) noexcept : _ptr(in_rhs.get()) {}

template<typename T>
const_sequence_iterator<T>& const_sequence_iterator<T>::operator=(const const_sequence_iterator& in_rhs) noexcept {
    _ptr = in_rhs._ptr;
    return *this;
}

template<typename T>
const_sequence_iterator<T>& const_sequence_iterator<T>::operator=(sequence_iterator<T> in_rhs) noexcept {
    _ptr = in_rhs.get();
    return *this;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE typename const_sequence_iterator<T>::const_pointer const_sequence_iterator<T>::get() noexcept {
    return _ptr;
}

template<typename T>
MBASE_INLINE typename const_sequence_iterator<T>::const_pointer const_sequence_iterator<T>::operator->() const noexcept {
    return _ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE typename const_sequence_iterator<T>::const_reference const_sequence_iterator<T>::operator*() const noexcept {
    return *_ptr;
}

template<typename T>
MBASE_INLINE typename const_sequence_iterator<T>::const_reference const_sequence_iterator<T>::operator[](difference_type in_index) {
    return *(_ptr + in_index);
}


template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE const_sequence_iterator<T> const_sequence_iterator<T>::operator+(difference_type in_rhs) noexcept {
    const_sequence_iterator csit(_ptr);
    csit += in_rhs;
    return csit;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T>& const_sequence_iterator<T>::operator+=(difference_type in_rhs) noexcept {
    _ptr += in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T>& const_sequence_iterator<T>::operator++() noexcept {
    ++_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const const_sequence_iterator<T> const_sequence_iterator<T>::operator++(int) noexcept {
    const_sequence_iterator csi(*this);
    --(*this);
    return csi;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T> const_sequence_iterator<T>::operator-(difference_type in_rhs) noexcept {
    const_sequence_iterator csit(_ptr);
    csit -= in_rhs;
    return csit;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T>& const_sequence_iterator<T>::operator-=(difference_type in_rhs) noexcept {
    _ptr -= in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T>& const_sequence_iterator<T>::operator--() noexcept {
    --_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const const_sequence_iterator<T> const_sequence_iterator<T>::operator--(int) noexcept {
    const_sequence_iterator csi(*this);
    --(*this);
    return csi;
}

template<typename T>
MBASE_INLINE typename const_sequence_iterator<T>::difference_type const_sequence_iterator<T>::operator-(const sequence_iterator<T>& in_rhs) noexcept {
    return static_cast<difference_type>(_ptr - in_rhs._ptr);
}

template<typename T>
MBASE_INLINE typename const_sequence_iterator<T>::difference_type const_sequence_iterator<T>::operator-(const const_sequence_iterator& in_rhs) noexcept {
    return static_cast<difference_type>(_ptr - in_rhs.get());
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_sequence_iterator<T>::operator==(const const_sequence_iterator& in_rhs) const noexcept {
    return _ptr == in_rhs._ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_sequence_iterator<T>::operator!=(const const_sequence_iterator& in_rhs) const noexcept {
    return _ptr != in_rhs._ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_sequence_iterator<T>::operator==(const sequence_iterator<T>& in_rhs) const noexcept {
    return _ptr == in_rhs.get();
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_sequence_iterator<T>::operator!=(const sequence_iterator<T>& in_rhs) const noexcept {
    return _ptr != in_rhs.get();
}

/* <-- REVERSE SEQUENCE ITERATOR IMPLEMENTATION --> */
template<typename T>
reverse_sequence_iterator<T>::reverse_sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T>
reverse_sequence_iterator<T>::reverse_sequence_iterator(const reverse_sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) 
{
}

template<typename T>
reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator=(const reverse_sequence_iterator& in_rhs) noexcept {
    _ptr = in_rhs._ptr;
    return *this;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE typename reverse_sequence_iterator<T>::pointer reverse_sequence_iterator<T>::get() noexcept {
    return _ptr;
}

template<typename T>
MBASE_INLINE typename reverse_sequence_iterator<T>::pointer reverse_sequence_iterator<T>::operator->() const noexcept {
    return _ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE typename reverse_sequence_iterator<T>::reference reverse_sequence_iterator<T>::operator*() const noexcept {
    return *_ptr;
}

template<typename T>
MBASE_INLINE typename reverse_sequence_iterator<T>::reference reverse_sequence_iterator<T>::operator[](difference_type in_index) {
    return *(_ptr - in_index);
}


template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE reverse_sequence_iterator<T> reverse_sequence_iterator<T>::operator+(difference_type in_rhs) noexcept {
    reverse_sequence_iterator rsi(_ptr);
    rsi += in_rhs;
    return rsi;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator+=(difference_type in_rhs) noexcept {
    _ptr -= in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator++() noexcept {
    --_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const reverse_sequence_iterator<T> reverse_sequence_iterator<T>::operator++(int) noexcept {
    reverse_sequence_iterator rsi(*this);
    ++(*this);
    return rsi;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T> reverse_sequence_iterator<T>::operator-(difference_type in_rhs) noexcept {
    reverse_sequence_iterator rsi(_ptr);
    rsi -= in_rhs;
    return rsi;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator-=(difference_type in_rhs) noexcept {
    _ptr += in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator--() noexcept {
    ++_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const reverse_sequence_iterator<T> reverse_sequence_iterator<T>::operator--(int) noexcept {
    reverse_sequence_iterator rsi(*this);
    --(*this);
    return rsi;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool reverse_sequence_iterator<T>::operator==(const reverse_sequence_iterator& in_rhs) const noexcept {
    return _ptr == in_rhs._ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool reverse_sequence_iterator<T>::operator!=(const reverse_sequence_iterator& in_rhs) const noexcept {
    return _ptr != in_rhs._ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool reverse_sequence_iterator<T>::operator==(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept {
    return _ptr == in_rhs.get();
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool reverse_sequence_iterator<T>::operator!=(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept {
    return _ptr != in_rhs.get();
}

/* <-- CONST REVERSE SEQUENCE ITERATOR IMPLEMENTATION --> */

template<typename T>
const_reverse_sequence_iterator<T>::const_reverse_sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T>
const_reverse_sequence_iterator<T>::const_reverse_sequence_iterator(const const_reverse_sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr)
{
}

template<typename T>
const_reverse_sequence_iterator<T>::const_reverse_sequence_iterator(reverse_sequence_iterator<T> in_rhs) noexcept : _ptr(in_rhs.get()) 
{
}

template<typename T>
const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator=(const const_reverse_sequence_iterator& in_rhs) noexcept {
    _ptr = in_rhs._ptr;
    return *this;
}

template<typename T>
const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator=(reverse_sequence_iterator<T> in_rhs) noexcept {
    _ptr = in_rhs.get();
    return *this;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE typename const_reverse_sequence_iterator<T>::const_pointer const_reverse_sequence_iterator<T>::get() noexcept {
    return _ptr;
}

template<typename T>
MBASE_INLINE typename const_reverse_sequence_iterator<T>::const_pointer const_reverse_sequence_iterator<T>::operator->() const noexcept {
    return _ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE typename const_reverse_sequence_iterator<T>::const_reference const_reverse_sequence_iterator<T>::operator*() const noexcept {
    return *_ptr;
}

template<typename T>
MBASE_INLINE typename const_reverse_sequence_iterator<T>::const_reference const_reverse_sequence_iterator<T>::operator[](difference_type in_index) {
    return *(_ptr - in_index);
}


template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE const_reverse_sequence_iterator<T> const_reverse_sequence_iterator<T>::operator+(difference_type in_rhs) noexcept {
    const_reverse_sequence_iterator rsi(_ptr);
    rsi += in_rhs;
    return rsi;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator+=(difference_type in_rhs) noexcept {
    _ptr -= in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator++() noexcept {
    --_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const const_reverse_sequence_iterator<T> const_reverse_sequence_iterator<T>::operator++(int) noexcept {
    const_reverse_sequence_iterator crsi(*this);
    ++(*this);
    return crsi;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T> const_reverse_sequence_iterator<T>::operator-(difference_type in_rhs) noexcept {
    const_reverse_sequence_iterator rsi(_ptr);
    rsi -= in_rhs;
    return rsi;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator-=(difference_type in_rhs) noexcept {
    _ptr += in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator--() noexcept {
    ++_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const const_reverse_sequence_iterator<T> const_reverse_sequence_iterator<T>::operator--(int) noexcept {
    const_reverse_sequence_iterator crsi(*this);
    --(*this);
    return crsi;

}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator==(const const_reverse_sequence_iterator& in_rhs) const noexcept {
    return _ptr == in_rhs._ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator!=(const const_reverse_sequence_iterator& in_rhs) const noexcept {
    return _ptr != in_rhs._ptr;
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator==(const reverse_sequence_iterator<T>& in_rhs) const noexcept {
    return _ptr == in_rhs.get();
}

template<typename T>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator!=(const reverse_sequence_iterator<T>& in_rhs) const noexcept {
    return _ptr != in_rhs.get();
}

MBASE_STD_END

#endif // !MBASE_SEQUENCE_ITERATOR_H
