#ifndef MBASE_STRING_H
#define MBASE_STRING_H

//#include <mbase/vector.h>
#include <mbase/char_stream.h>
#include <mbase/type_sequence.h> // mbase::type_sequence
#include <stdexcept>
#include <ctype.h>

#ifdef MBASE_PLATFORM_WINDOWS

#pragma comment(lib, "rpcrt4.lib")
#include <Windows.h> // For utf conversions
#include <rpc.h> // For UUID Generation

#endif // MBASE_PLATFORM_WINDOWS

#ifdef MBASE_PLATFORM_UNIX

#include <uuid/uuid.h> // For uuid generation
#include <iconv.h> // For utf conversions
#include <errno.h>

#endif // MBASE_PLATFORM_UNIX

MBASE_STD_BEGIN

static const U32 gStringDefaultCapacity = 8;

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

    /* ===== BUILDER METHODS BEGIN ===== */
    MBASE_INLINE_EXPR character_sequence() noexcept;
    MBASE_INLINE_EXPR character_sequence(pointer in_raw, size_type in_size, size_type in_capacity) noexcept; // THIS CONSTRUCTOR MUST NOT BE CALLED FROM OUTSIDE
    MBASE_INLINE_EXPR MBASE_EXPLICIT character_sequence(const Allocator& in_alloc) noexcept;
    MBASE_INLINE_EXPR character_sequence(size_type in_size, value_type in_ch, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs, size_type in_pos, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs, size_type in_pos, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc = Allocator());
    template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<value_type, typename std::iterator_traits<InputIt>::value_type>>>
    MBASE_INLINE_EXPR character_sequence(InputIt in_begin, InputIt in_end, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR character_sequence(const_pointer in_string) noexcept;
    MBASE_INLINE_EXPR character_sequence(const_pointer in_string, size_type in_length) noexcept;
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs) noexcept;
    MBASE_INLINE_EXPR character_sequence(const character_sequence& in_rhs, const Allocator& in_alloc);
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs) noexcept;
    MBASE_INLINE_EXPR character_sequence(character_sequence&& in_rhs, const Allocator& in_alloc);
    MBASE_INLINE_EXPR character_sequence(std::initializer_list<value_type> in_chars, const Allocator& in_alloc = Allocator());
    MBASE_INLINE_EXPR ~character_sequence() noexcept;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OPERATOR BUILDER METHODS BEGIN ===== */
    MBASE_INLINE character_sequence& operator=(const character_sequence& in_rhs) noexcept;
    MBASE_INLINE character_sequence& operator=(character_sequence&& in_rhs) noexcept;
    MBASE_INLINE character_sequence& operator=(const_pointer in_rhs) noexcept;
    MBASE_INLINE character_sequence& operator=(value_type in_rhs) noexcept;
    MBASE_INLINE character_sequence& operator=(std::initializer_list<value_type> in_chars) noexcept;
    MBASE_INLINE character_sequence& operator=(std::nullptr_t) = delete;
    /* ===== OPERATOR BUILDER METHODS END ===== */
    
    /* ===== ITERATOR METHODS BEGIN ===== */
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE iterator begin() noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE iterator end() noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator begin() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator end() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator cbegin() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator cend() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE reverse_iterator rbegin() noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reverse_iterator rbegin() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reverse_iterator crbegin() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE reverse_iterator rend() noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reverse_iterator rend() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reverse_iterator crend() const noexcept;
    /* ===== ITERATOR METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference operator[](size_type in_pos);
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference operator[](size_type in_pos) const;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR pointer data() noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_pointer data() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_pointer c_str() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference front() noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference front() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference back() noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference back() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference at(size_type in_pos);
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference at(size_type in_pos) const;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR Allocator get_allocator() const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find(const_pointer in_src, size_type in_pos = 0) const noexcept; // 3
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find(const character_sequence& in_src, size_type in_pos = 0) const noexcept; // 1
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find(value_type in_char, size_type in_pos = 0) const noexcept; // 4
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find(const_pointer in_src, size_type in_pos, size_type in_count) const noexcept; // 2
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type rfind(const character_sequence& in_src, size_type in_pos = npos) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type rfind(const_pointer in_str, size_type in_pos, size_type in_count) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type rfind(const_pointer in_str, size_type in_pos) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type rfind(value_type in_char, size_type in_pos = npos) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_first_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_first_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_first_of(const_pointer in_str, size_type in_pos = 0) const;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_first_of(value_type in_char, size_type in_pos = 0) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_first_not_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_first_not_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_first_not_of(const_pointer in_str, size_type in_pos = 0) const;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_first_not_of(value_type in_char, size_type in_pos = 0) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_last_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_last_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_last_of(const_pointer in_str, size_type in_pos = 0) const;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_last_of(value_type in_char, size_type in_pos = 0) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_last_not_of(const character_sequence& in_src, size_type in_pos = 0) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_last_not_of(const_pointer in_str, size_type in_pos, size_type in_count) const;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_last_not_of(const_pointer in_str, size_type in_pos = 0) const;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type find_last_not_of(value_type in_char, size_type in_pos = 0) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE character_sequence substr(size_type in_pos = 0, size_type in_count = npos) const;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE character_sequence subarray(size_type in_pos = 0, size_type in_count = npos) const;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool starts_with(value_type in_char) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool starts_with(const_pointer in_str) const;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool ends_with(value_type in_char) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool ends_with(const_pointer in_str) const;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool contains(value_type in_char) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool contains(const_pointer in_str) const;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 compare(const character_sequence& in_src) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 compare(size_type in_pos1, size_type in_count1, const character_sequence& in_src) const;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 compare(size_type in_pos1, size_type in_count1, const character_sequence& in_src, size_type in_pos2, size_type in_count2 = npos) const;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type get_serialized_size() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type size() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type length() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type max_size() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type capacity() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool empty() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_alnum(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_alnum(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_alnum() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_alpha(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_alpha(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_alpha() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_lower(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_lower(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_lower() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_upper(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_upper(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_upper() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_blank(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_blank(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_blank() const noexcept;
    // if there is a space character in range, the returned value will be false
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_control(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_control(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_control() const noexcept;
    // characters such as: 0x20(' '), 0x0c('\f'), 0x0a('\n'), 0x0d('\r'), 0x09('\t'), 0x0b('\v')
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_space(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_space(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_space() const noexcept;
    // characters such as: !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_punctuation(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_punctuation(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_punctuation() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_print(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_print(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_print() const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_digit(const_iterator in_begin, const_iterator in_end) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_digit(size_type in_from, size_type in_to) const noexcept;
    MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_digit() const noexcept;

    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const character_sequence& in_rhs) noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const character_sequence& in_rhs) noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator<(const character_sequence& in_rhs) noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator<=(const character_sequence& in_rhs) noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator>(const character_sequence& in_rhs) noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator>=(const character_sequence& in_rhs) noexcept;
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== STATE-MODIFIER METHODS BEGIN ===== */
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
    MBASE_INLINE_EXPR character_sequence& append(InputIt in_begin, InputIt in_end);
    MBASE_INLINE_EXPR character_sequence& append(std::initializer_list<value_type> in_chars);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, size_type in_count, value_type in_ch);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const_pointer in_str);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const_pointer in_str, size_type in_count);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const character_sequence& in_str);
    MBASE_INLINE_EXPR character_sequence& insert(size_type in_index, const character_sequence& in_str, size_type in_sindex, size_type in_count = npos);
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, value_type in_char);
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, size_type in_count, value_type in_char);
    template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<SeqType, typename std::iterator_traits<InputIt>::value_type>>>
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, InputIt in_begin, InputIt in_end);
    MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, std::initializer_list<value_type> in_chars);
    MBASE_INLINE GENERIC pop_back() noexcept;
    MBASE_INLINE GENERIC push_back(const value_type& in_character) noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE I32 to_i32() const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE I64 to_i64() const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE F32 to_f32() const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE F64 to_f64() const noexcept;
    MBASE_INLINE GENERIC to_lower(iterator in_from, iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_lower(reverse_iterator in_from, reverse_iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_lower(size_t in_from, size_t in_to) noexcept;
    MBASE_INLINE GENERIC to_lower() noexcept;
    MBASE_INLINE GENERIC to_upper(iterator in_from, iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_upper(reverse_iterator in_from, reverse_iterator in_to) noexcept;
    MBASE_INLINE GENERIC to_upper(size_t in_from, size_t in_to) noexcept;
    MBASE_INLINE GENERIC to_upper() noexcept;
    MBASE_INLINE GENERIC resize(size_type in_amount);
    MBASE_INLINE GENERIC resize(size_type in_amount, value_type in_char);
    MBASE_INLINE GENERIC swap(character_sequence& in_src) noexcept;
    MBASE_INLINE_EXPR GENERIC copy(pointer in_src, size_t in_len, size_t in_pos = 0) noexcept;
    MBASE_INLINE_EXPR GENERIC reserve(size_type in_new_cap);
    MBASE_INLINE_EXPR GENERIC clear() noexcept;
    MBASE_INLINE_EXPR GENERIC shrink_to_fit() noexcept;
    MBASE_INLINE_EXPR character_sequence& erase(size_type in_index = 0, size_type in_count = npos);
    MBASE_INLINE_EXPR iterator erase(const_iterator in_pos);
    MBASE_INLINE_EXPR iterator erase(const_iterator in_from, const_iterator in_to);
    MBASE_INLINE_EXPR character_sequence& replace(size_type in_pos, size_type in_count, const character_sequence& in_str); // IMPLEMENT
    MBASE_INLINE_EXPR character_sequence& replace(iterator in_begin, iterator in_end, const character_sequence& in_str); // IMPLEMENT 
    MBASE_INLINE_EXPR character_sequence& replace(size_type in_pos, size_type in_count, const character_sequence& in_str, size_type in_pos2, size_type in_count2); // IMPLEMENT
    MBASE_INLINE_EXPR character_sequence& replace(size_type in_pos, size_type in_count, const_pointer in_str, size_type in_count2); // IMPLEMENT
    MBASE_INLINE_EXPR character_sequence& replace(const_iterator in_begin, const_iterator in_end, const_pointer in_str, size_type in_count2); // IMPLEMENT
    MBASE_INLINE_EXPR character_sequence& replace(size_type in_pos, size_type in_count, size_type in_count2, value_type in_char); // IMPLEMENT
    MBASE_INLINE_EXPR character_sequence& replace(const_iterator in_begin, const_iterator in_end, size_type in_count2, value_type in_char); // IMPLEMENT
    MBASE_INLINE_EXPR character_sequence& remove_all(value_type in_value);
    /* ===== STATE-MODIFIER METHODS END ===== */

    /* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
    character_sequence& operator+=(const character_sequence& in_rhs) noexcept;
    // character_sequence& operator+=(const_pointer in_rhs) noexcept;
    character_sequence& operator+=(const value_type& in_character) noexcept;
    character_sequence& operator+=(std::initializer_list<value_type> in_chars);
    /* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

    MBASE_INLINE static character_sequence deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length, SIZE_T& bytes_processed);
    
    /* ===== NON-MODIFIER METHODS BEGIN ===== */
    template<typename SourceContainer>
    MBASE_INLINE GENERIC split(const character_sequence& in_delimiters, SourceContainer& out_strings) noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) I32 to_i32() noexcept { return SeqBase::cnv_to_i32(this->c_str()); }
    MBASE_ND(MBASE_RESULT_IGNORE) I32 to_i64() noexcept { return SeqBase::cnv_to_i64(this->c_str()); }
    MBASE_ND(MBASE_RESULT_IGNORE) F32 to_f32() noexcept { return SeqBase::cnv_to_f32(this->c_str()); }
    MBASE_ND(MBASE_RESULT_IGNORE) F64 to_f64() noexcept { return SeqBase::cnv_to_f64(this->c_str()); }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE character_sequence get_extension(character_sequence in_rhs)
    {
        for(reverse_iterator rIt = in_rhs.rbegin(); rIt != in_rhs.rend(); ++rIt)
        {
            if(*rIt == '.')
            {
                character_sequence extensionString;
                --rIt;
                for(; rIt != in_rhs.rbegin() - 1; --rIt)
                {
                    extensionString.push_back(*rIt);
                }
                return extensionString;
            }
        }
        return character_sequence();
    }
    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) const;
    /* ===== NON-MODIFIER METHODS END ===== */

    /* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
    
    // TODO: MOVE THIS SECTION TO TYPE_SEQUENCE
    MBASE_ND(MBASE_RESULT_IGNORE) static I32 to_i32(const character_sequence& in_string) noexcept { return SeqBase::cnv_to_i32(in_string.c_str()); }
    MBASE_ND(MBASE_RESULT_IGNORE) static I32 to_i64(const character_sequence& in_string) noexcept { return SeqBase::cnv_to_i64(in_string.c_str()); }
    MBASE_ND(MBASE_RESULT_IGNORE) static F32 to_f32(const character_sequence& in_string) noexcept { return SeqBase::cnv_to_f32(in_string.c_str()); }
    MBASE_ND(MBASE_RESULT_IGNORE) static F64 to_f64(const character_sequence& in_string) noexcept { return SeqBase::cnv_to_f64(in_string.c_str()); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_alnum(const value_type& in_char) noexcept { return SeqBase::type_is_alnum(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_alpha(const value_type& in_char) noexcept { return SeqBase::type_is_alpha(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_lower(const value_type& in_char) noexcept { return SeqBase::type_is_lower(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_upper(const value_type& in_char) noexcept { return SeqBase::type_is_upper(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_blank(const value_type& in_char) noexcept { return SeqBase::type_is_blank(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_control(const value_type& in_char) noexcept { return SeqBase::type_is_control(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_space(const value_type& in_char) noexcept { return SeqBase::type_is_space(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_punctuation(const value_type& in_char) noexcept { return SeqBase::type_is_punctuation(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_print(const value_type& in_char) noexcept { return SeqBase::type_is_print(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_digit(const value_type& in_char) noexcept { return SeqBase::type_is_digit(in_char); }
    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_integer(const_pointer in_string) noexcept 
    { 
        if (in_string == NULL || *in_string == SeqBase::null_value)
        {
            return 0;
        }

        if (*in_string == '-' || *in_string == '+') 
        {
            in_string++;
        }

        while (*in_string) 
        {
            if (!is_digit(*in_string)) 
            {
                return 0;
            }
            in_string++;
        }
        return 1;
    }

    MBASE_ND(MBASE_OBS_IGNORE) static MBASE_INLINE bool is_float(const_pointer in_string) noexcept
    {
        I32 tempDecimalControl = 0;
        I32 tempDigitControl = 0;

        if (in_string == NULL || *in_string == SeqBase::null_value)
        {
            return 0;
        }

        if (*in_string == '-' || *in_string == '+') 
        {
            in_string++;
        }

        while (*in_string) 
        {
            if (*in_string == '.') 
            {
                if (tempDecimalControl) 
                {
                    return 0;
                }
                tempDecimalControl = 1;
            }
            else if (is_digit(*in_string)) 
            {
                tempDigitControl = 1;
            }
            else 
            {
                return 0;
            }
            in_string++;
        }

        if (!tempDigitControl) 
        {
            return 0;
        }

        return 1;
    }

    template<typename ... Params>
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE character_sequence from_format(const_pointer in_format, Params ... in_params) noexcept;
    
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE character_sequence generate_uuid(bool in_remove_dashes = true) noexcept 
    {
#ifdef MBASE_PLATFORM_WINDOWS
        UUID tmpUuid;
        UuidCreate(&tmpUuid);
        IBYTEBUFFER uuidString;
        UuidToStringA(&tmpUuid, (RPC_CSTR*)&uuidString);

        if (in_remove_dashes)
        {
            I32 uuidLength = SeqBase::length_bytes(uuidString);
            character_sequence newUuid;
            newUuid.reserve(uuidLength);
            for (I32 i = 0; i < uuidLength; ++i)
            {
                if (uuidString[i] == '-')
                {
                    continue;
                }
                newUuid.push_back(uuidString[i]);
            }
            RpcStringFreeA((RPC_CSTR*)&uuidString);
            return newUuid;
        }
        character_sequence newUuid(uuidString);
        RpcStringFreeA((RPC_CSTR*)&uuidString);
        return newUuid;
#endif // MBASE_PLATFORM_WINDOWS

#ifdef MBASE_PLATFORM_UNIX
        // do not forget to link libuuid
        uuid_t tmpUuid;
        IBYTE uuidString[37];

        uuid_generate(tmpUuid);
        uuid_unparse_lower(tmpUuid, uuidString);

        if (in_remove_dashes)
        {
            character_sequence newUuid;
            newUuid.reserve(32);

            for (I32 i = 0; uuidString[i] != '\0'; ++i)
            {
                if (uuidString[i] == '-')
                {
                    continue;
                }
                newUuid.push_back(uuidString[i]);
            }
            return newUuid;
        }

        return character_sequence(uuidString);
#endif // MBASE_PLATFORM_LINUX
    }
    /* ===== NON-MEMBER FUNCTIONS END ===== */

    /* ===== OPERATOR NON-MEMBER FUNCTIONS BEGIN ===== */
    MBASE_INLINE_EXPR friend character_sequence operator+(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept {
        I32 totalSize = in_lhs.mSize + in_rhs.mSize;
        I32 totalCapacity = in_lhs.mCapacity;

        while (totalSize >= totalCapacity)
        {
            totalCapacity *= 2;
        }
        allocator<SeqType> alc;
        pointer new_data = alc.allocate(totalCapacity, true);
        //this->length();
        SeqBase::concat(new_data, in_lhs.mRawData, in_lhs.mSize);
        SeqBase::concat(new_data + in_lhs.mSize, in_rhs.mRawData, in_rhs.mSize);
        return character_sequence(new_data, totalSize, totalCapacity);
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(const character_sequence& in_lhs, const_pointer in_rhs) noexcept {
        size_type rhsSize = SeqBase::length_bytes(in_rhs);
        if (!rhsSize)
        {
            return character_sequence(in_lhs);
        }
        size_type totalSize = in_lhs.mSize + rhsSize;
        size_type totalCapacity = in_lhs.mCapacity;
        while (totalSize >= totalCapacity)
        {
            totalCapacity *= 2;
        }
        allocator<SeqType> alc;
        pointer new_data = alc.allocate(totalCapacity, true);

        SeqBase::concat(new_data, in_lhs.mRawData, in_lhs.mSize);
        SeqBase::concat(new_data + in_lhs.mSize, in_rhs, rhsSize);

        return character_sequence(new_data, totalSize, totalCapacity);
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(const character_sequence& in_lhs, value_type in_rhs) noexcept {
        character_sequence cs = in_lhs;
        cs.push_back(in_rhs);

        return cs;
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(const_pointer in_lhs, const character_sequence& in_rhs) {
        return character_sequence(in_lhs) + in_rhs;
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(value_type in_lhs, const character_sequence& in_rhs) {
        return character_sequence(1, in_lhs) + in_rhs;
    }
    MBASE_INLINE_EXPR friend character_sequence operator+(character_sequence&& in_lhs, character_sequence&& in_rhs) { return in_lhs + in_rhs; }
    MBASE_INLINE_EXPR friend character_sequence operator+(character_sequence&& in_lhs, const character_sequence& in_rhs) { return in_lhs + in_rhs; }
    MBASE_INLINE_EXPR friend character_sequence operator+(character_sequence&& in_lhs, const_pointer in_rhs) { return in_lhs + character_sequence(in_rhs); }
    MBASE_INLINE_EXPR friend character_sequence operator+(character_sequence&& in_lhs, value_type in_rhs) { return in_lhs + character_sequence(1, in_rhs); }
    MBASE_INLINE_EXPR friend character_sequence operator+(const character_sequence& in_lhs, character_sequence&& in_rhs) { return in_lhs + in_rhs; }
    MBASE_INLINE_EXPR friend character_sequence operator+(const_pointer in_lhs, character_sequence&& in_rhs) { return in_lhs + in_rhs; }
    MBASE_INLINE_EXPR friend character_sequence operator+(value_type in_lhs, character_sequence&& in_rhs) { return character_sequence(1, in_lhs) + in_rhs; }
    MBASE_INLINE_EXPR friend bool operator==(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept { return traits_type::is_equal(in_lhs.mRawData, in_rhs.mRawData); }
    MBASE_INLINE_EXPR friend bool operator!=(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept { return !traits_type::is_equal(in_lhs.mRawData, in_rhs.mRawData); }
    MBASE_INLINE_EXPR friend bool operator<(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept { return std::lexicographical_compare(in_lhs.cbegin(), in_lhs.cend(), in_rhs.cbegin(), in_rhs.cend()); }
    MBASE_INLINE_EXPR friend bool operator<=(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept { return std::lexicographical_compare(in_lhs.cbegin(), in_lhs.cend(), in_rhs.cbegin(), in_rhs.cend()) || in_lhs.is_equal(in_lhs.mRawData, in_rhs.mRawData); }
    MBASE_INLINE_EXPR friend bool operator>(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept { return std::lexicographical_compare(in_rhs.cbegin(), in_rhs.cend(), in_lhs.cbegin(), in_lhs.cend()); }
    MBASE_INLINE_EXPR friend bool operator>=(const character_sequence& in_lhs, const character_sequence& in_rhs) noexcept { return std::lexicographical_compare(in_rhs.cbegin(), in_rhs.cend(), in_lhs.cbegin(), in_lhs.cend()) || in_lhs.is_equal(in_lhs.mRawData, in_rhs.mRawData); }
    friend std::ostream& operator<<(std::ostream& os, const character_sequence& in_rhs) noexcept 
    {
        if (!in_rhs.mRawData)
        {
            // DO NOTHING IF THE DATA IS NOT PRESENT
        }
        else
        {
            os << in_rhs.data();
        }
        return os;
    }
    /* ===== OPERATOR NON-MEMBER FUNCTIONS END ===== */

    static MBASE_INLINE_EXPR const SIZE_T npos = -1;

private:

    /* ===== STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_INLINE GENERIC _resize(size_type in_size) noexcept;
    MBASE_INLINE GENERIC _resize(size_type in_size, value_type in_char) noexcept;
    MBASE_INLINE size_type _calculate_capacity(size_type in_size) noexcept;
    MBASE_INLINE GENERIC _build_string(size_type in_capacity) noexcept;
    MBASE_INLINE GENERIC _clear_self() noexcept;
    /* ===== STATE-MODIFIER METHODS END ===== */

    pointer mRawData;
    size_type mSize;
    size_type mCapacity;
    allocator_type mExternalAllocator;
};

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence() noexcept : mRawData(nullptr), mSize(0), mCapacity(gStringDefaultCapacity), mExternalAllocator(Allocator()) 
{
    _build_string(mCapacity);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(pointer in_raw, size_type in_size, size_type in_capacity) noexcept : mRawData(in_raw), mSize(in_size), mCapacity(in_capacity), mExternalAllocator(Allocator()) 
{
    // THIS CONSTRUCTOR MUST NOT BE CALLED FROM OUTSIDE
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const Allocator& in_alloc) noexcept : mRawData(nullptr), mSize(0), mCapacity(gStringDefaultCapacity), mExternalAllocator(in_alloc)
{
    _build_string(mCapacity);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(size_type in_size, value_type in_ch, const Allocator& in_alloc) : mRawData(nullptr), mSize(0), mExternalAllocator(in_alloc) 
{
    _build_string(this->_calculate_capacity(in_size));
    for(I32 i = 0; i < in_size; i++)
    {
        push_back(in_ch);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const character_sequence& in_rhs, size_type in_pos, const Allocator& in_alloc) : mRawData(nullptr), mSize(0), mExternalAllocator(in_alloc) 
{
    const_iterator itr_end = in_rhs.cend();
    const_iterator itr_pos = const_iterator(in_rhs.mRawData + in_pos);
    if (itr_pos > itr_end) {
        // MEANS WE ARE OUT OF BOUNDS
        _build_string(in_rhs.mCapacity);
        mSize = in_rhs.mSize;
        this->copy_bytes(mRawData, in_rhs.mRawData, mSize);
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
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(character_sequence&& in_rhs, size_type in_pos, const Allocator& in_alloc) : mRawData(nullptr), mSize(0), mExternalAllocator(in_alloc) 
{
    character_sequence cs(std::move(in_rhs));
    const_iterator itr_end = cs.cend();
    const_iterator itr_pos = const_iterator(cs.mRawData + in_pos);
    if (itr_pos > itr_end) {
        // MEANS WE ARE OUT OF BOUNDS
        _build_string(cs.mCapacity);
        mSize = cs.mSize;
        this->copy_bytes(mRawData, cs.mRawData, mSize);
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
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const character_sequence& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc) : mRawData(nullptr), mSize(0), mExternalAllocator(in_alloc) 
{
    const_iterator itr_end = in_rhs.cend();
    const_iterator itr_pos = const_iterator(in_rhs.mRawData + in_pos);
    if (itr_pos > itr_end || in_count == npos) {
        // MEANS WE ARE OUT OF BOUNDS
        _build_string(in_rhs.mCapacity);
        mSize = in_rhs.mSize;
        this->copy_bytes(mRawData, in_rhs.mRawData, mSize);
    }
    else {
        _build_string(in_rhs.mCapacity);
        for (difference_type i = 0; i < in_count; i++)
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
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(character_sequence&& in_rhs, size_type in_pos, size_type in_count, const Allocator& in_alloc) : mRawData(nullptr), mSize(0), mExternalAllocator(in_alloc) 
{
    character_sequence cs(std::move(in_rhs));
    const_iterator itr_end = cs.cend();
    const_iterator itr_pos = const_iterator(cs.mRawData + in_pos);
    if (itr_pos > itr_end || in_count == npos) {
        // MEANS WE ARE OUT OF BOUNDS
        _build_string(cs.mCapacity);
        mSize = cs.mSize;
        this->copy_bytes(mRawData, cs.mRawData, mSize);
    }
    else {
        _build_string(cs.mCapacity);
        for (difference_type i = 0; i < in_count; i++)
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
template<typename InputIt, typename>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(InputIt in_begin, InputIt in_end, const Allocator& in_alloc) : mRawData(nullptr), mSize(0), mCapacity(gStringDefaultCapacity), mExternalAllocator(Allocator())
{
    _build_string(mCapacity);
    for (in_begin; in_begin != in_end; in_begin++)
    {
        *this += *in_begin;
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const_pointer in_string) noexcept : mExternalAllocator(Allocator()) 
{
    mSize = this->length_bytes(in_string);
    _build_string(this->_calculate_capacity(mSize));
    this->copy_bytes(mRawData, in_string, mSize); // no need the include null-terminator since we zero the memory
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const_pointer in_string, size_type in_length) noexcept : mSize(in_length), mExternalAllocator(Allocator())
{
    _build_string(this->_calculate_capacity(mSize));
    this->copy_bytes(mRawData, in_string, mSize); // no need the include null-terminator since we zero the memory
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const character_sequence& in_rhs) noexcept : mRawData(nullptr), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity), mExternalAllocator(Allocator()) 
{
    _build_string(mCapacity);
    this->copy_bytes(mRawData, in_rhs.mRawData, mSize); // no need the include null-terminator since we zero the memory
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(character_sequence&& in_rhs) noexcept : mRawData(in_rhs.mRawData), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity), mExternalAllocator(Allocator()) 
{
    in_rhs.mRawData = NULL;
    in_rhs.mSize = 0;
    in_rhs.mCapacity = 0;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(const character_sequence& in_rhs, const Allocator& in_alloc) : mRawData(nullptr), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity), mExternalAllocator(in_alloc) 
{
    _build_string(mCapacity);
    this->copy_bytes(mRawData, in_rhs.mRawData, mSize); // no need the include null-terminator since we zero the memory
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(character_sequence&& in_rhs, const Allocator& in_alloc) : mRawData(in_rhs.mRawData), mSize(in_rhs.mSize), mCapacity(in_rhs.mCapacity), mExternalAllocator(in_alloc) 
{
    in_rhs.mRawData = NULL;
    in_rhs.mSize = 0;
    in_rhs.mCapacity = 0;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>::character_sequence(std::initializer_list<value_type> in_chars, const Allocator& in_alloc) : mRawData(nullptr), mSize(0), mCapacity(gStringDefaultCapacity), mExternalAllocator(in_alloc)
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
    if(mRawData)
    {
        mExternalAllocator.deallocate(mRawData);
        mRawData = nullptr;
        mSize = 0;
        mCapacity = 0;
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(const character_sequence& in_rhs) noexcept 
{
    _clear_self();
    mSize = in_rhs.mSize;
    _build_string(this->_calculate_capacity(mSize));
    this->copy_bytes(mRawData, in_rhs.mRawData, mSize);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(character_sequence&& in_rhs) noexcept 
{
    _clear_self();
    mSize = in_rhs.mSize;
    mCapacity = in_rhs.mCapacity;
    mRawData = in_rhs.mRawData;
    in_rhs.mRawData = nullptr;
    in_rhs.mSize = 0;
    in_rhs.mCapacity = 0;

    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(const_pointer in_rhs) noexcept 
{
    _clear_self();
    size_type st_length = this->length_bytes(in_rhs);
    mCapacity = this->_calculate_capacity(st_length);
    mSize = st_length;
    _build_string(mCapacity);
    this->copy_bytes(mRawData, in_rhs, st_length);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator=(value_type in_rhs) noexcept 
{
    clear();
    push_back(in_rhs);
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
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::begin() noexcept 
{
    return iterator(mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::end() noexcept 
{
    return iterator(mRawData + mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_iterator character_sequence<SeqType, SeqBase, Allocator>::begin() const noexcept
{
    return const_iterator(mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_iterator character_sequence<SeqType, SeqBase, Allocator>::end() const noexcept
{
    return const_iterator(mRawData + mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_iterator character_sequence<SeqType, SeqBase, Allocator>::cbegin() const noexcept 
{
    return const_iterator(mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_iterator character_sequence<SeqType, SeqBase, Allocator>::cend() const noexcept 
{
    return const_iterator(mRawData + mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::rbegin() noexcept 
{
    return reverse_iterator(mRawData + (mSize - 1));
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::rbegin() const noexcept
{
    return const_reverse_iterator(mRawData + (mSize - 1));
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::crbegin() const noexcept 
{
    return const_reverse_iterator(mRawData + (mSize - 1));
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::rend() noexcept 
{
    return reverse_iterator(mRawData - 1);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::rend() const noexcept
{
    return const_reverse_iterator(mRawData - 1);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::const_reverse_iterator character_sequence<SeqType, SeqBase, Allocator>::crend() const noexcept 
{
    return const_reverse_iterator(mRawData - 1);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::operator[](size_type in_pos)
{
    return mRawData[in_pos];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_reference character_sequence<SeqType, SeqBase, Allocator>::operator[](size_type in_pos) const
{
    return mRawData[in_pos];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::pointer character_sequence<SeqType, SeqBase, Allocator>::data() noexcept
{
    return mRawData;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_pointer character_sequence<SeqType, SeqBase, Allocator>::data() const noexcept
{
    return mRawData;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_pointer character_sequence<SeqType, SeqBase, Allocator>::c_str() const noexcept
{
    return mRawData;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::front() noexcept
{
    return mRawData[0];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_reference character_sequence<SeqType, SeqBase, Allocator>::front() const noexcept
{
    return mRawData[0];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::back() noexcept
{
    return mRawData[mSize - 1];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_reference character_sequence<SeqType, SeqBase, Allocator>::back() const noexcept
{
    return mRawData[mSize - 1];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::reference character_sequence<SeqType, SeqBase, Allocator>::at(size_type in_pos)
{
    // EXCEPTION WILL BE IMPLEMENTED HERE
    return mRawData[in_pos];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::const_reference character_sequence<SeqType, SeqBase, Allocator>::at(size_type in_pos) const
{
    // EXCEPTION WILL BE IMPLEMENTED HERE
    return mRawData[in_pos];
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR Allocator character_sequence<SeqType, SeqBase, Allocator>::get_allocator() const noexcept 
{
    return mExternalAllocator;
}


template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(const_pointer in_src, size_type in_pos) const noexcept 
{
    const_iterator itBegin = cbegin() + in_pos;
    const_pointer tmpResult = strstr(itBegin.get(), in_src);
    if (tmpResult)
    {
        return tmpResult - (mRawData + in_pos);
    }

    return npos;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(const character_sequence& in_src, size_type in_pos) const noexcept 
{
    return find(in_src.c_str(), in_pos);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(value_type in_char, size_type in_pos) const noexcept 
{
    const_iterator itBegin = cbegin() + in_pos;
    const_pointer tmpResult = strchr(itBegin.get(), in_char);
    if (tmpResult)
    {
        return tmpResult - (mRawData + in_pos);
    }

    return npos;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::find(const_pointer in_src, size_type in_pos, size_type in_count) const noexcept
{
    character_sequence cs(in_src);
    character_sequence newStr = std::move(cs.substr(0, in_count));

    return find(newStr, in_pos);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator> character_sequence<SeqType, SeqBase, Allocator>::substr(size_type in_pos, size_type in_count) const 
{
    const_iterator inIt = cbegin() + in_pos;
    if(inIt >= cend())
    {
        return *this;
    }

    else
    {
        character_sequence cs;
        for(size_type i = 0; i < in_count; i++)
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
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator> character_sequence<SeqType, SeqBase, Allocator>::subarray(size_type in_pos, size_type in_count) const
{
    return std::move(substr(in_pos, in_count));
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::starts_with(value_type in_char) const noexcept {
    if (front() == in_char)
    {
        return true;
    }

    return false;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::starts_with(const_pointer in_str) const
{
    size_type stringSize = this->length_bytes(in_str);
    if (stringSize > mSize)
    {
        return false;
    }

    return this->is_equal(mRawData, in_str, stringSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::ends_with(value_type in_char) const noexcept
{
    if (back() == in_char)
    {
        return true;
    }
    return false;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::ends_with(const_pointer in_str) const
{
    // MAY NOT WORK FOR NOW
    size_type stringSize = this->length_bytes(in_str);
    if (stringSize > mSize)
    {
        return false;
    }
    pointer lookupLocation = mRawData + (mSize - stringSize);
    return this->is_equal(lookupLocation, in_str, stringSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::contains(value_type in_char) const noexcept
{
    if (this->find(in_char) != npos)
    {
        return true;
    }
    return false;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::contains(const_pointer in_str) const
{
    if (this->find(in_str) != npos)
    {
        return true;
    }
    return false;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 character_sequence<SeqType, SeqBase, Allocator>::compare(const character_sequence& in_src) const noexcept
{
    return this->compare_bytes(mRawData, in_src.mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 character_sequence<SeqType, SeqBase, Allocator>::compare(size_type in_pos1, size_type in_count1, const character_sequence& in_src) const
{
    size_type toPosition = in_pos1 + in_count1;
    if(in_count1 > size() - in_pos1)
    {
        toPosition = size();
    }

    character_sequence cs(std::move(substr(in_pos1, in_count1)));

    return this->compare_bytes(cs.mRawData, in_src.mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 character_sequence<SeqType, SeqBase, Allocator>::compare(size_type in_pos1, size_type in_count1, const character_sequence& in_src, size_type in_pos2, size_type in_count2) const
{
    size_type toPosition = in_pos1 + in_count1;
    if (in_count1 > size() - in_pos1)
    {
        toPosition = size();
    }

    character_sequence cs(std::move(substr(in_pos1, toPosition)));

    toPosition = in_pos2 + in_count2;
    if (in_count2 > in_src.size() - in_pos2)
    {
        toPosition = in_src.size();
    }

    character_sequence cs2(std::move(substr(in_pos1, toPosition)));

    return this->compare_bytes(cs.mRawData, cs2.mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::get_serialized_size() const noexcept
{
    return sizeof(size_type) + mSize;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::size() const noexcept 
{
    return mSize;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::length() const noexcept 
{
    return mSize;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::max_size() const noexcept 
{
    size_type result = (std::numeric_limits<difference_type>::max)();
    return result;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::capacity() const noexcept 
{
    return mCapacity;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool character_sequence<SeqType, SeqBase, Allocator>::empty() const noexcept {
    return mSize == 0;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alnum(const_iterator in_begin, const_iterator in_end) const noexcept 
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
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alnum(size_type in_from, size_type in_to) const noexcept 
{
    return is_alnum(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alnum() const noexcept 
{
    return is_alnum(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alpha(const_iterator in_begin, const_iterator in_end) const noexcept 
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
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alpha(size_type in_from, size_type in_to) const noexcept 
{
    return is_alpha(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_alpha() const noexcept 
{
    return is_alpha(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_lower(const_iterator in_begin, const_iterator in_end) const noexcept 
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
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_lower(size_type in_from, size_type in_to) const noexcept 
{
    return is_lower(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_lower() const noexcept 
{
    return is_lower(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_upper(const_iterator in_begin, const_iterator in_end) const noexcept 
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
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_upper(size_type in_from, size_type in_to) const noexcept 
{
    return is_upper(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_upper() const noexcept 
{
    return is_upper(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_blank(const_iterator in_begin, const_iterator in_end) const noexcept 
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
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_blank(size_type in_from, size_type in_to) const noexcept 
{
    return is_blank(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_blank() const noexcept 
{
    return is_blank(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_control(const_iterator in_begin, const_iterator in_end) const noexcept 
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
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_control(size_type in_from, size_type in_to) const noexcept 
{
    return is_control(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_control() const noexcept 
{
    return is_control(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_space(const_iterator in_begin, const_iterator in_end) const noexcept 
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
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_space(size_type in_from, size_type in_to) const noexcept 
{
    return is_space(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_space() const noexcept 
{
    return is_space(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_punctuation(const_iterator in_begin, const_iterator in_end) const noexcept 
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
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_punctuation(size_type in_from, size_type in_to) const noexcept 
{
    return is_punctuation(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_punctuation() const noexcept 
{
    return is_punctuation(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_print(const_iterator in_begin, const_iterator in_end) const noexcept
{
    if (!size())
    {
        return false;
    }

    const_iterator It = in_begin;
    const_iterator endIt = in_end;

    for (It; It != endIt; It++)
    {
        if (!is_print(*It))
        {
            return false;
        }
    }

    return true;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_print(size_type in_from, size_type in_to) const noexcept
{
    return is_print(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_print() const noexcept
{
    return is_print(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_digit(const_iterator in_begin, const_iterator in_end) const noexcept
{
    if (!size())
    {
        return false;
    }

    const_iterator It = in_begin;
    const_iterator endIt = in_end;

    for (It; It != endIt; It++)
    {
        if (!is_digit(*It))
        {
            return false;
        }
    }

    return true;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_digit(size_type in_from, size_type in_to) const noexcept
{
    return is_digit(cbegin() + in_from, cbegin() + in_to);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::is_digit() const noexcept
{
    return is_digit(cbegin(), cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::operator==(const character_sequence& in_rhs) noexcept
{
    return this->is_equal(mRawData, in_rhs.mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::operator!=(const character_sequence& in_rhs) noexcept
{
    return !this->is_equal(mRawData, in_rhs.mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::operator<(const character_sequence& in_rhs) noexcept
{
    return std::lexicographical_compare(this->cbegin(), this->cend(), in_rhs.cbegin(), in_rhs.cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::operator<=(const character_sequence& in_rhs) noexcept
{
    return std::lexicographical_compare(this->cbegin(), this->cend(), in_rhs.cbegin(), in_rhs.cend()) || this->is_equal(mRawData, in_rhs.mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::operator>(const character_sequence& in_rhs) noexcept
{
    return std::lexicographical_compare(in_rhs.cbegin(), in_rhs.cend(), this->cbegin(), this->cend());
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool character_sequence<SeqType, SeqBase, Allocator>::operator>=(const character_sequence& in_rhs) noexcept
{
    return std::lexicographical_compare(in_rhs.cbegin(), in_rhs.cend(), this->cbegin(), this->cend()) || this->is_equal(mRawData, in_rhs.mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::assign(size_type in_count, value_type in_ch) 
{
    character_sequence freshString(in_count, in_ch);
    *this = std::move(freshString);
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
        for (size_type i = 0; i < in_count; i++)
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
template<typename InputIt, typename>
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::append(InputIt in_begin, InputIt in_end)
{
    for (in_begin; in_begin != in_end; in_begin++)
    {
        push_back(*in_begin);
    }
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
        for(size_type i = 0; i < in_count; i++)
        {
            push_back(in_ch);
        }
    }
    else
    {
        character_sequence newString(cbegin(), inIt);
        for(size_type i = 0; i < in_count; i++)
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
template<typename InputIt, typename>
MBASE_INLINE_EXPR typename character_sequence<SeqType, SeqBase, Allocator>::iterator character_sequence<SeqType, SeqBase, Allocator>::insert(const_iterator in_pos, InputIt in_begin, InputIt in_end)
{
    if (in_pos >= cend())
    {
        for (in_begin; in_begin != in_end; in_begin++)
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
    mRawData[mSize] = SeqBase::null_value;
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
    mRawData[mSize - 1] = in_character;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE I32 character_sequence<SeqType, SeqBase, Allocator>::to_i32() const noexcept 
{
    return atoi(mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE I64 character_sequence<SeqType, SeqBase, Allocator>::to_i64() const noexcept 
{
    return _atoi64(mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE F32 character_sequence<SeqType, SeqBase, Allocator>::to_f32() const noexcept 
{
    return strtof(mRawData, nullptr);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE F64 character_sequence<SeqType, SeqBase, Allocator>::to_f64() const noexcept 
{
    return atof(mRawData);
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
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::resize(size_type in_amount, value_type in_char)
{
    _resize(in_amount, in_char);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::swap(character_sequence& in_src) noexcept 
{
    std::swap(mRawData, in_src.mRawData);
    std::swap(mCapacity, in_src.mCapacity);
    std::swap(mSize, in_src.mSize);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR GENERIC character_sequence<SeqType, SeqBase, Allocator>::copy(pointer in_src, size_t in_len, size_t in_pos) noexcept 
{
    this->copy_bytes(in_src, mRawData + in_pos, in_len);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR GENERIC character_sequence<SeqType, SeqBase, Allocator>::clear() noexcept 
{
    this->fill(mRawData, SeqBase::null_value, mSize);
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
        character_sequence cs(mRawData, mSize);
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
MBASE_INLINE_EXPR character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::remove_all(value_type in_value)
{
    character_sequence newSequence;
    for(iterator It = begin(); It != end(); ++It)
    {
        if(*It != in_value)
        {
            newSequence.push_back(*It);
        }
    }

    *this = std::move(newSequence);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(const character_sequence& in_rhs) noexcept 
{
    if (!in_rhs.mSize)
    {
        return *this;
    }

    /*size_type totalSize = mSize + in_rhs.mSize;
    if (totalSize >= mCapacity)
    {
        while (totalSize >= mCapacity)
        {
            mCapacity *= 2;
        }
        _resize(mCapacity);
    }

    this->concat(mRawData + mSize, in_rhs.mRawData, in_rhs.mSize);
    mSize = totalSize;*/
    *this = *this + in_rhs;
    return *this;
}

//template<typename SeqType, typename SeqBase, typename Allocator>
//character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(const_pointer in_rhs) noexcept 
//{
//    if (!in_rhs)
//    {
//        return *this;
//    }
//    size_type rhsSize = this->length_bytes(in_rhs);
//    size_type totalSize = mSize + rhsSize;
//    if (totalSize == mSize)
//    {
//        return *this;
//    }
//
//    if (totalSize >= mCapacity)
//    {
//        while (totalSize >= mCapacity)
//        {
//            mCapacity *= 2;
//        }
//        _resize(mCapacity);
//    }
//
//    this->concat(mRawData + mSize, in_rhs, rhsSize);
//    mSize = totalSize;
//    return *this;
//}

template<typename SeqType, typename SeqBase, typename Allocator>
character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(const value_type& in_character) noexcept 
{
    push_back(in_character);
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
character_sequence<SeqType, SeqBase, Allocator>& character_sequence<SeqType, SeqBase, Allocator>::operator+=(std::initializer_list<value_type> in_chars) 
{
    append(std::move(in_chars));
    return *this;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::serialize(char_stream& out_buffer) const
{
    out_buffer.put_datan<SIZE_T>(mSize);
    if (mSize)
    {
        out_buffer.put_buffern(mRawData, mSize);
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator> character_sequence<SeqType, SeqBase, Allocator>::deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length, SIZE_T& bytes_processed)
{
    mbase::char_stream charStream(in_buffer, in_length);
    SIZE_T stringLength = charStream.get_datan<SIZE_T>();
    bytes_processed = sizeof(SIZE_T);
    if(stringLength)
    {
        IBYTEBUFFER strOffset = charStream.get_bufferc();
        bytes_processed += stringLength;
        character_sequence cs(strOffset, stringLength);
        charStream.advance(stringLength);

        return cs;
    }
    return character_sequence();
}

template<typename SeqType, typename SeqBase, typename Allocator>
template<typename ... Params>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE character_sequence<SeqType, SeqBase, Allocator> character_sequence<SeqType, SeqBase, Allocator>::from_format(const_pointer in_format, Params ... in_params) noexcept
{
    if(!in_format)
    {
        return character_sequence();
    }

    #ifdef MBASE_PLATFORM_WINDOWS
    size_type stringLength = _scprintf(in_format, std::forward<Params>(in_params)...); // FIND THE FKIN SIZE
    #endif

    #ifdef MBASE_PLATFORM_UNIX
    size_type stringLength = snprintf(NULL, 0, in_format, std::forward<Params>(in_params)...);
    #endif
    
    character_sequence newSequence;
    if (!stringLength)
    {
        return character_sequence();
    }

    pointer mString = newSequence.mExternalAllocator.allocate(stringLength + 1, true);
    newSequence.fill(mString, 0, stringLength + 1);
    
    sprintf(mString, in_format, std::forward<Params>(in_params)...);

    newSequence = character_sequence(mString);
    newSequence.mExternalAllocator.deallocate(mString);

    return newSequence;
}

template<typename SeqType, typename SeqBase, typename Allocator>
template<typename SourceContainer>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::split(const character_sequence& in_delimiters, SourceContainer& out_strings) noexcept
{
    character_sequence<SeqType, SeqBase, Allocator> oldString(mRawData, mSize);
    const_pointer delims = in_delimiters.c_str();
    pointer stringOut = SeqBase::string_token(mRawData, delims);
    while (stringOut != nullptr)
    {
        out_strings.push_back(stringOut);
        stringOut = SeqBase::string_token(nullptr, delims);
    }
    *this = std::move(oldString);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR bool operator==(const character_sequence<SeqType, SeqBase, Allocator>& in_lhs, const character_sequence<SeqType, SeqBase, Allocator>& in_rhs) noexcept 
{
    return SeqBase::is_equal(in_lhs.mRawData, in_rhs.mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE_EXPR bool operator!=(const character_sequence<SeqType, SeqBase, Allocator>& in_lhs, const character_sequence<SeqType, SeqBase, Allocator>& in_rhs) noexcept 
{
    return !SeqBase::is_equal(in_lhs.mRawData, in_rhs.mRawData);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::_resize(size_type in_size) noexcept
{
    if (in_size > mSize)
    {
        size_type newCap = _calculate_capacity(in_size);
        pointer new_data = mExternalAllocator.allocate(newCap, true);
        this->copy_bytes(new_data, mRawData, mSize);
        mExternalAllocator.deallocate(mRawData);
        mRawData = new_data;
        mSize = in_size;
        mCapacity = newCap;
    }
    else
    {
        difference_type expectedSize = mSize - in_size;
        reverse_iterator rit = rbegin();
        for (difference_type i = 0; i < expectedSize; i++)
        {
            *rit = SeqBase::null_value;
            rit++;
        }
        mSize = in_size;
    }
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::_resize(size_type in_size, value_type in_char) noexcept
{
    size_type oldSize = mSize;
    if (in_size > mSize)
    {
        _resize(in_size);
        for (size_type i = 0; i < mSize - oldSize; i++)
        {
            push_back(in_char);
        }
        return;
    }
    _resize(in_size);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE typename character_sequence<SeqType, SeqBase, Allocator>::size_type character_sequence<SeqType, SeqBase, Allocator>::_calculate_capacity(size_type in_size) noexcept
{
    // SELF-NOTE: MAY NOT BE PERFORMANT
    // SELF-NOTE: CONSIDER THE IMPLEMENTATION AGAIN
    size_type base_capacity = gStringDefaultCapacity;
    while (base_capacity <= in_size)
    {
        base_capacity *= 2;
    }

    return base_capacity;
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::_build_string(size_type in_capacity) noexcept
{
    mCapacity = in_capacity;
    mRawData = mExternalAllocator.allocate(mCapacity, true);
}

template<typename SeqType, typename SeqBase, typename Allocator>
MBASE_INLINE GENERIC character_sequence<SeqType, SeqBase, Allocator>::_clear_self() noexcept
{
    if(mRawData)
    {
        mExternalAllocator.deallocate(mRawData);
        mRawData = nullptr;
        mSize = 0;
        mCapacity = 0;
    }
}

using string = character_sequence<IBYTE>;
using wstring = character_sequence<wchar_t>;
using string_view = string; // STRING VIEW WILL BE IMPLEMENTED LATER

MBASE_INLINE mbase::string to_utf8(const mbase::wstring& in_str)
{
    const wchar_t* src = in_str.c_str();
    I32 src_length = in_str.size();
    if (!src_length)
    {
        return mbase::string();
    }
#ifdef MBASE_PLATFORM_WINDOWS
    I32 length = WideCharToMultiByte(CP_UTF8, 0, src, src_length, 0, 0, NULL, NULL);
    IBYTEBUFFER output_buffer = (IBYTEBUFFER)malloc((length + 1) * sizeof(IBYTE));
    if(!output_buffer)
    {
        return mbase::string();
    }
    
    WideCharToMultiByte(CP_UTF8, 0, src, src_length, output_buffer, length, NULL, NULL);
    output_buffer[length] = '\0';

    mbase::string outStr(output_buffer, length);
    free(output_buffer);

    return outStr;
#endif // MBASE_PLATFORM_WINDOWS

#ifdef MBASE_PLATFORM_UNIX
    // TODO: Return here for double checks
    src_length = in_str.size() * sizeof(wchar_t);

    iconv_t cd = iconv_open("UTF-8", "WCHAR_T");
    if (cd == (iconv_t)-1)
    {
        return mbase::string();
    }

    size_t out_size = src_length * 4 + 1;
    IBYTEBUFFER output_buffer = (IBYTEBUFFER)malloc(out_size);
    if (!output_buffer)
    {
        iconv_close(cd);
        return mbase::string();
    }

    IBYTEBUFFER out_ptr = output_buffer;
    IBYTEBUFFER in_ptr = (IBYTEBUFFER)src;
    size_t out_bytes_left = out_size - 1;
    size_t in_bytes_left = src_length;

    size_t result = iconv(cd, &in_ptr, &in_bytes_left, &out_ptr, &out_bytes_left);
    iconv_close(cd);

    if (result == (size_t)-1)
    {
        free(output_buffer);
        return mbase::string();
    }

    size_t length = out_size - out_bytes_left - 1;
    output_buffer[length] = '\0';

    mbase::string outStr(output_buffer, length);
    free(output_buffer);
    return outStr;
#endif // MBASE_PLATFORM_UNIX
}

MBASE_INLINE mbase::wstring from_utf8(const mbase::string& in_str)
{
    CBYTEBUFFER src = in_str.c_str();
    I32 src_length = in_str.size();
    if (!src_length)
    {
        return mbase::wstring();
    }
    
#ifdef MBASE_PLATFORM_WINDOWS
    I32 length = MultiByteToWideChar(CP_UTF8, 0, src, src_length, 0, 0);
    wchar_t* output_buffer = (wchar_t*)malloc((length + 1) * sizeof(wchar_t));
    if(!output_buffer)
    {
        return wstring();
    }

    if (output_buffer) {
        MultiByteToWideChar(CP_UTF8, 0, src, src_length, output_buffer, length);
        output_buffer[length] = L'\0';
    }

    mbase::wstring outStr(output_buffer, length);

    free(output_buffer);

    return outStr;
#endif // MBASE_PLATFORM_WINDOWS

#ifdef MBASE_PLATFORM_UNIX
    iconv_t cd = iconv_open("WCHAR_T", "UTF-8");
    if (cd == (iconv_t)-1)
    {
        return mbase::wstring();
    }

    size_t out_size = (src_length + 1) * sizeof(wchar_t);
    char* output_buffer = (char*)malloc(out_size);
    if (!output_buffer)
    {
        iconv_close(cd);
        return mbase::wstring();
    }

    char* out_ptr = output_buffer;
    char* in_ptr = (char*)src;
    size_t out_bytes_left = out_size - sizeof(wchar_t);
    size_t in_bytes_left = src_length;

    size_t result = iconv(cd, &in_ptr, &in_bytes_left, &out_ptr, &out_bytes_left);
    iconv_close(cd);

    if (result == (size_t)-1)
    {
        free(output_buffer);
        return mbase::wstring();
    }

    size_t length = (out_size - out_bytes_left - sizeof(wchar_t)) / sizeof(wchar_t);
    ((wchar_t*)output_buffer)[length] = L'\0';

    mbase::wstring outStr((wchar_t*)output_buffer, length);
    free(output_buffer);
    return outStr;
#endif
}

MBASE_INLINE mbase::string get_line()
{
    mbase::string builtString;
    for(I32 tmpChar; (tmpChar = getchar()) != '\n';)
    {
        if(tmpChar < 32)
        {
            continue;
        }
        builtString.push_back(tmpChar);
    }
    return builtString;
}

MBASE_INLINE mbase::wstring get_wline()
{
    mbase::wstring builtString;
    for(wint_t tmpChar; (tmpChar = getwchar()) != '\n';)
    {
        builtString.push_back(tmpChar);
    }
    return builtString;
}

MBASE_STD_END
template<>
struct std::hash<mbase::string> {
    std::size_t operator()(const mbase::string& in_rhs) const noexcept
    {
        mbase::string::const_pointer myBuffer = in_rhs.c_str();
        mbase::I32 myVal = 0;
        while(*myBuffer != '\0')
        {
            mbase::I32 tmp;
            myVal = (myVal << 4) + (*myBuffer);
            if((tmp = (myVal & 0xf0000000)))
            {
                myVal = myVal ^ (tmp >> 24);
                myVal = myVal ^ tmp;
            }
            ++myBuffer;
        }
        return myVal;
    }
};

template<>
struct std::hash<mbase::wstring> {
    std::size_t operator()(const mbase::wstring& in_rhs) const noexcept
    {
        mbase::wstring::const_pointer myBuffer = in_rhs.c_str();
        mbase::I32 myVal = 0;
        while (*myBuffer != L'\0')
        {
            mbase::I32 tmp;
            myVal = (myVal << 4) + (*myBuffer);
            if ((tmp = (myVal & 0xf0000000)))
            {
                myVal = myVal ^ (tmp >> 24);
                myVal = myVal ^ tmp;
            }
            ++myBuffer;
        }
        return myVal;
    }
};


#endif // MBASE_STRING_H