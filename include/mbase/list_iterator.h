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

    bi_list_iterator(pointer in_ptr) noexcept : forward_list_iterator<Type, DataT>(in_ptr) {}
    bi_list_iterator(const bi_list_iterator& in_rhs) noexcept : forward_list_iterator<Type, DataT>(in_rhs._ptr) {}

    MBASE_INLINE bi_list_iterator& operator-=(difference_type in_rhs) noexcept {
        for (size_type i = 0; i < in_rhs; i++)
        {
            this->_ptr = this->_ptr->prev;
        }
        return *this;
    }

    MBASE_INLINE bi_list_iterator& operator--() noexcept {
        this->_ptr = this->_ptr->prev;
        return *this;
    }

    MBASE_INLINE bi_list_iterator& operator--(int) noexcept {
        this->_ptr = this->_ptr->prev;
        return *this;
    }
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

    const_bi_list_iterator(pointer in_ptr) noexcept : const_forward_list_iterator<Type, DataT>(in_ptr) {}
    const_bi_list_iterator(const const_bi_list_iterator& in_rhs) noexcept : const_forward_list_iterator<Type, DataT>(in_rhs._ptr) {}
    const_bi_list_iterator(const bi_list_iterator<Type, DataT>& in_rhs) noexcept : const_forward_list_iterator<Type, DataT>(in_rhs.get()) {}

    MBASE_INLINE const_bi_list_iterator& operator-=(difference_type in_rhs) noexcept {
        for (size_type i = 0; i < in_rhs; i++)
        {
            this->_ptr = this->_ptr->prev;
        }
        return *this;
    }

    MBASE_INLINE const_bi_list_iterator& operator--() noexcept {
        this->_ptr = this->_ptr->prev;
        return *this;
    }

    MBASE_INLINE const_bi_list_iterator& operator--(int) noexcept {
        this->_ptr = this->_ptr->prev;
        return *this;
    }
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

    reverse_bi_list_iterator(pointer in_ptr) noexcept : backward_list_iterator<Type, DataT>(in_ptr) {}
    reverse_bi_list_iterator(const reverse_bi_list_iterator& in_rhs) noexcept : backward_list_iterator<Type, DataT>(in_rhs._ptr) {}

    MBASE_INLINE reverse_bi_list_iterator& operator-=(difference_type in_rhs) noexcept {
        for (size_type i = 0; i < in_rhs; i++)
        {
            this->_ptr = this->_ptr->next;
        }
        return *this;
    }

    MBASE_INLINE reverse_bi_list_iterator& operator--() noexcept {
        this->_ptr = this->_ptr->next;
        return *this;
    }

    MBASE_INLINE reverse_bi_list_iterator& operator--(int) noexcept {
        this->_ptr = this->_ptr->next;
        return *this;
    }
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

    const_reverse_bi_list_iterator(pointer in_ptr) noexcept : const_backward_list_iterator<Type, DataT>(in_ptr) {}
    const_reverse_bi_list_iterator(const const_reverse_bi_list_iterator& in_rhs) noexcept : const_backward_list_iterator<Type, DataT>(in_rhs._ptr) {}
    const_reverse_bi_list_iterator(const reverse_bi_list_iterator<Type, DataT>& in_rhs) noexcept : const_backward_list_iterator<Type, DataT>(in_rhs.get()) {}

    MBASE_INLINE const_reverse_bi_list_iterator& operator-=(difference_type in_rhs) noexcept {
        for (size_type i = 0; i < in_rhs; i++)
        {
            this->_ptr = this->_ptr->next;
        }
        return *this;
    }

    MBASE_INLINE const_pointer get() const noexcept {
        return this->_ptr;
    }

    MBASE_INLINE const_reverse_bi_list_iterator& operator--() noexcept {
        this->_ptr = this->_ptr->next;
        return *this;
    }

    MBASE_INLINE const_reverse_bi_list_iterator& operator--(int) noexcept {
        this->_ptr = this->_ptr->next;
        return *this;
    }
};

template<typename T, typename Allocator = mbase::allocator_simple<T>>
struct list_node {
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using move_reference = T&&;

    list_node* prev;
    list_node* next;
    pointer data;

    list_node(const_reference in_object) noexcept : prev(nullptr), next(nullptr) {
        data = Allocator::allocate(1);
        Allocator::construct(data, in_object);
    }

    list_node(move_reference in_object) noexcept : prev(nullptr), next(nullptr) {
        data = Allocator::allocate(1);
        Allocator::construct(data, std::move(in_object));
    }

    ~list_node() noexcept {
        // data is guaranteed to be present
        // so no need to null check
        data->~value_type();
        Allocator::deallocate(data);
    }
};

MBASE_STD_END

#endif // MBASE_LIST_ITERATOR_H

