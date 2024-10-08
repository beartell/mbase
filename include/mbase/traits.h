#ifndef MBASE_STD_CSH_H
#define MBASE_STD_CSH_H

#include <mbase/common.h>
#include <mbase/safe_buffer.h> // mbase::safe_buffer
#include <mbase/char_stream.h> // mbase::char_stream
#include <exception> // std::exception ---> For custom exceptions

MBASE_STD_BEGIN

/*

    --- CLASS INFORMATION ---
Identification: S0C13-OBJ-NA-ST

Name: serialize_helper

Parent: None

Behaviour List:
- Serializable

Description:
*/

static const SIZE_T gPairSerializeBlockLength = 8;

class invalid_format : public std::exception {
public:
    MSTRING what() const {
        return "invalid buffer format.";
    }
};

class invalid_size : public std::exception {
public:
    MSTRING what() const {
        return "invalid buffer size.";
    }
};

template <typename SerializedType>
struct serialize_helper {
	using value_type = SerializedType;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<SerializedType>;

	pointer value;

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_INLINE size_type get_serialized_size() const noexcept;
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) const;
    /* ===== STATE-MODIFIER METHODS END ===== */

    /* ===== NON-MODIFIER METHODS BEGIN ===== */
    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed);
    /* ===== NON-MODIFIER METHODS END ===== */
};

template<typename T1, typename T2>
struct pair {
    using first_type = T1;
    using second_type = T2;
    using size_type = SIZE_T;

    first_type first;
    second_type second;

    /* ===== BUILDER METHODS BEGIN ===== */
    MBASE_INLINE_EXPR pair(const T1& in_first, const T2& in_second) noexcept;
    MBASE_INLINE_EXPR pair(T1&& in_first, T2&& in_second) noexcept;
    MBASE_INLINE_EXPR pair() = default;
    MBASE_INLINE_EXPR pair(const pair&) = default;
    MBASE_INLINE_EXPR pair(pair&&) = default;
    MBASE_INLINE ~pair() = default;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OPERATOR BUILDER METHODS BEGIN ===== */
    MBASE_INLINE_EXPR pair& operator=(const pair& in_rhs) noexcept;
    MBASE_INLINE_EXPR pair& operator=(pair&& in_rhs) noexcept;
    /* ===== OPERATOR BUILDER METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_INLINE size_type get_serialized_size() const noexcept;
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_INLINE_EXPR GENERIC swap(pair& in_rhs) noexcept;
    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept;
    MBASE_INLINE static pair deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed);
    /* ===== STATE-MODIFIER METHODS END ===== */

    /* ===== NON-MODIFIER METHODS BEGIN ===== */
    MBASE_INLINE bool operator==(const pair& in_rhs);
    MBASE_INLINE bool operator!=(const pair& in_rhs);
    /* ===== NON-MODIFIER METHODS END ===== */
};

