#ifndef MBASE_UMAP_H
#define MBASE_UMAP_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <mbase/vector.h>
#include <mbase/list.h>
#include <mbase/traits.h>
#include <mbase/unordered_map_iterator.h>
#include <initializer_list>
#include <exception>
#include <utility>
#include <stdexcept>

MBASE_STD_BEGIN

static const SIZE_T gUmapDefaultBucketCount = 8;

template<typename Key, 
	typename Value, 
	typename Hash = std::hash<Key>,
	typename KeyEqual = std::equal_to<Key>,
	typename Allocator = mbase::allocator<mbase::pair<Key, Value>>>
class unordered_map {
public:
	using key_type = Key;
	using mapped_type = Value;
	using value_type = mbase::pair<Key, Value>;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using hasher = Hash;
	using bucket_node_type = mbase::list<value_type>;
	using bucket_type = mbase::vector<bucket_node_type>;
	using key_equal = KeyEqual;
	using allocator_type = Allocator;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename allocator_type::pointer;
	using const_pointer = typename allocator_type::const_pointer;
	using local_iterator = typename bucket_node_type::iterator;
	using const_local_iterator = typename bucket_node_type::const_iterator;
	using iterator = mbase::linear_bucket_iterator<unordered_map, local_iterator>;
	using const_iterator = mbase::const_linear_bucket_iterator<unordered_map, const_local_iterator>;
	using node_type = F32;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR unordered_map() noexcept;
	MBASE_INLINE_EXPR MBASE_EXPLICIT unordered_map(size_type in_bucket_count, const Hash& in_hash = Hash(), const key_equal& in_equal = key_equal(), const Allocator& in_alloc = Allocator()) noexcept;
	MBASE_INLINE_EXPR unordered_map(size_type in_bucket_count, const Allocator& in_alloc) noexcept;
	MBASE_INLINE_EXPR unordered_map(size_type in_bucket_count, const Hash& in_hash, const Allocator& in_alloc) noexcept;
	MBASE_EXPLICIT unordered_map(const Allocator& in_alloc) noexcept;
	/* InputIt versions does not exist for now */
	MBASE_INLINE_EXPR unordered_map(const unordered_map& in_rhs) noexcept;
	MBASE_INLINE_EXPR unordered_map(const unordered_map& in_rhs, const Allocator& in_alloc) noexcept;
	MBASE_INLINE_EXPR unordered_map(unordered_map&& in_rhs) noexcept;
	MBASE_INLINE_EXPR unordered_map(unordered_map&& in_rhs, const Allocator& in_alloc) noexcept;
	MBASE_INLINE_EXPR unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count = gUmapDefaultBucketCount, const Hash& in_hash = Hash(), const key_equal& in_equal = key_equal(), const Allocator& in_alloc = Allocator()) noexcept;
	MBASE_INLINE_EXPR unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Allocator& in_alloc) noexcept;
	MBASE_INLINE_EXPR unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Hash& in_hash, const Allocator& in_alloc) noexcept;
	MBASE_INLINE ~unordered_map();
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR unordered_map& operator=(const unordered_map& in_rhs) noexcept;
	MBASE_INLINE_EXPR unordered_map& operator=(unordered_map&& in_rhs) noexcept;
	MBASE_INLINE_EXPR unordered_map& operator=(std::initializer_list<value_type> in_pairs) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== ITERATOR METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR iterator begin() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR iterator end() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator begin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator end() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator cbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator cend() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR local_iterator begin(size_type in_n) noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR local_iterator end(size_type in_n) noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_local_iterator cbegin(size_type in_n) const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_local_iterator cend(size_type in_n) const noexcept;
	/* ===== ITERATOR METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type get_serialized_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type max_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool empty() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type bucket_count() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type max_bucket_count() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR hasher hash_function() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR key_equal key_eq() const noexcept;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type bucket_size(size_type in_bucket) const noexcept;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type bucket(const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type bucket(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR Value& at(const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR const Value& at(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type count(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR local_iterator find_local(const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR iterator find(const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR const_iterator find(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR bool contains(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR Value& operator[](const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR Value& operator[](Key&& in_key);
	/* ===== OBSERVATION METHODS END ===== */

	MBASE_INLINE_EXPR GENERIC clear() noexcept;
	MBASE_INLINE_EXPR iterator erase(iterator in_pos);
	MBASE_INLINE_EXPR iterator erase(const_iterator in_pos);
	MBASE_INLINE_EXPR size_type erase(const Key& in_key);
	MBASE_INLINE_EXPR GENERIC swap(unordered_map& in_rhs) noexcept;
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> insert(const value_type& in_value) noexcept;
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> insert(value_type&& in_value) noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_hint, const value_type& in_value);
	MBASE_INLINE_EXPR iterator insert(const_iterator in_hint, value_type&& in_value);
	MBASE_INLINE_EXPR GENERIC insert(std::initializer_list<value_type> in_pairs);

	/* ===== NON-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR GENERIC serialize(char_stream& out_buffer) noexcept 
	{
		if(mSize)
		{
			out_buffer.put_datan<size_type>(mSize);
			out_buffer.put_datan<size_type>(mBucketCount);
			mBucket.serialize(out_buffer);
		}
	}
	/* ===== NON-MODIFIER METHODS END ===== */

	/* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
	MBASE_INLINE static mbase::unordered_map<Key, Value, Hash, KeyEqual, Allocator> deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
	{
		mbase::unordered_map<Key, Value, Hash, KeyEqual, Allocator> newMap;
		if (in_length < sizeof(size_type) * 2)
		{
			throw mbase::invalid_size();
		}

		char_stream cs(in_src, in_length);
		size_type mapSize = cs.get_datan<size_type>();
		size_type bucketCount = cs.get_datan<size_type>();
		bytes_processed += sizeof(size_type) * 2;
		bucket_type bucketList = std::move(mbase::deserialize<bucket_type>(cs.get_bufferc(), cs.buffer_length() - cs.get_pos(), bytes_processed));
		newMap.mSize = mapSize;
		newMap.mBucketCount = bucketCount;
		newMap.mBucket = std::move(bucketList);
		
		return newMap;
	}
	/* ===== NON-MEMBER FUNCTIONS END ===== */

