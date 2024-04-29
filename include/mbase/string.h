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
    MBASE_INLINE_EXPR character_sequence(pointer in_raw, size_type in_size, size_type in_capacity) noexcept; // THIS CONSTRUCTOR MUST NOT BE CALLED FROM OUTSIDE
    MBASE_INLINE_EXPR MBASE_EXPLICIT character_sequence(const Allocator& in_alloc) noexcept;
    MBASE_INLINE_EXPR character_sequence(size_type in_size, value_type in_ch, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs, size_type in_pos, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs, size_type in_pos, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc = Allocator());
    template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<SeqType, typename std::iterator_traits<InputIt>::value_type>>>
    MBASE_INLINE_EXPR character_sequence(InputIt in_begin, InputIt in_end, const Allocator& in_alloc = Allocator()) : raw_data(nullptr), mSize(0), mCapacity(8), externalAllocator(Allocator()) {
        _build_string(mCapacity);
        for(in_begin; in_begin != in_end; in_begin++)
        {
            push_back(*in_begin);
        }
    }
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
    MBASE_INLINE character_sequence& operator=(value_type in_rhs) noexcept;
    MBASE_INLINE character_sequence& operator=(std::initializer_list<value_type> in_chars) noexcept;
    MBASE_INLINE character_sequence& operator=(std::nullptr_t) = delete;

    // observation functions
    MBASE_ND("raw string data being ignored") MBASE_INLINE_EXPR pointer data() const noexcept;
    MBASE_ND("raw string being ignored") MBASE_INLINE_EXPR const_pointer c_str() const noexcept;
    MBASE_ND("first character being ignored") MBASE_INLINE_EXPR reference front() noexcept;
    MBASE_ND("last character being ignored") MBASE_INLINE_EXPR reference back() noexcept;

    MBASE_ND("character unused") MBASE_INLINE_EXPR reference at(size_type in_pos);
    MBASE_ND("character unused") MBASE_INLINE_EXPR const_reference at(size_type in_pos) const;
    
    MBASE_ND("character unused") MBASE_INLINE_EXPR reference operator[](size_type in_pos);
    MBASE_ND("character unused") MBASE_INLINE_EXPR const_reference operator[](size_type in_pos) const;

    MBASE_ND("iterator being ignored") MBASE_INLINE iterator begin() noexcept;
    MBASE_ND("iterator being ignored") MBASE_INLINE iterator end() noexcept;
    MBASE_ND("const iterator being ignored") MBASE_INLINE const_iterator cbegin() const noexcept;
    MBASE_ND("const iterator being ignored") MBASE_INLINE const_iterator cend() const noexcept;
    MBASE_ND("reverse iterator being ignored") MBASE_INLINE reverse_iterator rbegin() noexcept;
    MBASE_ND("const reverse iterator being ignored") MBASE_INLINE const_reverse_iterator crbegin() const noexcept;
    MBASE_ND("reverse iterator being ignored") MBASE_INLINE reverse_iterator rend() noexcept;
    MBASE_ND("const reverse iterator being ignored") MBASE_INLINE const_reverse_iterator crend() const noexcept;

    MBASE_ND("ignoring the allocator") MBASE_INLINE_EXPR Allocator get_allocator() const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find(const_pointer in_src, size_type in_pos = 0) const noexcept; // 3
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find(const character_sequence& in_src, size_type in_pos = 0) const noexcept; // 1
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find(value_type in_char, size_type in_pos = 0) const noexcept; // 4
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find(const_pointer in_src, size_type in_pos, size_type in_count) const noexcept; // 2
    MBASE_ND("founded string not being used") MBASE_INLINE size_type rfind(const character_sequence& in_src, size_type in_pos = npos) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type rfind(const_pointer in_str, size_type in_pos, size_type in_count) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type rfind(const_pointer in_str, size_type in_pos) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type rfind(value_type in_char, size_type in_pos = npos) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_first_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_first_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_first_of(const_pointer in_str, size_type in_pos = 0) const;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_first_of(value_type in_char, size_type in_pos = 0) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_first_not_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_first_not_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_first_not_of(const_pointer in_str, size_type in_pos = 0) const;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_first_not_of(value_type in_char, size_type in_pos = 0) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_last_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_last_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_last_of(const_pointer in_str, size_type in_pos = 0) const;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_last_of(value_type in_char, size_type in_pos = 0) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_last_not_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_last_not_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_last_not_of(const_pointer in_str, size_type in_pos = 0) const;
    MBASE_ND("founded string not being used") MBASE_INLINE size_type find_last_not_of(value_type in_char, size_type in_pos = 0) const noexcept;
    MBASE_ND("founded string not being used") MBASE_INLINE character_sequence substr(size_type in_pos = 0, size_type in_count = npos) const;
    MBASE_ND("founded string not being used") MBASE_INLINE character_sequence subarray(size_type in_pos = 0, size_type in_count = npos) const;
    MBASE_ND("string observation ignored") MBASE_INLINE bool starts_with(value_type in_char) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool starts_with(MSTRING in_str) const;
    MBASE_ND("string observation ignored") MBASE_INLINE bool ends_with(value_type in_char) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool ends_with(MSTRING in_str) const;
    MBASE_ND("string observation ignored") MBASE_INLINE bool contains(value_type in_char) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool contains(MSTRING in_str) const;
    MBASE_ND("string observation ignored") MBASE_INLINE I32 compare(const character_sequence& in_src) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE I32 compare(size_type in_pos1, size_type in_pos2, const character_sequence& in_src) const;
    MBASE_ND("string observation ignored") MBASE_INLINE I32 compare(size_type in_pos1, size_type in_count1, const character_sequence& in_src, size_type in_pos2, size_type in_count2 = npos) const;
    MBASE_ND("string observation ignored") MBASE_INLINE I32 compare(const_pointer in_str) const;
    MBASE_ND("string observation ignored") MBASE_INLINE I32 compare(size_type in_pos1, size_type in_count1, const_pointer in_str) const;
    MBASE_ND("string observation ignored") MBASE_INLINE I32 compare(size_type in_pos1, size_type in_count1, const_pointer in_src, size_type in_count2) const;
    MBASE_ND("string observation ignored") MBASE_INLINE_EXPR size_type size() const noexcept;
    //MBASE_ND("string observation ignored") MBASE_INLINE_EXPR size_type length() const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE_EXPR size_type max_size() const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE_EXPR size_type capacity() const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE_EXPR bool empty() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_alnum(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_alnum(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_alnum() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_alpha(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_alpha(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_alpha() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_lower(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_lower(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_lower() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_upper(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_upper(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_upper() const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_blank(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_blank(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_blank() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_control(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_control(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_control() const noexcept;
    // characters such as: 0x20(' '), 0x0c('\f'), 0x0a('\n'), 0x0d('\r'), 0x09('\t'), 0x0b('\v')
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_space(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_space(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_space() const noexcept;
    // characters such as: !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_punctuation(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_punctuation(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND("string observation ignored") MBASE_INLINE bool is_punctuation() const noexcept;

    bool operator==(const character_sequence& in_rhs) noexcept;
    bool operator!=(const character_sequence& in_rhs) noexcept;

    MBASE_INLINE_EXPR character_sequence& assign(size_type in_count, value_type in_ch);
    MBASE_INLINE_EXPR character_sequence& assign(const character_sequence& in_str);
    MBASE_INLINE_EXPR character_sequence& assign(const character_sequence& in_str, size_type in_pos, size_type in_count = npos);
    MBASE_INLINE_EXPR character_sequence& assign(character_sequence&& in_str) noexcept;
    MBASE_INLINE_EXPR character_sequence& assign(const_pointer in_str, size_type in_count);
    MBASE_INLINE_EXPR character_sequence& assign(const_pointer in_str);
    template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<SeqType, typename std::iterator_traits<InputIt>::value_type>>>
    MBASE_INLINE_EXPR character_sequence& assign(InputIt in_begin, InputIt in_end) {
        character_sequence in_seq(in_begin, in_end);
        *this = std::move(in_seq);
        return *this;
    }
    MBASE_INLINE_EXPR character_sequence& assign(std::initializer_list<value_type> in_chars);
    MBASE_INLINE_EXPR character_sequence& append(size_type in_count, value_type in_char);
    MBASE_INLINE_EXPR character_sequence& append(const character_sequence& in_str);
    MBASE_INLINE_EXPR character_sequence& append(const character_sequence& in_str, size_type in_pos, size_type in_count = npos);
    MBASE_INLINE_EXPR character_sequence& append(const_pointer in_str, size_type in_count);
    MBASE_INLINE_EXPR character_sequence& append(const_pointer in_str);
    template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<SeqType, typename std::iterator_traits<InputIt>::value_type>>>
    MBASE_INLINE_EXPR character_sequence& append(InputIt in_begin, InputIt in_end) {
        for(in_begin; in_begin != in_end; in_begin++)
        {
            push_back(*in_begin);
        }
        return *this;
    }
    MBASE_INLINE_EXPR character_sequence& append(std::initializer_list<value_type> in_chars);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, size_type in_count, value_type in_ch);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const_pointer in_str);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const_pointer in_str, size_type in_count);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const character_sequence& in_str);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const character_sequence& in_str, size_type in_sindex, size_type in_count = npos);
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, value_type in_char);
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, size_type in_count, value_type in_char);
    template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<SeqType, typename std::iterator_traits<InputIt>::value_type>>>
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, InputIt in_begin, InputIt in_end) {
        if (in_pos >= cend())
        {
            for(in_begin; in_begin != in_end; in_begin++)
            {
                push_back(*in_begin);
            }
        }
        else
        {
            const_iterator inIt = in_pos;
            character_sequence newString(cbegin(), inIt);
            for (in_begin; in_begin != in_end; in_begin++)
            {
                newString.push_back(*in_begin);
            }

            for(inIt; inIt != cend(); inIt++)
            {
                newString.push_back(*inIt);
            }

            *this = std::move(newString);
        }

        return begin();
    }
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, std::initializer_list<value_type> in_chars);
    MBASE_INLINE GENERIC pop_back() noexcept;
    MBASE_INLINE GENERIC push_back(const value_type& in_character) noexcept;
    MBASE_ND("converted string not being used.") MBASE_INLINE I32 to_i32() const noexcept;
    MBASE_ND("converted string not being used.") MBASE_INLINE I64 to_i64() const noexcept;
    MBASE_ND("converted string not being used.") MBASE_INLINE F32 to_f32() const noexcept;
    MBASE_ND("converted string not being used.") MBASE_INLINE F64 to_f64() const noexcept;
    MBASE_INLINE GENERIC to_lower(iterator in_from, iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_lower(reverse_iterator in_from, reverse_iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_lower(size_t in_from, size_t in_to) noexcept;
    MBASE_INLINE GENERIC to_lower() noexcept;
    MBASE_INLINE GENERIC to_upper(iterator in_from, iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_upper(reverse_iterator in_from, reverse_iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_upper(size_t in_from, size_t in_to) noexcept;
    MBASE_INLINE GENERIC to_upper() noexcept;
    MBASE_INLINE GENERIC resize(size_type in_amount);
    MBASE_INLINE_EXPR GENERIC resize(size_type in_amount, value_type in_char); // DON'T KNOW WHAT TO IMPLEMENT
    MBASE_INLINE GENERIC swap(character_sequence& in_src) noexcept;
    MBASE_INLINE_EXPR GENERIC copy(pointer in_src, size_t in_len, size_t in_pos = 0) noexcept;
    MBASE_INLINE_EXPR GENERIC reserve(size_type in_new_cap);
    MBASE_INLINE_EXPR GENERIC clear() noexcept;
    MBASE_INLINE_EXPR GENERIC shrink_to_fit() noexcept;
    MBASE_INLINE_EXPR character_sequence& erase(size_type in_index = 0, size_type in_count = npos);
    MBASE_INLINE_EXPR iterator erase(const_iterator in_pos);
    MBASE_INLINE_EXPR iterator erase(const_iterator in_from, const_iterator in_to);
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
    MBASE_INLINE static character_sequence deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length) noexcept;
    
    MBASE_ND("converted string not being used.") I32 to_i32() noexcept { return atoi(this->c_str()); }
    MBASE_ND("converted string not being used.") I32 to_i64() noexcept { return _atoi64(this->c_str()); }
    MBASE_ND("converted string not being used.") F32 to_f32() noexcept { return strtof(this->c_str(), nullptr); }
    MBASE_ND("converted string not being used.") F64 to_f64() noexcept { return atof(this->c_str()); }
    MBASE_ND("converted string not being used.") static I32 to_i32(const character_sequence& in_string) noexcept { return atoi(in_string.c_str()); }
    MBASE_ND("converted string not being used.") static I32 to_i64(const character_sequence& in_string) noexcept { return _atoi64(in_string.c_str()); }
    MBASE_ND("converted string not being used.") static F32 to_f32(const character_sequence& in_string) noexcept { return strtof(in_string.c_str(), nullptr); }
    MBASE_ND("converted string not being used.") static F64 to_f64(const character_sequence& in_string) noexcept { return atof(in_string.c_str()); }

    MBASE_ND("string observation ignored") static MBASE_INLINE bool is_alnum(const value_type& in_char) noexcept { return isalnum(in_char); }
    MBASE_ND("string observation ignored") static MBASE_INLINE bool is_alpha(const value_type& in_char) noexcept { return isalpha(in_char); }
    MBASE_ND("string observation ignored") static MBASE_INLINE bool is_lower(const value_type& in_char) noexcept { return islower(in_char); }
    MBASE_ND("string observation ignored") static MBASE_INLINE bool is_upper(const value_type& in_char) noexcept { return isupper(in_char); }
    MBASE_ND("string observation ignored") static MBASE_INLINE bool is_blank(const value_type& in_char) noexcept { return isblank(in_char); }
    MBASE_ND("string observation ignored") static MBASE_INLINE bool is_control(const value_type& in_char) noexcept { return iscntrl(in_char); }
    MBASE_ND("string observation ignored") static MBASE_INLINE bool is_space(const value_type& in_char) noexcept { return isspace(in_char); }
    MBASE_ND("string observation ignored") static MBASE_INLINE bool is_punctuation(const value_type& in_char) noexcept { return ispunct(in_char); }

    template<typename ... Params>
    MBASE_ND("formatted string not being used") static character_sequence from_format(const_pointer in_format, Params ... in_params) noexcept {
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

    MBASE_INLINE_EXPR friend character_sequence operator+(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept {
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
    MBASE_INLINE_EXPR friend character_sequence operator+(const character_sequence& in_lhs, const_pointer in_rhs) noexcept {
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
    MBASE_INLINE_EXPR friend character_sequence operator+(const character_sequence& in_lhs, value_type in_rhs) noexcept {
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
    MBASE_INLINE_EXPR friend character_sequence operator+(const_pointer in_lhs, const character_sequence& in_rhs) {
        return character_sequence(in_lhs) + in_rhs;
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(value_type in_lhs, const character_sequence& in_rhs) {
        return character_sequence(1, in_lhs) + in_rhs;
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(character_sequence&& in_lhs, character_sequence&& in_rhs) {
        return std::move(in_lhs + in_rhs);
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(character_sequence&& in_lhs, const character_sequence& in_rhs) {
        return std::move(in_lhs + in_rhs);
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(character_sequence&& in_lhs, const_pointer in_rhs) {
        return std::move(in_lhs + character_sequence(in_rhs));
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(character_sequence&& in_lhs, value_type in_rhs) {
        return std::move(in_lhs + character_sequence(1, in_rhs));
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(const character_sequence& in_lhs, character_sequence&& in_rhs) {
        return std::move(in_lhs + in_rhs);
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(const_pointer in_lhs, character_sequence&& in_rhs) {
        return std::move(in_lhs + in_rhs);
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(value_type in_lhs, character_sequence&& in_rhs) {
        return std::move(character_sequence(1, in_lhs) + in_rhs);
    }

    MBASE_INLINE_EXPR friend bool operator==(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator!=(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator<(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator<=(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator>(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator>=(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator==(const character_sequence& in_lhs, const_pointer in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator!=(const character_sequence& in_lhs, const_pointer in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator<(const character_sequence& in_lhs, const_pointer in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator<=(const character_sequence& in_lhs, const_pointer in_rhs) noexcept;
    MBASE_INLINE_EXPR friend bool operator>(const character_sequence& in_lhs, const_pointer in_rhs) noexcept; // IMPL
    MBASE_INLINE_EXPR friend bool operator>=(const character_sequence& in_lhs, const_pointer in_rhs) noexcept; // IMPL
#if MBASE_CPP_VERSION >= 20
    MBASE_INLINE_EXPR friend bool operator<=>(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept; // IMPL
    MBASE_INLINE_EXPR friend bool operator<=>(const character_sequence& in_lhs, const_pointer in_rhs) noexcept; // IMPL
#endif // MBASE_CPP_VERSION >= 20
    MBASE_INLINE_EXPR friend bool operator==(const_pointer in_lhs, const character_sequence& in_rhs) noexcept; // IMPL
    MBASE_INLINE_EXPR friend bool operator!=(const_pointer in_lhs, const character_sequence& in_rhs) noexcept; // IMPL
    MBASE_INLINE_EXPR friend bool operator<(const_pointer in_lhs, const character_sequence& in_rhs) noexcept; // IMPL
    MBASE_INLINE_EXPR friend bool operator<=(const_pointer in_lhs, const character_sequence& in_rhs) noexcept; // IMPL
    MBASE_INLINE_EXPR friend bool operator>(const_pointer in_lhs, const character_sequence& in_rhs) noexcept; // IMPL
    MBASE_INLINE_EXPR friend bool operator>=(const_pointer in_lhs, const character_sequence& in_rhs) noexcept; // IMPL

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

    MBASE_INLINE GENERIC _resize(size_type in_size) noexcept 
    {
        if(in_size > mSize)
        {
            size_type newCap = _calculate_capacity(in_size);
            pointer new_data = externalAllocator.allocate(newCap, true);
            this->copy_bytes(new_data, raw_data, mSize);
            externalAllocator.deallocate(raw_data, 0);
            raw_data = new_data;
            mSize = in_size;
            mCapacity = newCap;
        }
        else
        {
            size_type expectedSize = mSize - in_size;
            //expectedSize = in_size;
            reverse_iterator rit = rbegin();
            for (I32 i = 0; i < expectedSize; i++)
            {
                *rit = '\0';
                rit++;
            }
            mSize = in_size;
        }
    }

    MBASE_INLINE size_type _calculate_capacity(size_type in_size) noexcept 
    {
        // SELF-NOTE: MAY NOT BE PERFORMANT
        // SELF-NOTE: CONSIDER THE IMPLEMENTATION AGAIN
        size_type base_capacity = 8;
        while(base_capacity < in_size)
        {
            base_capacity *= 2;
        }

        return base_capacity;
    }

    MBASE_INLINE GENERIC _build_string(size_type in_capacity) noexcept 
    {
        mCapacity = in_capacity;
        raw_data = externalAllocator.allocate(mCapacity, true);
    }
    allocator_type externalAllocator;
    pointer raw_data;
    size_type mCapacity;
    size_type mSize;
};

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence() noexcept : raw_data(nullptr), mSize(0), mCapacity(8), externalAllocator(Allocator()) 
{
    raw_data = externalAllocator.allocate(mCapacity, true);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(pointer in_raw, size_type in_size, size_type in_capacity) noexcept : raw_data(in_raw), mSize(in_size), mCapacity(in_capacity), externalAllocator(Allocator()) 
{
    // THIS CONSTRUCTOR MUST NOT BE CALLED FROM OUTSIDE
}


template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const Allocator& in_alloc) noexcept : raw_data(nullptr), mSize(0), mCapacity(8), externalAllocator(in_alloc) 
{
    raw_data = externalAllocator.allocate(mCapacity, true);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(size_type in_size, value_type in_ch, const Allocator& in_alloc) : raw_data(nullptr), mSize(0), externalAllocator(in_alloc) 
{
    _build_string(this->_calculate_capacity(in_size));
    for(I32 i = 0; i < in_size; i++)
    {
        push_back(in_ch);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const character_sequence& in_rhs, size_type in_pos, const Allocator& in_alloc) : raw_data(nullptr), mSize(0), externalAllocator(in_alloc) 
{
    const_iterator itr_end = in_rhs.cend();
    const_iterator itr_pos = const_iterator(in_rhs.raw_data + in_pos);
    if (itr_pos > itr_end) {
        // MEANS WE ARE OUT OF BOUNDS
        _build_string(in_rhs.mCapacity);
        mSize = in_rhs.mSize;
        this->copy_bytes(raw_data, in_rhs.raw_data, mSize);
    }
    else {
        _build_string(in_rhs.mCapacity);
        for(itr_pos; itr_pos != itr_end; itr_pos++)
        {
            push_back(*itr_pos);
        }
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(character_sequence&& in_rhs, size_type in_pos, const Allocator& in_alloc) : raw_data(nullptr), mSize(0), externalAllocator(in_alloc) 
{
    character_sequence cs(std::move(in_rhs));
    const_iterator itr_end = cs.cend();
    const_iterator itr_pos = const_iterator(cs.raw_data + in_pos);
    if (itr_pos > itr_end) {
        // MEANS WE ARE OUT OF BOUNDS
        _build_string(cs.mCapacity);
        mSize = cs.mSize;
        this->copy_bytes(raw_data, cs.raw_data, mSize);
    }
    else {
        _build_string(cs.mCapacity);
        for (itr_pos; itr_pos != itr_end; itr_pos++)
        {
            push_back(*itr_pos);
        }
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const character_sequence& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc) : raw_data(nullptr), mSize(0), externalAllocator(in_alloc) 
{
    const_iterator itr_end = in_rhs.cend();
    const_iterator itr_pos = const_iterator(in_rhs.raw_data + in_pos);
    if (itr_pos > itr_end || in_count == npos) {
        // MEANS WE ARE OUT OF BOUNDS
        _build_string(in_rhs.mCapacity);
        mSize = in_rhs.mSize;
        this->copy_bytes(raw_data, in_rhs.raw_data, mSize);
    }
    else {
        _build_string(in_rhs.mCapacity);
        for (I32 i = 0; i < in_count; i++)
        {
            if(itr_pos == itr_end)
            {
                break;
            }
            push_back(*itr_pos);
            itr_pos++;
        }
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(character_sequence&& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc) : raw_data(nullptr), mSize(0), externalAllocator(in_alloc) 
{
    character_sequence cs(std::move(in_rhs));
    const_iterator itr_end = cs.cend();
    const_iterator itr_pos = const_iterator(cs.raw_data + in_pos);
    if (itr_pos > itr_end || in_count == npos) {
        // MEANS WE ARE OUT OF BOUNDS
        _build_string(cs.mCapacity);
        mSize = cs.mSize;
        this->copy_bytes(raw_data, cs.raw_data, mSize);
    }
    else {
        _build_string(cs.mCapacity);
        for (I32 i = 0; i < in_count; i++)
        {
            if (itr_pos == itr_end)
            {
                break;
            }
            push_back(*itr_pos);
            itr_pos++;
        }
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const_pointer in_string) noexcept : externalAllocator(Allocator()) 
{
    mSize = this->length(in_string);
    _build_string(this->_calculate_capacity(mSize));
    this->copy_bytes(raw_data, in_string, mSize); // no need the include null-terminator since we zero the memory
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const_pointer in_string, size_type in_length) noexcept : externalAllocator(Allocator()) 
{
    mSize = in_length;
    _build_string(this->_calculate_capacity(mSize));
    this->copy_bytes(raw_data, in_string, mSize); // no need the include null-terminator since we zero the memory
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const character_sequence& in_rhs) noexcept : raw_data(nullptr), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity), externalAllocator(Allocator()) 
{
    raw_data = externalAllocator.allocate(mCapacity, true);
    this->copy_bytes(raw_data, in_rhs.raw_data, mSize); // no need the include null-terminator since we zero the memory
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(character_sequence&& in_rhs) noexcept : raw_data(in_rhs.raw_data), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity), externalAllocator(Allocator()) 
{
    in_rhs.raw_data = NULL;
    in_rhs.mSize = 0;
    in_rhs.mCapacity = 0;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const character_sequence& in_rhs, const Allocator& in_alloc) : raw_data(nullptr), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity), externalAllocator(in_alloc) 
{
    raw_data = externalAllocator.allocate(mCapacity, true);
    this->copy_bytes(raw_data, in_rhs.raw_data, mSize); // no need the include null-terminator since we zero the memory
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(character_sequence&& in_rhs, const Allocator& in_alloc) : raw_data(in_rhs.raw_data), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity), externalAllocator(in_alloc) 
{
    in_rhs.raw_data = NULL;
    in_rhs.mSize = 0;
    in_rhs.mCapacity = 0;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(std::initializer_list<value_type> in_chars, const Allocator& in_alloc) : raw_data(nullptr), mSize(0), mCapacity(8), externalAllocator(in_alloc) 
{
    _build_string(this->_calculate_capacity(in_chars.size()));
    const value_type* vt = in_chars.begin();
    for(vt; vt != in_chars.end(); vt++)
    {
        push_back(*vt);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::~character_sequence() noexcept 
{
    if(raw_data)
    {
        externalAllocator.deallocate(raw_data, 0);
    }   
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(const character_sequence& in_rhs) noexcept 
{
    if (raw_data)
    {
        externalAllocator.deallocate(raw_data, 0);
    }

    mSize = in_rhs.mSize;
    _build_string(in_rhs.mCapacity);
    this->copy_bytes(raw_data, in_rhs.raw_data, mSize);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(character_sequence&& in_rhs) noexcept 
{
    if (raw_data)
    {
        externalAllocator.deallocate(raw_data, 0);
    }

    mSize = in_rhs.mSize;
    mCapacity = in_rhs.mCapacity;
    raw_data = in_rhs.raw_data;
    in_rhs.raw_data = nullptr;
    in_rhs.mSize = 0;
    in_rhs.mCapacity = 0;

    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(const_pointer in_rhs) noexcept 
{
    if (raw_data)
    {
        externalAllocator.deallocate(raw_data, 0);
    }

    size_type st_length = this->length(in_rhs);
    mCapacity = this->_calculate_capacity(st_length);

    raw_data = externalAllocator.allocate(mCapacity, true);
    this->copy_bytes(raw_data, in_rhs, st_length);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(value_type in_rhs) noexcept 
{
    assign(1, in_rhs);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(std::initializer_list<value_type> in_chars) noexcept 
{
    character_sequence chr(in_chars);
    *this = std::move(chr);
    return *this;
}


template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("raw string data being ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::pointer character_sequence<SeqType, SeqBase, Allocator>::data() const noexcept 
{
    return raw_data;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("raw string being ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_pointer character_sequence<SeqType, SeqBase, Allocator>::c_str() const noexcept 
{
    return raw_data;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("first character being ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::front() noexcept 
{
    return raw_data[0];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("last character being ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::back() noexcept 
{
    return raw_data[mSize - 1];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("character unused") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::at(size_type in_pos) 
{
    // EXCEPTION WILL BE IMPLEMENTED HERE
    return raw_data[in_pos];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("character unused") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_reference character_sequence<SeqType, SeqBase, Allocator>::at(size_type in_pos) const 
{
    // EXCEPTION WILL BE IMPLEMENTED HERE
    return raw_data[in_pos];
}


template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("character unused") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::operator[](size_type in_pos) 
{
    return raw_data[in_pos];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("character unused") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_reference character_sequence<SeqType, SeqBase, Allocator>::operator[](size_type in_pos) const 
{
    return raw_data[in_pos];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::begin() noexcept 
{
    return iterator(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::end() noexcept 
{
    return iterator(raw_data + mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("const iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_iterator character_sequence<SeqType, SeqBase, Allocator>::cbegin() const noexcept 
{
    return const_iterator(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("const iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_iterator character_sequence<SeqType, SeqBase, Allocator>::cend() const noexcept 
{
    return const_iterator(raw_data + mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("reverse iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::rbegin() noexcept 
{
    return reverse_iterator(raw_data + (mSize - 1));
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("const reverse iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::crbegin() const noexcept 
{
    return const_reverse_iterator(raw_data + (mSize - 1));
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("reverse iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::rend() noexcept 
{
    return reverse_iterator(raw_data - 1);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("const reverse iterator being ignored") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::crend() const noexcept 
{
    return const_reverse_iterator(raw_data - 1);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("ignoring the allocator") MBASE_INLINE_EXPR Allocator character_sequence<SeqType, SeqBase, Allocator>::get_allocator() const noexcept 
{
    return externalAllocator;
}


template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("founded string not being used") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(const_pointer in_src, size_type in_pos) const noexcept 
{
    const_iterator itBegin = cbegin() + in_pos;
    const_pointer tmpResult = strstr(itBegin.get(), in_src);
    if (tmpResult)
    {
        return tmpResult - (raw_data + in_pos);
    }

    return npos;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("founded string not being used") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(const character_sequence& in_src, size_type in_pos) const noexcept 
{
    return find(in_src.c_str(), in_pos);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("founded string not being used") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(value_type in_char, size_type in_pos) const noexcept 
{
    const_iterator itBegin = cbegin() + in_pos;
    const_pointer tmpResult = strchr(itBegin.get(), in_char);
    if (tmpResult)
    {
        return tmpResult - (raw_data + in_pos);
    }

    return npos;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("founded string not being used") MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(const_pointer in_src, size_type in_pos, size_type in_count) const noexcept
{
    character_sequence cs(in_src);
    character_sequence newStr = std::move(cs.substr(0, in_count));

    return find(newStr, in_pos);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("founded string not being used") MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator> character_sequence<SeqType, SeqBase, Allocator>::substr(size_type in_pos, size_type in_count) const 
{
    const_iterator inIt = cbegin() + in_pos;
    if(inIt >= cend())
    {
        return *this;
    }

    else
    {
        character_sequence cs;
        for(difference_type i = 0; i < in_count; i++)
        {
            cs.push_back(*inIt);
            if(inIt == cend())
            {
                return cs;
            }
            inIt++;
        }
        
        return cs;
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("founded string not being used") MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator> character_sequence<SeqType, SeqBase, Allocator>::subarray(size_type in_pos, size_type in_count) const
{
    return std::move(substr(in_pos, in_count));
}


template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::size() const noexcept 
{
    return mSize;
}

//template<typename SeqType, typename SeqBase, typename Allocator>
//MBASE_ND("string observation ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::length() const noexcept 
//{
//    return mSize;
//}
template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::max_size() const noexcept 
{
    return std::numeric_limits<difference_type>::max();
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::capacity() const noexcept 
{
    return mCapacity;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE_EXPR bool character_sequence<SeqType, SeqBase, Allocator>::empty() const noexcept {
    return mSize == 0;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alnum(const_iterator in_begin, const_iterator in_end) const noexcept 
{
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
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alnum(size_type in_from, size_type in_to) const noexcept 
{
    return is_alnum(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alnum() const noexcept 
{
    return is_alnum(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alpha(const_iterator in_begin, const_iterator in_end) const noexcept 
{
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
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alpha(size_type in_from, size_type in_to) const noexcept 
{
    return is_alpha(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alpha() const noexcept 
{
    return is_alpha(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_lower(const_iterator in_begin, const_iterator in_end) const noexcept 
{
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
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_lower(size_type in_from, size_type in_to) const noexcept 
{
    return is_lower(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_lower() const noexcept 
{
    return is_lower(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_upper(const_iterator in_begin, const_iterator in_end) const noexcept 
{
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
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_upper(size_type in_from, size_type in_to) const noexcept 
{
    return is_upper(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_upper() const noexcept 
{
    return is_upper(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_blank(const_iterator in_begin, const_iterator in_end) const noexcept 
{
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
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_blank(size_type in_from, size_type in_to) const noexcept 
{
    return is_blank(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_blank() const noexcept 
{
    return is_blank(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_control(const_iterator in_begin, const_iterator in_end) const noexcept 
{
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
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_control(size_type in_from, size_type in_to) const noexcept 
{
    return is_control(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_control() const noexcept 
{
    return is_control(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_space(const_iterator in_begin, const_iterator in_end) const noexcept 
{
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
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_space(size_type in_from, size_type in_to) const noexcept 
{
    return is_space(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_space() const noexcept 
{
    return is_space(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_punctuation(const_iterator in_begin, const_iterator in_end) const noexcept 
{
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
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_punctuation(size_type in_from, size_type in_to) const noexcept 
{
    return is_punctuation(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("string observation ignored") MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_punctuation() const noexcept 
{
    return is_punctuation(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
bool character_sequence<SeqType, SeqBase, Allocator>::operator==(const character_sequence& in_rhs) noexcept 
{
    return this->is_equal(raw_data, in_rhs.raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
bool character_sequence<SeqType, SeqBase, Allocator>::operator!=(const character_sequence& in_rhs) noexcept 
{
    return !this->is_equal(raw_data, in_rhs.raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::assign(size_type in_count, value_type in_ch) 
{
    character_sequence freshString(in_count, in_ch);
    *this = freshString;
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::assign(const character_sequence& in_str) 
{
    *this = in_str;
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::assign(const character_sequence& in_str, size_type in_pos, size_type in_count) 
{
    const_iterator iPos = in_str.cbegin() + in_pos;
    const_iterator iEnd = in_str.cend();
    if (iPos > iEnd || in_count == npos)
    {
        *this = in_str;
    }
    else
    {
        character_sequence cs;
        for (I32 i = 0; i < in_count; i++)
        {
            cs.push_back(*iPos);
            iPos++;
        }
        *this = std::move(cs);
    }
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::assign(character_sequence&& in_str) noexcept 
{
    *this = std::move(in_str);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::assign(const_pointer in_str, size_type in_count) 
{
    *this = std::move(character_sequence(in_str, in_count));
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::assign(const_pointer in_str) 
{
    *this = in_str;
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::assign(std::initializer_list<value_type> in_chars) 
{
    character_sequence cseq(in_chars);
    *this = std::move(cseq);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::append(size_type in_count, value_type in_char) 
{
    for(I32 i = 0; i < in_count; i++)
    {
        push_back(in_char);
    }
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::append(const character_sequence& in_str) 
{
    *this += in_str;
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::append(const character_sequence& in_str, size_type in_pos, size_type in_count) 
{
    const_iterator iPos = in_str.cbegin() + in_pos;
    const_iterator iEnd = in_str.cend();
    if (iPos > iEnd || in_count == npos) 
    {
        *this += in_str;
    }
    else 
    {
        for(I32 i = 0; i < in_count; i++)
        {
            push_back(*iPos);
            iPos++;
        }
    }

    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::append(const_pointer in_str, size_type in_count) 
{
    character_sequence cs(in_str, in_count);
    *this += cs;
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::append(const_pointer in_str) 
{
    character_sequence cs(in_str);
    *this += cs;
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::append(std::initializer_list<value_type> in_chars)
{
    const value_type* vt = in_chars.begin();
    for(vt; vt != in_chars.end(); vt++)
    {
        push_back(*vt);
    }
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::insert(size_type in_index, size_type in_count, value_type in_ch) 
{
    const_iterator inIt = cbegin() + in_index;
    if(inIt >= cend())
    {
        for(I32 i = 0; i < in_count; i++)
        {
            push_back(in_ch);
        }
    }
    else
    {
        character_sequence newString(cbegin(), inIt);
        for(I32 i = 0; i < in_count; i++)
        {
            newString.push_back(in_ch);
        }

        for(inIt; inIt != cend(); inIt++)
        {
            newString.push_back(*inIt);
        }
        *this = std::move(newString);
    }

    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::insert(size_type in_index, const_pointer in_str)
{
    const_iterator inIt = cbegin() + in_index;

    if (inIt >= cend())
    {
        *this += in_str;
    }
    else
    {
        character_sequence newString(cbegin(), inIt);
        newString += in_str;
        
        for (inIt; inIt != cend(); inIt++)
        {
            newString.push_back(*inIt);
        }
        *this = std::move(newString);
    }
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::insert(size_type in_index, const_pointer in_str, size_type in_count)
{
    const_iterator inIt = cbegin() + in_index;
    if (inIt >= cend())
    {
        character_sequence cs(in_str, in_count);
        *this += cs;
    }
    else
    {
        character_sequence newString(cbegin(), inIt);
        character_sequence cs(in_str, in_count);
        newString += cs;
        for (inIt; inIt != cend(); inIt++)
        {
            newString.push_back(*inIt);
        }
        *this = std::move(newString);
    }
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::insert(size_type in_index, const character_sequence& in_str)
{
    const_iterator inIt = cbegin() + in_index;
    if (inIt >= cend())
    {
        *this += in_str;
    }
    else
    {
        character_sequence newString(cbegin(), inIt);
        newString += in_str;
        for (inIt; inIt != cend(); inIt++)
        {
            newString.push_back(*inIt);
        }

        *this = std::move(newString);
    }

    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::insert(size_type in_index, const character_sequence& in_str, size_type in_sindex, size_type in_count) 
{
    // NPOS RELATED
    const_iterator inIt = cbegin() + in_index;
    character_sequence newStr = in_str.substr(in_sindex, in_count);
    if (inIt >= cend() || in_count == npos)
    {
        *this += newStr;
    }
    else
    {
        character_sequence newString(cbegin(), inIt);
        newString += newStr;
        for(inIt; inIt != cend(); inIt++)
        {
            newString.push_back(*inIt);
        }

        *this = std::move(newString);
    }
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::insert(const_iterator in_pos, value_type in_char) 
{
    // do not know what to return
    if(in_pos >= cend())
    {
        push_back(in_char);
    }
    else
    {
        const_iterator inIt = in_pos;
        character_sequence newString(cbegin(), inIt);
        newString.push_back(in_char);
        for(inIt; inIt != cend(); inIt++)
        {
            newString.push_back(*inIt);
        }
        *this = std::move(newString);
    }
    return begin();
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::insert(const_iterator in_pos, size_type in_count, value_type in_char) 
{
    // do not know what to return
    if (in_pos >= cend())
    {
        for (difference_type i = 0; i < in_count; i++)
        {
            push_back(in_char);
        }
    }
    else
    {
        const_iterator inIt = in_pos;
        character_sequence newString(cbegin(), inIt);
        
        for(difference_type i = 0; i < in_count; i++)
        {
            newString.push_back(in_char);
        }

        for (inIt; inIt != cend(); inIt++)
        {
            newString.push_back(*inIt);
        }
        *this = std::move(newString);
    }
    return begin();

}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::insert(const_iterator in_pos, std::initializer_list<value_type> in_chars)
{
    if (in_pos >= cend())
    {
        *this += in_chars;
    }
    else
    {
        const_iterator inIt = in_pos;
        character_sequence newString(cbegin(), inIt);

        newString += in_chars;

        for (inIt; inIt != cend(); inIt++)
        {
            newString.push_back(*inIt);
        }
        *this = std::move(newString);
    }
    return begin();
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::pop_back() noexcept 
{
    mSize--;
    raw_data[mSize] = '\0';
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::push_back(const value_type& in_character) noexcept 
{
    mSize++;
    if (mSize >= mCapacity)
    {
        size_type currentSize = mSize;
        _resize(mCapacity *= 2);
        mSize = currentSize;
    }
    raw_data[mSize - 1] = in_character;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("converted string not being used.") MBASE_INLINE I32 character_sequence<SeqType, SeqBase, Allocator>::to_i32() const noexcept 
{
    return atoi(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("converted string not being used.") MBASE_INLINE I64 character_sequence<SeqType, SeqBase, Allocator>::to_i64() const noexcept 
{
    return _atoi64(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("converted string not being used.") MBASE_INLINE F32 character_sequence<SeqType, SeqBase, Allocator>::to_f32() const noexcept 
{
    return strtof(raw_data, nullptr);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND("converted string not being used.") MBASE_INLINE F64 character_sequence<SeqType, SeqBase, Allocator>::to_f64() const noexcept 
{
    return atof(raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_lower(iterator in_from, iterator in_to) noexcept 
{
    iterator It = in_from;
    iterator endIt = in_to;
    for (It; It != endIt; It++)
    {
        *It = tolower(*It);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_lower(reverse_iterator in_from, reverse_iterator in_to) noexcept 
{
    reverse_iterator rIt = in_from;
    reverse_iterator endrIt = in_to;
    for (rIt; rIt != endrIt; rIt++)
    {
        *rIt = tolower(*rIt);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_lower(size_t in_from, size_t in_to) noexcept 
{
    to_lower(begin() + in_from, end() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_lower() noexcept 
{
    to_lower(begin(), end());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_upper(iterator in_from, iterator in_to) noexcept 
{
    iterator It = in_from;
    iterator endIt = in_to;
    for (It; It != endIt; It++)
    {
        *It = toupper(*It);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_upper(reverse_iterator in_from, reverse_iterator in_to) noexcept 
{
    reverse_iterator rIt = in_from;
    reverse_iterator endrIt = in_to;
    for (rIt; rIt != endrIt; rIt++)
    {
        *rIt = toupper(*rIt);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_upper(size_t in_from, size_t in_to) noexcept 
{
    to_upper(begin() + in_from, end() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::to_upper() noexcept 
{
    to_upper(begin(), end());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::resize(size_type in_amount) 
{
    _resize(in_amount);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::swap(character_sequence& in_src) noexcept 
{
    std::swap(raw_data, in_src.raw_data);
    std::swap(mCapacity, in_src.mCapacity);
    std::swap(mSize, in_src.mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR GENERIC character_sequence<SeqType, SeqBase, Allocator>::copy(pointer in_src, size_t in_len, size_t in_pos) noexcept 
{
    this->copy_bytes(in_src, raw_data + in_pos, in_len);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR GENERIC character_sequence<SeqType, SeqBase, Allocator>::clear() noexcept 
{
    this->fill(raw_data, '\0', mSize);
    mSize = 0;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR GENERIC character_sequence<SeqType, SeqBase, Allocator>::reserve(size_type in_new_cap) {
    // kinda skeptic about this
    size_type actualSize = mSize;
    _resize(in_new_cap);
    mSize = actualSize;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR GENERIC character_sequence<SeqType, SeqBase, Allocator>::shrink_to_fit() noexcept
{
    if (mSize == mCapacity)
    {
        // DO NOTHING LITERALLY
        return;
    }
    else
    {
        character_sequence cs(raw_data, mSize);
        *this = std::move(cs);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::erase(size_type in_index, size_type in_count) {
    if(in_index > mSize)
    {
        throw std::out_of_range("index out of range");
    }

    const_iterator inPos = cbegin() + in_index;

    if(inPos == cend())
    {
        for(difference_type i = 0; i < in_count; i++)
        {
            pop_back();
        }
        
        return *this;
    }

    else
    {
        character_sequence cs(cbegin(), inPos);

        for(difference_type i = 0; i < in_count; i++)
        {
            inPos++;
        }

        for(inPos; inPos != cend(); inPos++)
        {
            cs.push_back(*inPos);
        }

        *this = std::move(cs);
    }

    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::erase(const_iterator in_pos) {
    if(in_pos >= cend())
    {
        pop_back();
    }
    else
    {
        character_sequence cs(cbegin(), in_pos);
        in_pos++;
        for(in_pos; in_pos != cend(); in_pos++)
        {
            cs.push_back(*in_pos);
        }

        *this = std::move(cs);

    }
    // TODO: RETURN THE CORRECT THING
    return begin();
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::erase(const_iterator in_from, const_iterator in_to) {
    if(in_to < in_from)
    {
        throw std::invalid_argument("from to where is inconsistent");
    }

    else
    {
        character_sequence cs(cbegin(), in_from);
        in_from++;
        for(in_from; in_from != cend(); in_from++)
        {
            if(in_from < in_to)
            {

            }
            else
            {
                cs.push_back(*in_from);
            }
        }

        *this = std::move(cs);
    }
    // TODO: RETURN THE CORRECT THING
    return begin();
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(size_type in_pos, size_type in_count, const character_sequence& in_str)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(const_iterator in_begin, const_iterator in_end, const character_sequence& in_str)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(size_type in_pos, size_type in_count, const character_sequence& in_str, size_type in_pos2, size_type in_count2)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(size_type in_pos, size_type in_count, const_pointer in_str, size_type in_count2)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(const_iterator in_begin, const_iterator in_end, const_pointer in_str, size_type in_count2)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(size_type in_pos, size_type in_count, const_pointer in_str)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(const_iterator in_begin, const_iterator in_end, const_pointer in_str)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(size_type in_pos, size_type in_count, size_type in_count2, value_type in_char)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(const_iterator in_begin, const_iterator in_end, size_type in_count2, value_type in_char)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::replace(const_iterator in_begin, const_iterator in_end, std::initializer_list<value_type> in_chars)
{
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(const character_sequence& in_rhs) noexcept 
{
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
character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(const_pointer in_rhs) noexcept 
{
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
character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(const value_type& in_character) noexcept 
{
    push_back(in_character);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::serialize(safe_buffer& out_buffer) 
{
    if (mSize)
    {
        out_buffer.bfLength = mSize;
        out_buffer.bfSource = this->externalAllocator.allocate(mSize);

        this->copy_bytes(out_buffer.bfSource, raw_data, mSize); // DO NOT INCLUDE NULL TERMINATOR
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator> character_sequence<SeqType, SeqBase, Allocator>::deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length) noexcept 
{
    return character_sequence(in_buffer, in_length);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR bool operator==(const character_sequence<SeqType, SeqBase, Allocator>& in_lhs, const character_sequence<SeqType, SeqBase, Allocator>& in_rhs) noexcept 
{
    return type_sequence<SeqType>::is_equal(in_lhs.raw_data, in_rhs.raw_data);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR bool operator!=(const character_sequence<SeqType, SeqBase, Allocator>& in_lhs, const character_sequence<SeqType, SeqBase, Allocator>& in_rhs) noexcept 
{
    return !type_sequence<SeqType>::is_equal(in_lhs.raw_data, in_rhs.raw_data);
}


using string = character_sequence<IBYTE>;
using wstring = string; // WSTRING WILL BE IMPLEMENTED LATER
using string_view = string; // STRING VIEW WILL BE IMPLEMENTED LATER

MBASE_STD_END

#endif // MBASE_STRING_H