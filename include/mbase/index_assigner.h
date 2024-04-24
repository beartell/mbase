#ifndef MBASE_INDEX_ASSIGNER_H
#define MBASE_INDEX_ASSIGNER_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/safe_buffer.h>
#include <mbase/stack.h>

MBASE_STD_BEGIN

#define MBASE_DEFAULT_INDEX_ASSIGNER_CAPACITY 512

template<typename IndexType = SIZE_T, typename InactiveIndexContainer = mbase::vector<IndexType>>
class index_assigner {
public:
	using _InactiveIndexContainer = mbase::stack<IndexType, InactiveIndexContainer>;
	using size_type = SIZE_T;

	MBASE_INLINE index_assigner() noexcept;
	MBASE_INLINE MBASE_EXPLICIT index_assigner(size_type in_capacity) noexcept;

	MBASE_INLINE index_assigner(const index_assigner& in_rhs) noexcept;
	MBASE_INLINE index_assigner(index_assigner&& in_rhs) noexcept;

	index_assigner& operator=(const index_assigner& in_rhs) noexcept;
	index_assigner& operator=(index_assigner&& in_rhs) noexcept;

	MBASE_ND("ignoring the returned index") MBASE_INLINE IndexType peek_index() noexcept;
	MBASE_ND("ignoring the returned index") MBASE_INLINE IndexType get_index() noexcept;
	MBASE_INLINE GENERIC release_index(const IndexType& in_index) noexcept;

	MBASE_ND("ignoring the container observation") MBASE_INLINE size_type iic_size() const noexcept;
	MBASE_ND("ignoring the container observation") MBASE_INLINE size_type capacity() const noexcept;

	//MBASE_INLINE GENERIC serialize(safe_buffer* out_buffer);
	//MBASE_INLINE static index_assigner deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length) noexcept;

private:
	_InactiveIndexContainer IIH;
	IndexType mIndex;
	size_type mCapacity;
};

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>::index_assigner() noexcept : IIH(), mIndex(0), mCapacity(MBASE_DEFAULT_INDEX_ASSIGNER_CAPACITY)
{
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>::index_assigner(size_type in_capacity) noexcept : IIH(), mIndex(0), mCapacity(in_capacity)
{
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>::index_assigner(const index_assigner& in_rhs) noexcept : IIH(in_rhs.IIH), mIndex(in_rhs.mIndex), mCapacity(in_rhs.mCapacity)
{
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>::index_assigner(index_assigner&& in_rhs) noexcept : IIH(std::move(in_rhs.IIH)), mIndex(std::move(in_rhs.mIndex)), mCapacity(in_rhs.mCapacity)
{
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>& index_assigner<IndexType, InactiveIndexContainer>::operator=(const index_assigner& in_rhs) noexcept
{
	IIH = in_rhs.IIH;
	mIndex = in_rhs.mIndex;
	mCapacity = in_rhs.mCapacity;

	return *this;
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>& index_assigner<IndexType, InactiveIndexContainer>::operator=(index_assigner&& in_rhs) noexcept
{
	IIH = std::move(in_rhs.IIH);
	mIndex = std::move(in_rhs.mIndex);
	mCapacity = in_rhs.mCapacity;

	return *this;
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_ND("ignoring the returned index") MBASE_INLINE IndexType index_assigner<IndexType, InactiveIndexContainer>::peek_index() noexcept
{
	IndexType iT;
	if(IIH.size())
	{
		iT = IIH.top();
	}
	else
	{
		iT = mIndex;
	}

	return iT;
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_ND("ignoring the returned index") MBASE_INLINE IndexType index_assigner<IndexType, InactiveIndexContainer>::get_index() noexcept
{
	IndexType iT;
	if (IIH.size())
	{
		iT = std::move(IIH.top());
	}
	else
	{
		iT = mIndex++;
	}

	return iT;
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE GENERIC index_assigner<IndexType, InactiveIndexContainer>::release_index(const IndexType& in_index) noexcept
{
	IIH.push(in_index);
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_ND("ignoring the container observation") MBASE_INLINE SIZE_T index_assigner<IndexType, InactiveIndexContainer>::iic_size() const noexcept
{
	return IIH.size();
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_ND("ignoring the container observation") MBASE_INLINE SIZE_T index_assigner<IndexType, InactiveIndexContainer>::capacity() const noexcept
{
	return mCapacity;
}


MBASE_STD_END

#endif // MBASE_INDEX_ASSIGNER_H