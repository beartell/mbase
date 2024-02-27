#ifndef MBASE_STRING_H
#define MBASE_STRING_H

#include <mbase/vector.h>
#include <mbase/type_sequence.h>
#include <mbase/safe_buffer.h>

MBASE_STD_BEGIN

class character_sequence : public type_sequence<IBYTE> {
public:
    using Allocator = mbase::allocator<IBYTE>;

    character_sequence() noexcept : raw_data(nullptr), mSize(0), mCapacity(8) {
        raw_data = externalAllocator.allocate(mCapacity, true);
    }

    character_sequence(IBYTEBUFFER in_data, size_type in_size, size_type in_capacity) noexcept : raw_data(in_data), mSize(in_size), mCapacity(in_capacity) {}

    character_sequence(MSTRING in_string) noexcept {
        size_type tmp_st_length = type_sequence::length(in_string);
        size_type string_length = tmp_st_length / 8;
        size_type base_capacity = 8;
        while (string_length--)
        {
            // calculating capacity
            base_capacity *= 2;
        }
        mCapacity = base_capacity;
        mSize = tmp_st_length;
        raw_data = externalAllocator.allocate(base_capacity, true);
        type_sequence::copy_bytes(raw_data, in_string, tmp_st_length + 1);
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
        raw_data = externalAllocator.allocate(base_capacity, true);
        type_sequence::copy_bytes(raw_data, in_string, in_length);
    }

