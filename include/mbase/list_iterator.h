#ifndef MBASE_LIST_ITERATOR_H
#define MBASE_LIST_ITERATOR_H

#include <mbase/common.h>

MBASE_STD_BEGIN

template<typename T, typename DataT>
class forward_list_iterator;

template<typename T, typename DataT>
class const_forward_list_iterator;

template<typename T, typename DataT>
class backward_list_iterator;

template<typename T, typename DataT>
class const_backward_list_iterator;

template<typename T, typename DataT>
class forward_list_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    MBASE_INLINE forward_list_iterator(pointer in_ptr) noexcept;
    MBASE_INLINE forward_list_iterator(const forward_list_iterator& in_rhs) noexcept;

    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE pointer get() const noexcept;
    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE DataT& operator*() const noexcept;
    MBASE_INLINE DataT* operator->() const noexcept;

    MBASE_ND("ignoring iterator index") MBASE_INLINE forward_list_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE forward_list_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE forward_list_iterator& operator++() noexcept;
    MBASE_INLINE forward_list_iterator operator++(int) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const forward_list_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const forward_list_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const const_forward_list_iterator<T, DataT>& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_forward_list_iterator<T, DataT>& in_rhs) const noexcept;

protected:
    pointer _ptr;
};

template<typename T, typename DataT>
class const_forward_list_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    const_forward_list_iterator(pointer in_ptr) noexcept;
    const_forward_list_iterator(const const_forward_list_iterator& in_rhs) noexcept;
    const_forward_list_iterator(const forward_list_iterator<T, DataT>& in_rhs) noexcept;

    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE const_pointer get() const noexcept;
    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE const DataT& operator*() const noexcept;
    MBASE_INLINE const DataT* operator->() const noexcept;

    MBASE_ND("ignoring iterator index") MBASE_INLINE const_forward_list_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE const_forward_list_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_forward_list_iterator& operator++() noexcept;
    MBASE_INLINE const_forward_list_iterator operator++(int) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const const_forward_list_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_forward_list_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const forward_list_iterator<T, DataT>& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const forward_list_iterator<T, DataT>& in_rhs) const noexcept;

protected:
    pointer _ptr;
};

template<typename T, typename DataT>
class backward_list_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    backward_list_iterator(pointer in_ptr) noexcept;
    backward_list_iterator(const backward_list_iterator& in_rhs) noexcept;

    MBASE_INLINE pointer get() const noexcept;
    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE DataT& operator*() const noexcept;
    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE DataT* operator->() const noexcept;

    MBASE_ND("ignoring iterator index") MBASE_INLINE backward_list_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE backward_list_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE backward_list_iterator& operator++() noexcept;
    MBASE_INLINE backward_list_iterator operator++(int) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const backward_list_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const backward_list_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const const_backward_list_iterator<T, DataT>& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_backward_list_iterator<T, DataT>& in_rhs) const noexcept;

protected:
    pointer _ptr;
};

template<typename T, typename DataT>
class const_backward_list_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    const_backward_list_iterator(pointer in_ptr) noexcept;
    const_backward_list_iterator(const const_backward_list_iterator& in_rhs) noexcept;
    const_backward_list_iterator(const backward_list_iterator<T, DataT>& in_rhs) noexcept;

    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE const_pointer get() const noexcept;
    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE const DataT& operator*() const noexcept;
    MBASE_INLINE const DataT* operator->() const noexcept;

    MBASE_ND("ignoring iterator index") MBASE_INLINE const_backward_list_iterator operator+(difference_type in_rhs) noexcept;
    MBASE_INLINE const_backward_list_iterator& operator+=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_backward_list_iterator& operator++() noexcept;
    MBASE_INLINE const_backward_list_iterator operator++(int) noexcept;

    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const const_backward_list_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_backward_list_iterator& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator==(const backward_list_iterator<T, DataT>& in_rhs) const noexcept;
    MBASE_ND("ignoring equality comparison") MBASE_INLINE bool operator!=(const backward_list_iterator<T, DataT>& in_rhs) const noexcept;