private:
	size_type mBucketCount;
	Hash mHash;
	key_equal mKeyEqual;
	bucket_type mBucket;
	size_type mSize;

	local_iterator _is_key_duplicate(bucket_node_type& in_value, const Key& in_key) const noexcept;
	local_iterator _is_key_duplicate(bucket_node_type& in_value, value_type& in_pair) const noexcept;
	iterator _erase(const Key& in_key) noexcept;
};

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map() noexcept : mBucketCount(gUmapDefaultBucketCount), mHash(), mKeyEqual(), mBucket(mBucketCount, bucket_node_type()), mSize(0)
{
	
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(size_type in_bucket_count, const Hash& in_hash, const key_equal& in_equal, [[maybe_unused]] const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(in_equal), mBucket(mBucketCount, bucket_node_type()), mSize(0)
{
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(size_type in_bucket_count, [[maybe_unused]] const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(), mKeyEqual(), mBucket(mBucketCount), mSize(0)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(size_type in_bucket_count, const Hash& in_hash, [[maybe_unused]] const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(), mBucket(mBucketCount), mSize(0)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map([[maybe_unused]] const Allocator& in_alloc) noexcept : mBucketCount(gUmapDefaultBucketCount), mHash(), mKeyEqual(), mBucket(mBucketCount), mSize(0)
{

}
/* InputIt versions does not exist for now */
template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(const unordered_map& in_rhs) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(in_rhs.mBucket), mSize(0)
{
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(const unordered_map& in_rhs, [[maybe_unused]] const Allocator& in_alloc) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(in_rhs.mBucket), mSize(0)
{
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(unordered_map&& in_rhs) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(std::move(in_rhs.mBucket)), mSize(0)
{
	
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(unordered_map&& in_rhs, [[maybe_unused]] const Allocator& in_alloc) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(std::move(in_rhs.mBucket)), mSize(0)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Hash& in_hash, const key_equal& in_equal, [[maybe_unused]] const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(in_equal), mSize(0)
{
	const value_type* currentObj = in_pairs.begin();
	while (currentObj != in_pairs.end())
	{
		insert(*currentObj);
		currentObj++;
	}
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, [[maybe_unused]] const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mBucket(mBucketCount), mSize(0)
{
	const value_type* currentObj = in_pairs.begin();
	while (currentObj != in_pairs.end())
	{
		insert(*currentObj);
		currentObj++;
	}
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Hash& in_hash, [[maybe_unused]] const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(), mBucket(mBucketCount), mSize(0)
{
	const value_type* currentObj = in_pairs.begin();
	while (currentObj != in_pairs.end())
	{
		insert(*currentObj);
		currentObj++;
	}
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE unordered_map<Key, Value, Hash, KeyEqual, Allocator>::~unordered_map()
{
	clear();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>& unordered_map<Key, Value, Hash, KeyEqual, Allocator>::operator=(const unordered_map& in_rhs) noexcept
{
	mBucketCount = in_rhs.mBucketCount;
	mHash = in_rhs.mHash;
	mKeyEqual = in_rhs.mKeyEqual;
	mBucket = in_rhs.mBucket;
	mSize = in_rhs.mSize;

	return *this;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>& unordered_map<Key, Value, Hash, KeyEqual, Allocator>::operator=(unordered_map&& in_rhs) noexcept
{
	mBucketCount = in_rhs.mBucketCount;
	mHash = in_rhs.mHash;
	mKeyEqual = in_rhs.mKeyEqual;
	mBucket = std::move(in_rhs.mBucket);
	mSize = in_rhs.mSize;

	in_rhs.mSize = 0;

	return *this;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>& unordered_map<Key, Value, Hash, KeyEqual, Allocator>::operator=(std::initializer_list<value_type> in_pairs) noexcept
{
	clear();
	const value_type* currentObj = in_pairs.begin();
	while (currentObj != in_pairs.end())
	{
		insert(*currentObj);
		currentObj++;
	}
	return *this;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::begin() noexcept
{
	for (size_type i = 0; i < mBucketCount; i++)
	{
		local_iterator firstItem = begin(i);
		if (firstItem != end(i))
		{
			return iterator(this, i, firstItem);
		}
	}
	return iterator(this, 0, mBucket[0].begin());
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::end() noexcept
{
	if(!mSize)
	{
		return iterator(this, 0, end(0));
	}

	for (size_type i = mBucketCount - 1;;)
	{
		local_iterator lastItem = begin(i);
		if (lastItem != end(i))
		{
			return iterator(this, i, end(i));
		}

		if(i == 0)
		{
			break;
		}
		i--;
	}
	return iterator(this, 0, end(0));
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::const_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::begin() const noexcept
{
	return cbegin();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::const_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::end() const noexcept
{
	return cend();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::const_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::cbegin() const noexcept
{
	for (size_type i = 0; i < mBucketCount; i++)
	{
		const_local_iterator firstItem = cbegin(i);
		if (firstItem != cend(i))
		{
			//return const_iterator(this, i, firstItem);
			return const_iterator(const_cast<unordered_map*>(this), i, firstItem);
		}
	}
	return const_iterator(const_cast<unordered_map*>(this), 0, cbegin(0));
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::const_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::cend() const noexcept
{
	if(!mSize)
	{
		return const_iterator(const_cast<unordered_map*>(this), 0, cend(0));
	}

	for (I32 i = static_cast<I32>(mBucketCount - 1); i >= 0; i--)
	{
		const_local_iterator lastItem = cbegin(i);
		if (lastItem != cend(i))
		{
			return const_iterator(const_cast<unordered_map*>(this), i, cend(i));
		}
	}
	return const_iterator(const_cast<unordered_map*>(this), 0, cend(0));
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::local_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::begin(size_type in_n) noexcept
{
	bucket_node_type& bucketNode = mBucket[in_n];
	return bucketNode.begin();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::local_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::end(size_type in_n) noexcept
{
	bucket_node_type& bucketNode = mBucket[in_n];
	return bucketNode.end();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::const_local_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::cbegin(size_type in_n) const noexcept
{
	const bucket_node_type& bucketNode = mBucket[in_n];
	return bucketNode.cbegin();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::const_local_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::cend(size_type in_n) const noexcept
{
	const bucket_node_type& bucketNode = mBucket[in_n];
	return bucketNode.cend();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::get_serialized_size() const noexcept {
	if(!mSize)
	{
		return 0;
	}
	
	size_type totalSerializedSize = 0;

	for(size_type i = 0; i < mBucketCount; ++i)
	{
		if(mBucket[i].size())
		{
			totalSerializedSize += mBucket[i].get_serialized_size();
		}
	}

	return totalSerializedSize + (sizeof(SIZE_T) * 2);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size() const noexcept
{
	return mSize;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::max_size() const noexcept
{
	size_type result = (std::numeric_limits<difference_type>::max)();
	return result;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool unordered_map<Key, Value, Hash, KeyEqual, Allocator>::empty() const noexcept
{
	return mSize == 0;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::bucket_count() const noexcept
{
	return mBucketCount;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::max_bucket_count() const noexcept
{
	size_type result = (std::numeric_limits<difference_type>::max)();
	return result;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::hasher unordered_map<Key, Value, Hash, KeyEqual, Allocator>::hash_function() const noexcept
{
	return mHash;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::key_equal unordered_map<Key, Value, Hash, KeyEqual, Allocator>::key_eq() const noexcept
{
	return mKeyEqual;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::bucket_size(size_type in_bucket) const noexcept
{
	return mBucket[in_bucket].size();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::bucket(const Key& in_key)
{
	size_type bucketIndex = mHash(in_key) % mBucketCount;
	return bucketIndex;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::bucket(const Key& in_key) const
{
	size_type bucketIndex = mHash(in_key) % mBucketCount;
	return bucketIndex;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR Value& unordered_map<Key, Value, Hash, KeyEqual, Allocator>::at(const Key& in_key)
{
	iterator foundKey = find(in_key);
	if (foundKey == end())
	{
		throw std::out_of_range("value is not in range");
	}

	return foundKey->second;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR const Value& unordered_map<Key, Value, Hash, KeyEqual, Allocator>::at(const Key& in_key) const
{
	const_iterator foundKey = find(in_key);
	if (foundKey == cend())
	{
		throw std::out_of_range("value is not in range");
	}

	return foundKey->second;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::count(const Key& in_key) const
{
	if (find(in_key) == end())
	{
		return 0;
	}
	return 1;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::local_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::find_local(const Key& in_key)
{
	size_type bucketIndex = bucket(in_key);
	for (local_iterator It = mBucket[bucketIndex].begin(); It != mBucket[bucketIndex].end(); ++It)
	{
		if (It->first == in_key)
		{
			return It;
		}
	}
	return mBucket[bucketIndex].end();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::find(const Key& in_key)
{
	size_type bucketIndex = bucket(in_key);
	for (local_iterator It = mBucket[bucketIndex].begin(); It != mBucket[bucketIndex].end(); ++It)
	{
		if (It->first == in_key)
		{
			return iterator(this, bucketIndex, It);
		}
	}
	return end();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::const_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::find(const Key& in_key) const
{
	size_type bucketIndex = bucket(in_key);
	for (const_local_iterator It = mBucket[bucketIndex].cbegin(); It != mBucket[bucketIndex].cend(); ++It)
	{
		if (It->first == in_key)
		{
			return const_iterator(const_cast<unordered_map*>(this), bucketIndex, It);
		}
	}
	return cend();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR bool unordered_map<Key, Value, Hash, KeyEqual, Allocator>::contains(const Key& in_key) const
{
	return count(in_key);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR Value& unordered_map<Key, Value, Hash, KeyEqual, Allocator>::operator[](const Key& in_key)
{
	if(find(in_key) == end())
	{
		return insert(mbase::make_pair(in_key, mapped_type())).first->second;
	}
	return at(in_key);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR Value& unordered_map<Key, Value, Hash, KeyEqual, Allocator>::operator[](Key&& in_key)
{
	if (find(in_key) == end())
	{
		return insert(mbase::make_pair(std::move(in_key), mapped_type())).first->second;
	}
	return at(std::move(in_key));
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR GENERIC unordered_map<Key, Value, Hash, KeyEqual, Allocator>::clear() noexcept
{
	for(iterator It = begin(); It != end();)
	{
		It = erase(It);
	}
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::erase(iterator in_pos)
{
	if (in_pos == end())
	{
		return end();
	}
	return _erase(in_pos->first);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::erase(const_iterator in_pos)
{
	if (in_pos == cend())
	{
		return end();
	}
	return _erase(in_pos->first);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::size_type unordered_map<Key, Value, Hash, KeyEqual, Allocator>::erase(const Key& in_key)
{
	_erase(in_key);
	return 0;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR GENERIC unordered_map<Key, Value, Hash, KeyEqual, Allocator>::swap(unordered_map& in_rhs) noexcept {
	std::swap(mBucketCount, in_rhs.mBucketCount);
	std::swap(mHash, in_rhs.mHash);
	std::swap(mKeyEqual, in_rhs.mKeyEqual);
	std::swap(mBucket, in_rhs.mBucket);
	std::swap(mSize, in_rhs.mSize);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR mbase::pair<typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator, bool> unordered_map<Key, Value, Hash, KeyEqual, Allocator>::insert(const value_type& in_value) noexcept
{
	size_type bucketIndex = bucket(in_value.first);
	bucket_node_type& bucketNode = mBucket[bucketIndex];
	local_iterator duplicateKey = _is_key_duplicate(bucketNode, in_value);
	if (duplicateKey == bucketNode.end())
	{
		++mSize;
		bucketNode.push_back(in_value);
		//local_iterator lastItem = bucketNode.insert(bucketNode.end(), in_value);
		//return mbase::make_pair(iterator(this, bucketIndex, lastItem), true);
		return mbase::make_pair(iterator(this, bucketIndex, bucketNode.end_node()), true);

	}

	*duplicateKey = in_value;
	return mbase::make_pair(iterator(this, bucketIndex, duplicateKey), true);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR mbase::pair<typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator, bool> unordered_map<Key, Value, Hash, KeyEqual, Allocator>::insert(value_type&& in_value) noexcept
{
	size_type bucketIndex = bucket(in_value.first);
	bucket_node_type& bucketNode = mBucket[bucketIndex];
	local_iterator duplicateKey = _is_key_duplicate(bucketNode, in_value);
	if (duplicateKey == bucketNode.end())
	{
		++mSize;
		bucketNode.push_back(std::move(in_value));
		//local_iterator lastItem = bucketNode.insert(bucketNode.end(), std::move(in_value));
		//return mbase::make_pair(iterator(this, bucketIndex, lastItem), true);
		return mbase::make_pair(iterator(this, bucketIndex, bucketNode.end_node()), true);

	}

	*duplicateKey = std::move(in_value);
	return mbase::make_pair(iterator(this, bucketIndex, duplicateKey), true);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::insert([[maybe_unused]] const_iterator in_hint, const value_type& in_value) 
{
	return insert(in_value);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::insert([[maybe_unused]] const_iterator in_hint, value_type&& in_value) 
{
	return insert(std::move(in_value));
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR GENERIC unordered_map<Key, Value, Hash, KeyEqual, Allocator>::insert(std::initializer_list<value_type> in_pairs) 
{
	const value_type* iBegin = in_pairs.begin();
	for (iBegin; iBegin != in_pairs.end(); ++iBegin)
	{
		insert(*iBegin);
	}
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::local_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::_is_key_duplicate(bucket_node_type& in_value, const Key& in_key) const noexcept
{
	for (typename bucket_node_type::iterator It = in_value.begin(); It != in_value.end(); It++)
	{
		if (It->first == in_key)
		{
			return It;
		}
	}
	return in_value.end();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::local_iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::_is_key_duplicate(bucket_node_type& in_value, value_type& in_pair) const noexcept
{
	return this->_is_key_duplicate(in_value, in_pair.first);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, Value, Hash, KeyEqual, Allocator>::iterator unordered_map<Key, Value, Hash, KeyEqual, Allocator>::_erase(const Key& in_key) noexcept
{
	size_type bucketIndex = bucket(in_key);
	bucket_node_type& bucketNode = mBucket[bucketIndex];
	local_iterator duplicateKey = _is_key_duplicate(bucketNode, in_key);
	if (duplicateKey != bucketNode.end())
	{
		// Either the empty bucket returns
		// Bucket index is incorrect?
		// 
		--mSize;
		if(bucketNode.size() == 1)
		{
			bucketNode.erase(duplicateKey);
			if(bucketIndex == mBucketCount - 1)
			{
				// means we are removing the last element of the last bucket
				// or more human terms, this is the last element in our map
				return end();
			}
			else
			{
				for(bucketIndex; bucketIndex < mBucketCount; ++bucketIndex)
				{
					if(mBucket[bucketIndex].size())
					{
						return iterator(this, bucketIndex, begin(bucketIndex));
					}
				}
				return end();
			}
		}
		else
		{
			return iterator(this, bucketIndex, bucketNode.erase(duplicateKey));
		}
	}
	return end();
}

MBASE_STD_END

#endif // MBASE_UMAP_H