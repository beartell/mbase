#ifndef MBASE_STRING_H
#define MBASE_STRING_H

#include <mbase/vector.h>
#include <mbase/type_sequence.h>
#include <mbase/safe_buffer.h>

MBASE_STD_BEGIN

/* --- OBJECT BEHAVIOURS --- */

/*
    Default Constructible
    Move Constructible
    Copy Constructible
    Destructible
    Copy Assignable
    Move Assignable
    Swappable
    Allocate Aware
    Equality Comparable
    Sign Comparable
    Serializable
    Arithmetic Operable
    Iterable
    Type Aware
    Debuggable
*/

/* --- OBJECT BEHAVIOURS --- */

template<typename SeqType, typename SeqBase = type_sequence<SeqType>, typename Allocator = mbase::allocator<SeqType>>
class character_sequence : public SeqBase {
public:
    using traits_type = SeqBase;
    using allocator_type = Allocator;
    using value_type = typename SeqBase::value_type;
    using size_type = typename SeqBase::size_type;
    using difference_type = typename SeqBase::difference_type;
    using reference = typename SeqBase::reference;
    using const_reference = typename SeqBase::const_reference;
    using pointer = typename SeqBase::pointer;
    using const_pointer = typename SeqBase::const_pointer;
    using iterator = typename SeqBase::iterator;
    using const_iterator = typename SeqBase::const_iterator;
    using reverse_iterator = typename SeqBase::reverse_iterator;
    using const_reverse_iterator = typename SeqBase::const_reverse_iterator;

    MBASE_INLINE_EXPR character_sequence() noexcept;
    MBASE_INLINE_EXPR MBASE_EXPLICIT character_sequence(const Allocator& in_alloc) noexcept;
    MBASE_INLINE_EXPR character_sequence(size_type in_size, value_type in_ch, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs, size_type in_pos, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs, size_type in_pos, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc = Allocator());
    //MBASE_INLINE_EXPR character_sequence(const_pointer in_string, size_type in_count, const Allocator& in_alloc = Allocator());
    //MBASE_INLINE_EXPR character_sequence(const_pointer in_string, const Allocator& in_alloc = Allocator());
    template<typename InputIt>
    MBASE_INLINE_EXPR character_sequence(InputIt in_begin, InputIt in_end, const Allocator& in_alloc = Allocator());
    
