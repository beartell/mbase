#ifndef MBASE_SEQUENCE_ITERATOR_H
#define MBASE_SEQUENCE_ITERATOR_H

#include <mbase/common.h>

MBASE_STD_BEGIN

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
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;

    sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}
    sequence_iterator(const sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}

    sequence_iterator& operator=(const sequence_iterator& in_rhs) noexcept {
        _ptr = in_rhs._ptr;
        return *this;
    }

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE pointer get() noexcept {
        return _ptr;
    }

    MBASE_INLINE pointer operator->() const noexcept {
        return _ptr;
    }

    USED_RETURN("internal data that is access through the iterator should be used") MBASE_INLINE reference operator*() const noexcept {
        return *_ptr;
    }

    USED_RETURN("ignoring iterator index") MBASE_INLINE sequence_iterator& operator+(difference_type in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    MBASE_INLINE sequence_iterator& operator+=(difference_type in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    MBASE_INLINE sequence_iterator& operator++() noexcept {
        ++_ptr;
        return *this;
    }

    MBASE_INLINE sequence_iterator& operator++(int) noexcept {
        ++_ptr;
        return *this;
    }

    MBASE_INLINE sequence_iterator& operator-(difference_type in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    MBASE_INLINE sequence_iterator& operator-=(difference_type in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    MBASE_INLINE sequence_iterator& operator--() noexcept {
        --_ptr;
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const sequence_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const sequence_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const const_sequence_iterator<T>& in_rhs) const noexcept {
        return _ptr == in_rhs.get();
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_sequence_iterator<T>& in_rhs) const noexcept {
        return _ptr != in_rhs.get();
    }

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

    const_sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}
    const_sequence_iterator(const const_sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}
    const_sequence_iterator(sequence_iterator<T> in_rhs) noexcept : _ptr(in_rhs.get()) {}

    const_sequence_iterator& operator=(const const_sequence_iterator& in_rhs) noexcept {
        _ptr = in_rhs._ptr;
        return *this;
    }

    const_sequence_iterator& operator=(sequence_iterator<T> in_rhs) noexcept {
        _ptr = in_rhs.get();
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE const_pointer get() noexcept {
        return _ptr;
    }

    MBASE_INLINE const_pointer operator->() const noexcept {
        return _ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE const_reference operator*() const noexcept {
        return *_ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE const_sequence_iterator& operator+(difference_type in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    MBASE_INLINE const_sequence_iterator& operator+=(difference_type in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    MBASE_INLINE const_sequence_iterator& operator++() noexcept {
        ++_ptr;
        return *this;
    }

    MBASE_INLINE const_sequence_iterator& operator++(int) noexcept {
        ++_ptr;
        return *this;
    }

    MBASE_INLINE const_sequence_iterator& operator-(difference_type in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    MBASE_INLINE const_sequence_iterator& operator-=(difference_type in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    MBASE_INLINE const_sequence_iterator& operator--() noexcept {
        --_ptr;
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const const_sequence_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_sequence_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const sequence_iterator<T>& in_rhs) const noexcept {
        return _ptr == in_rhs.get();
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const sequence_iterator<T>& in_rhs) const noexcept {
        return _ptr != in_rhs.get();
    }

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

    reverse_sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}
    reverse_sequence_iterator(const reverse_sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}

    reverse_sequence_iterator& operator=(const reverse_sequence_iterator& in_rhs) noexcept {
        _ptr = in_rhs._ptr;
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE pointer get() noexcept {
        return _ptr;
    }

    MBASE_INLINE pointer operator->() const noexcept {
        return _ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE reference operator*() const noexcept {
        return *_ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE reverse_sequence_iterator& operator+(difference_type in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    MBASE_INLINE reverse_sequence_iterator& operator+=(difference_type in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    MBASE_INLINE reverse_sequence_iterator& operator++() noexcept {
        --_ptr;
        return *this;
    }

    MBASE_INLINE reverse_sequence_iterator& operator++(int) noexcept {
        --_ptr;
        return *this;
    }

    MBASE_INLINE reverse_sequence_iterator& operator-(difference_type in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    MBASE_INLINE reverse_sequence_iterator& operator-=(difference_type in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    MBASE_INLINE reverse_sequence_iterator& operator--() noexcept {
        ++_ptr;
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const reverse_sequence_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const reverse_sequence_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept {
        return _ptr == in_rhs.get();
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_reverse_sequence_iterator<T>& in_rhs) const noexcept {
        return _ptr != in_rhs.get();
    }

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

    const_reverse_sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}
    const_reverse_sequence_iterator(const const_reverse_sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}
    const_reverse_sequence_iterator(reverse_sequence_iterator<T> in_rhs) noexcept : _ptr(in_rhs.get()) {}

    const_reverse_sequence_iterator& operator=(const const_reverse_sequence_iterator& in_rhs) noexcept {
        _ptr = in_rhs._ptr;
        return *this;
    }

    const_reverse_sequence_iterator& operator=(reverse_sequence_iterator<T> in_rhs) noexcept {
        _ptr = in_rhs.get();
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE const_pointer get() noexcept {
        return _ptr;
    }

    MBASE_INLINE const_pointer operator->() const noexcept {
        return _ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE const_reference operator*() const noexcept {
        return *_ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE const_reverse_sequence_iterator& operator+(difference_type in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    MBASE_INLINE const_reverse_sequence_iterator& operator+=(difference_type in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    MBASE_INLINE const_reverse_sequence_iterator& operator++() noexcept {
        --_ptr;
        return *this;
    }

    MBASE_INLINE const_reverse_sequence_iterator& operator++(int) noexcept {
        --_ptr;
        return *this;
    }

    MBASE_INLINE const_reverse_sequence_iterator& operator-(difference_type in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    MBASE_INLINE const_reverse_sequence_iterator& operator-=(difference_type in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    MBASE_INLINE const_reverse_sequence_iterator& operator--() noexcept {
        ++_ptr;
        return *this;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const const_reverse_sequence_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const const_reverse_sequence_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator==(const reverse_sequence_iterator<T>& in_rhs) const noexcept {
        return _ptr == in_rhs.get();
    }

    USED_RETURN("ignoring equality comparison") MBASE_INLINE bool operator!=(const reverse_sequence_iterator<T>& in_rhs) const noexcept {
        return _ptr != in_rhs.get();
    }

protected:
    pointer _ptr;
};

MBASE_STD_END

#endif // !MBASE_SEQUENCE_ITERATOR_H
