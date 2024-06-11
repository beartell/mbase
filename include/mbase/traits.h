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

template <typename SerializedType>
struct serialize_helper {
	using value_type = SerializedType;
    using size_type = SIZE_T;
	using pointer = std::add_pointer_t<SerializedType>;

	pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept 
    {
        return value->get_serialized_size();
    }

	MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept 
    {
		value->serialize(out_buffer);
	}

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
    {
        return value_type::deserialize(in_src, in_length);
    }
};

template<>
struct serialize_helper<I8> {
	using value_type = I8;
    using size_type = SIZE_T;
	using pointer = PTR8;

	pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept 
    {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept 
    {
        out_buffer.putcn(*value);
	}

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
    {
        PTR8 bf = reinterpret_cast<PTR8>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<I16> {
    using value_type = I16;
    using size_type = SIZE_T;
    using pointer = PTR16;

    pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept 
    {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept 
    {
        out_buffer.put_datan<value_type>(*value);
        //out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(I16));
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
    {
        PTR16 bf = reinterpret_cast<PTR16>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<I32> {
    using value_type = I32;
    using size_type = SIZE_T;
    using pointer = PTR32;

    pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept 
    {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept 
    {
        out_buffer.put_datan<value_type>(*value);
        //out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(I32));
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
    {
        PTR32 bf = reinterpret_cast<PTR32>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<I64> {
    using value_type = I64;
    using size_type = SIZE_T;
    using pointer = PTR64;

    pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept 
    {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept 
    {
        out_buffer.put_datan<value_type>(*value);
        //out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(I64));
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
    {
        PTR64 bf = reinterpret_cast<PTR64>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<U8> {
    using value_type = U8;
    using size_type = SIZE_T;
    using pointer = PTRU8;

    pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept 
    {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept 
    {
        out_buffer.put_datan<value_type>(*value);
        //out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(U8));
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
    {
        PTRU8 bf = reinterpret_cast<PTRU8>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<U16> {
    using value_type = U16;
    using size_type = SIZE_T;
    using pointer = PTRU16;

    pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept 
    {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept 
    {
        out_buffer.put_datan<value_type>(*value);
        //out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(U16));
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
    {
        PTRU16 bf = reinterpret_cast<PTRU16>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<U32> {
    using value_type = U32;
    using size_type = SIZE_T;
    using pointer = PTRU32;

    pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept 
    {
        out_buffer.put_datan<value_type>(*value);
        //out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(U32));
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
    {
        PTRU32 bf = reinterpret_cast<PTRU32>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<U64> {
    using value_type = U64;
    using size_type = SIZE_T;
    using pointer = PTRU64;

    pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept 
    {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept 
    {
        out_buffer.put_datan<value_type>(*value);
        //out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(U64));
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
    {
        PTRU64 bf = reinterpret_cast<PTRU64>(in_src);
        return *bf;
    }
};

SIZE_T get_serialized_size() noexcept
{
    return 0;
}

template<typename FirstType, typename ... TypesRest>
SIZE_T get_serialized_size(const FirstType& in_arg, const TypesRest&... in_rest) noexcept
{
    serialize_helper<FirstType> helper;
    helper.value = const_cast<FirstType*>(&in_arg);
    SIZE_T myResult = helper.get_serialized_size() + get_serialized_size(std::forward<TypesRest>(in_rest)...);
    return myResult;
}

template<typename SerializedType>
GENERIC serialize(const SerializedType& in_value, char_stream& out_buffer) noexcept 
{
    serialize_helper<SerializedType> helper;
    helper.value = const_cast<SerializedType*>(&in_value);
    helper.serialize(out_buffer);
}

template<typename SerializedType>
SerializedType deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept 
{
    serialize_helper<SerializedType> helper;
    return helper.deserialize(in_src, in_length);
}

static const SIZE_T gPairSerializeBlockLength = 8;

template<typename T1, typename T2>
struct pair {
    using first_type = T1;
    using second_type = T2;
    using size_type = SIZE_T;

    first_type first;
    second_type second;

    MBASE_INLINE_EXPR pair() noexcept
    {
    }

    MBASE_INLINE_EXPR pair(const T1& in_first, const T2& in_second) noexcept : first(in_first), second(in_second)
    {
    }

    MBASE_INLINE_EXPR pair(T1&& in_first, T2&& in_second) noexcept : first(std::move(in_first)), second(std::move(in_second))
    {
    }

    MBASE_INLINE_EXPR pair(const pair&) = default;
    MBASE_INLINE_EXPR pair(pair&&) = default;
    ~pair() = default;

    MBASE_INLINE_EXPR pair& operator=(const pair& in_rhs) noexcept
    {
        first = in_rhs.first;
        second = in_rhs.second;

        return *this;
    }

    MBASE_INLINE_EXPR pair& operator=(pair&& in_rhs) noexcept
    {
        first = std::move(in_rhs.first);
        second = std::move(in_rhs.second);

        return *this;
    }

    MBASE_INLINE_EXPR GENERIC swap(pair& in_rhs) noexcept {
        std::swap(first, in_rhs.first);
        std::swap(second, in_rhs.second);
    }

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        size_type firstBlockSize = mbase::get_serialized_size(first) + gPairSerializeBlockLength;
        size_type secondBlockSize = mbase::get_serialized_size(second) + gPairSerializeBlockLength;

        return firstBlockSize + secondBlockSize;
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept
    {
        SIZE_T blockLength = mbase::get_serialized_size(first);
        out_buffer.put_datan<SIZE_T>(blockLength);
        mbase::serialize(first, out_buffer);


        blockLength = mbase::get_serialized_size(second);
        out_buffer.put_datan<SIZE_T>(blockLength);
        mbase::serialize(second, out_buffer);
    }

    MBASE_INLINE static pair deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept
    {
        char_stream cs(in_src, in_length);
        
        SIZE_T blockLength = cs.get_datan<SIZE_T>();
        first_type ft(std::move(mbase::deserialize<first_type>(cs.get_bufferc(), blockLength)));

        cs.advance(blockLength);
        blockLength = cs.get_datan<SIZE_T>();
        second_type st(std::move(mbase::deserialize<second_type>(cs.get_bufferc(), blockLength)));

        return { std::move(ft), std::move(st) };
    }

    bool operator==(const pair& in_rhs) 
    {
        return (first == in_rhs.first ? true : false) && (second == in_rhs.second ? true : false);
    }

    bool operator!=(const pair& in_rhs)
    {
        return (first != in_rhs.first ? true : false) && (second != in_rhs.second ? true : false);
    }

};

template<typename T1, typename T2>
mbase::pair<T1, T2> make_pair(T1 in_first, T2 in_second) noexcept
{
    return mbase::pair<T1, T2>(in_first, in_second);
}

template<typename T1, typename T2>
std::pair<T1, T2> to_stdpair(const mbase::pair<T1, T2>& in_pair) noexcept
{
    return std::make_pair(in_pair.first, in_pair.second);
}

template<typename T1, typename T2>
std::pair<T1, T2> to_stdpair(mbase::pair<T1, T2>&& in_pair) noexcept
{
    return std::make_pair(std::move(in_pair.first), std::move(in_pair.second));
}


MBASE_STD_END

#endif // MBASE_STD_CSH_H