protected:
    pointer _ptr;
};

template<typename Type, typename DataT>
class bi_list_iterator : public forward_list_iterator<Type, DataT> {
public:
#if MBASE_CPP_VERSION >= 20
    using value_type = typename forward_list_iterator<Type, DataT>::value_type;
    using pointer = typename forward_list_iterator<Type, DataT>::pointer;
    using const_pointer = typename forward_list_iterator<Type, DataT>::const_pointer;
    using reference = typename forward_list_iterator<Type, DataT>::reference;
    using size_type = typename forward_list_iterator<Type, DataT>::size_type;
    using difference_type = typename forward_list_iterator<Type, DataT>::difference_type;
#endif
    using iterator_category = std::bidirectional_iterator_tag;

    bi_list_iterator(pointer in_ptr) noexcept;
    bi_list_iterator(const bi_list_iterator& in_rhs) noexcept;

    MBASE_INLINE bi_list_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE bi_list_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE bi_list_iterator& operator--() noexcept;
    MBASE_INLINE bi_list_iterator& operator--(int) noexcept;
};

template<typename Type, typename DataT>
class const_bi_list_iterator : public const_forward_list_iterator<Type, DataT> {
public:
#if MBASE_CPP_VERSION >= 20
    using value_type = typename const_forward_list_iterator<Type, DataT>::value_type;
    using pointer = typename const_forward_list_iterator<Type, DataT>::pointer;
    using const_pointer = typename const_forward_list_iterator<Type, DataT>::const_pointer;
    using reference = typename const_forward_list_iterator<Type, DataT>::reference;
    using size_type = typename const_forward_list_iterator<Type, DataT>::size_type;
    using difference_type = typename const_forward_list_iterator<Type, DataT>::difference_type;
#endif
    using iterator_category = std::bidirectional_iterator_tag;

    const_bi_list_iterator(pointer in_ptr) noexcept;
    const_bi_list_iterator(const const_bi_list_iterator& in_rhs) noexcept;
    const_bi_list_iterator(const bi_list_iterator<Type, DataT>& in_rhs) noexcept;

    MBASE_INLINE const_bi_list_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE const_bi_list_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_bi_list_iterator& operator--() noexcept;
    MBASE_INLINE const_bi_list_iterator& operator--(int) noexcept;
};

template<typename Type, typename DataT>
class reverse_bi_list_iterator : public backward_list_iterator<Type, DataT> {
public:
#if MBASE_CPP_VERSION >= 20
    using value_type = typename backward_list_iterator<Type, DataT>::value_type;
    using pointer = typename backward_list_iterator<Type, DataT>::pointer;
    using const_pointer = typename backward_list_iterator<Type, DataT>::const_pointer;
    using reference = typename backward_list_iterator<Type, DataT>::reference;
    using size_type = typename backward_list_iterator<Type, DataT>::size_type;
    using difference_type = typename backward_list_iterator<Type, DataT>::difference_type;
#endif
    using iterator_category = std::bidirectional_iterator_tag;

    reverse_bi_list_iterator(pointer in_ptr) noexcept;
    reverse_bi_list_iterator(const reverse_bi_list_iterator& in_rhs) noexcept;

    MBASE_INLINE reverse_bi_list_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_bi_list_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE reverse_bi_list_iterator& operator--() noexcept;
    MBASE_INLINE reverse_bi_list_iterator& operator--(int) noexcept;
};

template<typename Type, typename DataT>
class const_reverse_bi_list_iterator : public const_backward_list_iterator<Type, DataT> {
public:
#if MBASE_CPP_VERSION >= 20
    using value_type = typename const_backward_list_iterator<Type, DataT>::value_type;
    using pointer = typename const_backward_list_iterator<Type, DataT>::pointer;
    using const_pointer = typename const_backward_list_iterator<Type, DataT>::const_pointer;
    using reference = typename const_backward_list_iterator<Type, DataT>::reference;
    using size_type = typename const_backward_list_iterator<Type, DataT>::size_type;
    using difference_type = typename const_backward_list_iterator<Type, DataT>::difference_type;
#endif
    using iterator_category = std::bidirectional_iterator_tag;

