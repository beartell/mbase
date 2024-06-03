#ifndef MBASE_STACK_H
#define MBASE_STACK_H

#include <mbase/common.h>
#include <mbase/vector.h> // mbase::vector

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

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE stack() noexcept;
	MBASE_INLINE stack(const stack& in_rhs) noexcept;
	MBASE_INLINE stack(stack&& in_rhs) noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	MBASE_INLINE stack& operator=(const stack& in_rhs) noexcept;
	MBASE_INLINE stack& operator=(stack&& in_rhs) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type get_serialized_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference top() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference top() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool empty() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) SourceContainer& getHandler();
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR GENERIC push(const T& in_data) noexcept;
	MBASE_INLINE_EXPR GENERIC push(T&& in_data) noexcept;
	MBASE_INLINE_EXPR GENERIC pop() noexcept;
	MBASE_INLINE GENERIC deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
	MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept;
	/* ===== NON-MEMBER FUNCTIONS END ===== */

private:
	SourceContainer mSourceContainer;
};

template<typename T, typename SourceContainer>
MBASE_INLINE stack<T, SourceContainer>::stack() noexcept {}

template<typename T, typename SourceContainer>
MBASE_INLINE stack<T, SourceContainer>::stack(const stack& in_rhs) noexcept {
	mSourceContainer = in_rhs.mSourceContainer;
}

template<typename T, typename SourceContainer>
MBASE_INLINE stack<T, SourceContainer>::stack(stack&& in_rhs) noexcept {
	mSourceContainer = std::move(in_rhs.mSourceContainer);
}

template<typename T, typename SourceContainer>
MBASE_INLINE stack<T, SourceContainer>& stack<T, SourceContainer>::operator=(const stack& in_rhs) noexcept
{
	mSourceContainer = in_rhs.mSourceContainer;
	return *this;
}

template<typename T, typename SourceContainer>
MBASE_INLINE stack<T, SourceContainer>& stack<T, SourceContainer>::operator=(stack&& in_rhs) noexcept
{
	mSourceContainer = std::move(in_rhs.mSourceContainer);
	return *this;
}

template<typename T, typename SourceContainer>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR typename stack<T, SourceContainer>::size_type stack<T, SourceContainer>::get_serialized_size() const noexcept
{
	return mSourceContainer.get_serialized_size();
}

template<typename T, typename SourceContainer>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename stack<T, SourceContainer>::reference stack<T, SourceContainer>::top() noexcept {
	return mSourceContainer.back();
}

template<typename T, typename SourceContainer>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename stack<T, SourceContainer>::const_reference stack<T, SourceContainer>::top() const noexcept {
	return mSourceContainer.back();
}

template<typename T, typename SourceContainer>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool stack<T, SourceContainer>::empty() const noexcept {
	return mSourceContainer.empty();
}

template<typename T, typename SourceContainer>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename stack<T, SourceContainer>::size_type stack<T, SourceContainer>::size() const noexcept {
	return mSourceContainer.size();
}

template<typename T, typename SourceContainer>
MBASE_ND(MBASE_OBS_IGNORE) SourceContainer& stack<T, SourceContainer>::getHandler() {
	return mSourceContainer;
}

template<typename T, typename SourceContainer>
MBASE_INLINE_EXPR GENERIC stack<T, SourceContainer>::push(const T& in_data) noexcept {
	mSourceContainer.push_back(in_data);
}

template<typename T, typename SourceContainer>
MBASE_INLINE_EXPR GENERIC stack<T, SourceContainer>::push(T&& in_data) noexcept {
	mSourceContainer.push_back(std::move(in_data));
}

template<typename T, typename SourceContainer>
MBASE_INLINE_EXPR GENERIC stack<T, SourceContainer>::pop() noexcept {
	mSourceContainer.pop_back();
}

template<typename T, typename SourceContainer>
MBASE_INLINE GENERIC stack<T, SourceContainer>::serialize(safe_buffer& out_buffer) noexcept {
	mSourceContainer.serialize(out_buffer);
}

template<typename T, typename SourceContainer>
MBASE_INLINE GENERIC stack<T, SourceContainer>::deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length) noexcept {
	mSourceContainer = mSourceContainer.deserialize(in_buffer, in_length);
}

MBASE_STD_END

#endif // MBASE_STACK_H