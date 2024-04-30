#ifndef MBASE_STD_CSH_H
#define MBASE_STD_CSH_H

#include <mbase/common.h>
#include <mbase/safe_buffer.h>

MBASE_STD_BEGIN

template <typename SerializedType>
struct serialize_helper {
	using value_type = SerializedType;
	using pointer = std::add_pointer_t<SerializedType>;

	pointer value;

	MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
		// FOR GENERIC TYPES
		value->serialize(out_buffer);
	}

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
        return value->deserialize(in_src, in_length);
    }
};

template<>
struct serialize_helper<I8> {
	using value_type = I8;
	using pointer = PTR8;

	pointer value;

    MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
        MB_SET_SAFE_BUFFER(out_buffer, sizeof(I8));
        PTR8 serializedBuffer = reinterpret_cast<PTR8>(out_buffer.bfSource);
        *serializedBuffer = *value;
	}

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
        PTR8 bf = reinterpret_cast<PTR8>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<I16> {
    using value_type = I16;
    using pointer = PTR16;

    pointer value;

    MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
        MB_SET_SAFE_BUFFER(out_buffer, sizeof(I16));
        PTR16 serializedBuffer = reinterpret_cast<PTR16>(out_buffer.bfSource);
        *serializedBuffer = *value;
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
        PTR16 bf = reinterpret_cast<PTR16>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<I32> {
    using value_type = I32;
    using pointer = PTR32;

    pointer value;

    MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
        MB_SET_SAFE_BUFFER(out_buffer, sizeof(I32));
        PTR32 serializedBuffer = reinterpret_cast<PTR32>(out_buffer.bfSource);
        *serializedBuffer = *value;
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
        PTR32 bf = reinterpret_cast<PTR32>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<I64> {
    using value_type = I64;
    using pointer = PTR64;

    pointer value;

    MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
        MB_SET_SAFE_BUFFER(out_buffer, sizeof(I64));
        PTR64 serializedBuffer = reinterpret_cast<PTR64>(out_buffer.bfSource);
        *serializedBuffer = *value;
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
        PTR64 bf = reinterpret_cast<PTR64>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<U8> {
    using value_type = U8;
    using pointer = PTRU8;

    pointer value;

    MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
        MB_SET_SAFE_BUFFER(out_buffer, sizeof(U8));
        PTRU8 serializedBuffer = reinterpret_cast<PTRU8>(out_buffer.bfSource);
        *serializedBuffer = *value;
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
        PTRU8 bf = reinterpret_cast<PTRU8>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<U16> {
    using value_type = U16;
    using pointer = PTRU16;

    pointer value;

    MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
        MB_SET_SAFE_BUFFER(out_buffer, sizeof(U16));
        PTRU16 serializedBuffer = reinterpret_cast<PTRU16>(out_buffer.bfSource);
        *serializedBuffer = *value;
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
        PTRU16 bf = reinterpret_cast<PTRU16>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<U32> {
    using value_type = U32;
    using pointer = PTRU32;

    pointer value;

    MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
        MB_SET_SAFE_BUFFER(out_buffer, sizeof(U32));
        PTRU32 serializedBuffer = reinterpret_cast<PTRU32>(out_buffer.bfSource);
        *serializedBuffer = *value;
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
        PTRU32 bf = reinterpret_cast<PTRU32>(in_src);
        return *bf;
    }
};

template <>
struct serialize_helper<U64> {
    using value_type = U64;
    using pointer = PTRU64;

    pointer value;

    MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
        MB_SET_SAFE_BUFFER(out_buffer, sizeof(U64));
        PTRU64 serializedBuffer = reinterpret_cast<PTRU64>(out_buffer.bfSource);
        *serializedBuffer = *value;
    }

    MBASE_INLINE value_type deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
        PTRU64 bf = reinterpret_cast<PTRU64>(in_src);
        return *bf;
    }
};

MBASE_STD_END

#endif // MBASE_STD_CSH_H