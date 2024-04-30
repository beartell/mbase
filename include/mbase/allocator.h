#ifndef MBASE_ALLOCATOR_H
#define MBASE_ALLOCATOR_H

#include <mbase/common.h>
#include <type_traits>
MBASE_STD_BEGIN

// DEFAULT SPECIAL MEMBERS
// NOTE: SINCE ALLOCATOR CLASS IS STATELESS, NO NEED FOR SPECIAL MEMBERS
template <typename T = I8>
class allocator_simple {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const pointer;
	using void_pointer = PTRGENERIC;
	using const_void_pointer = CPTRGENERIC;
	using difference_type = PTRDIFF;
	using size_type = SIZE_T;

	MBASE_ND("dynamically allocated memory unused") static MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount) noexcept {
		if (in_amount <= 0)
		{
			return nullptr;
		}
		return static_cast<pointer>(::operator new(sizeof(value_type) * in_amount));
	}

	MBASE_ND("dynamically allocated memory unused") static MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount, bool in_zero_memory) noexcept {
		if (in_amount <= 0)
		{
			return nullptr;
		}

		T* out_data = static_cast<pointer>(::operator new(sizeof(value_type) * in_amount));
		memset(out_data, 0, sizeof(value_type) * in_amount);

		return out_data;
	}

	static MBASE_INLINE_EXPR GENERIC deallocate(pointer src) noexcept {
		::operator delete(src);
	}

	template<class... Args>
	static MBASE_INLINE_EXPR GENERIC construct(pointer src, Args&& ... args) noexcept {
		::new((void_pointer)src) T(std::forward<Args>(args)...);
	}

	static MBASE_INLINE_EXPR GENERIC destroy(pointer src) noexcept {
		if (!src)
		{
			return;
		}
		delete src;
	}

	static MBASE_INLINE_EXPR GENERIC destroy_all(pointer src) noexcept {
		if(!src)
		{
			return;
		}

		delete [] src;
	}
};

template<typename T = I8>
class allocator {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const pointer;
	using void_pointer = PTRGENERIC;
	using const_void_pointer = CPTRGENERIC;
	using difference_type = PTRDIFF;
	using size_type = SIZE_T;
	
	MBASE_ND("dynamically allocated memory unused") MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount) const;
	MBASE_ND("dynamically allocated memory unused") MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount, bool in_zero_memory) const;
	MBASE_ND("dynamically allocated memory unused") MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount, const T* base) const;
	MBASE_INLINE_EXPR GENERIC deallocate(T* src, SIZE_T in_amount) const;

	template< class... Args >
	MBASE_INLINE_EXPR GENERIC construct(T* src, Args&& ... args) const;
	MBASE_INLINE_EXPR GENERIC destroy(T* src) const;
};

template<typename T>
MBASE_ND("dynamically allocated memory unused") MBASE_INLINE_EXPR T* allocator<T>::allocate(SIZE_T in_amount) const
{
	if(in_amount <= 0)
	{
		return nullptr;
	}
	return static_cast<T*>(::operator new(sizeof(value_type) * in_amount));
}

template<typename T>
MBASE_ND("dynamically allocated memory unused") MBASE_INLINE_EXPR T* allocator<T>::allocate(SIZE_T in_amount, bool in_zero_memory) const
{
	if(in_amount <= 0)
	{
		return nullptr;
	}

	T* out_data = static_cast<T*>(::operator new(sizeof(value_type) * in_amount));
	memset(out_data, 0, sizeof(value_type) * in_amount);

	return out_data;
}

template<typename T>
MBASE_ND("dynamically allocated memory unused") MBASE_INLINE_EXPR T* allocator<T>::allocate(SIZE_T in_amount, const T* base) const
{
	if(in_amount <= 0 || !base)
	{
		return nullptr;
	}

	return static_cast<T*>(::operator new(sizeof(value_type) * in_amount, base));
}

template<typename T>
MBASE_INLINE_EXPR GENERIC allocator<T>::deallocate(T* src, SIZE_T in_amount) const
{
	::operator delete(src);
}

template<typename T>
MBASE_INLINE_EXPR GENERIC allocator<T>::destroy(T* src) const
{
	if(!src)
	{
		return;
	}
	delete src;
}

template<typename T>
template<class ...Args>
MBASE_INLINE_EXPR GENERIC allocator<T>::construct(T* src, Args && ...args) const
{
	::new((void*)src) T(std::forward<Args>(args)...);
}

MBASE_STD_END

#endif // !MBASE_ALLOCATOR_H


