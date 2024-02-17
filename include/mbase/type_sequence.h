#ifndef MBASE_TYPE_SEQUENCE_H
#define MBASE_TYPE_SEQUENCE_H

#include <mbase/common.h>
#include <mbase/allocator.h>

MBASE_STD_BEGIN

// TWO TYPES OF STRINGS
// BYTE SEQUENCE STRING (NULL TERMINATED)
// WIDE BYTE STRING

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

    USED_RETURN MBASE_INLINE pointer get() noexcept {
        return _ptr;
    }

    USED_RETURN MBASE_INLINE reference operator*() const noexcept {
        return *_ptr;
    }

    USED_RETURN MBASE_INLINE sequence_iterator& operator+(difference_type in_rhs) noexcept {
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

    USED_RETURN MBASE_INLINE bool operator==(const sequence_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN MBASE_INLINE bool operator!=(const sequence_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
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

    USED_RETURN MBASE_INLINE pointer get() noexcept {
        return _ptr;
    }

    USED_RETURN MBASE_INLINE const_reference operator*() const noexcept {
        return *_ptr;
    }

    USED_RETURN MBASE_INLINE const_sequence_iterator& operator+(difference_type in_rhs) noexcept {
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

    USED_RETURN MBASE_INLINE bool operator==(const const_sequence_iterator& in_rhs) const noexcept {
        return _ptr == in_rhs._ptr;
    }

    USED_RETURN MBASE_INLINE bool operator!=(const const_sequence_iterator& in_rhs) const noexcept {
        return _ptr != in_rhs._ptr;
    }

protected:
    pointer _ptr;
};


template<typename T, typename Alloc = allocator<T>>
class type_sequence {
public:
    using value_type = T;
    using allocator_type = Alloc;
    using pointer = T*;
    using const_pointer = const pointer;
    using reference = value_type&;
    using const_reference = const T&;
    using move_reference = value_type&&;
    using size_type = SIZE_T;
    using iterator = sequence_iterator<T>;
    using const_iterator = const_sequence_iterator<T>;

    static MBASE_INLINE_EXPR pointer copy_bytes(pointer out_target, const_pointer in_source, size_type in_length) noexcept {
        return static_cast<pointer>(memcpy(out_target, in_source, sizeof(value_type) * in_length));
    }

    static MBASE_INLINE_EXPR pointer concat(pointer out_target, const_pointer in_source, size_type in_length) {
        return static_cast<pointer>(strncat(out_target, in_source, sizeof(value_type) * in_length));
    }

    USED_RETURN static MBASE_INLINE_EXPR bool is_equal(const_pointer in_lhs, const_pointer in_rhs, size_type in_length) noexcept {
        if (!memcmp(in_lhs, in_rhs, sizeof(value_type) * in_length))
        {
            return true;
        }
        return false;
    }

    USED_RETURN static MBASE_INLINE_EXPR bool is_equal(const_pointer in_lhs, const_pointer in_rhs) noexcept {
        if (!strcmp(in_lhs, in_rhs))
        {
            return true;
        }
        return false;
    }

    USED_RETURN static MBASE_INLINE_EXPR size_type length(const_pointer in_target) noexcept {
        return strlen(in_target);
    }

    USED_RETURN static MBASE_INLINE_EXPR I32 compare(const_pointer in_lhs, const_pointer in_rhs, size_type in_length) noexcept {
        return memcmp(in_lhs, in_rhs, sizeof(value_type) * in_length);
    }

    USED_RETURN static MBASE_INLINE_EXPR I32 compare(const_pointer in_lhs, const_pointer in_rhs) noexcept {
        return strcmp(in_lhs, in_rhs);
    }

    static MBASE_INLINE_EXPR pointer fill(pointer in_target, value_type in_value, size_type in_length) noexcept {
        memset(in_target, in_value, sizeof(value_type) * in_length);
        return in_target;
    }
};

MBASE_STD_END

#endif // !MBASE_TYPE_SEQUENCE_H