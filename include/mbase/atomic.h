#ifndef MBASE_ATOMIC_H
#define MBASE_ATOMIC_H

#include <mbase/common.h>
#include <Windows.h>

MBASE_STD_BEGIN

template<typename T>
class atomic {
public:
	using value_type = T;

	atomic(const T& in_rhs) noexcept;
	atomic() noexcept;
	atomic(const atomic& in_rhs) noexcept;

	atomic& operator=(const atomic& in_rhs) noexcept;
	atomic& operator=(const T& in_rhs) noexcept;

	atomic& operator+=(const T& in_rhs) noexcept;
	atomic& operator++() noexcept;
	atomic& operator++(int) noexcept;
	atomic& operator-=(const T& in_rhs) noexcept;
	atomic& operator--() noexcept;
	atomic& operator--(int) noexcept;

	USED_RETURN("ignoring the atomic variable") value_type get_val() noexcept;

	friend atomic operator+(const atomic& in_lhs, const T& in_rhs) noexcept {
		value_type vt = InterlockedExchange64((LONG64*)&in_lhs.val, in_lhs.val);
		vt += in_rhs;
		return atomic(vt);
	}

	friend atomic operator-(const atomic& in_lhs, const T& in_rhs) noexcept {
		value_type vt = InterlockedExchange64((LONG64*)&in_lhs.val, in_lhs.val);
		vt -= in_rhs;
		return atomic(vt);
	}

private:
	__declspec(align(sizeof(value_type))) volatile value_type val;
};

template<typename T>
atomic<T>::atomic(const T& in_rhs) noexcept : val(in_rhs) {

}

template<typename T>
atomic<T>::atomic() noexcept : val(0) {

}

template<typename T>
atomic<T>::atomic(const atomic& in_rhs) noexcept {
	val = in_rhs.val;
}

template<typename T>
atomic<T>& atomic<T>::operator=(const atomic& in_rhs) noexcept {
	val = in_rhs.val;
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator=(const T& in_rhs) noexcept {
	InterlockedExchange64((LONG64*)&val, in_rhs);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator+=(const T& in_rhs) noexcept {
	InterlockedAdd64((LONG64*)&val, in_rhs);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator++() noexcept {
	InterlockedIncrement64((LONG64*)&val);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator++(int) noexcept {
	InterlockedIncrement64((LONG64*)&val);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator-=(const T& in_rhs) noexcept {
	InterlockedAdd64((LONG64*)&val, -in_rhs);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator--() noexcept {
	InterlockedDecrement64((LONG64*)&val);
	return *this;
}

template<typename T>
atomic<T>& atomic<T>::operator--(int) noexcept {
	InterlockedDecrement64((LONG64*)&val);
	return *this;
}

template<typename T>
USED_RETURN("ignoring the atomic variable") T atomic<T>::get_val() noexcept {
	return val;
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