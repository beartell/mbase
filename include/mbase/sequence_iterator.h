#ifndef MBASE_SEQUENCE_ITERATOR_H
#define MBASE_SEQUENCE_ITERATOR_H

#include <mbase/common.h>
#include <iterator>

MBASE_STD_BEGIN

/*

    --- CLASS INFORMATION ---
Identification: S0C33-OBJ-UD-ST

Name: safe_buffer

Parent: None

Behaviour List:
- Default Constructible
- Destructible
- Move Constructible
- Move Assignable

Description:

*/

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

    /* ===== BUILDER METHODS BEGIN ===== */
    sequence_iterator() noexcept;
    sequence_iterator(pointer in_ptr) noexcept;
    sequence_iterator(const sequence_iterator& in_rhs) noexcept;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OPERATOR BUILDER METHODS BEGIN ===== */
    sequence_iterator& operator=(const sequence_iterator& in_rhs) noexcept;
    /* ===== OPERATOR BUILDER METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE pointer get() noexcept;
    MBASE_INLINE pointer operator->() noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE reference operator*() noexcept;
    MBASE_INLINE reference operator[](difference_type in_index);
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE sequence_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE sequence_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE sequence_iterator& operator++() noexcept;
    MBASE_INLINE sequence_iterator operator++(int) noexcept;
    MBASE_INLINE sequence_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE sequence_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE sequence_iterator& operator--() noexcept;
    MBASE_INLINE sequence_iterator operator--(int) noexcept;
    MBASE_INLINE difference_type operator-(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE difference_type operator-(const const_sequence_iterator<T>& in_rhs) noexcept;
    /* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

    /* ===== NON-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const sequence_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const sequence_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const const_sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const const_sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_INLINE bool operator<(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<(const const_sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const const_sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const const_sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const const_sequence_iterator<T>& in_rhs) noexcept;
    /* ===== NON-MODIFIER METHODS END ===== */

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

    /* ===== BUILDER METHODS BEGIN ===== */
    const_sequence_iterator() noexcept;
    const_sequence_iterator(pointer in_ptr) noexcept;
    const_sequence_iterator(const const_sequence_iterator& in_rhs) noexcept;
    const_sequence_iterator(sequence_iterator<T> in_rhs) noexcept;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OPERATOR BUILDER METHODS BEGIN ===== */
    const_sequence_iterator& operator=(const const_sequence_iterator& in_rhs) noexcept;
    const_sequence_iterator& operator=(sequence_iterator<T> in_rhs) noexcept;
    /* ===== OPERATOR BUILDER METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_pointer get() const noexcept;
    MBASE_INLINE const_pointer operator->() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reference operator*() const noexcept;
    MBASE_INLINE const_reference operator[](difference_type in_index) const;
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE const_sequence_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE const_sequence_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_sequence_iterator& operator++() noexcept;
    MBASE_INLINE const_sequence_iterator operator++(int) noexcept;
    MBASE_INLINE const_sequence_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE const_sequence_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_sequence_iterator& operator--() noexcept;
    MBASE_INLINE const_sequence_iterator operator--(int) noexcept;
    MBASE_INLINE difference_type operator-(const const_sequence_iterator& in_rhs) noexcept;
    /* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

    /* ===== NON-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const const_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const const_sequence_iterator& in_rhs) const noexcept;
    MBASE_INLINE bool operator<(const const_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const const_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const const_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const const_sequence_iterator& in_rhs) noexcept;
    /* ===== NON-MODIFIER METHODS BEGIN ===== */
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

    /* ===== BUILDER METHODS BEGIN ===== */
    reverse_sequence_iterator() noexcept;
    reverse_sequence_iterator(pointer in_ptr) noexcept;
    reverse_sequence_iterator(const reverse_sequence_iterator& in_rhs) noexcept;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OPERATOR BUILDER METHODS BEGIN ===== */
    reverse_sequence_iterator& operator=(const reverse_sequence_iterator& in_rhs) noexcept;
    /* ===== OPERATOR BUILDER METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE pointer get() noexcept;
    MBASE_INLINE pointer operator->() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE reference operator*() const noexcept;
    MBASE_INLINE reference operator[](difference_type in_index);
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE reverse_sequence_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_sequence_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_sequence_iterator& operator++() noexcept;
    MBASE_INLINE reverse_sequence_iterator operator++(int) noexcept;
    MBASE_INLINE reverse_sequence_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_sequence_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_sequence_iterator& operator--() noexcept;
    MBASE_INLINE reverse_sequence_iterator operator--(int) noexcept;
    /* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

    /* ===== NON-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const reverse_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const reverse_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept;
    MBASE_INLINE bool operator<(const reverse_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const reverse_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const reverse_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const reverse_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<(const const_reverse_sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const const_reverse_sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const const_reverse_sequence_iterator<T>& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const const_reverse_sequence_iterator<T>& in_rhs) noexcept;
    /* ===== NON-MODIFIER METHODS END ===== */

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

    /* ===== BUILDER METHODS BEGIN ===== */
    const_reverse_sequence_iterator() noexcept;
    const_reverse_sequence_iterator(pointer in_ptr) noexcept;
    const_reverse_sequence_iterator(const const_reverse_sequence_iterator& in_rhs) noexcept;
    const_reverse_sequence_iterator(reverse_sequence_iterator<T> in_rhs) noexcept;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OPERATOR BUILDER METHODS BEGIN ===== */
    const_reverse_sequence_iterator& operator=(const const_reverse_sequence_iterator& in_rhs) noexcept;
    const_reverse_sequence_iterator& operator=(reverse_sequence_iterator<T> in_rhs) noexcept;
    /* ===== OPERATOR BUILDER METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_pointer get() noexcept;
    MBASE_INLINE const_pointer operator->() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reference operator*() const noexcept;
    MBASE_INLINE const_reference operator[](difference_type in_index);
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE const_reverse_sequence_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator& operator++() noexcept;
    MBASE_INLINE const_reverse_sequence_iterator operator++(int) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_sequence_iterator& operator--() noexcept;
    MBASE_INLINE const_reverse_sequence_iterator operator--(int) noexcept;
    /* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

    /* ===== NON-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const const_reverse_sequence_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const const_reverse_sequence_iterator& in_rhs) const noexcept;
    MBASE_INLINE bool operator<(const const_reverse_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>(const const_reverse_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator>=(const const_reverse_sequence_iterator& in_rhs) noexcept;
    MBASE_INLINE bool operator<=(const const_reverse_sequence_iterator& in_rhs) noexcept;
    /* ===== NON-MODIFIER METHODS END ===== */