template<typename ConvertedType, typename StringType = mbase::string>
struct string_converter {
    using value_type = ConvertedType;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<ConvertedType>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const I8, StringType> {
    using value_type = const I8;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<value_type>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const I16, StringType> {
    using value_type = const I16;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<value_type>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const I32, StringType> {
    using value_type = const I32;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<value_type>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const I64, StringType> {
    using value_type = const I64;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<const I64>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const U8, StringType> {
    using value_type = const U8;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<value_type>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const U16, StringType> {
    using value_type = const U16;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<value_type>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const U32, StringType> {
    using value_type = const U32;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<value_type>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const U64, StringType> {
    using value_type = const U64;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<value_type>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const F32, StringType> {
    using value_type = const F32;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<value_type>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename StringType>
struct string_converter<const F64, StringType> {
    using value_type = const F64;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<value_type>;
    
	pointer value;

    MBASE_INLINE StringType to_string();
};

template<typename SerializedType>
MBASE_INLINE typename serialize_helper<SerializedType>::size_type serialize_helper<SerializedType>::get_serialized_size() const noexcept
{
    return value->get_serialized_size();
}

template<typename SerializedType>
MBASE_INLINE GENERIC serialize_helper<SerializedType>::serialize(char_stream& out_buffer) const
{
    value->serialize(out_buffer);
}

template<typename SerializedType>
MBASE_INLINE typename serialize_helper<SerializedType>::value_type serialize_helper<SerializedType>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    return value_type::deserialize(in_src, in_length, bytes_processed);
}

template<>
MBASE_INLINE typename serialize_helper<I8>::size_type serialize_helper<I8>::get_serialized_size() const noexcept
{
    return sizeof(typename serialize_helper<I8>::value_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<I8>::serialize(char_stream& out_buffer) const
{
    out_buffer.putcn(*value);
}

template<>
MBASE_INLINE typename serialize_helper<I8>::value_type serialize_helper<I8>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if(in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    bytes_processed += sizeof(value_type);
    PTR8 bf = reinterpret_cast<PTR8>(in_src);
    return *bf;
}

template<>
MBASE_INLINE typename serialize_helper<I16>::size_type serialize_helper<I16>::get_serialized_size() const noexcept
{
    return sizeof(typename serialize_helper<I16>::value_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<I16>::serialize(char_stream& out_buffer) const
{
    out_buffer.put_datan<value_type>(*value);
}

template<>
MBASE_INLINE typename serialize_helper<I16>::value_type serialize_helper<I16>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if (in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    bytes_processed += sizeof(value_type);
    PTR16 bf = reinterpret_cast<PTR16>(in_src);
    return *bf;
}

template<>
MBASE_INLINE typename serialize_helper<I32>::size_type serialize_helper<I32>::get_serialized_size() const noexcept
{
    return sizeof(typename serialize_helper<I32>::value_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<I32>::serialize(char_stream& out_buffer) const
{
    out_buffer.put_datan<value_type>(*value);
}

template<>
MBASE_INLINE typename serialize_helper<I32>::value_type serialize_helper<I32>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if (in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    bytes_processed += sizeof(value_type);
    PTR32 bf = reinterpret_cast<PTR32>(in_src);
    return *bf;
}

template<>
MBASE_INLINE typename serialize_helper<I64>::size_type serialize_helper<I64>::get_serialized_size() const noexcept
{
    return sizeof(typename serialize_helper<I64>::value_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<I64>::serialize(char_stream& out_buffer) const
{
    out_buffer.put_datan<value_type>(*value);
}

template<>
MBASE_INLINE typename serialize_helper<I64>::value_type serialize_helper<I64>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if (in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    bytes_processed += sizeof(value_type);
    PTR64 bf = reinterpret_cast<PTR64>(in_src);
    return *bf;
}

template<>
MBASE_INLINE typename serialize_helper<U8>::size_type serialize_helper<U8>::get_serialized_size() const noexcept
{
    return sizeof(typename serialize_helper<U8>::value_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<U8>::serialize(char_stream& out_buffer) const
{
    out_buffer.put_datan<value_type>(*value);
}

template<>
MBASE_INLINE typename serialize_helper<U8>::value_type serialize_helper<U8>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if (in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    bytes_processed += sizeof(value_type);
    PTRU8 bf = reinterpret_cast<PTRU8>(in_src);
    return *bf;
}

template<>
MBASE_INLINE typename serialize_helper<U16>::size_type serialize_helper<U16>::get_serialized_size() const noexcept
{
    return sizeof(typename serialize_helper<U16>::value_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<U16>::serialize(char_stream& out_buffer) const
{
    out_buffer.put_datan<value_type>(*value);
}

template<>
MBASE_INLINE typename serialize_helper<U16>::value_type serialize_helper<U16>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if (in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    bytes_processed += sizeof(value_type);
    PTRU16 bf = reinterpret_cast<PTRU16>(in_src);
    return *bf;
}

template<>
MBASE_INLINE typename serialize_helper<U32>::size_type serialize_helper<U32>::get_serialized_size() const noexcept
{
    return sizeof(typename serialize_helper<U32>::value_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<U32>::serialize(char_stream& out_buffer) const
{
    out_buffer.put_datan<value_type>(*value);
}

template<>
MBASE_INLINE typename serialize_helper<U32>::value_type serialize_helper<U32>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if (in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    bytes_processed += sizeof(value_type);
    PTRU32 bf = reinterpret_cast<PTRU32>(in_src);
    return *bf;
}

template<>
MBASE_INLINE typename serialize_helper<U64>::size_type serialize_helper<U64>::get_serialized_size() const noexcept
{
    return sizeof(typename serialize_helper<U64>::value_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<U64>::serialize(char_stream& out_buffer) const
{
    out_buffer.put_datan<value_type>(*value);
}

template<>
MBASE_INLINE typename serialize_helper<U64>::value_type serialize_helper<U64>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if (in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    bytes_processed += sizeof(value_type);
    PTRU64 bf = reinterpret_cast<PTRU64>(in_src);
    return *bf;
}

template<>
MBASE_INLINE typename serialize_helper<F32>::size_type serialize_helper<F32>::get_serialized_size() const noexcept
{
    return sizeof(typename IF32::int_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<F32>::serialize(char_stream& out_buffer) const
{
    IF32 if32;
    if32.mFloat = *value;

    serialize_helper<I32> i32Serializer;
    i32Serializer.value = &if32.mInt;

    out_buffer.put_datan<typename IF32::int_type>(*i32Serializer.value);
}

template<>
MBASE_INLINE typename serialize_helper<F32>::value_type serialize_helper<F32>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if (in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    IF32 if32;
    serialize_helper<I32> i32Serializer;

    if32.mInt = i32Serializer.deserialize(in_src, in_length, bytes_processed);
    return if32.mFloat;
}

template<>
MBASE_INLINE typename serialize_helper<F64>::size_type serialize_helper<F64>::get_serialized_size() const noexcept
{
    return sizeof(typename IF64::int_type);
}

template<>
MBASE_INLINE GENERIC serialize_helper<F64>::serialize(char_stream& out_buffer) const
{
    IF64 if64;
    if64.mFloat = *value;

    serialize_helper<I64> i64Serializer;
    i64Serializer.value = &if64.mInt;

    out_buffer.put_datan<typename IF64::int_type>(*i64Serializer.value);
}

template<>
MBASE_INLINE typename serialize_helper<F64>::value_type serialize_helper<F64>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    if (in_length < sizeof(value_type))
    {
        throw mbase::invalid_size();
    }
    IF64 if64;
    serialize_helper<I64> i64Serializer;

    if64.mInt = i64Serializer.deserialize(in_src, in_length, bytes_processed);
    return if64.mFloat;
}

template<typename ConvertedType, typename StringType>
MBASE_INLINE StringType string_converter<ConvertedType, StringType>::to_string()
{
    return value->to_string();
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const I8, StringType>::to_string()
{
    IBYTE emptyString[4] = {0};
    sprintf(emptyString, "%d", *value);
    return StringType(emptyString);
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const I16, StringType>::to_string()
{
    IBYTE emptyString[6] = {0};
    sprintf(emptyString, "%d", *value);
    return StringType(emptyString);
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const I32, StringType>::to_string()
{
    IBYTE emptyString[12] = {0};
    sprintf(emptyString, "%d", *value);
    return StringType(emptyString);
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const I64, StringType>::to_string()
{
    IBYTE emptyString[24] = {0};
    sprintf(emptyString, "%d", *value);
    return StringType(emptyString);
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const U8, StringType>::to_string()
{
    IBYTE emptyString[4] = {0};
    sprintf(emptyString, "%u", *value);
    return StringType(emptyString);
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const U16, StringType>::to_string()
{
    IBYTE emptyString[6] = {0};
    sprintf(emptyString, "%u", *value);
    return StringType(emptyString);
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const U32, StringType>::to_string()
{
    IBYTE emptyString[12] = {0};
    sprintf(emptyString, "%u", *value);
    return StringType(emptyString);
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const U64, StringType>::to_string()
{
    IBYTE emptyString[24] = {0};
    sprintf(emptyString, "%u", *value);
    return StringType(emptyString);
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const F32, StringType>::to_string()
{
    IBYTE emptyString[24] = {0};
    sprintf(emptyString, "%f", *value);
    return StringType(emptyString);
}

template<typename StringType>
MBASE_INLINE StringType string_converter<const F64, StringType>::to_string()
{
    IBYTE emptyString[24] = {0};
    sprintf(emptyString, "%lf", *value);
    return StringType(emptyString);
}

MBASE_INLINE SIZE_T get_serialized_size() noexcept
{
    return 0;
}

template<typename FirstType, typename ... TypesRest>
MBASE_INLINE SIZE_T get_serialized_size(const FirstType& in_arg, const TypesRest&... in_rest) noexcept
{
    serialize_helper<FirstType> helper;
    helper.value = const_cast<FirstType*>(&in_arg);
    SIZE_T myResult = helper.get_serialized_size() + get_serialized_size(std::forward<TypesRest>(in_rest)...);
    return myResult;
}

template<typename SerializedType>
MBASE_INLINE GENERIC serialize(const SerializedType& in_value, char_stream& out_buffer) noexcept
{
    serialize_helper<SerializedType> helper;
    helper.value = const_cast<SerializedType*>(&in_value);
    helper.serialize(out_buffer);
}

template<typename SerializedType>
MBASE_INLINE SerializedType deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed) noexcept
{
    serialize_helper<SerializedType> helper;
    return helper.deserialize(in_src, in_length, bytes_processed);
}

template<typename T1, typename T2>
MBASE_INLINE mbase::pair<T1, T2> make_pair(const T1& in_first, const T2& in_second) noexcept
{
    return mbase::pair<T1, T2>(in_first, in_second);
}

template<typename T1, typename T2>
MBASE_INLINE std::pair<T1, T2> to_stdpair(const mbase::pair<T1, T2>& in_pair) noexcept
{
    return std::make_pair(in_pair.first, in_pair.second);
}

template<typename T1, typename T2>
MBASE_INLINE std::pair<T1, T2> to_stdpair(mbase::pair<T1, T2>&& in_pair) noexcept
{
    return std::make_pair(std::move(in_pair.first), std::move(in_pair.second));
}

template<typename T1, typename T2>
MBASE_INLINE_EXPR pair<T1, T2>::pair(const T1& in_first, const T2& in_second) noexcept : first(in_first), second(in_second)
{
}

template<typename T1, typename T2>
MBASE_INLINE_EXPR pair<T1, T2>::pair(T1&& in_first, T2&& in_second) noexcept : first(std::move(in_first)), second(std::move(in_second))
{
}

template<typename T1, typename T2>
MBASE_INLINE_EXPR pair<T1, T2>& pair<T1, T2>::operator=(const pair& in_rhs) noexcept
{
    first = in_rhs.first;
    second = in_rhs.second;

    return *this;
}

template<typename T1, typename T2>
MBASE_INLINE_EXPR pair<T1, T2>& pair<T1, T2>::operator=(pair&& in_rhs) noexcept
{
    first = std::move(in_rhs.first);
    second = std::move(in_rhs.second);

    return *this;
}

template<typename T1, typename T2>
MBASE_INLINE_EXPR GENERIC pair<T1, T2>::swap(pair& in_rhs) noexcept {
    std::swap(first, in_rhs.first);
    std::swap(second, in_rhs.second);
}

template<typename T1, typename T2>
MBASE_INLINE typename pair<T1, T2>::size_type pair<T1, T2>::get_serialized_size() const noexcept {
    size_type firstBlockSize = mbase::get_serialized_size(first) + gPairSerializeBlockLength;
    size_type secondBlockSize = mbase::get_serialized_size(second) + gPairSerializeBlockLength;

    return firstBlockSize + secondBlockSize;
}

template<typename T1, typename T2>
MBASE_INLINE GENERIC pair<T1, T2>::serialize(char_stream& out_buffer) noexcept
{
    SIZE_T blockLength = mbase::get_serialized_size(first);
    out_buffer.put_datan<SIZE_T>(blockLength);
    mbase::serialize(first, out_buffer);

    blockLength = mbase::get_serialized_size(second);
    out_buffer.put_datan<SIZE_T>(blockLength);
    mbase::serialize(second, out_buffer);
}

template<typename T1, typename T2>
MBASE_INLINE pair<T1, T2> pair<T1, T2>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
    char_stream cs(in_src, in_length);

    if(in_length < sizeof(SIZE_T) * 2)
    {
        throw mbase::invalid_size();
    }

    SIZE_T blockLength = cs.get_datan<SIZE_T>();
    bytes_processed += sizeof(SIZE_T);
    first_type ft(std::move(mbase::deserialize<first_type>(cs.get_bufferc(), blockLength, bytes_processed)));

    cs.advance(blockLength);
    blockLength = cs.get_datan<SIZE_T>();
    bytes_processed += sizeof(SIZE_T);
    second_type st(std::move(mbase::deserialize<second_type>(cs.get_bufferc(), blockLength, bytes_processed)));

    return { std::move(ft), std::move(st) };
}

template<typename T1, typename T2>
MBASE_INLINE bool pair<T1, T2>::operator==(const pair& in_rhs)
{
    return (first == in_rhs.first ? true : false) && (second == in_rhs.second ? true : false);
}

template<typename T1, typename T2>
MBASE_INLINE bool pair<T1, T2>::operator!=(const pair& in_rhs)
{
    return (first != in_rhs.first ? true : false) && (second != in_rhs.second ? true : false);
}

template<typename ConvertedType, typename StringType = mbase::string>
MBASE_INLINE StringType to_string(const ConvertedType& in_type)
{
    string_converter<const ConvertedType, StringType> tempConverter;
    tempConverter.value = &in_type;
    return tempConverter.to_string();
}

MBASE_STD_END

#endif // MBASE_STD_CSH_H