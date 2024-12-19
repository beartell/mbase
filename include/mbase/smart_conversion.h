#ifndef MBASE_SMART_CONVERSION_H
#define MBASE_SMART_CONVERSION_H

#include <mbase/common.h>
#include <mbase/string.h>

MBASE_STD_BEGIN

template<typename Type, typename StringType = mbase::string>
struct smart_conversion {};

template<typename StringType>
struct smart_conversion<I8, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static I8 apply(typename string_type::const_pointer in_string)
    {
        return static_cast<I8>(string_type::cnv_to_i32(in_string));
    }
};

template<typename StringType>
struct smart_conversion<I16, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static I16 apply(typename string_type::const_pointer in_string)
    {
        return static_cast<I16>(string_type::cnv_to_i32(in_string));
    }
};

template<typename StringType>
struct smart_conversion<I32, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static I32 apply(typename string_type::const_pointer in_string)
    {
        return string_type::cnv_to_i32(in_string);
    }
};

template<typename StringType>
struct smart_conversion<I64, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static I64 apply(typename string_type::const_pointer in_string)
    {
        return string_type::cnv_to_i64(in_string);
    }
};

template<typename StringType>
struct smart_conversion<U8, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static U8 apply(typename string_type::const_pointer in_string)
    {
        return static_cast<U8>(string_type::cnv_to_i32(in_string));
    }
};

template<typename StringType>
struct smart_conversion<U16, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static U16 apply(typename string_type::const_pointer in_string)
    {
        return static_cast<U16>(string_type::cnv_to_i32(in_string));
    }
};

template<typename StringType>
struct smart_conversion<U32, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static U32 apply(typename string_type::const_pointer in_string)
    {
        return static_cast<U32>(string_type::cnv_to_i32(in_string));
    }
};

template<typename StringType>
struct smart_conversion<U64, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static U64 apply(typename string_type::const_pointer in_string)
    {
        return static_cast<U64>(string_type::cnv_to_i64(in_string));
    }
};

template<typename StringType>
struct smart_conversion<F32, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static F32 apply(typename string_type::const_pointer in_string)
    {
        return string_type::cnv_to_f32(in_string);
    }
};

template<typename StringType>
struct smart_conversion<F64, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static F64 apply(typename string_type::const_pointer in_string)
    {
        return string_type::cnv_to_f64(in_string);
    }
};

template<typename StringType>
struct smart_conversion<mbase::string, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static mbase::string apply(typename string_type::const_pointer in_string)
    {
        return in_string;
    }
};

template<typename StringType>
struct smart_conversion<mbase::wstring, StringType>{
    using string_type = std::conditional_t<std::is_same_v<StringType, mbase::string>, mbase::string, mbase::wstring>;
    static mbase::wstring apply(typename string_type::const_pointer in_string)
    {
        return in_string;
    }
};

MBASE_STD_END

#endif // MBASE_SMART_CONVERSION_H