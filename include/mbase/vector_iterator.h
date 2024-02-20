#ifndef MBASE_VECTOR_ITERATOR_H
#define MBASE_VECTOR_ITERATOR_H

#include <mbase/common.h>
#include <mbase/sequence_iterator.h>

MBASE_STD_BEGIN

template<typename T>
class vector_iterator : public sequence_iterator<T> {
public:
	vector_iterator(pointer in_ptr) noexcept : sequence_iterator(in_ptr) {}
	vector_iterator(const vector_iterator& in_rhs) noexcept : sequence_iterator(in_rhs._ptr) {}

	MBASE_INLINE pointer operator->() const noexcept {
		return _ptr;
	}

	MBASE_INLINE pointer get() const noexcept {
		return _ptr;
	}

	USED_RETURN MBASE_INLINE bool operator==(const vector_iterator& in_rhs) const noexcept {
		return _ptr == in_rhs._ptr;
	}

	USED_RETURN MBASE_INLINE bool operator!=(const vector_iterator& in_rhs) const noexcept {
		return _ptr != in_rhs._ptr;
	}
};

template <typename T>
class const_vector_iterator : public const_sequence_iterator<T> {
public:
	const_vector_iterator(pointer in_ptr) noexcept : const_sequence_iterator(in_ptr) {}
	const_vector_iterator(const const_vector_iterator& in_rhs) noexcept : const_sequence_iterator(in_rhs._ptr) {}

	USED_RETURN MBASE_INLINE const T& operator*() const noexcept {
		return *_ptr;
	}

	MBASE_INLINE const T* operator->() const noexcept {
		return _ptr;
	}

	MBASE_INLINE const T* get() const noexcept {
		return _ptr;
	}

	USED_RETURN MBASE_INLINE bool operator==(const const_vector_iterator& in_rhs) const noexcept {
		return _ptr == in_rhs._ptr;
	}

	USED_RETURN MBASE_INLINE bool operator!=(const const_vector_iterator& in_rhs) const noexcept {
		return _ptr != in_rhs._ptr;
	}
};


template<typename T>
class reverse_vector_iterator : public reverse_sequence_iterator<T> {
public:
	reverse_vector_iterator(pointer in_ptr) noexcept : reverse_sequence_iterator(in_ptr) {}
	reverse_vector_iterator(const reverse_vector_iterator& in_rhs) noexcept : reverse_sequence_iterator(in_rhs._ptr) {}

	MBASE_INLINE pointer operator->() const noexcept {
		return _ptr;
	}

	MBASE_INLINE pointer get() const noexcept {
		return _ptr;
	}

	USED_RETURN MBASE_INLINE bool operator==(const reverse_vector_iterator& in_rhs) const noexcept {
		return _ptr == in_rhs._ptr;
	}

	USED_RETURN MBASE_INLINE bool operator!=(const reverse_vector_iterator& in_rhs) const noexcept {
		return _ptr != in_rhs._ptr;
	}
};

template <typename T>
class const_reverse_vector_iterator : public const_reverse_sequence_iterator<T> {
public:
	const_reverse_vector_iterator(pointer in_ptr) noexcept : const_reverse_sequence_iterato(in_ptr) {}
	const_reverse_vector_iterator(const const_reverse_vector_iterator& in_rhs) noexcept : const_reverse_sequence_iterator(in_rhs._ptr) {}

	USED_RETURN MBASE_INLINE const T& operator*() const noexcept {
		return *_ptr;
	}

	MBASE_INLINE const T* operator->() const noexcept {
		return _ptr;
	}

	MBASE_INLINE const T* get() const noexcept {
		return _ptr;
	}

	USED_RETURN MBASE_INLINE bool operator==(const const_reverse_vector_iterator& in_rhs) const noexcept {
		return _ptr == in_rhs._ptr;
	}

	USED_RETURN MBASE_INLINE bool operator!=(const const_reverse_vector_iterator& in_rhs) const noexcept {
		return _ptr != in_rhs._ptr;
	}
};


MBASE_STD_END

#endif // !MBASE_VECTOR_ITERATOR_H
