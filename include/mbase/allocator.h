#ifndef MBASE_ALLOCATOR_H
#define MBASE_ALLOCATOR_H

#include <mbase/common.h>
#include <utility> // std::forward
#include <cstring>
MBASE_STD_BEGIN

/* 

	--- CLASS INFORMATION ---
Identification: S0C1-UTL-NA-ST

Name: allocator_simple

Parent: None

Behaviour List:
- Allocate Aware
- Templated
- Stateless
- Type Aware

Description:
It provides an allocation interface to the user,
without the necessity of object instantiation.

*/

#define MBASE_ALLOCATE_WARNING "dynamically allocated memory unused"

template <typename T>
class allocator_simple {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const pointer;
	using void_pointer = PTRGENERIC;
	using const_void_pointer = CPTRGENERIC;
	using difference_type = PTRDIFF;
	using size_type = SIZE_T;

	/* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
	MBASE_ND(MBASE_ALLOCATE_WARNING) static MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount) noexcept 
	{
		if (in_amount <= 0)
		{
			return nullptr;
		}
		return static_cast<pointer>(::operator new(sizeof(value_type) * in_amount));
	}

	MBASE_ND(MBASE_ALLOCATE_WARNING) static MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount, bool in_zero_memory) noexcept 
	{
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
	static MBASE_INLINE_EXPR GENERIC construct(pointer src, Args&& ... args) noexcept 
	{
		::new((void_pointer)src) T(std::forward<Args>(args)...);
	}

	static MBASE_INLINE_EXPR GENERIC destroy(pointer src) noexcept 
	{
		if (!src)
		{
			return;
		}
		delete src;
	}

	static MBASE_INLINE_EXPR GENERIC destroy_all(pointer src) noexcept 
	{
		if(!src)
		{
			return;
		}

		delete [] src;
	}
	/* ===== NON-MEMBER FUNCTIONS END ===== */
};

/*

	--- CLASS INFORMATION ---
Identification: S0C2-UTL-NA-ST

Name: allocator

Parent: None

Behaviour List:
- Allocate Aware
- Templated
- Stateless
- Type Aware

Description:
It provides an allocation interface to the user
identical to the std::allocator. Which means that for
allocation routines to be used, it must be instantiated first.

Most containers in the library use this class as an internal
allocator for their data.

*/


template<typename T>
class allocator {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const pointer;
	using void_pointer = PTRGENERIC;
	using const_void_pointer = CPTRGENERIC;
	using difference_type = PTRDIFF;
	using size_type = SIZE_T;
	
	/* ===== NON-MODIFIER METHODS BEGIN ===== */
	MBASE_ND(MBASE_ALLOCATE_WARNING) MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount) const;
	MBASE_ND(MBASE_ALLOCATE_WARNING) MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount, bool in_zero_memory) const;
	MBASE_ND(MBASE_ALLOCATE_WARNING) MBASE_INLINE_EXPR T* allocate(SIZE_T in_amount, const T* base) const;
	MBASE_INLINE_EXPR GENERIC deallocate(T* src, SIZE_T in_amount) const;
	template< class... Args >
	MBASE_INLINE_EXPR GENERIC construct(T* src, Args&& ... args) const;
	MBASE_INLINE_EXPR GENERIC destroy(T* src) const;
	/* ===== NON-MODIFIER METHODS END ===== */
};

template<typename T>
MBASE_ND(MBASE_ALLOCATE_WARNING) MBASE_INLINE_EXPR T* allocator<T>::allocate(SIZE_T in_amount) const
{
	if(in_amount <= 0)
	{
		return nullptr;
	}
	return static_cast<T*>(::operator new(sizeof(value_type) * in_amount));
}

template<typename T>
MBASE_ND(MBASE_ALLOCATE_WARNING) MBASE_INLINE_EXPR T* allocator<T>::allocate(SIZE_T in_amount, bool in_zero_memory) const
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
MBASE_ND(MBASE_ALLOCATE_WARNING) MBASE_INLINE_EXPR T* allocator<T>::allocate(SIZE_T in_amount, const T* base) const
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


