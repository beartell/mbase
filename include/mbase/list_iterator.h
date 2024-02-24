#ifndef MBASE_LIST_ITERATOR_H
#define MBASE_LIST_ITERATOR_H

#include <mbase/common.h>
#include <iterator>

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

    forward_list_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}
    forward_list_iterator(const forward_list_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE pointer get() const noexcept {
        return _ptr;
    }

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE DataT& operator*() const noexcept {
        return *_ptr->data;
    }

    MBASE_INLINE DataT* operator->() const noexcept {
        return _ptr->data;
    }

    USED_RETURN("ignoring iterator index") MBASE_INLINE forward_list_iterator& operator+(difference_type in_rhs) noexcept {

        for (size_type i = 0; i < in_rhs; i++)
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
        if (!_ptr)
        {
            return *this;
        }

        _ptr = _ptr->next;
        return *this;
    }

    MBASE_INLINE forward_list_iterator& operator++(int) noexcept {
        if (!_ptr)
        {
            return *this;
        }

        _ptr = _ptr->next;
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const forward_list_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const forward_list_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const const_forward_list_iterator<T, DataT>& in_rhs) const noexcept {
        return _ptr == in_rhs.get();
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_forward_list_iterator<T, DataT>& in_rhs) const noexcept {
        return _ptr != in_rhs.get();
    }

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

    const_forward_list_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}
    const_forward_list_iterator(const const_forward_list_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}
    const_forward_list_iterator(const forward_list_iterator<T, DataT>& in_rhs) noexcept : _ptr(in_rhs.get()) {}

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE const_pointer get() const noexcept {
        return _ptr;
    }

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE const DataT& operator*() const noexcept {
        return *_ptr->data;
    }

    MBASE_INLINE const DataT* operator->() const noexcept {
        return _ptr->data;
    }

    USED_RETURN("ignoring iterator index") MBASE_INLINE const_forward_list_iterator& operator+(difference_type in_rhs) noexcept {

        for (size_type i = 0; i < in_rhs; i++)
        {
            _ptr = _ptr->next;
        }
        return *this;
    }

    MBASE_INLINE const_forward_list_iterator& operator+=(difference_type in_rhs) noexcept {
        for (size_type i = 0; i < in_rhs; i++)
        {
            _ptr = _ptr->next;
        }
        return *this;
    }

    MBASE_INLINE const_forward_list_iterator& operator++() noexcept {
        if (!_ptr)
        {
            return *this;
        }

        _ptr = _ptr->next;
        return *this;
    }

    MBASE_INLINE const_forward_list_iterator& operator++(int) noexcept {
        if (!_ptr)
        {
            return *this;
        }

        _ptr = _ptr->next;
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const const_forward_list_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_forward_list_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const forward_list_iterator<T, DataT>& in_rhs) const noexcept {
        return _ptr == in_rhs.get();
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const forward_list_iterator<T, DataT>& in_rhs) const noexcept {
        return _ptr != in_rhs.get();
    }

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

    backward_list_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}
    backward_list_iterator(const backward_list_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}

    MBASE_INLINE pointer get() const noexcept {
        return _ptr;
    }

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE DataT& operator*() const noexcept {
        return *_ptr->data;
    }

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE DataT* operator->() const noexcept {
        return _ptr->data;
    }

    USED_RETURN("ignoring iterator index") MBASE_INLINE backward_list_iterator& operator+(difference_type in_rhs) noexcept {

        for (size_type i = 0; i < in_rhs; i++)
        {
            _ptr = _ptr->prev;
        }
        return *this;
    }

    MBASE_INLINE backward_list_iterator& operator+=(difference_type in_rhs) noexcept {
        for (size_type i = 0; i < in_rhs; i++)
        {
            _ptr = _ptr->prev;
        }
        return *this;
    }

    MBASE_INLINE backward_list_iterator& operator++() noexcept {
        if (!_ptr)
        {
            return *this;
        }

        _ptr = _ptr->prev;
        return *this;
    }

    MBASE_INLINE backward_list_iterator& operator++(int) noexcept {
        if (!_ptr)
        {
            return *this;
        }

        _ptr = _ptr->prev;
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const backward_list_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const backward_list_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const const_backward_list_iterator<T, DataT>& in_rhs) const noexcept {
        return _ptr == in_rhs.get();
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_backward_list_iterator<T, DataT>& in_rhs) const noexcept {
        return _ptr != in_rhs.get();
    }

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

    const_backward_list_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}
    const_backward_list_iterator(const const_backward_list_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}
    const_backward_list_iterator(const backward_list_iterator<T, DataT>& in_rhs) noexcept : _ptr(in_rhs.get()) {}

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE const_pointer get() const noexcept {
        return _ptr;
    }

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE const DataT& operator*() const noexcept {
        return *_ptr->data;
    }

    MBASE_INLINE const DataT* operator->() const noexcept {
        return _ptr->data;
    }

    USED_RETURN("ignoring iterator index") MBASE_INLINE const_backward_list_iterator& operator+(difference_type in_rhs) noexcept {
        
        for(size_type i = 0; i < in_rhs; i++)
        {
            _ptr = _ptr->prev;
        }
        return *this;
    }

    MBASE_INLINE const_backward_list_iterator& operator+=(difference_type in_rhs) noexcept {
        for (size_type i = 0; i < in_rhs; i++)
        {
            _ptr = _ptr->prev;
        }
        return *this;
    }

    MBASE_INLINE const_backward_list_iterator& operator++() noexcept {
        if(!_ptr)
        {
            return *this;
        }

        _ptr = _ptr->prev;
        return *this;
    }

    MBASE_INLINE const_backward_list_iterator& operator++(int) noexcept {
        if(!_ptr)
        {
            return *this;
        }

        _ptr = _ptr->prev;
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const const_backward_list_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_backward_list_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const backward_list_iterator<T, DataT>& in_rhs) const noexcept {
        return _ptr == in_rhs.get();
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const backward_list_iterator<T, DataT>& in_rhs) const noexcept {
        return _ptr != in_rhs.get();
    }

protected:
    pointer _ptr;
};

MBASE_STD_END

#endif // MBASE_LIST_ITERATOR_H

