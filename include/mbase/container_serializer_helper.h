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

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        return value->get_serialized_size();
    }

	MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept {
		value->serialize(out_buffer);
	}

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) {
        return value_type::deserialize(in_src, in_length);
    }
};

template<>
struct serialize_helper<I8> {
	using value_type = I8;
    using size_type = SIZE_T;
	using pointer = PTR8;

	pointer value;

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept {
        out_buffer.putcn(*value);

        /*MB_SET_SAFE_BUFFER(out_buffer, sizeof(I8));
        PTR8 serializedBuffer = reinterpret_cast<PTR8>(out_buffer.bfSource);
        *serializedBuffer = *value;*/
	}

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) {
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

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept {
        out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(I16));

        /*MB_SET_SAFE_BUFFER(out_buffer, sizeof(I16));
        PTR16 serializedBuffer = reinterpret_cast<PTR16>(out_buffer.bfSource);
        *serializedBuffer = *value;*/
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) {
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

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept {
        out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(I32));

        /*MB_SET_SAFE_BUFFER(out_buffer, sizeof(I32));
        PTR32 serializedBuffer = reinterpret_cast<PTR32>(out_buffer.bfSource);
        *serializedBuffer = *value;*/
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) {
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

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept {
        out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(I64));

        /*MB_SET_SAFE_BUFFER(out_buffer, sizeof(I64));
        PTR64 serializedBuffer = reinterpret_cast<PTR64>(out_buffer.bfSource);
        *serializedBuffer = *value;*/
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) {
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

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept {
        out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(U8));

        /*MB_SET_SAFE_BUFFER(out_buffer, sizeof(U8));
        PTRU8 serializedBuffer = reinterpret_cast<PTRU8>(out_buffer.bfSource);
        *serializedBuffer = *value;*/
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) {
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

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept {
        out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(U16));

        /*MB_SET_SAFE_BUFFER(out_buffer, sizeof(U16));
        PTRU16 serializedBuffer = reinterpret_cast<PTRU16>(out_buffer.bfSource);
        *serializedBuffer = *value;*/
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) {
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

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept {
        out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(U32));

        /*MB_SET_SAFE_BUFFER(out_buffer, sizeof(U32));
        PTRU32 serializedBuffer = reinterpret_cast<PTRU32>(out_buffer.bfSource);
        *serializedBuffer = *value;*/
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) {
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

    MBASE_INLINE size_type get_serialized_size() const noexcept {
        return sizeof(typename value_type);
    }

    MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept {
        out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(value), sizeof(U64));

        /*MB_SET_SAFE_BUFFER(out_buffer, sizeof(U64));
        PTRU64 serializedBuffer = reinterpret_cast<PTRU64>(out_buffer.bfSource);
        *serializedBuffer = *value;*/
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) {
        PTRU64 bf = reinterpret_cast<PTRU64>(in_src);
        return *bf;
    }
};

MBASE_STD_END

#endif // MBASE_STD_CSH_H