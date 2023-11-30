#ifndef MBASE_LIST_ITERATOR_H
#define MBASE_LIST_ITERATOR_H

#include <mbase/common.h>

MBASE_STD_BEGIN

template<typename T, typename DataT>
class forward_list_iterator {
public:
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    forward_list_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}
    forward_list_iterator(const forward_list_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}

    USED_RETURN MBASE_INLINE DataT& operator*() const noexcept {
        return *_ptr->data;
    }

    MBASE_INLINE DataT* operator->() const noexcept {
        return _ptr->data;
    }

    USED_RETURN MBASE_INLINE forward_list_iterator& operator+(difference_type in_rhs) noexcept {
        
        for(size_type i = 0; i < in_rhs; i++)
        {
            _ptr = _ptr->next;
        }
        return *this;
    }

    MBASE_INLINE forward_list_iterator& operator+=(difference_type in_rhs) noexcept {
        for (size_type i = 0; i < in_rhs; i++)
        {
            _ptr = _ptr->next;
        }
        return *this;
    }

    MBASE_INLINE forward_list_iterator& operator++() noexcept {
        _ptr = _ptr->next;
        return *this;
    }

    MBASE_INLINE forward_list_iterator& operator++(int) noexcept {
        _ptr = _ptr->next;
        return *this;
    }

    USED_RETURN MBASE_INLINE bool operator==(const forward_list_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN MBASE_INLINE bool operator!=(const forward_list_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

protected:
    pointer _ptr;
};

MBASE_STD_END

#endif // MBASE_LIST_ITERATOR_H

