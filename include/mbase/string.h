#ifndef MBASE_STRING_H
#define MBASE_STRING_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <iostream>

MBASE_STD_BEGIN

// TWO TYPES OF STRINGS
// BYTE SEQUENCE STRING (NULL TERMINATED)
// WIDE BYTE STRING

template<typename T>
class sequence_iterator {
public:
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    using size_type = SIZE_T;
    using ptrdiff = PTRDIFF;
    sequence_iterator(pointer in_ptr) noexcept : _ptr(in_ptr) {}

    sequence_iterator(const sequence_iterator& in_rhs) noexcept : _ptr(in_rhs._ptr) {}

    sequence_iterator& operator=(const sequence_iterator& in_rhs) noexcept {
        _ptr = in_rhs._ptr;
        return *this;
    }

    USED_RETURN MBASE_INLINE reference operator*() const noexcept {
        return *_ptr;
    }

    USED_RETURN MBASE_INLINE sequence_iterator& operator+(ptrdiff in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    USED_RETURN MBASE_INLINE sequence_iterator& operator+=(ptrdiff in_rhs) noexcept {
        _ptr += in_rhs;
        return *this;
    }

    USED_RETURN MBASE_INLINE sequence_iterator& operator++() noexcept {
        ++_ptr;
        return *this;
    }

    USED_RETURN MBASE_INLINE sequence_iterator& operator++(int) noexcept {
        ++_ptr;
        return *this;
    }

    USED_RETURN MBASE_INLINE sequence_iterator& operator-(ptrdiff in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    USED_RETURN MBASE_INLINE sequence_iterator& operator-=(ptrdiff in_rhs) noexcept {
        _ptr -= in_rhs;
        return *this;
    }

    USED_RETURN MBASE_INLINE sequence_iterator& operator--() noexcept {
        --_ptr;
        return *this;
    }

    USED_RETURN MBASE_INLINE bool operator==(const sequence_iterator& in_rhs) const noexcept {
        return type_sequence<value_type>::is_equal(_ptr, in_rhs._ptr);
    }

    USED_RETURN MBASE_INLINE bool operator!=(const sequence_iterator& in_rhs) const noexcept {
        return !type_sequence<value_type>::is_equal(_ptr, in_rhs._ptr);
    }

private:
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
    using const_reference = const reference;
    using size_type = SIZE_T;
    using iterator = sequence_iterator<T>;
    using const_iterator = sequence_iterator<const T>;

    static MBASE_INLINE_EXPR pointer copy(pointer out_target, const_pointer in_source, size_type in_length) noexcept {
        return static_cast<pointer>(memcpy(out_target, in_source, sizeof(value_type) * in_length));
    }

    static MBASE_INLINE_EXPR pointer concat(pointer out_target, const_pointer in_source, size_type in_length) {
        return static_cast<pointer>(strncat(out_target, in_source, sizeof(value_type) * in_length));
    }

    USED_RETURN static MBASE_INLINE_EXPR bool is_equal(const_pointer in_lhs, const_pointer in_rhs, size_type in_length) noexcept {
        if(!memcmp(in_lhs, in_rhs, sizeof(value_type) * in_length))
        {
            return true;
        }
        return false;
    }

    USED_RETURN static MBASE_INLINE_EXPR bool is_equal(const_pointer in_lhs, const_pointer in_rhs) noexcept {
        if(!strcmp(in_lhs, in_rhs))
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

    static MBASE_INLINE_EXPR pointer fill(pointer in_target, value_type in_value, size_type in_length) noexcept {
        while(in_length--)
        {
            *in_target = in_value;
            in_target++;
        }

        return in_target;
    }
};

class character_sequence : public type_sequence<IBYTE> {
public:
    character_sequence() noexcept : raw_data(nullptr), mSize(0), mCapacity(8) {}

    character_sequence(IBYTEPTR in_data, size_type in_size, size_type in_capacity) noexcept : raw_data(in_data), mSize(in_size), mCapacity(in_capacity) {}

    ~character_sequence() noexcept {
        if (!raw_data)
        {
            // DO NOTHING IF THE DATA IS NOT PRESENT
        }
        else
        {
            _all.deallocate(raw_data, 0);
        }
    }

    character_sequence(MSTRING in_string) noexcept {
        size_type tmp_st_length = type_sequence::length(in_string);
        size_type string_length = tmp_st_length / 8;
        size_type base_capacity = 8;
        while(string_length--)
        {
            // calculating capacity
            base_capacity *= 2;
        }
        mCapacity = base_capacity;
        mSize = tmp_st_length;
        raw_data = _all.allocate(base_capacity, true);
        type_sequence::copy(raw_data, in_string, tmp_st_length + 1);
    }

    character_sequence(MSTRING in_string, size_type in_length) noexcept {
        size_type string_length = in_length / 8;
        size_type base_capacity = 8;
        while (string_length--)
        {
            // calculating capacity
            base_capacity *= 2;
        }
        mCapacity = base_capacity;
        mSize = in_length;
        raw_data = _all.allocate(base_capacity, true);
        type_sequence::copy(raw_data, in_string, in_length);
    }

    character_sequence(const character_sequence& in_rhs) noexcept : raw_data(nullptr), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity) {
        raw_data = _all.allocate(mCapacity, true);
        type_sequence::copy(raw_data, in_rhs.raw_data, mSize);
    }

    character_sequence(character_sequence&& in_rhs) noexcept : raw_data(in_rhs.raw_data), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity) {
        in_rhs.raw_data = NULL;
    }

    character_sequence& operator=(const character_sequence& in_rhs) noexcept {
        if(raw_data)
        {
            _all.deallocate(raw_data, mCapacity);
        }

        mSize = in_rhs.mSize;
        mCapacity = in_rhs.mCapacity;

        raw_data = _all.allocate(in_rhs.mCapacity, true);
        type_sequence::copy(raw_data, in_rhs.raw_data, in_rhs.mSize);
        return *this;
    }

    character_sequence& operator=(MSTRING in_rhs) noexcept {
        if (raw_data)
        {
            _all.deallocate(raw_data, mCapacity);
        }

        size_type st_length = type_sequence::length(in_rhs);
        mSize = st_length;
        mCapacity = 8; // Base capacity
        
        while(mCapacity <= st_length)
        {
            // grow until the capacity exceeds the string
            mCapacity *= 2;
        }
        
        raw_data = _all.allocate(mCapacity, true);
        type_sequence::copy(raw_data, in_rhs, st_length);

        return *this;
    }

    character_sequence& operator=(character_sequence&& in_rhs) noexcept {
        mSize = in_rhs.mSize;
        mCapacity = in_rhs.mCapacity;
        raw_data = in_rhs.raw_data;
        in_rhs = nullptr;
    }

    character_sequence& operator+=(const character_sequence& in_rhs) noexcept {
        if(!in_rhs.mSize)
        {
            return *this;
        }

        size_type totalSize = mSize + in_rhs.mSize;
        size_type totalCapacity = in_rhs.mCapacity;
        if (totalSize >= totalCapacity)
        {
           while(totalSize >= totalCapacity)
           {
               totalCapacity *= 2;
           }
           _resize(totalCapacity);
        }

        mSize = totalSize;

        type_sequence::concat(raw_data + mSize, in_rhs.raw_data, in_rhs.mSize);
        return *this;
    }

    character_sequence& operator+=(MSTRING in_rhs) noexcept {
        if(!in_rhs)
        {
            return *this;
        }
        size_type rhsSize = type_sequence::length(in_rhs);
        size_type totalSize = mSize + rhsSize;
        if(totalSize == mSize)
        {
            return *this;
        }

        if(totalSize >= mCapacity)
        {
            while(totalSize >= mCapacity)
            {
                mCapacity *= 2;
            }
            _resize(mCapacity);
        }
        
        type_sequence::concat(raw_data + mSize, in_rhs, rhsSize);
        mSize = totalSize;
        return *this;
    }

    friend character_sequence operator+(const character_sequence& in_lhs, MSTRING in_rhs) noexcept {
        size_type rhsSize = type_sequence::length(in_rhs);
        if (!rhsSize) 
        {
            return character_sequence(in_lhs);
        }
        size_type totalSize = in_lhs.mSize + type_sequence::length(in_rhs);
        size_type totalCapacity = in_lhs.mCapacity;
        while(totalSize >= totalCapacity) 
        {
            totalCapacity *= 2;
        }

        allocator all;
        IBYTEPTR new_data = all.allocate(totalCapacity, true);
        type_sequence::concat(new_data, in_lhs.raw_data, in_lhs.mSize);
        type_sequence::concat(new_data + in_lhs.mSize, in_rhs, rhsSize);

        return character_sequence(new_data, totalSize, totalCapacity);
    }

    friend character_sequence operator+(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept {
        size_type totalSize = in_lhs.mSize + in_rhs.mSize;
        size_type totalCapacity = in_lhs.mCapacity;

        while(totalSize >= totalCapacity)
        {
            totalCapacity *= 2;
        }
        allocator all;
        IBYTEPTR new_data = all.allocate(totalCapacity, true);
        
        type_sequence::concat(new_data, in_lhs.raw_data, in_lhs.mSize);
        type_sequence::concat(new_data + in_lhs.mSize, in_rhs.raw_data, in_rhs.mSize);

        return character_sequence(new_data, totalSize, totalCapacity);
    }
    
    friend bool operator==(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept {
        return type_sequence::is_equal(in_lhs.raw_data, in_rhs.raw_data);
    }

    friend bool operator!=(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept {
        return !type_sequence::is_equal(in_lhs.raw_data, in_rhs.raw_data);
    }

    friend std::ostream& operator<<(std::ostream& os, const character_sequence& in_rhs) noexcept {
        if(!in_rhs.raw_data)
        {
            
        }
        else
        {
            os << in_rhs.data();
        }
        return os;
    }

    USED_RETURN MBASE_INLINE reference front() const noexcept {
        return raw_data[0];
    }

    USED_RETURN MBASE_INLINE reference back() const noexcept {
        return raw_data[mSize - 1];
    }

    USED_RETURN MBASE_INLINE iterator begin() const noexcept {
        return iterator(raw_data);
    }

    USED_RETURN MBASE_INLINE iterator end() const noexcept {
        return iterator(raw_data + mSize);
    }

    USED_RETURN MBASE_INLINE const_iterator cbegin() const noexcept {
        return const_iterator(raw_data);
    }

    USED_RETURN MBASE_INLINE const_iterator cend() const noexcept {
        return const_iterator(raw_data + mSize);
    }

    USED_RETURN MBASE_INLINE_EXPR size_type size() const noexcept {
        return mSize;
    }

    USED_RETURN MBASE_INLINE_EXPR size_type capacity() const noexcept {
        return mCapacity - 1;
    }

    USED_RETURN MBASE_INLINE const_pointer c_str() const noexcept {
        return raw_data;
    }

    USED_RETURN MBASE_INLINE pointer data() const noexcept {
        return raw_data;
    }

    USED_RETURN MBASE_INLINE reference operator[](size_type in_pos) const noexcept {
        return raw_data[in_pos];
    }

    USED_RETURN MBASE_INLINE allocator_type get_allocator() const noexcept {
        return _all;
    }

private:

    MBASE_INLINE GENERIC _resize(SIZE_T in_size) noexcept {
        IBYTEPTR new_data = _all.allocate(in_size, true);
        type_sequence::copy(new_data, raw_data, mSize);
        _all.deallocate(raw_data, mSize);
        raw_data = new_data;
        mCapacity = in_size;
    }

    allocator_type _all;
    IBYTEPTR raw_data;
    SIZE_T mCapacity;
    SIZE_T mSize;
};

MBASE_STD_END

#endif // MBASE_STRING_H