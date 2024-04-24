#ifndef MBASE_STACK_H
#define MBASE_STACK_H

#include <mbase/common.h>
#include <mbase/vector.h>

MBASE_STD_BEGIN

template<typename T, typename SourceContainer = mbase::vector<T>>
class stack {
public:
	using value_type = T;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using reference = value_type&;
	using const_reference = const T&;
	using move_reference = T&&;
	using pointer = T*;
	using const_pointer = const T*;

	stack() noexcept {}

	stack(const stack& in_rhs) noexcept {
		_Sc = in_rhs._Sc;
	}

	stack(stack&& in_rhs) noexcept {
		_Sc = std::move(in_rhs._Sc);
	}

	MBASE_ND("returned element being ignored") MBASE_INLINE_EXPR reference top() noexcept {
		return _Sc.back();
	}

	MBASE_ND("returned element being ignored") MBASE_INLINE_EXPR const_reference top() const noexcept {
		return _Sc.back();
	}

	MBASE_INLINE_EXPR bool empty() const noexcept {
		return _Sc.empty();
	}

	MBASE_ND("container observation ignored") MBASE_INLINE_EXPR size_type size() const noexcept {
		return _Sc.size();
	}

	MBASE_INLINE_EXPR GENERIC push(const T& in_data) noexcept {
		_Sc.push_back(in_data);
	}

	MBASE_INLINE_EXPR GENERIC push(T&& in_data) noexcept {
		_Sc.push_back(std::move(in_data));
	}

	MBASE_INLINE_EXPR GENERIC pop() noexcept {
		_Sc.pop_back();
	}

	MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept {
		_Sc.serialize(out_buffer);
	}

	MBASE_INLINE GENERIC deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length) noexcept {
		_Sc.deserialize(in_buffer, in_length);
	}

	SourceContainer& getHandler() {
		return _Sc;
	}

private:
	SourceContainer _Sc;
};

MBASE_STD_END

#endif // MBASE_STACK_H