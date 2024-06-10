#ifndef MBASE_UMAP_H
#define MBASE_UMAP_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <mbase/vector.h>
#include <mbase/list.h>
#include <mbase/traits.h>
#include <mbase/unordered_map_iterator.h>
#include <initializer_list>
#include <utility>

MBASE_STD_BEGIN

static const SIZE_T gUmapDefaultBucketCount = 16;

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
	using bucket_node_type = mbase::list<typename value_type>;
	using bucket_type = mbase::vector<typename bucket_node_type>;
	using key_equal = KeyEqual;
	using allocator_type = Allocator;
	using reference = typename value_type&;
	using const_reference = typename const value_type&;
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
	~unordered_map();
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR unordered_map& operator=(const unordered_map& in_rhs) noexcept;
	MBASE_INLINE_EXPR unordered_map& operator=(unordered_map&& in_rhs) noexcept;
	MBASE_INLINE_EXPR unordered_map& operator=(std::initializer_list<value_type> in_pairs) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== ITERATOR METHODS BEGIN ===== */
	/*MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR iterator begin() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR iterator end() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator begin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator end() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator cbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator cend() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR local_iterator begin() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR local_iterator end() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_local_iterator begin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_local_iterator end() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_local_iterator cbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_local_iterator cend() const noexcept;*/
	/* ===== ITERATOR METHODS END ===== */

	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR local_iterator begin(size_type in_n) noexcept
	{
		bucket_node_type& bucketNode = mBucket[in_n];
		return bucketNode.begin();
	}

	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR local_iterator end(size_type in_n) noexcept
	{
		bucket_node_type& bucketNode = mBucket[in_n];
		return bucketNode.end();
	}

	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_local_iterator cbegin(size_type in_n) const noexcept
	{
		const bucket_node_type& bucketNode = mBucket[in_n];
		return bucketNode.cbegin();
	}

	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_local_iterator cend(size_type in_n) const noexcept
	{
		const bucket_node_type& bucketNode = mBucket[in_n];
		return bucketNode.cend();
	}

	iterator begin() noexcept 
	{
		for(size_type i = 0; i < mBucketCount; i++)
		{
			local_iterator firstItem = begin(i);
			if(firstItem != end(i))
			{
				return iterator(this, i, firstItem);
			}
		}
		return iterator();
	}

	iterator end() noexcept 
	{
		for(size_type i = mBucketCount - 1; i >= 0; i++)
		{
			local_iterator lastItem = begin(i);
			if(lastItem != end(i))
			{
				// end(i); ----> Is the last item
				return iterator(this, i, end(i));
			}
		}
		return iterator(this, 0, mBucket[0].end());
	}

	const_iterator cbegin() const noexcept 
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
		return const_iterator();
	}

	const_iterator cend() const noexcept
	{
		for (size_type i = mBucketCount - 1; i >= 0; i++)
		{
			const_local_iterator lastItem = cbegin(i);
			if (lastItem != cend(i))
			{
				// end(i); ----> Is the last item
				return const_iterator(const_cast<unordered_map*>(this), i, cend(i));
			}
		}
		return const_iterator(const_cast<unordered_map*>(this), 0, mBucket[0].cend());
	}


	/* ===== OBSERVATION METHODS BEGIN ===== */
	size_type get_serialized_size() const noexcept {
		return mbase::get_serialized_size(mBucket);
	}
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type size() const noexcept
	{
		return mSize;
	}
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type max_size() const noexcept
	{
		size_type result = (std::numeric_limits<difference_type>::max)();
		return result;
	}
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool empty() const noexcept
	{
		return mSize == 0;
	}
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type bucket_count() const
	{
		return mBucketCount;
	}
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type max_bucket_count() const
	{
		size_type result = (std::numeric_limits<difference_type>::max)();
		return result;
	}
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR hasher hash_function() const
	{
		return mHash;
	}
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR key_equal key_eq() const
	{
		return mKeyEqual;
	}
 	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type bucket_size(size_type in_bucket) const
	{
		return mBucket[in_bucket].size();
	}
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type bucket(const Key& in_key)
	{
		size_type bucketIndex = mHash(in_key) % mBucketCount;
		return bucketIndex;
	}
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR Value& at(const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR const Value& at(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type count(const Key& in_key) const
	{
		if(find(in_key) == end())
		{
			return 0;
		}
		return 1;
	}

	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR local_iterator find_local(const Key& in_key) 
	{
		size_type bucketIndex = bucket(in_key);
		for (local_iterator It = mBucket[bucketIndex].begin(); It != mBucket[bucketIndex].end(); It++)
		{
			if (It->first == in_key)
			{
				return It;
			}
		}
		return mBucket[bucketIndex].end();
	}

	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR iterator find(const Key& in_key)
	{
		size_type bucketIndex = bucket(in_key);
		for (local_iterator It = mBucket[bucketIndex].begin(); It != mBucket[bucketIndex].end(); It++)
		{
			if (It->first == in_key)
			{
				return iterator(this, bucketIndex, It);
			}
		}
		return end();
	}

	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR const_iterator find(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR bool contains(const Key& in_key) const
	{
		return count(in_key);
	}
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR mbase::pair<iterator, iterator> equal_range(const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR mbase::pair<const_iterator, const_iterator> equal_range(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) Value& operator[](const Key& in_key)
	{
		return at(in_key);
	}
	MBASE_ND(MBASE_RESULT_IGNORE) Value& operator[](Key&& in_key)
	{
		return at(std::move(in_key));
	}
	/* ===== OBSERVATION METHODS END ===== */

	MBASE_INLINE_EXPR GENERIC clear() noexcept
	{
		mBucket = std::move(bucket_type(mBucketCount, bucket_node_type()));
		mSize = 0;
	}

	MBASE_INLINE_EXPR iterator erase(iterator in_pos)
	{
		if(in_pos == end())
		{
			return end();
		}
		return _erase(in_pos->first);
	}
	MBASE_INLINE_EXPR iterator erase(const_iterator in_pos);
	MBASE_INLINE_EXPR iterator erase(const_iterator in_first, const_iterator in_end);
	MBASE_INLINE_EXPR size_type erase(const Key& in_key)
	{
		_erase(in_key);
		return 0;
	}
	MBASE_INLINE_EXPR GENERIC swap(unordered_map& in_rhs) noexcept;
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> insert(const value_type& in_value) noexcept
	{
		size_type bucketIndex = bucket(in_value.first);
		bucket_node_type& bucketNode = mBucket[bucketIndex];
		local_iterator duplicateKey = _is_key_duplicate(bucketNode, in_value);
		if (duplicateKey == bucketNode.end())
		{
			++mSize;
			bucketNode.push_back(in_value);
			return mbase::make_pair(iterator(), true);
		}

		*duplicateKey = in_value;
		return mbase::make_pair(iterator(), true);
	}

	MBASE_INLINE_EXPR mbase::pair<iterator, bool> insert(value_type&& in_value) noexcept
	{
		size_type bucketIndex = bucket(in_value.first);
		bucket_node_type& bucketNode = mBucket[bucketIndex];
		local_iterator duplicateKey = _is_key_duplicate(bucketNode, in_value);
		if(duplicateKey == bucketNode.end())
		{
			++mSize;
			bucketNode.push_back(std::move(in_value));
			return mbase::make_pair(iterator(), true);
		}
		
		*duplicateKey = std::move(in_value);
		return mbase::make_pair(iterator(), true);
	}

	template<typename P, typename = std::enable_if_t<std::is_constructible_v<value_type, P&&>>>
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> insert(P&& in_value) noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_hint, const value_type& in_value);
	MBASE_INLINE_EXPR iterator insert(const_iterator in_hint, value_type&& in_value);
	template<typename P, typename = std::enable_if_t<std::is_constructible_v<value_type, P&&>>>
	MBASE_INLINE_EXPR iterator insert(const_iterator in_hint, P&& in_value);
	/* INPUT IT WILL BE SET TOO */
	MBASE_INLINE_EXPR GENERIC insert(std::initializer_list<value_type> in_pairs);
	template<typename ... Args>
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> emplace(Args&&... in_args);
	template<typename ... Args>
	MBASE_INLINE_EXPR iterator emplace_hint(const_iterator in_hint, Args&&... in_args);
	template<typename ... Args>
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> try_emplace(const Key& in_key, Args&&... in_args);
	template<typename ... Args>
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> try_emplace(Key&& in_key, Args&&... in_args);
	template<typename ... Args>
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> try_emplace(const_iterator in_hint, const Key& in_key, Args&&... in_args);
	template<typename ... Args>
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> try_emplace(const_iterator in_hint, Key&& in_key, Args&&... in_args);

private:
	size_type mBucketCount;
	Hash mHash;
	key_equal mKeyEqual;
	bucket_type mBucket;
	size_type mSize;

	local_iterator _is_key_duplicate(bucket_node_type& in_value, const Key& in_key) const noexcept
	{
		typename bucket_node_type::iterator It = in_value.begin();

		for (It; It != in_value.end(); It++)
		{
			if (It->first == in_key)
			{
				return It;
			}
		}
		return in_value.end();
	}

	local_iterator _is_key_duplicate(bucket_node_type& in_value, value_type& in_pair) const noexcept
	{
		return _is_key_duplicate(in_value, in_pair.first);
	}

	iterator _erase(const Key& in_key) noexcept 
	{
		size_type bucketIndex = bucket(in_key);
		bucket_node_type& bucketNode = mBucket[bucketIndex];
		local_iterator duplicateKey = _is_key_duplicate(bucketNode, in_key);
		if (duplicateKey != bucketNode.end())
		{
			iterator(this, bucketIndex, bucketNode.erase(duplicateKey));
		}
		return iterator();
	}
};

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map() noexcept : mBucketCount(gUmapDefaultBucketCount), mHash(), mKeyEqual(), mBucket(mBucketCount, bucket_node_type()), mSize(0)
{
	
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(size_type in_bucket_count, const Hash& in_hash, const key_equal& in_equal, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(in_equal), mBucket(mBucketCount, bucket_node_type()), mSize(0)
{
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(size_type in_bucket_count, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(), mKeyEqual(), mBucket(mBucketCount), mSize(0)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(size_type in_bucket_count, const Hash& in_hash, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(), mBucket(mBucketCount), mSize(0)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(const Allocator& in_alloc) noexcept : mBucketCount(gUmapDefaultBucketCount), mHash(), mKeyEqual(), mBucket(mBucketCount), mSize(0)
{

}
/* InputIt versions does not exist for now */
template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(const unordered_map& in_rhs) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(in_rhs.mBucket), mSize(0)
{
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(const unordered_map& in_rhs, const Allocator& in_alloc) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(in_rhs.mBucket), mSize(0)
{
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(unordered_map&& in_rhs) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(std::move(in_rhs.mBucket)), mSize(0)
{
	
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(unordered_map&& in_rhs, const Allocator& in_alloc) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(std::move(in_rhs.mBucket)), mSize(0)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Hash& in_hash, const key_equal& in_equal, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(in_equal), mSize(0)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mBucket(mBucketCount), mSize(0)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Hash& in_hash, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(), mBucket(mBucketCount), mSize(0)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, Value, Hash, KeyEqual, Allocator>::~unordered_map()
{
	//clear();
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
	return *this;
}


MBASE_STD_END

#endif // MBASE_UMAP_H