protected:
    pointer _ptr;
};

/* <-- SEQUENCE ITERATOR IMPLEMENTATION --> */
template<typename T>
sequence_iterator<T>::sequence_iterator() noexcept : _ptr(nullptr)
{
}

template<typename T>
sequence_iterator<T>::sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T>
sequence_iterator<T>::sequence_iterator(const sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) 
{
}

template<typename T>
sequence_iterator<T>& sequence_iterator<T>::operator=(const sequence_iterator& in_rhs) noexcept 
{
    _ptr = in_rhs._ptr;
    return *this;
}

template<typename T>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename sequence_iterator<T>::pointer sequence_iterator<T>::get() noexcept
{
    return _ptr;
}

template<typename T>
MBASE_INLINE  typename sequence_iterator<T>::pointer sequence_iterator<T>::operator->() noexcept 
{
    return _ptr;
}

template<typename T>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename sequence_iterator<T>::reference sequence_iterator<T>::operator*() noexcept 
{
    return *_ptr;
}

template<typename T>
MBASE_INLINE typename sequence_iterator<T>::reference sequence_iterator<T>::operator[](difference_type in_index) 
{
    return *(_ptr + in_index);
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE sequence_iterator<T> sequence_iterator<T>::operator+(difference_type in_rhs) noexcept 
{
    sequence_iterator sit(_ptr);
    sit += in_rhs;
    return sit;
}

template<typename T>
MBASE_INLINE sequence_iterator<T>& sequence_iterator<T>::operator+=(difference_type in_rhs) noexcept 
{
    _ptr += in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE sequence_iterator<T>& sequence_iterator<T>::operator++() noexcept 
{
    ++_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE sequence_iterator<T> sequence_iterator<T>::operator++(int) noexcept 
{
    sequence_iterator si(*this);
    ++(*this);
    return si;
}

template<typename T>
MBASE_INLINE sequence_iterator<T> sequence_iterator<T>::operator-(difference_type in_rhs) noexcept 
{
    sequence_iterator sit(_ptr);
    sit -= in_rhs;
    return sit;
}

template<typename T>
MBASE_INLINE sequence_iterator<T>& sequence_iterator<T>::operator-=(difference_type in_rhs) noexcept 
{
    _ptr -= in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE sequence_iterator<T>& sequence_iterator<T>::operator--() noexcept 
{
    --_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE sequence_iterator<T> sequence_iterator<T>::operator--(int) noexcept 
{
    sequence_iterator si(*this);
    --(*this);
    return si;
}

template<typename T>
MBASE_INLINE typename sequence_iterator<T>::difference_type sequence_iterator<T>::operator-(const sequence_iterator& in_rhs) noexcept 
{
    return static_cast<difference_type>(_ptr - in_rhs._ptr);
}

template<typename T>
MBASE_INLINE typename sequence_iterator<T>::difference_type sequence_iterator<T>::operator-(const const_sequence_iterator<T>& in_rhs) noexcept 
{
    return static_cast<difference_type>(_ptr - in_rhs.get());
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool sequence_iterator<T>::operator==(const sequence_iterator& in_rhs) const noexcept 
{
    return _ptr == in_rhs._ptr;
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool sequence_iterator<T>::operator!=(const sequence_iterator& in_rhs) const noexcept 
{
    return _ptr != in_rhs._ptr;
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool sequence_iterator<T>::operator==(const const_sequence_iterator<T>& in_rhs) const noexcept 
{
    return _ptr == in_rhs.get();
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool sequence_iterator<T>::operator!=(const const_sequence_iterator<T>& in_rhs) const noexcept 
{
    return _ptr != in_rhs.get();
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator<(const sequence_iterator& in_rhs) noexcept 
{
    return (_ptr < in_rhs._ptr);
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator>(const sequence_iterator& in_rhs) noexcept 
{
    return (_ptr > in_rhs._ptr);
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator>=(const sequence_iterator& in_rhs) noexcept 
{
    return (_ptr >= in_rhs._ptr);
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator<=(const sequence_iterator& in_rhs) noexcept 
{
    return (_ptr <= in_rhs._ptr);
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator<(const const_sequence_iterator<T>& in_rhs) noexcept 
{
    return (_ptr < in_rhs.get());
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator>(const const_sequence_iterator<T>& in_rhs) noexcept 
{
    return (_ptr > in_rhs.get());
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator>=(const const_sequence_iterator<T>& in_rhs) noexcept 
{
    return (_ptr >= in_rhs.get());
}

template<typename T>
MBASE_INLINE bool sequence_iterator<T>::operator<=(const const_sequence_iterator<T>& in_rhs) noexcept 
{
    return (_ptr <= in_rhs.get());
}

/* <-- CONST SEQUENCE ITERATOR IMPLEMENTATION --> */

template<typename T>
const_sequence_iterator<T>::const_sequence_iterator() noexcept : _ptr(nullptr)
{
}

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
const_sequence_iterator<T>& const_sequence_iterator<T>::operator=(const const_sequence_iterator& in_rhs) noexcept
{
    _ptr = in_rhs._ptr;
    return *this;
}

template<typename T>
const_sequence_iterator<T>& const_sequence_iterator<T>::operator=(sequence_iterator<T> in_rhs) noexcept
{
    _ptr = in_rhs.get();
    return *this;
}

template<typename T>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename const_sequence_iterator<T>::const_pointer const_sequence_iterator<T>::get() const noexcept 
{
    return _ptr;
}

template<typename T>
MBASE_INLINE typename const_sequence_iterator<T>::const_pointer const_sequence_iterator<T>::operator->() const noexcept 
{
    return _ptr;
}

template<typename T>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename const_sequence_iterator<T>::const_reference const_sequence_iterator<T>::operator*() const noexcept 
{
    return *_ptr;
}

template<typename T>
MBASE_INLINE typename const_sequence_iterator<T>::const_reference const_sequence_iterator<T>::operator[](difference_type in_index) const 
{
    return *(_ptr + in_index);
}


template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE const_sequence_iterator<T> const_sequence_iterator<T>::operator+(difference_type in_rhs) noexcept
{
    const_sequence_iterator csit(_ptr);
    csit += in_rhs;
    return csit;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T>& const_sequence_iterator<T>::operator+=(difference_type in_rhs) noexcept 
{
    _ptr += in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T>& const_sequence_iterator<T>::operator++() noexcept
{
    ++_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T> const_sequence_iterator<T>::operator++(int) noexcept
{
    const_sequence_iterator csi(*this);
    ++(*this);
    return csi;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T> const_sequence_iterator<T>::operator-(difference_type in_rhs) noexcept
{
    const_sequence_iterator csit(_ptr);
    csit -= in_rhs;
    return csit;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T>& const_sequence_iterator<T>::operator-=(difference_type in_rhs) noexcept
{
    _ptr -= in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T>& const_sequence_iterator<T>::operator--() noexcept 
{
    --_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const_sequence_iterator<T> const_sequence_iterator<T>::operator--(int) noexcept 
{
    const_sequence_iterator csi(*this);
    --(*this);
    return csi;
}

template<typename T>
MBASE_INLINE typename const_sequence_iterator<T>::difference_type const_sequence_iterator<T>::operator-(const const_sequence_iterator& in_rhs) noexcept
{
    return static_cast<difference_type>(_ptr - in_rhs.get());
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool const_sequence_iterator<T>::operator==(const const_sequence_iterator& in_rhs) const noexcept
{
    return _ptr == in_rhs._ptr;
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool const_sequence_iterator<T>::operator!=(const const_sequence_iterator& in_rhs) const noexcept
{
    return _ptr != in_rhs._ptr;
}

template<typename T>
MBASE_INLINE bool const_sequence_iterator<T>::operator<(const const_sequence_iterator& in_rhs) noexcept 
{
    return (_ptr < in_rhs._ptr);
}

template<typename T>
MBASE_INLINE bool const_sequence_iterator<T>::operator>(const const_sequence_iterator& in_rhs) noexcept 
{
    return (_ptr > in_rhs._ptr);
}

template<typename T>
MBASE_INLINE bool const_sequence_iterator<T>::operator>=(const const_sequence_iterator& in_rhs) noexcept 
{
    return (_ptr >= in_rhs._ptr);
}

template<typename T>
MBASE_INLINE bool const_sequence_iterator<T>::operator<=(const const_sequence_iterator& in_rhs) noexcept 
{
    return (_ptr <= in_rhs._ptr);
}

/* <-- REVERSE SEQUENCE ITERATOR IMPLEMENTATION --> */
template<typename T>
reverse_sequence_iterator<T>::reverse_sequence_iterator() noexcept : _ptr(nullptr)
{
}

template<typename T>
reverse_sequence_iterator<T>::reverse_sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T>
reverse_sequence_iterator<T>::reverse_sequence_iterator(const reverse_sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) 
{
}

template<typename T>
reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator=(const reverse_sequence_iterator& in_rhs) noexcept
{
    _ptr = in_rhs._ptr;
    return *this;
}

template<typename T>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename reverse_sequence_iterator<T>::pointer reverse_sequence_iterator<T>::get() noexcept 
{
    return _ptr;
}

template<typename T>
MBASE_INLINE typename reverse_sequence_iterator<T>::pointer reverse_sequence_iterator<T>::operator->() const noexcept 
{
    return _ptr;
}

template<typename T>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename reverse_sequence_iterator<T>::reference reverse_sequence_iterator<T>::operator*() const noexcept 
{
    return *_ptr;
}

template<typename T>
MBASE_INLINE typename reverse_sequence_iterator<T>::reference reverse_sequence_iterator<T>::operator[](difference_type in_index) 
{
    return *(_ptr - in_index);
}


template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE reverse_sequence_iterator<T> reverse_sequence_iterator<T>::operator+(difference_type in_rhs) noexcept 
{
    reverse_sequence_iterator rsi(_ptr);
    rsi += in_rhs;
    return rsi;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator+=(difference_type in_rhs) noexcept
{
    _ptr -= in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator++() noexcept
{
    --_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T> reverse_sequence_iterator<T>::operator++(int) noexcept
{
    reverse_sequence_iterator rsi(*this);
    ++(*this);
    return rsi;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T> reverse_sequence_iterator<T>::operator-(difference_type in_rhs) noexcept
{
    reverse_sequence_iterator rsi(_ptr);
    rsi -= in_rhs;
    return rsi;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator-=(difference_type in_rhs) noexcept 
{
    _ptr += in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T>& reverse_sequence_iterator<T>::operator--() noexcept
{
    ++_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE reverse_sequence_iterator<T> reverse_sequence_iterator<T>::operator--(int) noexcept 
{
    reverse_sequence_iterator rsi(*this);
    --(*this);
    return rsi;
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool reverse_sequence_iterator<T>::operator==(const reverse_sequence_iterator& in_rhs) const noexcept
{
    return _ptr == in_rhs._ptr;
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool reverse_sequence_iterator<T>::operator!=(const reverse_sequence_iterator& in_rhs) const noexcept
{
    return _ptr != in_rhs._ptr;
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool reverse_sequence_iterator<T>::operator==(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept
{
    return _ptr == in_rhs.get();
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool reverse_sequence_iterator<T>::operator!=(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept 
{
    return _ptr != in_rhs.get();
}

template<typename T>
MBASE_INLINE bool reverse_sequence_iterator<T>::operator<(const reverse_sequence_iterator& in_rhs) noexcept
{
    return (in_rhs._ptr < _ptr);
}

template<typename T>
MBASE_INLINE bool reverse_sequence_iterator<T>::operator>(const reverse_sequence_iterator& in_rhs) noexcept
{
    return (in_rhs._ptr > _ptr);
}

template<typename T>
MBASE_INLINE bool reverse_sequence_iterator<T>::operator>=(const reverse_sequence_iterator& in_rhs) noexcept
{
    return (in_rhs._ptr >= _ptr);
}

template<typename T>
MBASE_INLINE bool reverse_sequence_iterator<T>::operator<=(const reverse_sequence_iterator& in_rhs) noexcept
{
    return (in_rhs._ptr <= _ptr);
}

template<typename T>
MBASE_INLINE bool reverse_sequence_iterator<T>::operator<(const const_reverse_sequence_iterator<T>& in_rhs) noexcept
{
    return (in_rhs.get() < _ptr);
}

template<typename T>
MBASE_INLINE bool reverse_sequence_iterator<T>::operator>(const const_reverse_sequence_iterator<T>& in_rhs) noexcept
{
    return (in_rhs.get() > _ptr);
}

template<typename T>
MBASE_INLINE bool reverse_sequence_iterator<T>::operator>=(const const_reverse_sequence_iterator<T>& in_rhs) noexcept
{
    return (in_rhs.get() >= _ptr);
}

template<typename T>
MBASE_INLINE bool reverse_sequence_iterator<T>::operator<=(const const_reverse_sequence_iterator<T>& in_rhs) noexcept
{
    return (in_rhs.get() <= _ptr);
}

/* <-- CONST REVERSE SEQUENCE ITERATOR IMPLEMENTATION --> */

template<typename T>
const_reverse_sequence_iterator<T>::const_reverse_sequence_iterator() noexcept : _ptr(nullptr)
{
}

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
const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator=(const const_reverse_sequence_iterator& in_rhs) noexcept
{
    _ptr = in_rhs._ptr;
    return *this;
}

template<typename T>
const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator=(reverse_sequence_iterator<T> in_rhs) noexcept 
{
    _ptr = in_rhs.get();
    return *this;
}

template<typename T>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename const_reverse_sequence_iterator<T>::const_pointer const_reverse_sequence_iterator<T>::get() noexcept 
{
    return _ptr;
}

template<typename T>
MBASE_INLINE typename const_reverse_sequence_iterator<T>::const_pointer const_reverse_sequence_iterator<T>::operator->() const noexcept 
{
    return _ptr;
}

template<typename T>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename const_reverse_sequence_iterator<T>::const_reference const_reverse_sequence_iterator<T>::operator*() const noexcept 
{
    return *_ptr;
}

template<typename T>
MBASE_INLINE typename const_reverse_sequence_iterator<T>::const_reference const_reverse_sequence_iterator<T>::operator[](difference_type in_index) 
{
    return *(_ptr - in_index);
}


template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE const_reverse_sequence_iterator<T> const_reverse_sequence_iterator<T>::operator+(difference_type in_rhs) noexcept
{
    const_reverse_sequence_iterator rsi(_ptr);
    rsi += in_rhs;
    return rsi;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator+=(difference_type in_rhs) noexcept
{
    _ptr -= in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator++() noexcept 
{
    --_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T> const_reverse_sequence_iterator<T>::operator++(int) noexcept 
{
    const_reverse_sequence_iterator crsi(*this);
    ++(*this);
    return crsi;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T> const_reverse_sequence_iterator<T>::operator-(difference_type in_rhs) noexcept
{
    const_reverse_sequence_iterator rsi(_ptr);
    rsi -= in_rhs;
    return rsi;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator-=(difference_type in_rhs) noexcept
{
    _ptr += in_rhs;
    return *this;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T>& const_reverse_sequence_iterator<T>::operator--() noexcept 
{
    ++_ptr;
    return *this;
}

template<typename T>
MBASE_INLINE const_reverse_sequence_iterator<T> const_reverse_sequence_iterator<T>::operator--(int) noexcept 
{
    const_reverse_sequence_iterator crsi(*this);
    --(*this);
    return crsi;

}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator==(const const_reverse_sequence_iterator& in_rhs) const noexcept 
{
    return _ptr == in_rhs._ptr;
}

template<typename T>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator!=(const const_reverse_sequence_iterator& in_rhs) const noexcept
{
    return _ptr != in_rhs._ptr;
}

template<typename T>
MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator<(const const_reverse_sequence_iterator& in_rhs) noexcept
{
    return (in_rhs._ptr < _ptr);
}

template<typename T>
MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator>(const const_reverse_sequence_iterator& in_rhs) noexcept
{
    return (in_rhs._ptr > _ptr);
}

template<typename T>
MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator>=(const const_reverse_sequence_iterator& in_rhs) noexcept
{
    return (in_rhs._ptr >= _ptr);
}

template<typename T>
MBASE_INLINE bool const_reverse_sequence_iterator<T>::operator<=(const const_reverse_sequence_iterator& in_rhs) noexcept
{
    return (in_rhs._ptr <= _ptr);
}

MBASE_STD_END

#endif // !MBASE_SEQUENCE_ITERATOR_H
