#ifndef MBASE_UNORDERED_MAP_ITERATOR_H
#define MBASE_UNORDERED_MAP_ITERATOR_H

#include <mbase/common.h>
#include <iterator>

MBASE_STD_BEGIN

template<typename MapContainer, typename BucketIterator>
class linear_bucket_iterator {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename MapContainer::value_type;
	using pointer = typename MapContainer::pointer;
	using const_pointer = typename MapContainer::const_pointer;
	using reference = typename MapContainer::reference;
	using size_type = typename MapContainer::size_type;
	using difference_type = typename MapContainer::difference_type;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE linear_bucket_iterator() noexcept;
	MBASE_INLINE linear_bucket_iterator(MapContainer* in_map, I64 in_bucket_index) noexcept;
	MBASE_INLINE linear_bucket_iterator(MapContainer* in_map, I64 in_bucket_index, BucketIterator in_iterator) noexcept;
	MBASE_INLINE linear_bucket_iterator(const linear_bucket_iterator& in_rhs) noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	MBASE_INLINE linear_bucket_iterator& operator=(const linear_bucket_iterator& in_rhs) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE pointer get() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE reference operator*() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE pointer operator->() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I64 get_bucket_index() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE BucketIterator& get_bucket_iterator() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const BucketIterator& get_bucket_iterator() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE linear_bucket_iterator& operator++() noexcept;
	MBASE_INLINE linear_bucket_iterator& operator--() noexcept;
	MBASE_INLINE linear_bucket_iterator operator++(int) noexcept;
	MBASE_INLINE linear_bucket_iterator operator--(int) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== NON-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE bool operator==(const linear_bucket_iterator& in_rhs) const noexcept;
	MBASE_INLINE bool operator!=(const linear_bucket_iterator& in_rhs) const noexcept;
	/* ===== NON-MODIFIER METHODS END ===== */
private:
	MapContainer* mMapContainer;
	BucketIterator mBucketListIterator;
	I64 mBucketIndex;
};

