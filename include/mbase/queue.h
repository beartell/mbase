#ifndef MBASE_QUEUE_H
#define MBASE_QUEUE_H

#include <mbase/common.h>
#include <mbase/list.h>

MBASE_STD_BEGIN

template<typename T, typename SourceContainer = mbase::list<T>>
class queue {
public:
	using value_type = T;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using reference = value_type&;
	using const_reference = const T&;
	using move_reference = T&&;
	using pointer = T*;
	using const_pointer = const T*;

	queue() noexcept {

	}

	queue(const queue& in_rhs) noexcept {
		_Sc = in_rhs._Sc;
	}

	queue(queue&& in_rhs) noexcept {
		_Sc = std::move(in_rhs._Sc);
	}

	USED_RETURN MBASE_INLINE_EXPR const_reference front() const noexcept {
		return _Sc.front();
	}

	USED_RETURN MBASE_INLINE_EXPR bool empty() const noexcept {
		return _Sc.empty();
	}

	USED_RETURN MBASE_INLINE_EXPR size_type size() const noexcept {
		return _Sc.size();
	}

	MBASE_INLINE_EXPR GENERIC push(const T& in_data) noexcept {
		_Sc.push_back(in_data);
	}

	MBASE_INLINE_EXPR GENERIC push(T&& in_data) noexcept {
		_Sc.push_back(std::move(in_data));
	}

	MBASE_INLINE_EXPR GENERIC pop() noexcept {
		_Sc.pop_front();
	}

	SourceContainer& getHandler() {
		return _Sc;
	}

private:
	SourceContainer _Sc;
};

MBASE_STD_END

#endif // MBASE_QUEUE_H