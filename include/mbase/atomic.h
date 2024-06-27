#ifndef MBASE_ATOMIC_H
#define MBASE_ATOMIC_H

#include <mbase/common.h>

#ifdef MBASE_PLATFORM_WINDOWS
#include <Windows.h> // InterlockedExchange64, InterlockedAdd64, InterlockedIncrement64, InterlockedDecrement64
#endif

MBASE_STD_BEGIN

/*

	--- CLASS INFORMATION ---
Identification: S0C3-OBJ-UD-NS

Name: atomic

Parent: None

Behaviour List:
- Default Constructible
- Copy Constructible
- Equality Comparable
- Copy Assignable
- Templated
- Type Aware
- Sign Comparable

Description:
This class is identical to std::atomic. 
However, it will most possibly be deprecated and not be used in the future.
Reason: It's because std::atomic seems sufficient.

*/


template<typename T>
class atomic {
public:
	using value_type = T;

	/* ===== BUILDER METHODS BEGIN ===== */
	atomic(const T& in_rhs) noexcept;
	atomic() noexcept;
	atomic(const atomic& in_rhs) noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	atomic& operator=(const atomic& in_rhs) noexcept;
	atomic& operator=(const T& in_rhs) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) value_type get_val() noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
	atomic& operator+=(const T& in_rhs) noexcept;
	atomic& operator++() noexcept;
	atomic& operator++(int) noexcept;
	atomic& operator-=(const T& in_rhs) noexcept;
	atomic& operator--() noexcept;
	atomic& operator--(int) noexcept;
	/* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

	/* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
	friend atomic operator+(const atomic& in_lhs, const T& in_rhs) noexcept 
	{
		value_type vt = InterlockedExchange64((LONG64*)&in_lhs.val, in_lhs.val);
		vt += in_rhs;
		return atomic(vt);
	}
	friend atomic operator-(const atomic& in_lhs, const T& in_rhs) noexcept 
	{
		value_type vt = InterlockedExchange64((LONG64*)&in_lhs.val, in_lhs.val);
		vt -= in_rhs;
		return atomic(vt);
	}
	/* ===== NON-MEMBER FUNCTIONS END ===== */

private:
	__declspec(align(sizeof(value_type))) volatile value_type mVal;
};

template<typename T>
atomic<T>::atomic(const T& in_rhs) noexcept : mVal(in_rhs)
{

}

template<typename T>
atomic<T>::atomic() noexcept : mVal(0)
{

}

template<typename T>
atomic<T>::atomic(const atomic& in_rhs) noexcept
{
	mVal = in_rhs.mVal;
}

template<typename T>
atomic<T>& atomic<T>::operator=(const atomic& in_rhs) noexcept
{
	mVal = in_rhs.mVal;
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator=(const T& in_rhs) noexcept
{
	InterlockedExchange64((LONG64*)&mVal, in_rhs);
	return *this;
}

template<typename T>
MBASE_ND(MBASE_OBS_IGNORE) T atomic<T>::get_val() noexcept
{
	return mVal;
}

template<typename T>
atomic<T>& atomic<T>::operator+=(const T& in_rhs) noexcept 
{
	InterlockedAdd64((LONG64*)&mVal, in_rhs);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator++() noexcept 
{
	InterlockedIncrement64((LONG64*)&mVal);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator++(int) noexcept 
{
	InterlockedIncrement64((LONG64*)&mVal);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator-=(const T& in_rhs) noexcept
{
	InterlockedAdd64((LONG64*)&mVal, -in_rhs);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator--() noexcept 
{
	InterlockedDecrement64((LONG64*)&mVal);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator--(int) noexcept
{
	InterlockedDecrement64((LONG64*)&mVal);
	return *this;
}

using atomic_i8 = mbase::atomic<I8>;
using atomic_i16 = mbase::atomic<I16>;
using atomic_i32 = mbase::atomic<I32>;
using atomic_i64 = mbase::atomic<I64>;

using atomic_u8 = mbase::atomic<U8>;
using atomic_u16 = mbase::atomic<U16>;
using atomic_u32 = mbase::atomic<U32>;
using atomic_u64 = mbase::atomic<U64>;

MBASE_STD_END

#endif // MBASE_ATOMIC_H