    character_sequence(const character_sequence& in_rhs) noexcept : raw_data(nullptr), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity) {
        raw_data = externalAllocator.allocate(mCapacity, true);
        type_sequence::copy_bytes(raw_data, in_rhs.raw_data, mSize);
    }

    character_sequence(character_sequence&& in_rhs) noexcept : raw_data(in_rhs.raw_data), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity) {
        in_rhs.raw_data = NULL;
    }

    ~character_sequence() noexcept {
        externalAllocator.destroy(raw_data);
    }

    character_sequence& operator=(const character_sequence& in_rhs) noexcept {
        if(raw_data)
        {
            externalAllocator.deallocate(raw_data, 0);
        }

        mSize = in_rhs.mSize;
        mCapacity = in_rhs.mCapacity;

        raw_data = externalAllocator.allocate(in_rhs.mCapacity, true);
        type_sequence::copy_bytes(raw_data, in_rhs.raw_data, in_rhs.mSize);
        return *this;
    }

    character_sequence& operator=(MSTRING in_rhs) noexcept {

        if (raw_data)
        {
            externalAllocator.deallocate(raw_data, 0);
        }

        size_type st_length = type_sequence::length(in_rhs);
        mSize = st_length;
        mCapacity = 8; // Base capacity
        
        while(mCapacity <= st_length)
        {
            // grow until the capacity exceeds the string
            mCapacity *= 2;
        }
        
        raw_data = externalAllocator.allocate(mCapacity, true);
        type_sequence::copy_bytes(raw_data, in_rhs, st_length);

        return *this;
    }

    character_sequence& operator=(character_sequence&& in_rhs) noexcept {
        if (raw_data)
        {
            externalAllocator.deallocate(raw_data, 0);
        }

        mSize = in_rhs.mSize;
        mCapacity = in_rhs.mCapacity;
        raw_data = in_rhs.raw_data;
        in_rhs.raw_data = nullptr;
        return *this;
    }

    character_sequence& operator+=(const character_sequence& in_rhs) noexcept {
        if(!in_rhs.mSize)
        {
            return *this;
        }

        size_type totalSize = mSize + in_rhs.mSize;
        if (totalSize >= mCapacity)
        {
           while(totalSize >= mCapacity)
           {
               mCapacity *= 2;
           }
           _resize(mCapacity);
        }

        type_sequence::concat(raw_data + mSize, in_rhs.raw_data, in_rhs.mSize);
        mSize = totalSize;
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

    character_sequence& operator+=(const IBYTE& in_character) noexcept {
        mSize++;
        if(mSize >= mCapacity)
        {
            _resize(++mCapacity);
        }
        raw_data[mSize-1] = in_character;
        return *this;
    }

    MBASE_INLINE GENERIC pop_back() noexcept {
        mSize--;
        raw_data[mSize] = '\0';
    }

    MBASE_INLINE GENERIC push_back(const IBYTE& in_character) noexcept {
        mSize++;
        if (mSize >= mCapacity)
        {
            _resize(++mCapacity);
        }
        raw_data[mSize - 1] = in_character;
    }

    USED_RETURN("founded string not being used") MBASE_INLINE size_type find(MSTRING in_src, size_type in_pos = 0) const noexcept {
        MSTRING tmpResult = strstr(raw_data + in_pos, in_src);
        if (tmpResult)
        {
            return tmpResult - (raw_data + in_pos);
        }

        return npos;
    }

    USED_RETURN("founded string not being used") MBASE_INLINE size_type find(const character_sequence& in_src, size_type in_pos = 0) const noexcept {
        return find(in_src.c_str(), in_pos);
    }

    USED_RETURN("founded string not being used") MBASE_INLINE size_type find(IBYTE in_char, size_type in_pos = 0) const noexcept {
        MSTRING tmpResult = strchr(raw_data + in_pos, in_char);
        if(tmpResult)
        {
            return tmpResult - (raw_data + in_pos);
        }

        return npos;
    }

    template<typename SourceContainer = mbase::vector<character_sequence>>
    MBASE_INLINE GENERIC split(const character_sequence& in_delimiters, SourceContainer& out_strings) noexcept {
        IBYTEBUFFER delims = in_delimiters.c_str();
        IBYTEBUFFER stringOut = strtok(raw_data, delims);
        while(stringOut != nullptr)
        {
            out_strings.push_back(stringOut);
            stringOut = strtok(nullptr, delims);
        }
    }

    template<typename ... Params>
    USED_RETURN("formatted string not being used") static character_sequence from_format(MSTRING in_format, Params ... in_params) noexcept {
        I32 stringLength = _scprintf(in_format, std::forward<Params>(in_params)...); // FIND THE FKIN SIZE
        character_sequence newSequence;
        if(!stringLength)
        {
            return newSequence;
        }

        IBYTEBUFFER mString = new IBYTE[stringLength + 1];
        fill(mString, 0, stringLength + 1);
        sprintf(mString, in_format, std::forward<Params>(in_params)...);
        newSequence = std::move(character_sequence(mString));
        delete[]mString;

        return newSequence;
    }

    USED_RETURN("converted string not being used.") static I32 to_i32(const character_sequence& in_string) noexcept {
        return atoi(in_string.c_str());
    }

    USED_RETURN("converted string not being used.") static I32 to_i64(const character_sequence& in_string) noexcept {
        return _atoi64(in_string.c_str());
    }

    USED_RETURN("converted string not being used.") static F32 to_f32(const character_sequence& in_string) noexcept {
        return strtof(in_string.c_str(), nullptr);
    }

    USED_RETURN("converted string not being used.") static F64 to_f64(const character_sequence& in_string) noexcept {
        return atof(in_string.c_str());
    }

    USED_RETURN("converted string not being used.") MBASE_INLINE I32 to_i32() const noexcept {
        return atoi(raw_data);
    }

    USED_RETURN("converted string not being used.") MBASE_INLINE I64 to_i64() const noexcept {
        return _atoi64(raw_data);
    }

    USED_RETURN("converted string not being used.") MBASE_INLINE F32 to_f32() const noexcept {
        return strtof(raw_data, nullptr);
    }

    USED_RETURN("converted string not being used.") MBASE_INLINE F64 to_f64() const noexcept {
        return atof(raw_data);
    }

    MBASE_INLINE GENERIC to_lower(iterator in_from, iterator in_to) noexcept {
        iterator It = in_from;
        iterator endIt = in_to;
        for (It; It != endIt; It++)
        {
            *It = tolower(*It);
        }
    }

    MBASE_INLINE GENERIC to_lower(reverse_iterator in_from, reverse_iterator in_to) noexcept {
        reverse_iterator rIt = in_from;
        reverse_iterator endrIt = in_to;
        for(rIt; rIt != endrIt; rIt++)
        {
            *rIt = tolower(*rIt);
        }
    }

    MBASE_INLINE GENERIC to_lower(size_t in_from, size_t in_to) noexcept {
        to_lower(begin() + in_from, end() + in_to);
    }

    MBASE_INLINE GENERIC to_lower() noexcept {
        to_lower(begin(), end());
    }

    MBASE_INLINE GENERIC to_upper(iterator in_from, iterator in_to) noexcept {
        iterator It = in_from;
        iterator endIt = in_to;
        for (It; It != endIt; It++)
        {
            *It = toupper(*It);
        }
    }

    MBASE_INLINE GENERIC to_upper(reverse_iterator in_from, reverse_iterator in_to) noexcept {
        reverse_iterator rIt = in_from;
        reverse_iterator endrIt = in_to;
        for (rIt; rIt != endrIt; rIt++)
        {
            *rIt = toupper(*rIt);
        }
    }

    MBASE_INLINE GENERIC to_upper(size_t in_from, size_t in_to) noexcept {
        to_upper(begin() + in_from, end() + in_to);
    }

    MBASE_INLINE GENERIC to_upper() noexcept {
        to_upper(begin(), end());
    }

    MBASE_INLINE GENERIC resize(size_type in_amount) {
        _resize(in_amount);
    }

    MBASE_INLINE GENERIC swap(character_sequence& in_src) noexcept {
        std::swap(raw_data, in_src.raw_data);
        std::swap(mCapacity, in_src.mCapacity);
        std::swap(mSize, in_src.mSize);
    }

    MBASE_INLINE GENERIC copy(IBYTEBUFFER in_src, size_t in_len, size_t in_pos = 0) noexcept {
        copy_bytes(in_src, raw_data + in_pos, in_len);
    }

    USED_RETURN("iterator being ignored") MBASE_INLINE iterator begin() const noexcept {
        return iterator(raw_data);
    }

    USED_RETURN("iterator being ignored") MBASE_INLINE iterator end() const noexcept {
        return iterator(raw_data + mSize);
    }

    USED_RETURN("const iterator being ignored") MBASE_INLINE const_iterator cbegin() const noexcept {
        return const_iterator(raw_data);
    }

    USED_RETURN("const iterator being ignored") MBASE_INLINE const_iterator cend() const noexcept {
        return const_iterator(raw_data + mSize);
    }

    USED_RETURN("raw string being ignored") MBASE_INLINE const_pointer c_str() const noexcept {
        return raw_data;
    }

    USED_RETURN("reverse iterator being ignored") MBASE_INLINE reverse_iterator rbegin() const noexcept {
        return reverse_iterator(raw_data + (mSize - 1));
    }

    USED_RETURN("const reverse iterator being ignored") MBASE_INLINE const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(raw_data + (mSize - 1));
    }

    USED_RETURN("reverse iterator being ignored") MBASE_INLINE reverse_iterator rend() const noexcept {
        return reverse_iterator(raw_data - 1);
    }

    USED_RETURN("const reverse iterator being ignored") MBASE_INLINE const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(raw_data - 1);
    }

    USED_RETURN("raw string data being ignored") MBASE_INLINE pointer data() const noexcept {
        return raw_data;
    }

    USED_RETURN("first character being ignored") MBASE_INLINE reference front() const noexcept {
        return raw_data[0];
    }

    USED_RETURN("last character being ignored") MBASE_INLINE reference back() const noexcept {
        return raw_data[mSize - 1];
    }

    USED_RETURN("character unused") MBASE_INLINE reference at(size_type in_pos) const noexcept {
        return raw_data[in_pos];
    }

    USED_RETURN("character unused") MBASE_INLINE reference operator[](size_type in_pos) const noexcept {
        return raw_data[in_pos];
    }

    USED_RETURN("string observation ignored") MBASE_INLINE_EXPR size_type size() const noexcept {
        return mSize;
    }

    USED_RETURN("string observation ignored") MBASE_INLINE_EXPR size_type capacity() const noexcept {
        return mCapacity - 1;
    }

    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_alnum(const IBYTE& in_char) noexcept {
        return isalnum(in_char);
    }

    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alnum(const_iterator in_begin, const_iterator in_end) const noexcept {
        if(!size())
        {
            return false;
        }

        const_iterator It = in_begin;
        const_iterator endIt = in_end;
        
        for(It; It != endIt; It++)
        {
            if(!is_alnum(*It))
            {
                return false;
            }
        }
        return true;
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alnum(size_type in_from, size_type in_to) const noexcept {
        return is_alnum(cbegin() + in_from, cbegin() + in_to);
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alnum() const noexcept {
        return is_alnum(cbegin(), cend());
    }

    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_alpha(const IBYTE& in_char) noexcept {
        return isalpha(in_char);
    }

    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alpha(const_iterator in_begin, const_iterator in_end) const noexcept {
        if (!size())
        {
            return false;
        }

        const_iterator It = in_begin;
        const_iterator endIt = in_end;

        for (It; It != endIt; It++)
        {
            if (!is_alpha(*It))
            {
                return false;
            }
        }
        return true;
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alpha(size_type in_from, size_type in_to) const noexcept {
        return is_alpha(cbegin() + in_from, cbegin() + in_to);
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alpha() const noexcept {
        return is_alpha(cbegin(), cend());
    }

    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_lower(const IBYTE& in_char) noexcept {
        return islower(in_char);
    }

    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_lower(const_iterator in_begin, const_iterator in_end) const noexcept {
        if (!size())
        {
            return false;
        }

        const_iterator It = in_begin;
        const_iterator endIt = in_end;

        for (It; It != endIt; It++)
        {
            if (!is_lower(*It))
            {
                return false;
            }
        }
        return true;
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_lower(size_type in_from, size_type in_to) const noexcept {
        return is_lower(cbegin() + in_from, cbegin() + in_to);
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_lower() const noexcept {
        return is_lower(cbegin(), cend());
    }

    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_upper(const IBYTE& in_char) noexcept {
        return isupper(in_char);
    }

    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_upper(const_iterator in_begin, const_iterator in_end) const noexcept {
        if (!size())
        {
            return false;
        }

        const_iterator It = in_begin;
        const_iterator endIt = in_end;

        for (It; It != endIt; It++)
        {
            if (!is_upper(*It))
            {
                return false;
            }
        }
        return true;
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_upper(size_type in_from, size_type in_to) const noexcept {
        return is_upper(cbegin() + in_from, cbegin() + in_to);
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_upper() const noexcept {
        return is_upper(cbegin(), cend());
    }

    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_blank(const IBYTE& in_char) noexcept {
        return isblank(in_char);
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_blank(const_iterator in_begin, const_iterator in_end) const noexcept {
        if (!size())
        {
            return false;
        }

        const_iterator It = in_begin;
        const_iterator endIt = in_end;

        for (It; It != endIt; It++)
        {
            if (!is_blank(*It))
            {
                return false;
            }
        }
        return true;
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_blank(size_type in_from, size_type in_to) const noexcept {
        return is_blank(cbegin() + in_from, cbegin() + in_to);
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_blank() const noexcept {
        return is_blank(cbegin(), cend());
    }

    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_control(const IBYTE& in_char) noexcept {
        return iscntrl(in_char);
    }

    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_control(const_iterator in_begin, const_iterator in_end) const noexcept {
        if (!size())
        {
            return false;
        }

        const_iterator It = in_begin;
        const_iterator endIt = in_end;

        for (It; It != endIt; It++)
        {
            if (!is_control(*It))
            {
                return false;
            }
        }
        return true;
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_control(size_type in_from, size_type in_to) const noexcept {
        return is_control(cbegin() + in_from, cbegin() + in_to);
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_control() const noexcept {
        return is_control(cbegin(), cend());
    }

    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_space(const IBYTE& in_char) noexcept {
        return isspace(in_char);
    }

    // characters such as: 0x20(' '), 0x0c('\f'), 0x0a('\n'), 0x0d('\r'), 0x09('\t'), 0x0b('\v')
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_space(const_iterator in_begin, const_iterator in_end) const noexcept {
        if (!size())
        {
            return false;
        }

        const_iterator It = in_begin;
        const_iterator endIt = in_end;

        for (It; It != endIt; It++)
        {
            if (!is_space(*It))
            {
                return false;
            }
        }
        return true;
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_space(size_type in_from, size_type in_to) const noexcept {
        return is_space(cbegin() + in_from, cbegin() + in_to);
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_space() const noexcept {
        return is_space(cbegin(), cend());
    }

    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_punctuation(const IBYTE& in_char) noexcept {
        return ispunct(in_char);
    }

    // characters such as: !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_punctuation(const_iterator in_begin, const_iterator in_end) const noexcept {
        if (!size())
        {
            return false;
        }

        const_iterator It = in_begin;
        const_iterator endIt = in_end;

        for (It; It != endIt; It++)
        {
            if (!is_punctuation(*It))
            {
                return false;
            }
        }
        return true;
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_punctuation(size_type in_from, size_type in_to) const noexcept {
        return is_punctuation(cbegin() + in_from, cbegin() + in_to);
    }

    USED_RETURN("string observation ignored") MBASE_INLINE bool is_punctuation() const noexcept {
        return is_punctuation(cbegin(), cend());
    }

    friend character_sequence operator+(const character_sequence& in_lhs, MSTRING in_rhs) noexcept {
        size_type rhsSize = type_sequence::length(in_rhs);
        if (!rhsSize)
        {
            return character_sequence(in_lhs);
        }
        size_type totalSize = in_lhs.mSize + type_sequence::length(in_rhs);
        size_type totalCapacity = in_lhs.mCapacity;
        while (totalSize >= totalCapacity)
        {
            totalCapacity *= 2;
        }
        IBYTEBUFFER new_data = mbase::allocator_simple<IBYTE>::allocate(totalCapacity, true);

        type_sequence::concat(new_data, in_lhs.raw_data, in_lhs.mSize);
        type_sequence::concat(new_data + in_lhs.mSize, in_rhs, rhsSize);

        return character_sequence(new_data, totalSize, totalCapacity);
    }

    friend character_sequence operator+(const character_sequence& in_lhs, const IBYTE& in_rhs) noexcept {
        size_type totalCapacity = in_lhs.mCapacity;
        if(in_lhs.mSize + 1 >= totalCapacity)
        {
            totalCapacity *= 2;
        }

        IBYTEBUFFER new_data = mbase::allocator_simple<IBYTE>::allocate(totalCapacity, true);
        type_sequence::concat(new_data, in_lhs.raw_data, in_lhs.mSize);
        type_sequence::concat(new_data + in_lhs.mSize, (IBYTEBUFFER)&in_rhs, 1);

        return character_sequence(new_data, in_lhs.mSize + 1, totalCapacity);
    }

    friend character_sequence operator+(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept {
        size_type totalSize = in_lhs.mSize + in_rhs.mSize;
        size_type totalCapacity = in_lhs.mCapacity;

        while (totalSize >= totalCapacity)
        {
            totalCapacity *= 2;
        }
        IBYTEBUFFER new_data = mbase::allocator_simple<IBYTE>::allocate(totalCapacity, true);

        type_sequence::concat(new_data, in_lhs.raw_data, in_lhs.mSize);
        type_sequence::concat(new_data + in_lhs.mSize, in_rhs.raw_data, in_rhs.mSize);

        return character_sequence(new_data, totalSize, totalCapacity);
    }

    bool operator==(const character_sequence& in_rhs) noexcept {
        return type_sequence::is_equal(raw_data, in_rhs.raw_data);
    }

    bool operator!=(const character_sequence& in_rhs) noexcept {
        return !type_sequence::is_equal(raw_data, in_rhs.raw_data);
    }

    friend bool operator==(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept {
        return type_sequence::is_equal(in_lhs.raw_data, in_rhs.raw_data);
    }

    friend bool operator!=(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept {
        return !type_sequence::is_equal(in_lhs.raw_data, in_rhs.raw_data);
    }

    friend std::ostream& operator<<(std::ostream& os, const character_sequence& in_rhs) noexcept {
        if (!in_rhs.raw_data)
        {
            // DO NOTHING IF THE DATA IS NOT PRESENT
        }
        else
        {
            os << in_rhs.data();
        }
        return os;
    }

    MBASE_INLINE GENERIC serialize(safe_buffer* out_buffer) {
        if(mSize)
        {
            out_buffer->bfLength = mSize;
            out_buffer->bfSource = new IBYTE[mSize];

            copy_bytes(out_buffer->bfSource, raw_data, mSize); // DO NOT INCLUDE NULL TERMINATOR
        }
    }

    MBASE_INLINE static character_sequence deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length) noexcept {
        return mbase::character_sequence(in_buffer, in_length);
    }

    static MBASE_INLINE_EXPR const SIZE_T npos = -1;

private:

    MBASE_INLINE GENERIC _resize(SIZE_T in_size) noexcept {
        size_type expectedSize = mSize;
        if(in_size < mSize)
        {
            // We will shrink if the given size is less then our string size
            expectedSize = in_size;
        }

        IBYTEBUFFER new_data = externalAllocator.allocate(in_size, true);
        copy_bytes(new_data, raw_data, expectedSize);
        externalAllocator.deallocate(raw_data, 0);
        raw_data = new_data;
        mCapacity = in_size;
        mSize = expectedSize;
    }
    Allocator externalAllocator;
    IBYTEBUFFER raw_data;
    SIZE_T mCapacity;
    SIZE_T mSize;
};

using string = character_sequence;
using wstring = string; // WSTRING WILL BE IMPLEMENTED LATER
using string_view = string; // STRING VIEW WILL BE IMPLEMENTED LATER

MBASE_STD_END

#endif // MBASE_STRING_H