template<typename MapContainer, typename BucketIterator>
class const_linear_bucket_iterator {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename MapContainer::value_type;
	using pointer = typename MapContainer::pointer;
	using const_pointer = typename MapContainer::const_pointer;
	using reference = typename MapContainer::reference;
	using const_reference = typename MapContainer::const_reference;
	using size_type = typename MapContainer::size_type;
	using difference_type = typename MapContainer::difference_type;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE const_linear_bucket_iterator() noexcept;
	MBASE_INLINE const_linear_bucket_iterator(MapContainer* in_map, I64 in_bucket_index) noexcept;
	MBASE_INLINE const_linear_bucket_iterator(MapContainer* in_map, I64 in_bucket_index, BucketIterator in_iterator) noexcept;
	MBASE_INLINE const_linear_bucket_iterator(const const_linear_bucket_iterator& in_rhs) noexcept;
	MBASE_INLINE const_linear_bucket_iterator(const linear_bucket_iterator<MapContainer, BucketIterator>& in_rhs) noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	MBASE_INLINE const_linear_bucket_iterator& operator=(const const_linear_bucket_iterator& in_rhs) noexcept;
	MBASE_INLINE const_linear_bucket_iterator& operator=(const linear_bucket_iterator<MapContainer, BucketIterator>& in_rhs) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_pointer get() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reference operator*() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_pointer operator->() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I64 get_bucket_index() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const BucketIterator& get_bucket_iterator() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE const_linear_bucket_iterator& operator++() noexcept;
	MBASE_INLINE const_linear_bucket_iterator& operator--() noexcept;
	MBASE_INLINE const_linear_bucket_iterator operator++(int) noexcept;
	MBASE_INLINE const_linear_bucket_iterator operator--(int) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== NON-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE bool operator==(const const_linear_bucket_iterator& in_rhs) const noexcept;
	MBASE_INLINE bool operator!=(const const_linear_bucket_iterator& in_rhs) const noexcept;
	/* ===== NON-MODIFIER METHODS END ===== */
private:
	MapContainer* mMapContainer;
	BucketIterator mBucketListIterator;
	I64 mBucketIndex;
};

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE linear_bucket_iterator<MapContainer, BucketIterator>::linear_bucket_iterator() noexcept : mMapContainer(nullptr), mBucketListIterator(), mBucketIndex(0)
{
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE linear_bucket_iterator<MapContainer, BucketIterator>::linear_bucket_iterator(MapContainer* in_map, I64 in_bucket_index) noexcept : mMapContainer(in_map), mBucketListIterator(), mBucketIndex(in_bucket_index)
{
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE linear_bucket_iterator<MapContainer, BucketIterator>::linear_bucket_iterator(MapContainer* in_map, I64 in_bucket_index, BucketIterator in_iterator) noexcept : mMapContainer(in_map), mBucketListIterator(in_iterator), mBucketIndex(in_bucket_index)
{
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE linear_bucket_iterator<MapContainer, BucketIterator>::linear_bucket_iterator(const linear_bucket_iterator& in_rhs) noexcept : mMapContainer(in_rhs.mMapContainer), mBucketListIterator(in_rhs.mBucketListIterator), mBucketIndex(in_rhs.mBucketIndex)
{
}


template<typename MapContainer, typename BucketIterator>
MBASE_INLINE linear_bucket_iterator<MapContainer, BucketIterator>& linear_bucket_iterator<MapContainer, BucketIterator>::operator=(const linear_bucket_iterator& in_rhs) noexcept
{
	mMapContainer = in_rhs.mMapContainer;
	mBucketListIterator = in_rhs.mBucketListIterator;
	mBucketIndex = in_rhs.mBucketIndex;
	return *this;
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename linear_bucket_iterator<MapContainer, BucketIterator>::pointer linear_bucket_iterator<MapContainer, BucketIterator>::get() noexcept
{
	return mBucketListIterator.get();
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename linear_bucket_iterator<MapContainer, BucketIterator>::reference linear_bucket_iterator<MapContainer, BucketIterator>::operator*() noexcept
{
	return *mBucketListIterator;
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename linear_bucket_iterator<MapContainer, BucketIterator>::pointer linear_bucket_iterator<MapContainer, BucketIterator>::operator->() noexcept
{
	return this->get();
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I64 linear_bucket_iterator<MapContainer, BucketIterator>::get_bucket_index() const noexcept
{
	return mBucketIndex;
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE BucketIterator& linear_bucket_iterator<MapContainer, BucketIterator>::get_bucket_iterator() noexcept
{
	return mBucketListIterator;
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const BucketIterator& linear_bucket_iterator<MapContainer, BucketIterator>::get_bucket_iterator() const noexcept
{
	return mBucketListIterator;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE linear_bucket_iterator<MapContainer, BucketIterator>& linear_bucket_iterator<MapContainer, BucketIterator>::operator++() noexcept
{
	++mBucketListIterator;
	if (mBucketListIterator == mMapContainer->end(mBucketIndex))
	{
		++mBucketIndex;
		BucketIterator bucketIter;
		for (; mBucketIndex < mMapContainer->bucket_count(); ++mBucketIndex)
		{
			bucketIter = mMapContainer->begin(mBucketIndex);
			if (bucketIter != mMapContainer->end(mBucketIndex))
			{
				this->mBucketListIterator = bucketIter;
				return *this;
			}
		}

		*this = mMapContainer->end();
	}
	return *this;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE linear_bucket_iterator<MapContainer, BucketIterator>& linear_bucket_iterator<MapContainer, BucketIterator>::operator--() noexcept
{
	--mBucketListIterator;
	if (mBucketListIterator == mMapContainer->begin(mBucketIndex))
	{
		--mBucketIndex;
		BucketIterator bucketIter;
		for (mBucketIndex; mBucketIndex > -1; --mBucketIndex)
		{
			bucketIter = mMapContainer->end(mBucketIndex);
		}
		*this = mMapContainer->end();
	}

	*this = mMapContainer->end();

	return *this;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE linear_bucket_iterator<MapContainer, BucketIterator> linear_bucket_iterator<MapContainer, BucketIterator>::operator++(int) noexcept
{
	linear_bucket_iterator lbi(*this);
	++(*this);
	return lbi;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE linear_bucket_iterator<MapContainer, BucketIterator> linear_bucket_iterator<MapContainer, BucketIterator>::operator--(int) noexcept
{
	linear_bucket_iterator lbi(*this);
	--(*this);
	return lbi;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE bool linear_bucket_iterator<MapContainer, BucketIterator>::operator==(const linear_bucket_iterator& in_rhs) const noexcept
{
	return mMapContainer == in_rhs.mMapContainer && mBucketListIterator == in_rhs.mBucketListIterator && mBucketIndex == in_rhs.mBucketIndex;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE bool linear_bucket_iterator<MapContainer, BucketIterator>::operator!=(const linear_bucket_iterator& in_rhs) const noexcept
{
	return !(mMapContainer == in_rhs.mMapContainer && mBucketListIterator == in_rhs.mBucketListIterator && mBucketIndex == in_rhs.mBucketIndex);
}

/* CONST */

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator>::const_linear_bucket_iterator() noexcept : mMapContainer(nullptr), mBucketListIterator(), mBucketIndex(0)
{
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator>::const_linear_bucket_iterator(MapContainer* in_map, I64 in_bucket_index) noexcept : mMapContainer(in_map), mBucketListIterator(), mBucketIndex(in_bucket_index)
{
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator>::const_linear_bucket_iterator(MapContainer* in_map, I64 in_bucket_index, BucketIterator in_iterator) noexcept : mMapContainer(in_map), mBucketListIterator(in_iterator), mBucketIndex(in_bucket_index)
{
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator>::const_linear_bucket_iterator(const const_linear_bucket_iterator& in_rhs) noexcept : mMapContainer(in_rhs.mMapContainer), mBucketListIterator(in_rhs.mBucketListIterator), mBucketIndex(in_rhs.mBucketIndex)
{
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator>::const_linear_bucket_iterator(const linear_bucket_iterator<MapContainer, BucketIterator>& in_rhs) noexcept : mMapContainer(in_rhs.mMapContainer), mBucketListIterator(in_rhs.mBucketListIterator), mBucketIndex(in_rhs.mBucketIndex)
{
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator>& const_linear_bucket_iterator<MapContainer, BucketIterator>::operator=(const const_linear_bucket_iterator& in_rhs) noexcept
{
	mMapContainer = in_rhs.mMapContainer;
	mBucketListIterator = in_rhs.mBucketListIterator;
	mBucketIndex = in_rhs.mBucketIndex;
	return *this;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator>& const_linear_bucket_iterator<MapContainer, BucketIterator>::operator=(const linear_bucket_iterator<MapContainer, BucketIterator>& in_rhs) noexcept
{
	mMapContainer = in_rhs.mMapContainer;
	mBucketListIterator = in_rhs.mBucketListIterator;
	mBucketIndex = in_rhs.mBucketIndex;
	return *this;
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename const_linear_bucket_iterator<MapContainer, BucketIterator>::const_pointer const_linear_bucket_iterator<MapContainer, BucketIterator>::get() const noexcept
{
	return (const_pointer)mBucketListIterator.get();
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename const_linear_bucket_iterator<MapContainer, BucketIterator>::const_reference const_linear_bucket_iterator<MapContainer, BucketIterator>::operator*() const noexcept
{
	return *mBucketListIterator;
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename const_linear_bucket_iterator<MapContainer, BucketIterator>::const_pointer const_linear_bucket_iterator<MapContainer, BucketIterator>::operator->() const noexcept
{
	return this->get();
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I64 const_linear_bucket_iterator<MapContainer, BucketIterator>::get_bucket_index() const noexcept
{
	return mBucketIndex;
}

template<typename MapContainer, typename BucketIterator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const BucketIterator& const_linear_bucket_iterator<MapContainer, BucketIterator>::get_bucket_iterator() const noexcept
{
	return mBucketListIterator;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator>& const_linear_bucket_iterator<MapContainer, BucketIterator>::operator++() noexcept
{
	++mBucketListIterator;
	if (mBucketListIterator == mMapContainer->cend(mBucketIndex))
	{
		++mBucketIndex;
		BucketIterator bucketIter;
		for (mBucketIndex; mBucketIndex < mMapContainer->bucket_count(); ++mBucketIndex)
		{
			bucketIter = mMapContainer->cbegin(mBucketIndex);
			if (bucketIter != mMapContainer->cend(mBucketIndex))
			{
				this->mBucketListIterator = bucketIter;
				return *this;
			}
		}

		*this = mMapContainer->cend();
	}
	return *this;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator>& const_linear_bucket_iterator<MapContainer, BucketIterator>::operator--() noexcept
{
	--mBucketListIterator;
	if (mBucketListIterator == mMapContainer->cbegin(mBucketIndex))
	{
		--mBucketIndex;
		BucketIterator bucketIter;
		for (mBucketIndex; mBucketIndex > -1; --mBucketIndex)
		{
			bucketIter = mMapContainer->cend(mBucketIndex);
		}
		*this = mMapContainer->cend();
	}

	*this = mMapContainer->cend();

	return *this;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator> const_linear_bucket_iterator<MapContainer, BucketIterator>::operator++(int) noexcept
{
	const_linear_bucket_iterator lbi(*this);
	++(*this);
	return lbi;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE const_linear_bucket_iterator<MapContainer, BucketIterator> const_linear_bucket_iterator<MapContainer, BucketIterator>::operator--(int) noexcept
{
	const_linear_bucket_iterator lbi(*this);
	--(*this);
	return lbi;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE bool const_linear_bucket_iterator<MapContainer, BucketIterator>::operator==(const const_linear_bucket_iterator& in_rhs) const noexcept
{
	return mMapContainer == in_rhs.mMapContainer && mBucketListIterator == in_rhs.mBucketListIterator && mBucketIndex == in_rhs.mBucketIndex;
}

template<typename MapContainer, typename BucketIterator>
MBASE_INLINE bool const_linear_bucket_iterator<MapContainer, BucketIterator>::operator!=(const const_linear_bucket_iterator& in_rhs) const noexcept
{
	return !(mMapContainer == in_rhs.mMapContainer && mBucketListIterator == in_rhs.mBucketListIterator && mBucketIndex == in_rhs.mBucketIndex);
}

MBASE_STD_END

#endif // !MBASE_UNORDERED_MAP_ITERATOR_H
