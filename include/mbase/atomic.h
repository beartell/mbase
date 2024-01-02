#ifndef MBASE_ATOMIC_H
#define MBASE_ATOMIC_H

#include <mbase/common.h>
#include <Windows.h>

MBASE_STD_BEGIN

template<typename T>
class atomic {
public:
	using value_type = T;

	atomic(const T& in_rhs) noexcept : val(in_rhs){}
	atomic() noexcept : val(0){}
	atomic(const atomic& in_rhs) noexcept {
		val = in_rhs.val;
	}

	atomic& operator=(const atomic& in_rhs) noexcept {
		val = in_rhs.val;
		return *this;
	}

	atomic& operator=(const T& in_rhs) noexcept {
		InterlockedExchange64((LONG64*)&val, in_rhs);
		return *this;
	}

	friend atomic operator+(const atomic& in_lhs, const T& in_rhs) noexcept {
		value_type vt = 0;
		InterlockedExchange64((LONG64*)&vt, in_lhs.val);
		InterlockedAdd64((LONG64*)&vt, in_rhs);
		return atomic(vt);
	}

	friend atomic operator-(const atomic& in_lhs, const T& in_rhs) noexcept {
		value_type vt = 0;
		InterlockedExchange64((LONG64*)&vt, in_lhs.val);
		InterlockedAdd64((LONG64*)&vt, -in_rhs);
		return atomic(vt);
	}

	atomic& operator+=(const T& in_rhs) noexcept {
		InterlockedAdd64((LONG64*)&val, in_rhs);
		return *this;
	}

	atomic& operator++() noexcept {
		InterlockedIncrement64((LONG64*)&val);
		return *this;
	}

	atomic& operator++(int) noexcept {
		InterlockedIncrement64((LONG64*)&val);
		return *this;
	}

	atomic& operator-=(const T& in_rhs) noexcept {
		InterlockedAdd64((LONG64*)&val, -in_rhs);
		return *this;
	}

	atomic& operator--() noexcept {
		InterlockedDecrement64((LONG64*)&val);
		return *this;
	}

	atomic& operator--(int) noexcept {
		InterlockedDecrement64((LONG64*)&val);
		return *this;
	}

	value_type get_val() noexcept {
		return val;
	}

private:
	value_type val;
};

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