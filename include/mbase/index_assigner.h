#ifndef MBASE_INDEX_ASSIGNER_H
#define MBASE_INDEX_ASSIGNER_H

#include <mbase/common.h>
#include <mbase/vector.h> // mbase::vector
#include <mbase/safe_buffer.h> // mbase::safe_buffer
#include <mbase/stack.h> // mbase::stack

MBASE_STD_BEGIN

#define MBASE_DEFAULT_INDEX_ASSIGNER_CAPACITY 512

/*

	--- CLASS INFORMATION ---
Identification: S0C14-OBJ-UD-ST

Name: index_assigner

Parent: None

Behaviour List:
- Default Constructible
- Copy Constructible
- Move Constructible
- Destructible
- Copy Assignable
- Move Assignable
- Templated
- Serializable

Description:
index_assigner is used for indexing the external sequential data types.
Everytime user gets an index using get_index, the index counter is incremented by one.

Whenever an index being returned using release_index, index_assigner will hold a stack of
inactive indexes and if there is any inactive index, get_index will return the recent inactive
index to the user, which will make the index reusable.

*/


template<typename IndexType = SIZE_T, typename InactiveIndexContainer = mbase::vector<IndexType>>
class index_assigner {
public:
	using _InactiveIndexContainer = mbase::stack<IndexType, InactiveIndexContainer>;
	using size_type = SIZE_T;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE index_assigner() noexcept;
	MBASE_INLINE MBASE_EXPLICIT index_assigner(size_type in_capacity) noexcept;
	MBASE_INLINE index_assigner(const index_assigner& in_rhs) noexcept;
	MBASE_INLINE index_assigner(index_assigner&& in_rhs) noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	index_assigner& operator=(const index_assigner& in_rhs) noexcept;
	index_assigner& operator=(index_assigner&& in_rhs) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type iic_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type capacity() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE IndexType peek_index() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE IndexType get_index() noexcept;
	MBASE_INLINE GENERIC release_index(const IndexType& in_index) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	//MBASE_INLINE GENERIC serialize(safe_buffer* out_buffer);
	//MBASE_INLINE static index_assigner deserialize(IBYTEBUFFER in_buffer, SIZE_T in_length) noexcept;

private:
	_InactiveIndexContainer mIIH;
	IndexType mIndex;
	size_type mCapacity;
};

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>::index_assigner() noexcept : mIIH(), mIndex(0), mCapacity(MBASE_DEFAULT_INDEX_ASSIGNER_CAPACITY)
{
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>::index_assigner(size_type in_capacity) noexcept : mIIH(), mIndex(0), mCapacity(in_capacity)
{
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>::index_assigner(const index_assigner& in_rhs) noexcept : mIIH(in_rhs.mIIH), mIndex(in_rhs.mIndex), mCapacity(in_rhs.mCapacity)
{
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>::index_assigner(index_assigner&& in_rhs) noexcept : mIIH(std::move(in_rhs.mIIH)), mIndex(std::move(in_rhs.mIndex)), mCapacity(in_rhs.mCapacity)
{
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>& index_assigner<IndexType, InactiveIndexContainer>::operator=(const index_assigner& in_rhs) noexcept
{
	mIIH = in_rhs.mIIH;
	mIndex = in_rhs.mIndex;
	mCapacity = in_rhs.mCapacity;

	return *this;
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_INLINE index_assigner<IndexType, InactiveIndexContainer>& index_assigner<IndexType, InactiveIndexContainer>::operator=(index_assigner&& in_rhs) noexcept
{
	mIIH = std::move(in_rhs.mIIH);
	mIndex = std::move(in_rhs.mIndex);
	mCapacity = in_rhs.mCapacity;

	return *this;
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE SIZE_T index_assigner<IndexType, InactiveIndexContainer>::iic_size() const noexcept
{
	return mIIH.size();
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE SIZE_T index_assigner<IndexType, InactiveIndexContainer>::capacity() const noexcept
{
	return mCapacity;
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE IndexType index_assigner<IndexType, InactiveIndexContainer>::peek_index() noexcept
{
	IndexType iT;
	if(mIIH.size())
	{
		iT = mIIH.top();
	}
	else
	{
		iT = mIndex;
	}

	return iT;
}

template<typename IndexType, typename InactiveIndexContainer>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE IndexType index_assigner<IndexType, InactiveIndexContainer>::get_index() noexcept
{
	IndexType iT;
	if (mIIH.size())
	{
		iT = std::move(mIIH.top());
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
	mIIH.push(in_index);
}

MBASE_STD_END

#endif // MBASE_INDEX_ASSIGNER_H