    MBASE_INLINE_EXPR character_sequence(pointer in_data, size_type in_size, size_type in_capacity) noexcept;
    MBASE_INLINE_EXPR character_sequence(const_pointer in_string) noexcept;
    MBASE_INLINE_EXPR character_sequence(const_pointer in_string, size_type in_length) noexcept;
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs) noexcept;
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs, const Allocator& in_alloc);
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs) noexcept;
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs, const Allocator& in_alloc);
    MBASE_INLINE_EXPR character_sequence(std::initializer_list<value_type> in_chars, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR ~character_sequence() noexcept;

    MBASE_INLINE character_sequence& operator=(const character_sequence& in_rhs) noexcept;
    MBASE_INLINE character_sequence& operator=(character_sequence&& in_rhs) noexcept;
    MBASE_INLINE character_sequence& operator=(const_pointer in_rhs) noexcept;

    // observation functions
    USED_RETURN("raw string data being ignored") MBASE_INLINE_EXPR pointer data() const noexcept;
    USED_RETURN("raw string being ignored") MBASE_INLINE_EXPR const_pointer c_str() const noexcept;
    USED_RETURN("first character being ignored") MBASE_INLINE_EXPR reference front() noexcept;
    USED_RETURN("last character being ignored") MBASE_INLINE_EXPR reference back() noexcept;
    USED_RETURN("first character being ignored") MBASE_INLINE_EXPR const_reference front() const noexcept;
    USED_RETURN("last character being ignored") MBASE_INLINE_EXPR const_reference back() const noexcept;

    USED_RETURN("character unused") MBASE_INLINE_EXPR reference at(size_type in_pos);
    USED_RETURN("character unused") MBASE_INLINE_EXPR const_reference at(size_type in_pos) const;
    
    USED_RETURN("character unused") MBASE_INLINE_EXPR reference operator[](size_type in_pos);
    USED_RETURN("character unused") MBASE_INLINE_EXPR const_reference operator[](size_type in_pos) const;

    USED_RETURN("iterator being ignored") MBASE_INLINE iterator begin() noexcept;
    USED_RETURN("iterator being ignored") MBASE_INLINE const_iterator begin() const noexcept;
    USED_RETURN("iterator being ignored") MBASE_INLINE iterator end() noexcept;
    USED_RETURN("iterator being ignored") MBASE_INLINE const_iterator end() const noexcept;
    USED_RETURN("const iterator being ignored") MBASE_INLINE const_iterator cbegin() const noexcept;
    USED_RETURN("const iterator being ignored") MBASE_INLINE const_iterator cend() const noexcept;
    USED_RETURN("reverse iterator being ignored") MBASE_INLINE reverse_iterator rbegin() noexcept;
    USED_RETURN("reverse iterator being ignored") MBASE_INLINE const_reverse_iterator rbegin() const noexcept;
    USED_RETURN("const reverse iterator being ignored") MBASE_INLINE const_reverse_iterator crbegin() const noexcept;
    USED_RETURN("reverse iterator being ignored") MBASE_INLINE reverse_iterator rend() noexcept;
    USED_RETURN("reverse iterator being ignored") MBASE_INLINE const_reverse_iterator rend() const noexcept;
    USED_RETURN("const reverse iterator being ignored") MBASE_INLINE const_reverse_iterator crend() const noexcept;

    USED_RETURN("ignoring the allocator") MBASE_INLINE_EXPR Allocator get_allocator() const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find(const_pointer in_src, size_type in_pos = 0) const noexcept; // 3
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find(const character_sequence& in_src, size_type in_pos = 0) const noexcept; // 1
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find(value_type in_char, size_type in_pos = 0) const noexcept; // 4
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find(const_pointer in_src, size_type in_pos, size_type in_count) const;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type rfind(const character_sequence& in_src, size_type in_pos = npos) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type rfind(const_pointer in_str, size_type in_pos, size_type in_count) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type rfind(const_pointer in_str, size_type in_pos) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type rfind(value_type in_char, size_type in_pos = npos) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_first_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_first_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_first_of(const_pointer in_str, size_type in_pos = 0) const;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_first_of(value_type in_char, size_type in_pos = 0) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_first_not_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_first_not_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_first_not_of(const_pointer in_str, size_type in_pos = 0) const;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_first_not_of(value_type in_char, size_type in_pos = 0) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_last_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_last_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_last_of(const_pointer in_str, size_type in_pos = 0) const;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_last_of(value_type in_char, size_type in_pos = 0) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_last_not_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_last_not_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_last_not_of(const_pointer in_str, size_type in_pos = 0) const;
    USED_RETURN("founded string not being used") MBASE_INLINE size_type find_last_not_of(value_type in_char, size_type in_pos = 0) const noexcept;
    USED_RETURN("founded string not being used") MBASE_INLINE character_sequence substr(size_type in_pos = 0, size_type in_count = npos) const;
    USED_RETURN("founded string not being used") MBASE_INLINE character_sequence subarray(size_type in_pos = 0, size_type in_count = npos) const;
    // --- MISSING ---> starts_with
    USED_RETURN("string observation ignored") MBASE_INLINE I32 compare(const character_sequence& in_src) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE I32 compare(size_type in_pos1, size_type in_pos2, const character_sequence& in_src) const;
    USED_RETURN("string observation ignored") MBASE_INLINE I32 compare(size_type in_pos1, size_type in_count1, const character_sequence& in_src, size_type in_pos2, size_type in_count2 = npos) const;
    USED_RETURN("string observation ignored") MBASE_INLINE I32 compare(const_pointer in_str) const;
    USED_RETURN("string observation ignored") MBASE_INLINE I32 compare(size_type in_pos1, size_type in_count1, const_pointer in_str) const;
    USED_RETURN("string observation ignored") MBASE_INLINE I32 compare(size_type in_pos1, size_type in_count1, const_pointer in_src, size_type in_count2) const;
    USED_RETURN("string observation ignored") MBASE_INLINE_EXPR size_type size() const noexcept;
    //USED_RETURN("string observation ignored") MBASE_INLINE_EXPR size_type length() const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE_EXPR size_type max_size() const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE_EXPR size_type capacity() const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE_EXPR bool empty() const noexcept;
    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alnum(const_iterator in_begin, const_iterator in_end) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alnum(size_type in_from, size_type in_to) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alnum() const noexcept;
    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alpha(const_iterator in_begin, const_iterator in_end) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alpha(size_type in_from, size_type in_to) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_alpha() const noexcept;
    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_lower(const_iterator in_begin, const_iterator in_end) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_lower(size_type in_from, size_type in_to) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_lower() const noexcept;
    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_upper(const_iterator in_begin, const_iterator in_end) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_upper(size_type in_from, size_type in_to) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_upper() const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_blank(const_iterator in_begin, const_iterator in_end) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_blank(size_type in_from, size_type in_to) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_blank() const noexcept;
    // if there is a space character in range, the returned value will be false
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_control(const_iterator in_begin, const_iterator in_end) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_control(size_type in_from, size_type in_to) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_control() const noexcept;
    // characters such as: 0x20(' '), 0x0c('\f'), 0x0a('\n'), 0x0d('\r'), 0x09('\t'), 0x0b('\v')
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_space(const_iterator in_begin, const_iterator in_end) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_space(size_type in_from, size_type in_to) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_space() const noexcept;
    // characters such as: !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_punctuation(const_iterator in_begin, const_iterator in_end) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_punctuation(size_type in_from, size_type in_to) const noexcept;
    USED_RETURN("string observation ignored") MBASE_INLINE bool is_punctuation() const noexcept;

    bool operator==(const character_sequence& in_rhs) noexcept;
    bool operator!=(const character_sequence& in_rhs) noexcept;

    MBASE_INLINE_EXPR character_sequence& assign(size_type in_count, value_type in_ch);
    MBASE_INLINE_EXPR character_sequence& assign(const character_sequence& in_str);
    MBASE_INLINE_EXPR character_sequence& assign(const character_sequence& in_str, size_type in_pos, size_type in_count = npos);
    MBASE_INLINE_EXPR character_sequence& assign(character_sequence&& in_str) noexcept;
    MBASE_INLINE_EXPR character_sequence& assign(const_pointer in_str, size_type in_count);
    MBASE_INLINE_EXPR character_sequence& assign(const_pointer in_str);
    template<typename InputIt>
    MBASE_INLINE_EXPR character_sequence& assign(InputIt in_begin, InputIt in_end);
    MBASE_INLINE_EXPR character_sequence& assign(std::initializer_list<value_type> in_chars);
    MBASE_INLINE GENERIC pop_back() noexcept;
    MBASE_INLINE GENERIC push_back(const value_type& in_character) noexcept;
    USED_RETURN("converted string not being used.") MBASE_INLINE I32 to_i32() const noexcept;
    USED_RETURN("converted string not being used.") MBASE_INLINE I64 to_i64() const noexcept;
    USED_RETURN("converted string not being used.") MBASE_INLINE F32 to_f32() const noexcept;
    USED_RETURN("converted string not being used.") MBASE_INLINE F64 to_f64() const noexcept;
    MBASE_INLINE GENERIC to_lower(iterator in_from, iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_lower(reverse_iterator in_from, reverse_iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_lower(size_t in_from, size_t in_to) noexcept;
    MBASE_INLINE GENERIC to_lower() noexcept;
    MBASE_INLINE GENERIC to_upper(iterator in_from, iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_upper(reverse_iterator in_from, reverse_iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_upper(size_t in_from, size_t in_to) noexcept;
    MBASE_INLINE GENERIC to_upper() noexcept;
    MBASE_INLINE GENERIC resize(size_type in_amount);
    MBASE_INLINE_EXPR GENERIC resize(size_type in_amount, value_type in_char);
    MBASE_INLINE GENERIC swap(character_sequence& in_src) noexcept;
    MBASE_INLINE_EXPR GENERIC copy(pointer in_src, size_t in_len, size_t in_pos = 0) noexcept;
    MBASE_INLINE_EXPR GENERIC reserve(size_type in_new_cap);
    MBASE_INLINE_EXPR GENERIC clear() noexcept;
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, size_type in_count, value_type in_ch);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const_pointer in_str);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const_pointer in_str, size_type in_count);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const character_sequence& in_str);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const character_sequence& in_str, size_type in_sindex, size_type in_count = npos);
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, value_type in_char);
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, size_type in_count, value_type in_char);
    template<typename InputIt>
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, InputIt in_begin, InputIt in_end);
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, std::initializer_list<value_type> in_chars);
    MBASE_INLINE_EXPR character_sequence& erase(size_type in_index = 0, size_type count = npos);
    MBASE_INLINE_EXPR iterator erase(const_iterator in_pos);
    MBASE_INLINE_EXPR iterator erase(const_iterator in_from, const_iterator in_to);
    MBASE_INLINE_EXPR character_sequence& append(size_type in_count, value_type in_char);
    MBASE_INLINE_EXPR character_sequence& append(const character_sequence& in_str);
    MBASE_INLINE_EXPR character_sequence& append(const character_sequence& in_str, size_type in_pos, size_type in_count = npos);
    MBASE_INLINE_EXPR character_sequence& append(const_pointer in_str, size_type in_count);
    MBASE_INLINE_EXPR character_sequence& append(const_pointer in_str);
    template<typename InputIt>
    MBASE_INLINE_EXPR character_sequence& append(InputIt in_begin, InputIt in_end);
    MBASE_INLINE_EXPR character_sequence& append(std::initializer_list<value_type> in_chars);
    MBASE_INLINE_EXPR character_sequence& replace(size_type in_pos, size_type in_count, const character_sequence& in_str);
    MBASE_INLINE_EXPR character_sequence& replace(const_iterator in_begin, const_iterator in_end, const character_sequence& in_str);
    MBASE_INLINE_EXPR character_sequence& replace(size_type in_pos, size_type in_count, const character_sequence& in_str, size_type in_pos2, size_type in_count2);
    MBASE_INLINE_EXPR character_sequence& replace(size_type in_pos, size_type in_count, const_pointer in_str, size_type in_count2);
    MBASE_INLINE_EXPR character_sequence& replace(const_iterator in_begin, const_iterator in_end, const_pointer in_str, size_type in_count2);
    MBASE_INLINE_EXPR character_sequence& replace(size_type in_pos, size_type in_count, const_pointer in_str);
    MBASE_INLINE_EXPR character_sequence& replace(const_iterator in_begin, const_iterator in_end, const_pointer in_str);
    MBASE_INLINE_EXPR character_sequence& replace(size_type in_pos, size_type in_count, size_type in_count2, value_type in_char);
    MBASE_INLINE_EXPR character_sequence& replace(const_iterator in_begin, const_iterator in_end, size_type in_count2, value_type in_char);
    MBASE_INLINE_EXPR character_sequence& replace(const_iterator in_begin, const_iterator in_end, std::initializer_list<value_type> in_chars);
    MBASE_INLINE_EXPR size_type erase(character_sequence& in_str, value_type in_val);
    template<typename SourceContainer = mbase::vector<character_sequence>>
    MBASE_INLINE GENERIC split(const character_sequence& in_delimiters, SourceContainer& out_strings) noexcept {
        const_pointer delims = in_delimiters.c_str();
        pointer stringOut = strtok(raw_data, delims);
        while (stringOut != nullptr)
        {
            out_strings.push_back(stringOut);
            stringOut = strtok(nullptr, delims);
        }
    }

    character_sequence& operator+=(const character_sequence& in_rhs) noexcept;
    character_sequence& operator+=(const_pointer in_rhs) noexcept;
    character_sequence& operator+=(const value_type& in_character) noexcept;
    character_sequence& operator+=(std::initializer_list<value_type> in_chars);

    MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer);
    MBASE_INLINE static character_sequence deserialize(pointer in_buffer, SIZE_T in_length) noexcept;

    USED_RETURN("converted string not being used.") static I32 to_i32(const character_sequence& in_string) noexcept { return atoi(in_string.c_str()); }
    USED_RETURN("converted string not being used.") static I32 to_i64(const character_sequence& in_string) noexcept { return _atoi64(in_string.c_str()); }
    USED_RETURN("converted string not being used.") static F32 to_f32(const character_sequence& in_string) noexcept { return strtof(in_string.c_str(), nullptr); }
    USED_RETURN("converted string not being used.") static F64 to_f64(const character_sequence& in_string) noexcept { return atof(in_string.c_str()); }

    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_alnum(const value_type& in_char) noexcept { return isalnum(in_char); }
    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_alpha(const value_type& in_char) noexcept { return isalpha(in_char); }
    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_lower(const value_type& in_char) noexcept { return islower(in_char); }
    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_upper(const value_type& in_char) noexcept { return isupper(in_char); }
    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_blank(const value_type& in_char) noexcept { return isblank(in_char); }
    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_control(const value_type& in_char) noexcept { return iscntrl(in_char); }
    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_space(const value_type& in_char) noexcept { return isspace(in_char); }
    USED_RETURN("string observation ignored") static MBASE_INLINE bool is_punctuation(const value_type& in_char) noexcept { return ispunct(in_char); }

    template<typename ... Params>
    USED_RETURN("formatted string not being used") static character_sequence from_format(const_pointer in_format, Params ... in_params) noexcept {
        size_type stringLength = _scprintf(in_format, std::forward<Params>(in_params)...); // FIND THE FKIN SIZE
        character_sequence newSequence;
        if (!stringLength)
        {
            return newSequence;
        }

        pointer mString = newSequence.externalAllocator.allocate(stringLength + 1, true);
        newSequence.fill(mString, 0, stringLength + 1);
        sprintf(mString, in_format, std::forward<Params>(in_params)...);
        newSequence = std::move(character_sequence(mString));
        newSequence.externalAllocator.deallocate(mString, 0);

        return newSequence;
    }

    friend character_sequence operator+(const character_sequence& in_lhs, const_pointer in_rhs) noexcept {
        size_type rhsSize = this->length(in_rhs);
        if (!rhsSize)
        {
            return character_sequence(in_lhs);
        }
        size_type totalSize = in_lhs.mSize + this->length(in_rhs);
        size_type totalCapacity = in_lhs.mCapacity;
        while (totalSize >= totalCapacity)
        {
            totalCapacity *= 2;
        }
        pointer new_data = externalAllocator.allocate(totalCapacity, true);

        this->concat(new_data, in_lhs.raw_data, in_lhs.mSize);
        this->concat(new_data + in_lhs.mSize, in_rhs, rhsSize);

        return character_sequence(new_data, totalSize, totalCapacity);
    }

    friend character_sequence operator+(const character_sequence& in_lhs, const value_type& in_rhs) noexcept {
        size_type rhsSize = this->length(in_rhs);
        if (!rhsSize)
        {
            return character_sequence(in_lhs);
        }
        size_type totalSize = in_lhs.mSize + type_sequence<value_type>::length(in_rhs);
        size_type totalCapacity = in_lhs.mCapacity;
        while (totalSize >= totalCapacity)
        {
            totalCapacity *= 2;
        }
        pointer new_data = externalAllocator.allocate(totalCapacity, true);

        this->concat(new_data, in_lhs.raw_data, in_lhs.mSize);
        this->concat(new_data + in_lhs.mSize, in_rhs, rhsSize);

        return character_sequence(new_data, totalSize, totalCapacity);
    }

    friend character_sequence operator+(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept {
        I32 totalSize = in_lhs.mSize + in_rhs.mSize;
        I32 totalCapacity = in_lhs.mCapacity;

        while (totalSize >= totalCapacity)
        {
            totalCapacity *= 2;
        }
        pointer new_data = mbase::allocator_simple<value_type>::allocate(totalCapacity, true);
        //this->length();
        type_sequence<value_type>::concat(new_data, in_lhs.raw_data, in_lhs.mSize);
        type_sequence<value_type>::concat(new_data + in_lhs.mSize, in_rhs.raw_data, in_rhs.mSize);

        return character_sequence(new_data, totalSize, totalCapacity);
    }
    friend bool operator==(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept;
    friend bool operator!=(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept;
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

    static MBASE_INLINE_EXPR const SIZE_T npos = -1;
private:

    MBASE_INLINE GENERIC _resize(size_type in_size) noexcept {
        size_type expectedSize = mSize;
        if(in_size < mSize)
        {
            // We will shrink if the given size is less then our string size
            expectedSize = in_size;
        }

        pointer new_data = externalAllocator.allocate(in_size, true);
        this->copy_bytes(new_data, raw_data, expectedSize);
        externalAllocator.deallocate(raw_data, 0);
        raw_data = new_data;
        mCapacity = in_size;
        mSize = expectedSize;
    }
    allocator_type externalAllocator;
    pointer raw_data;
    size_type mCapacity;
    size_type mSize;
};

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence() noexcept : raw_data(nullptr), mSize(0), mCapacity(8) {
    raw_data = externalAllocator.allocate(mCapacity, true);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(pointer in_data, size_type in_size, size_type in_capacity) noexcept : raw_data(in_data), mSize(in_size), mCapacity(in_capacity)
{
    // IMPLEMENT THIS
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const_pointer in_string) noexcept {
    size_type tmp_st_length = this->length(in_string);
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
    this->copy_bytes(raw_data, in_string, tmp_st_length + 1);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const_pointer in_string, size_type in_length) noexcept {
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
    this->copy_bytes(raw_data, in_string, in_length);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const character_sequence& in_rhs) noexcept : raw_data(nullptr), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity) {
    raw_data = externalAllocator.allocate(mCapacity, true);
    this->copy_bytes(raw_data, in_rhs.raw_data, mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(character_sequence&& in_rhs) noexcept : raw_data(in_rhs.raw_data), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity) {
    in_rhs.raw_data = NULL;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::~character_sequence() noexcept {
    externalAllocator.destroy(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(const character_sequence& in_rhs) noexcept {
    if (raw_data)
    {
        externalAllocator.deallocate(raw_data, 0);
    }

    mSize = in_rhs.mSize;
    mCapacity = in_rhs.mCapacity;

    raw_data = externalAllocator.allocate(in_rhs.mCapacity, true);
    this->copy_bytes(raw_data, in_rhs.raw_data, in_rhs.mSize);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(const_pointer in_rhs) noexcept {

    if (raw_data)
    {
        externalAllocator.deallocate(raw_data, 0);
    }

    size_type st_length = this->length(in_rhs);
    mSize = st_length;
    mCapacity = 8; // Base capacity

    while (mCapacity <= st_length)
    {
        // grow until the capacity exceeds the string
        mCapacity *= 2;
    }

    raw_data = externalAllocator.allocate(mCapacity, true);
    this->copy_bytes(raw_data, in_rhs, st_length);

    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(character_sequence&& in_rhs) noexcept {
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

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("raw string data being ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::pointer character_sequence<SeqType, SeqBase, Allocator>::data() const noexcept {
    return raw_data;
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("raw string being ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_pointer character_sequence<SeqType, SeqBase, Allocator>::c_str() const noexcept {
    return raw_data;
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("first character being ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::front() noexcept {
    return raw_data[0];
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("last character being ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::back() noexcept {
    return raw_data[mSize - 1];
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("character unused") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::at(size_type in_pos) {
    // EXCEPTION WILL BE IMPLEMENTED HERE
    return raw_data[in_pos];
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("character unused") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::operator[](size_type in_pos) {
    return raw_data[in_pos];
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::begin() noexcept {
    return iterator(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::end() noexcept {
    return iterator(raw_data + mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("const iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_iterator character_sequence<SeqType, SeqBase, Allocator>::cbegin() const noexcept {
    return const_iterator(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("const iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_iterator character_sequence<SeqType, SeqBase, Allocator>::cend() const noexcept {
    return const_iterator(raw_data + mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("reverse iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::rbegin() noexcept {
    return reverse_iterator(raw_data + (mSize - 1));
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("const reverse iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::crbegin() const noexcept {
    return const_reverse_iterator(raw_data + (mSize - 1));
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("reverse iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::rend() noexcept {
    return reverse_iterator(raw_data - 1);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("const reverse iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::crend() const noexcept {
    return const_reverse_iterator(raw_data - 1);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("founded string not being used") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(const_pointer in_src, size_type in_pos) const noexcept {
    const_pointer tmpResult = strstr(raw_data + in_pos, in_src);
    if (tmpResult)
    {
        return tmpResult - (raw_data + in_pos);
    }

    return npos;
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("founded string not being used") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(const character_sequence& in_src, size_type in_pos) const noexcept {
    return find(in_src.c_str(), in_pos);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("founded string not being used") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(value_type in_char, size_type in_pos) const noexcept {
    const_pointer tmpResult = strchr(raw_data + in_pos, in_char);
    if (tmpResult)
    {
        return tmpResult - (raw_data + in_pos);
    }

    return npos;
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::size() const noexcept {
    return mSize;
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::capacity() const noexcept {
    return mCapacity - 1;
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alnum(const_iterator in_begin, const_iterator in_end) const noexcept {
    if (!size())
    {
        return false;
    }

    const_iterator It = in_begin;
    const_iterator endIt = in_end;

    for (It; It != endIt; It++)
    {
        if (!is_alnum(*It))
        {
            return false;
        }
    }
    return true;
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alnum(size_type in_from, size_type in_to) const noexcept {
    return is_alnum(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alnum() const noexcept {
    return is_alnum(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alpha(const_iterator in_begin, const_iterator in_end) const noexcept {
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

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alpha(size_type in_from, size_type in_to) const noexcept {
    return is_alpha(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alpha() const noexcept {
    return is_alpha(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_lower(const_iterator in_begin, const_iterator in_end) const noexcept {
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

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_lower(size_type in_from, size_type in_to) const noexcept {
    return is_lower(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_lower() const noexcept {
    return is_lower(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_upper(const_iterator in_begin, const_iterator in_end) const noexcept {
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

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_upper(size_type in_from, size_type in_to) const noexcept {
    return is_upper(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_upper() const noexcept {
    return is_upper(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_blank(const_iterator in_begin, const_iterator in_end) const noexcept {
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

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_blank(size_type in_from, size_type in_to) const noexcept {
    return is_blank(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_blank() const noexcept {
    return is_blank(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_control(const_iterator in_begin, const_iterator in_end) const noexcept {
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

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_control(size_type in_from, size_type in_to) const noexcept {
    return is_control(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_control() const noexcept {
    return is_control(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_space(const_iterator in_begin, const_iterator in_end) const noexcept {
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

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_space(size_type in_from, size_type in_to) const noexcept {
    return is_space(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_space() const noexcept {
    return is_space(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_punctuation(const_iterator in_begin, const_iterator in_end) const noexcept {
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

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_punctuation(size_type in_from, size_type in_to) const noexcept {
    return is_punctuation(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_punctuation() const noexcept {
    return is_punctuation(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
bool character_sequence<SeqType, SeqBase, Allocator>::operator==(const character_sequence& in_rhs) noexcept {
    return this->is_equal(raw_data, in_rhs.raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
bool character_sequence<SeqType, SeqBase, Allocator>::operator!=(const character_sequence& in_rhs) noexcept {
    return !this->is_equal(raw_data, in_rhs.raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(const character_sequence& in_rhs) noexcept {
    if (!in_rhs.mSize)
    {
        return *this;
    }

    size_type totalSize = mSize + in_rhs.mSize;
    if (totalSize >= mCapacity)
    {
        while (totalSize >= mCapacity)
        {
            mCapacity *= 2;
        }
        _resize(mCapacity);
    }

    this->concat(raw_data + mSize, in_rhs.raw_data, in_rhs.mSize);
    mSize = totalSize;
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(const_pointer in_rhs) noexcept {
    if (!in_rhs)
    {
        return *this;
    }
    size_type rhsSize = this->length(in_rhs);
    size_type totalSize = mSize + rhsSize;
    if (totalSize == mSize)
    {
        return *this;
    }

    if (totalSize >= mCapacity)
    {
        while (totalSize >= mCapacity)
        {
            mCapacity *= 2;
        }
        _resize(mCapacity);
    }

    this->concat(raw_data + mSize, in_rhs, rhsSize);
    mSize = totalSize;
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(const value_type& in_character) noexcept {
    mSize++;
    if (mSize >= mCapacity)
    {
        _resize(++mCapacity);
    }
    raw_data[mSize - 1] = in_character;
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::pop_back() noexcept {
    mSize--;
    raw_data[mSize] = '\0';
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::push_back(const value_type& in_character) noexcept {
    mSize++;
    if (mSize >= mCapacity)
    {
        _resize(++mCapacity);
    }
    raw_data[mSize - 1] = in_character;
}

//template<typename SourceContainer>
//MBASE_INLINE GENERIC character_sequence::split(const character_sequence& in_delimiters, SourceContainer& out_strings) noexcept {
//    const_pointer delims = in_delimiters.c_str();
//    pointer stringOut = strtok(raw_data, delims);
//    while (stringOut != nullptr)
//    {
//        out_strings.push_back(stringOut);
//        stringOut = strtok(nullptr, delims);
//    }
//}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("converted string not being used.") MBASE_INLINE I32 character_sequence<SeqType, SeqBase, Allocator>::to_i32() const noexcept {
    return atoi(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("converted string not being used.") MBASE_INLINE I64 character_sequence<SeqType, SeqBase, Allocator>::to_i64() const noexcept {
    return _atoi64(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("converted string not being used.") MBASE_INLINE F32 character_sequence<SeqType, SeqBase, Allocator>::to_f32() const noexcept {
    return strtof(raw_data, nullptr);
}

template<typename SeqType, typename SeqBase, typename Allocator>
USED_RETURN("converted string not being used.") MBASE_INLINE F64 character_sequence<SeqType, SeqBase, Allocator>::to_f64() const noexcept {
    return atof(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_lower(iterator in_from, iterator in_to) noexcept {
    iterator It = in_from;
    iterator endIt = in_to;
    for (It; It != endIt; It++)
    {
        *It = tolower(*It);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_lower(reverse_iterator in_from, reverse_iterator in_to) noexcept {
    reverse_iterator rIt = in_from;
    reverse_iterator endrIt = in_to;
    for (rIt; rIt != endrIt; rIt++)
    {
        *rIt = tolower(*rIt);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_lower(size_t in_from, size_t in_to) noexcept {
    to_lower(begin() + in_from, end() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_lower() noexcept {
    to_lower(begin(), end());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_upper(iterator in_from, iterator in_to) noexcept {
    iterator It = in_from;
    iterator endIt = in_to;
    for (It; It != endIt; It++)
    {
        *It = toupper(*It);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_upper(reverse_iterator in_from, reverse_iterator in_to) noexcept {
    reverse_iterator rIt = in_from;
    reverse_iterator endrIt = in_to;
    for (rIt; rIt != endrIt; rIt++)
    {
        *rIt = toupper(*rIt);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_upper(size_t in_from, size_t in_to) noexcept {
    to_upper(begin() + in_from, end() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_upper() noexcept {
    to_upper(begin(), end());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::resize(size_type in_amount) {
    _resize(in_amount);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::swap(character_sequence& in_src) noexcept {
    std::swap(raw_data, in_src.raw_data);
    std::swap(mCapacity, in_src.mCapacity);
    std::swap(mSize, in_src.mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR GENERIC character_sequence<SeqType, SeqBase, Allocator>::copy(pointer in_src, size_t in_len, size_t in_pos) noexcept {
    this->copy_bytes(in_src, raw_data + in_pos, in_len);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::serialize(safe_buffer& out_buffer) {
    if (mSize)
    {
        out_buffer.bfLength = mSize;
        out_buffer.bfSource = new value_type[mSize];

        this->copy_bytes(out_buffer.bfSource, raw_data, mSize); // DO NOT INCLUDE NULL TERMINATOR
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator> character_sequence<SeqType, SeqBase, Allocator>::deserialize(pointer in_buffer, SIZE_T in_length) noexcept {
    return mbase::character_sequence(in_buffer, in_length);
}

template<typename SeqType, typename SeqBase, typename Allocator>
bool operator==(const character_sequence<SeqType, SeqBase, Allocator>& in_lhs, const character_sequence<SeqType, SeqBase, Allocator>& in_rhs) noexcept {
    return type_sequence<SeqType>::is_equal(in_lhs.raw_data, in_rhs.raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
bool operator!=(const character_sequence<SeqType, SeqBase, Allocator>& in_lhs, const character_sequence<SeqType, SeqBase, Allocator>& in_rhs) noexcept {
    return !type_sequence<SeqType>::is_equal(in_lhs.raw_data, in_rhs.raw_data);
}

using string = character_sequence<IBYTE>;
using wstring = string; // WSTRING WILL BE IMPLEMENTED LATER
using string_view = string; // STRING VIEW WILL BE IMPLEMENTED LATER

MBASE_STD_END

#endif // MBASE_STRING_H