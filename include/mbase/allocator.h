#ifndef MBASE_ALLOCATOR_H
#define MBASE_ALLOCATOR_H

#include <mbase/common.h>
#include <type_traits>
MBASE_STD_BEGIN

// DEFAULT SPECIAL MEMBERS
// NOTE: SINCE ALLOCATOR CLASS IS STATELESS, NO NEED FOR 
template<typename T = I8>
class allocator {
public:
	using value_type = T;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const reference;
	using size_type = SIZE_T;
	using diffference_type = PTRDIFF;
	
	USED_RETURN MBASE_INLINE_EXPR pointer allocate(SIZE_T in_amount) const;
	USED_RETURN MBASE_INLINE_EXPR pointer allocate(SIZE_T in_amount, bool in_zero_memory) const;
	USED_RETURN MBASE_INLINE_EXPR pointer allocate(const T* base, SIZE_T in_amount) const;
	MBASE_INLINE_EXPR GENERIC deallocate(T* src, SIZE_T in_amount) const;

	template< class... Args >
	MBASE_INLINE_EXPR GENERIC construct(T* src, Args&& ... args) const;
	MBASE_INLINE_EXPR GENERIC destroy(T* src) const;

	//GENERIC wowman();

};

template<typename T>
USED_RETURN MBASE_INLINE_EXPR T* allocator<T>::allocate(SIZE_T in_amount) const
{
	if(in_amount <= 0)
	{
		return nullptr;
	}
	return static_cast<pointer>(::operator new(alignof(value_type) * in_amount));
}

template<typename T>
USED_RETURN MBASE_INLINE_EXPR T* allocator<T>::allocate(SIZE_T in_amount, bool in_zero_memory) const
{
	if(in_amount <= 0)
	{
		return nullptr;
	}

	T* out_data = static_cast<pointer>(::operator new(sizeof(value_type) * in_amount));
	memset(out_data, 0, sizeof(value_type) * in_amount);

	return out_data;
}

template<typename T>
USED_RETURN MBASE_INLINE_EXPR T* allocator<T>::allocate(const T* base, SIZE_T in_amount) const
{
	if(in_amount <= 0 || !base)
	{
		return nullptr;
	}

	return static_cast<pointer>(::operator new(alignof(value_type) * in_amount, base));
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