    const_reverse_bi_list_iterator(pointer in_ptr) noexcept;
    const_reverse_bi_list_iterator(const const_reverse_bi_list_iterator& in_rhs) noexcept;
    const_reverse_bi_list_iterator(const reverse_bi_list_iterator<Type, DataT>& in_rhs) noexcept;

    MBASE_INLINE const_pointer get() const noexcept;

    MBASE_INLINE const_reverse_bi_list_iterator operator-(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_bi_list_iterator& operator-=(difference_type in_rhs) noexcept;
    MBASE_INLINE const_reverse_bi_list_iterator& operator--() noexcept;
    MBASE_INLINE const_reverse_bi_list_iterator& operator--(int) noexcept;
};

/* <-- FORWARD LIST ITERATOR IMPLEMENTATION --> */
template<typename T, typename DataT>
MBASE_INLINE forward_list_iterator<T, DataT>::forward_list_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T, typename DataT>
MBASE_INLINE forward_list_iterator<T, DataT>::forward_list_iterator(const forward_list_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr)
{
}

template<typename T, typename DataT>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE typename forward_list_iterator<T, DataT>::pointer forward_list_iterator<T, DataT>::get() const noexcept {
    return _ptr;
}

template<typename T, typename DataT>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE DataT& forward_list_iterator<T, DataT>::operator*() const noexcept {
    return _ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE DataT* forward_list_iterator<T, DataT>::operator->() const noexcept {
    return &_ptr->data;
}

template<typename T, typename DataT>
MBASE_ND("ignoring iterator index") MBASE_INLINE forward_list_iterator<T, DataT> forward_list_iterator<T, DataT>::operator+(difference_type in_rhs) noexcept {

    forward_list_iterator fli(*this);
    fli += in_rhs;
    return fli;
}

template<typename T, typename DataT>
MBASE_INLINE forward_list_iterator<T, DataT>& forward_list_iterator<T, DataT>::operator+=(difference_type in_rhs) noexcept {
    for (size_type i = 0; i < in_rhs; i++)
    {
        _ptr = _ptr->next;
    }
    return *this;
}

template<typename T, typename DataT>
MBASE_INLINE forward_list_iterator<T, DataT>& forward_list_iterator<T, DataT>::operator++() noexcept {
    if (!_ptr)
    {
        return *this;
    }

    _ptr = _ptr->next;
    return *this;
}

template<typename T, typename DataT>
MBASE_INLINE forward_list_iterator<T, DataT> forward_list_iterator<T, DataT>::operator++(int) noexcept {
    if (!_ptr)
    {
        return *this;
    }
    forward_list_iterator fli(*this);
    _ptr = _ptr->next;
    return fli;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool forward_list_iterator<T, DataT>::operator==(const forward_list_iterator& in_rhs) const noexcept {
    return _ptr == in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool forward_list_iterator<T, DataT>::operator!=(const forward_list_iterator& in_rhs) const noexcept {
    return _ptr != in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool forward_list_iterator<T, DataT>::operator==(const const_forward_list_iterator<T, DataT>& in_rhs) const noexcept {
    return _ptr == in_rhs.get();
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool forward_list_iterator<T, DataT>::operator!=(const const_forward_list_iterator<T, DataT>& in_rhs) const noexcept {
    return _ptr != in_rhs.get();
}

/* <-- CONST FORWARD LIST ITERATOR IMPLEMENTATION --> */
template<typename T, typename DataT>
const_forward_list_iterator<T, DataT>::const_forward_list_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T, typename DataT>
const_forward_list_iterator<T, DataT>::const_forward_list_iterator(const const_forward_list_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) 
{
}

template<typename T, typename DataT>
const_forward_list_iterator<T, DataT>::const_forward_list_iterator(const forward_list_iterator<T, DataT>& in_rhs) noexcept : _ptr(in_rhs.get()) 
{
}

template<typename T, typename DataT>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE typename const_forward_list_iterator<T, DataT>::const_pointer const_forward_list_iterator<T, DataT>::get() const noexcept {
    return _ptr;
}

template<typename T, typename DataT>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE const DataT& const_forward_list_iterator<T, DataT>::operator*() const noexcept {
    return _ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE const DataT* const_forward_list_iterator<T, DataT>::operator->() const noexcept {
    return &_ptr->data;
}

template<typename T, typename DataT>
MBASE_ND("ignoring iterator index") MBASE_INLINE const_forward_list_iterator<T, DataT> const_forward_list_iterator<T, DataT>::operator+(difference_type in_rhs) noexcept {
    const_forward_list_iterator cfli(*this);
    cfli += in_rhs;
    return cfli;
}

template<typename T, typename DataT>
MBASE_INLINE const_forward_list_iterator<T, DataT>& const_forward_list_iterator<T, DataT>::operator+=(difference_type in_rhs) noexcept {
    for (size_type i = 0; i < in_rhs; i++)
    {
        _ptr = _ptr->next;
    }
    return *this;
}

template<typename T, typename DataT>
MBASE_INLINE const_forward_list_iterator<T, DataT>& const_forward_list_iterator<T, DataT>::operator++() noexcept {
    if (!_ptr)
    {
        return *this;
    }

    _ptr = _ptr->next;
    return *this;
}

template<typename T, typename DataT>
MBASE_INLINE const_forward_list_iterator<T, DataT> const_forward_list_iterator<T, DataT>::operator++(int) noexcept {
    if (!_ptr)
    {
        return *this;
    }

    const_forward_list_iterator fli(*this);
    _ptr = _ptr->next;
    return fli;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_forward_list_iterator<T, DataT>::operator==(const const_forward_list_iterator& in_rhs) const noexcept {
    return _ptr == in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_forward_list_iterator<T, DataT>::operator!=(const const_forward_list_iterator& in_rhs) const noexcept {
    return _ptr != in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_forward_list_iterator<T, DataT>::operator==(const forward_list_iterator<T, DataT>& in_rhs) const noexcept {
    return _ptr == in_rhs.get();
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_forward_list_iterator<T, DataT>::operator!=(const forward_list_iterator<T, DataT>& in_rhs) const noexcept {
    return _ptr != in_rhs.get();
}

/* <-- REVERSE LIST ITERATOR IMPLEMENTATION --> */
template<typename T, typename DataT>
backward_list_iterator<T, DataT>::backward_list_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T, typename DataT>
backward_list_iterator<T, DataT>::backward_list_iterator(const backward_list_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}

template<typename T, typename DataT>
MBASE_INLINE typename backward_list_iterator<T, DataT>::pointer backward_list_iterator<T, DataT>::get() const noexcept {
    return _ptr;
}

template<typename T, typename DataT>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE DataT& backward_list_iterator<T, DataT>::operator*() const noexcept {
    return _ptr->data;
}

template<typename T, typename DataT>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE DataT* backward_list_iterator<T, DataT>::operator->() const noexcept {
    return &_ptr->data;
}

template<typename T, typename DataT>
MBASE_ND("ignoring iterator index") MBASE_INLINE backward_list_iterator<T, DataT> backward_list_iterator<T, DataT>::operator+(difference_type in_rhs) noexcept {
    backward_list_iterator bli(*this);
    bli += in_rhs;
    return bli;
}

template<typename T, typename DataT>
MBASE_INLINE backward_list_iterator<T, DataT>& backward_list_iterator<T, DataT>::operator+=(difference_type in_rhs) noexcept {
    for (size_type i = 0; i < in_rhs; i++)
    {
        _ptr = _ptr->prev;
    }
    return *this;
}

template<typename T, typename DataT>
MBASE_INLINE backward_list_iterator<T, DataT>& backward_list_iterator<T, DataT>::operator++() noexcept {
    if (!_ptr)
    {
        return *this;
    }

    _ptr = _ptr->prev;
    return *this;
}

template<typename T, typename DataT>
MBASE_INLINE backward_list_iterator<T, DataT> backward_list_iterator<T, DataT>::operator++(int) noexcept {
    if (!_ptr)
    {
        return *this;
    }
    backward_list_iterator fli(*this);
    _ptr = _ptr->prev;
    return fli;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool backward_list_iterator<T, DataT>::operator==(const backward_list_iterator& in_rhs) const noexcept {
    return _ptr == in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool backward_list_iterator<T, DataT>::operator!=(const backward_list_iterator& in_rhs) const noexcept {
    return _ptr != in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool backward_list_iterator<T, DataT>::operator==(const const_backward_list_iterator<T, DataT>& in_rhs) const noexcept {
    return _ptr == in_rhs.get();
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool backward_list_iterator<T, DataT>::operator!=(const const_backward_list_iterator<T, DataT>& in_rhs) const noexcept {
    return _ptr != in_rhs.get();
}

/* <-- CONST REVERSE LIST ITERATOR IMPLEMENTATION --> */
template<typename T, typename DataT>
const_backward_list_iterator<T, DataT>::const_backward_list_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) 
{
}

template<typename T, typename DataT>
const_backward_list_iterator<T, DataT>::const_backward_list_iterator(const const_backward_list_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr)
{
}

template<typename T, typename DataT>
const_backward_list_iterator<T, DataT>::const_backward_list_iterator(const backward_list_iterator<T, DataT>& in_rhs) noexcept : _ptr(in_rhs.get()) 
{
}

template<typename T, typename DataT>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE typename const_backward_list_iterator<T, DataT>::const_pointer const_backward_list_iterator<T, DataT>::get() const noexcept {
    return _ptr;
}

template<typename T, typename DataT>
MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE const DataT& const_backward_list_iterator<T, DataT>::operator*() const noexcept {
    return _ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE const DataT* const_backward_list_iterator<T, DataT>::operator->() const noexcept {
    return &_ptr->data;
}

template<typename T, typename DataT>
MBASE_ND("ignoring iterator index") MBASE_INLINE const_backward_list_iterator<T, DataT> const_backward_list_iterator<T, DataT>::operator+(difference_type in_rhs) noexcept {
    const_backward_list_iterator cbli(*this);
    cbli += in_rhs;
    return cbli;
}

template<typename T, typename DataT>
MBASE_INLINE const_backward_list_iterator<T, DataT>& const_backward_list_iterator<T, DataT>::operator+=(difference_type in_rhs) noexcept {
    for (size_type i = 0; i < in_rhs; i++)
    {
        _ptr = _ptr->prev;
    }
    return *this;
}

template<typename T, typename DataT>
MBASE_INLINE const_backward_list_iterator<T, DataT>& const_backward_list_iterator<T, DataT>::operator++() noexcept {
    if (!_ptr)
    {
        return *this;
    }

    _ptr = _ptr->prev;
    return *this;
}

template<typename T, typename DataT>
MBASE_INLINE const_backward_list_iterator<T, DataT> const_backward_list_iterator<T, DataT>::operator++(int) noexcept {
    if (!_ptr)
    {
        return *this;
    }
    const_backward_list_iterator cbli(*this);
    _ptr = _ptr->prev;
    return cbli;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_backward_list_iterator<T, DataT>::operator==(const const_backward_list_iterator& in_rhs) const noexcept {
    return _ptr == in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_backward_list_iterator<T, DataT>::operator!=(const const_backward_list_iterator& in_rhs) const noexcept {
    return _ptr != in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_backward_list_iterator<T, DataT>::operator==(const backward_list_iterator<T, DataT>& in_rhs) const noexcept {
    return _ptr == in_rhs.get();
}

template<typename T, typename DataT>
MBASE_ND("ignoring equality comparison") MBASE_INLINE bool const_backward_list_iterator<T, DataT>::operator!=(const backward_list_iterator<T, DataT>& in_rhs) const noexcept {
    return _ptr != in_rhs.get();
}

/* <-- BIDIRECTIONAL LIST ITERATOR IMPLEMENTATION -->*/
template<typename Type, typename DataT>
bi_list_iterator<Type, DataT>::bi_list_iterator(pointer in_ptr) noexcept : forward_list_iterator<Type, DataT>(in_ptr) 
{
}

template<typename Type, typename DataT>
bi_list_iterator<Type, DataT>::bi_list_iterator(const bi_list_iterator& in_rhs) noexcept : forward_list_iterator<Type, DataT>(in_rhs._ptr)
{
}

template<typename Type, typename DataT>
MBASE_INLINE bi_list_iterator<Type, DataT> bi_list_iterator<Type, DataT>::operator-(difference_type in_rhs) noexcept {
    bi_list_iterator bli(*this);
    bli -= in_rhs;
    return bli;
}

template<typename Type, typename DataT>
MBASE_INLINE bi_list_iterator<Type, DataT>& bi_list_iterator<Type, DataT>::operator-=(difference_type in_rhs) noexcept {
    for (size_type i = 0; i < in_rhs; i++)
    {
        this->_ptr = this->_ptr->prev;
    }
    return *this;
}

template<typename Type, typename DataT>
MBASE_INLINE bi_list_iterator<Type, DataT>& bi_list_iterator<Type, DataT>::operator--() noexcept {
    this->_ptr = this->_ptr->prev;
    return *this;
}

template<typename Type, typename DataT>
MBASE_INLINE bi_list_iterator<Type, DataT>& bi_list_iterator<Type, DataT>::operator--(int) noexcept {
    bi_list_iterator bi(*this);
    this->_ptr = this->_ptr->prev;
    return bi;
}

/* <-- CONST BIDIRECTIONAL LIST ITERATOR IMPLEMENTATION -->*/
template<typename Type, typename DataT>
const_bi_list_iterator<Type, DataT>::const_bi_list_iterator(pointer in_ptr) noexcept : const_forward_list_iterator<Type, DataT>(in_ptr) 
{
}

template<typename Type, typename DataT>
const_bi_list_iterator<Type, DataT>::const_bi_list_iterator(const const_bi_list_iterator& in_rhs) noexcept : const_forward_list_iterator<Type, DataT>(in_rhs._ptr) 
{
}

template<typename Type, typename DataT>
const_bi_list_iterator<Type, DataT>::const_bi_list_iterator(const bi_list_iterator<Type, DataT>& in_rhs) noexcept : const_forward_list_iterator<Type, DataT>(in_rhs.get()) 
{
}

template<typename Type, typename DataT>
MBASE_INLINE const_bi_list_iterator<Type, DataT> const_bi_list_iterator<Type, DataT>::operator-(difference_type in_rhs) noexcept {
    const_bi_list_iterator bli(*this);
    bli -= in_rhs;
    return bli;
}

template<typename Type, typename DataT>
MBASE_INLINE const_bi_list_iterator<Type, DataT>& const_bi_list_iterator<Type, DataT>::operator-=(difference_type in_rhs) noexcept {
    for (size_type i = 0; i < in_rhs; i++)
    {
        this->_ptr = this->_ptr->prev;
    }
    return *this;
}

template<typename Type, typename DataT>
MBASE_INLINE const_bi_list_iterator<Type, DataT>& const_bi_list_iterator<Type, DataT>::operator--() noexcept {
    this->_ptr = this->_ptr->prev;
    return *this;
}

template<typename Type, typename DataT>
MBASE_INLINE const_bi_list_iterator<Type, DataT>& const_bi_list_iterator<Type, DataT>::operator--(int) noexcept {
    this->_ptr = this->_ptr->prev;
    return *this;
}

/* <-- REVERSE BIDIRECTIONAL LIST ITERATOR IMPLEMENTATION -->*/
template<typename Type, typename DataT>
reverse_bi_list_iterator<Type, DataT>::reverse_bi_list_iterator(pointer in_ptr) noexcept : backward_list_iterator<Type, DataT>(in_ptr) 
{
}

template<typename Type, typename DataT>
reverse_bi_list_iterator<Type, DataT>::reverse_bi_list_iterator(const reverse_bi_list_iterator& in_rhs) noexcept : backward_list_iterator<Type, DataT>(in_rhs._ptr) 
{
}

template<typename Type, typename DataT>
MBASE_INLINE reverse_bi_list_iterator<Type, DataT> reverse_bi_list_iterator<Type, DataT>::operator-(difference_type in_rhs) noexcept {
    reverse_bi_list_iterator bli(*this);
    bli -= in_rhs;
    return bli;
}

template<typename Type, typename DataT>
MBASE_INLINE reverse_bi_list_iterator<Type, DataT>& reverse_bi_list_iterator<Type, DataT>::operator-=(difference_type in_rhs) noexcept {
    for (size_type i = 0; i < in_rhs; i++)
    {
        this->_ptr = this->_ptr->next;
    }
    return *this;
}

template<typename Type, typename DataT>
MBASE_INLINE reverse_bi_list_iterator<Type, DataT>& reverse_bi_list_iterator<Type, DataT>::operator--() noexcept {
    this->_ptr = this->_ptr->next;
    return *this;
}

template<typename Type, typename DataT>
MBASE_INLINE reverse_bi_list_iterator<Type, DataT>& reverse_bi_list_iterator<Type, DataT>::operator--(int) noexcept {
    this->_ptr = this->_ptr->next;
    return *this;
}

/* <-- CONST REVERSE BIDIRECTIONAL LIST ITERATOR IMPLEMENTATION -->*/
template<typename Type, typename DataT>
const_reverse_bi_list_iterator<Type, DataT>::const_reverse_bi_list_iterator(pointer in_ptr) noexcept : const_backward_list_iterator<Type, DataT>(in_ptr) 
{
}

template<typename Type, typename DataT>
const_reverse_bi_list_iterator<Type, DataT>::const_reverse_bi_list_iterator(const const_reverse_bi_list_iterator& in_rhs) noexcept : const_backward_list_iterator<Type, DataT>(in_rhs._ptr) 
{
}

template<typename Type, typename DataT>
const_reverse_bi_list_iterator<Type, DataT>::const_reverse_bi_list_iterator(const reverse_bi_list_iterator<Type, DataT>& in_rhs) noexcept : const_backward_list_iterator<Type, DataT>(in_rhs.get()) 
{
}

template<typename Type, typename DataT>
MBASE_INLINE const_reverse_bi_list_iterator<Type, DataT> const_reverse_bi_list_iterator<Type, DataT>::operator-(difference_type in_rhs) noexcept {
    const_reverse_bi_list_iterator bli(*this);
    bli -= in_rhs;
    return bli;
}

template<typename Type, typename DataT>
MBASE_INLINE const_reverse_bi_list_iterator<Type, DataT>& const_reverse_bi_list_iterator<Type, DataT>::operator-=(difference_type in_rhs) noexcept {
    for (size_type i = 0; i < in_rhs; i++)
    {
        this->_ptr = this->_ptr->next;
    }
    return *this;
}

template<typename Type, typename DataT>
MBASE_INLINE typename const_reverse_bi_list_iterator<Type, DataT>::const_pointer const_reverse_bi_list_iterator<Type, DataT>::get() const noexcept {
    return this->_ptr;
}

template<typename Type, typename DataT>
MBASE_INLINE const_reverse_bi_list_iterator<Type, DataT>& const_reverse_bi_list_iterator<Type, DataT>::operator--() noexcept {
    this->_ptr = this->_ptr->next;
    return *this;
}

template<typename Type, typename DataT>
MBASE_INLINE const_reverse_bi_list_iterator<Type, DataT>& const_reverse_bi_list_iterator<Type, DataT>::operator--(int) noexcept {
    this->_ptr = this->_ptr->next;
    return *this;
}

MBASE_STD_END

#endif // MBASE_LIST_ITERATOR_H

