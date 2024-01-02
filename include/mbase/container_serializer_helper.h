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

	GENERIC serialize(safe_buffer* out_buffer) {
		// FOR GENERIC TYPES
		value->serialize(out_buffer);
	}
};

template<>
struct serialize_helper<I8> {
	using value_type = I8;
	using pointer = PTR8;

	pointer value;

	GENERIC serialize(safe_buffer* out_buffer) {
		PTR8 serializedBuffer = new I8;
		*serializedBuffer = *value;
		out_buffer->bfLength = sizeof(I8);
        out_buffer->bfSource = reinterpret_cast<IBYTEBUFFER>(serializedBuffer);
	}
};

template <>
struct serialize_helper<I16> {
    using value_type = I16;
    using pointer = PTR16;

    pointer value;

    GENERIC serialize(safe_buffer* out_buffer) {
        PTR16 serializedBuffer = new I16;
        *serializedBuffer = *value;
        out_buffer->bfLength = sizeof(I16);
        out_buffer->bfSource = reinterpret_cast<IBYTEBUFFER>(serializedBuffer);
    }
};

template <>
struct serialize_helper<I32> {
    using value_type = I32;
    using pointer = PTR32;

    pointer value;

    GENERIC serialize(safe_buffer* out_buffer) {
        PTR32 serializedBuffer = new I32;
        *serializedBuffer = *value;
        out_buffer->bfLength = sizeof(I32);
        out_buffer->bfSource = reinterpret_cast<IBYTEBUFFER>(serializedBuffer);
    }
};

template <>
struct serialize_helper<I64> {
    using value_type = I64;
    using pointer = PTR64;

    pointer value;

    GENERIC serialize(safe_buffer* out_buffer) {
        PTR64 serializedBuffer = new I64;
        *serializedBuffer = *value;
        out_buffer->bfLength = sizeof(I64);
        out_buffer->bfSource = reinterpret_cast<IBYTEBUFFER>(serializedBuffer);
    }
};

template <>
struct serialize_helper<U8> {
    using value_type = U8;
    using pointer = PTRU8;

    pointer value;

    GENERIC serialize(safe_buffer* out_buffer) {
        PTRU8 serializedBuffer = new U8;
        *serializedBuffer = *value;
        out_buffer->bfLength = sizeof(U8);
        out_buffer->bfSource = reinterpret_cast<IBYTEBUFFER>(serializedBuffer);
    }
};

template <>
struct serialize_helper<U16> {
    using value_type = U16;
    using pointer = PTRU16;

    pointer value;

    GENERIC serialize(safe_buffer* out_buffer) {
        PTRU16 serializedBuffer = new U16;
        *serializedBuffer = *value;
        out_buffer->bfLength = sizeof(U16);
        out_buffer->bfSource = reinterpret_cast<IBYTEBUFFER>(serializedBuffer);
    }
};

template <>
struct serialize_helper<U32> {
    using value_type = U32;
    using pointer = PTRU32;

    pointer value;

    GENERIC serialize(safe_buffer* out_buffer) {
        PTRU32 serializedBuffer = new U32;
        *serializedBuffer = *value;
        out_buffer->bfLength = sizeof(U32);
        out_buffer->bfSource = reinterpret_cast<IBYTEBUFFER>(serializedBuffer);
    }
};

template <>
struct serialize_helper<U64> {
    using value_type = U64;
    using pointer = PTRU64;

    pointer value;

    GENERIC serialize(safe_buffer* out_buffer) {
        PTRU64 serializedBuffer = new U64;
        *serializedBuffer = *value;
        out_buffer->bfLength = sizeof(U64);
        out_buffer->bfSource = reinterpret_cast<IBYTEBUFFER>(serializedBuffer);
    }
};

MBASE_STD_END

#endif // MBASE_STD_CSH_H