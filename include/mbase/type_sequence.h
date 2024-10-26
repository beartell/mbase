#ifndef MBASE_TYPE_SEQUENCE_H
#define MBASE_TYPE_SEQUENCE_H

#include <mbase/common.h>
#include <mbase/sequence_iterator.h> // mbase::sequence_iterator, mbase::const_sequence_iterator, mbase::reverse_iterator, mbase::const_reverse_sequence_iterator
#include <mbase/allocator.h> // mbase::allocator

MBASE_STD_BEGIN

// TWO TYPES OF STRINGS
// BYTE SEQUENCE STRING (NULL TERMINATED)
// WIDE BYTE STRING

template<typename T, typename Alloc = allocator<T>>
class type_sequence {
public:
    using value_type = T;
    using allocator_type = Alloc;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using move_reference = T&&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;
    using iterator = sequence_iterator<T>;
    using const_iterator = const_sequence_iterator<T>;
    using reverse_iterator = reverse_sequence_iterator<T>;
    using const_reverse_iterator = const_reverse_sequence_iterator<T>;

    static const value_type null_value = '\0';

    static MBASE_INLINE pointer copy_bytes(pointer out_target, const_pointer in_source) noexcept { return strcpy(out_target, in_source); }
    static MBASE_INLINE pointer copy_bytes(pointer out_target, const_pointer in_source, size_type in_length) noexcept { return static_cast<pointer>(memcpy(out_target, in_source, sizeof(value_type) * in_length)); }
    static MBASE_INLINE pointer concat(pointer out_target, const_pointer in_source, size_type in_length) { return static_cast<pointer>(strncat(out_target, in_source, sizeof(value_type) * in_length)); }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool is_equal(const_pointer in_lhs, const_pointer in_rhs, size_type in_length) noexcept 
    {
        return !memcmp(in_lhs, in_rhs, sizeof(value_type) * in_length) ? true : false;
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool is_equal(const_pointer in_lhs, const_pointer in_rhs) noexcept 
    {
        return !strcmp(in_lhs, in_rhs) ? true : false;
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE size_type length_bytes(const_pointer in_target) noexcept { return strlen(in_target); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE I32 compare_bytes(const_pointer in_lhs, const_pointer in_rhs, size_type in_length) noexcept { return memcmp(in_lhs, in_rhs, sizeof(value_type) * in_length); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE I32 compare_bytes(const_pointer in_lhs, const_pointer in_rhs) noexcept { return strcmp(in_lhs, in_rhs); }

    static MBASE_INLINE pointer fill(pointer in_target, value_type in_value, size_type in_length) noexcept 
    {
        memset(in_target, in_value, sizeof(value_type) * in_length);
        return in_target;
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE I32 cnv_to_i32(const_pointer in_str)
    {
        pointer ptr;
        return strtol(in_str, &ptr, 10);
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE I64 cnv_to_i64(const_pointer in_str)
    {
        pointer ptr;
        return strtoimax(in_str, &ptr, 10);
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE F32 cnv_to_f32(const_pointer in_str)
    {
        pointer ptr;
        return strtof(in_str, &ptr);
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE F64 cnv_to_f64(const_pointer in_str)
    {
        pointer ptr;
        return strtod(in_str, &ptr);
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_alnum(const value_type& in_char) noexcept { return isalnum(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_alpha(const value_type& in_char) noexcept { return isalpha(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_lower(const value_type& in_char) noexcept { return islower(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_upper(const value_type& in_char) noexcept { return isupper(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_blank(const value_type& in_char) noexcept { return isblank(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_control(const value_type& in_char) noexcept { return iscntrl(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_space(const value_type& in_char) noexcept { return isspace(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_punctuation(const value_type& in_char) noexcept { return ispunct(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_print(const value_type& in_char) noexcept { return isprint(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_digit(const value_type& in_char) noexcept { return isdigit(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE pointer string_token(pointer in_data, const_pointer in_delim) { return strtok(in_data, in_delim); }
};

template <>
class type_sequence<wchar_t> {
public:
    using value_type = wchar_t;
    using allocator_type = allocator<wchar_t>;
    using pointer = wchar_t*;
    using const_pointer = const wchar_t*;
    using reference = wchar_t&;
    using const_reference = const wchar_t&;
    using move_reference = wchar_t&&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;
    using iterator = sequence_iterator<wchar_t>;
    using const_iterator = const_sequence_iterator<wchar_t>;
    using reverse_iterator = reverse_sequence_iterator<wchar_t>;
    using const_reverse_iterator = const_reverse_sequence_iterator<wchar_t>;

    static const value_type null_value = L'\0';

    static MBASE_INLINE pointer copy_bytes(pointer out_target, const_pointer in_source) noexcept { return wcscpy(out_target, in_source); }
    static MBASE_INLINE pointer copy_bytes(pointer out_target, const_pointer in_source, size_type in_length) noexcept { return static_cast<pointer>(memcpy(out_target, in_source, sizeof(value_type) * in_length)); }
    static MBASE_INLINE pointer concat(pointer out_target, const_pointer in_source, size_type in_length) { return static_cast<pointer>(wcsncat(out_target, in_source, sizeof(value_type) * in_length)); }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool is_equal(const_pointer in_lhs, const_pointer in_rhs, size_type in_length) noexcept
    {
        return !memcmp(in_lhs, in_rhs, sizeof(value_type) * in_length) ? true : false;
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool is_equal(const_pointer in_lhs, const_pointer in_rhs) noexcept
    {
        return !wcscmp(in_lhs, in_rhs) ? true : false;
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE size_type length_bytes(const_pointer in_target) noexcept { return wcslen(in_target); }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE I32 compare_bytes(const_pointer in_lhs, const_pointer in_rhs, size_type in_length) noexcept { return memcmp(in_lhs, in_rhs, sizeof(value_type) * in_length); }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE I32 compare_bytes(const_pointer in_lhs, const_pointer in_rhs) noexcept { return wcscmp(in_lhs, in_rhs); }

    static MBASE_INLINE pointer fill(pointer in_target, value_type in_value, size_type in_length) noexcept
    {
        memset(in_target, in_value, sizeof(value_type) * in_length);
        return in_target;
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE I32 cnv_to_i32(const_pointer in_str)
    {
        pointer ptr;
        return wcstol(in_str, &ptr, 10);
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE I64 cnv_to_i64(const_pointer in_str)
    {
        pointer ptr;
        return wcstoimax(in_str, &ptr, 10);
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE F32 cnv_to_f32(const_pointer in_str)
    {
        pointer ptr;
        return wcstof(in_str, &ptr);
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE F64 cnv_to_f64(const_pointer in_str)
    {
        pointer ptr;
        return wcstod(in_str, &ptr);
    }

    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_alnum(const value_type& in_char) noexcept { return iswalnum(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_alpha(const value_type& in_char) noexcept { return iswalnum(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_lower(const value_type& in_char) noexcept { return iswlower(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_upper(const value_type& in_char) noexcept { return iswupper(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_blank(const value_type& in_char) noexcept { return iswblank(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_control(const value_type& in_char) noexcept { return iswcntrl(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_space(const value_type& in_char) noexcept { return iswspace(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_punctuation(const value_type& in_char) noexcept { return iswpunct(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_print(const value_type& in_char) noexcept { return iswprint(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE bool type_is_digit(const value_type& in_char) noexcept { return iswdigit(in_char); }
    MBASE_ND(MBASE_RESULT_IGNORE) static MBASE_INLINE pointer string_token(pointer in_data, const_pointer in_delim) { return wcstok(in_data, in_delim); }
};

MBASE_STD_END

#endif // !MBASE_TYPE_SEQUENCE_H