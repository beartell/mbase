#ifndef MBASE_UMAP_H
#define MBASE_UMAP_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <mbase/vector.h>
#include <mbase/list.h>
#include <mbase/traits.h>
#include <initializer_list>
#include <utility>

MBASE_STD_BEGIN

static const U32 gUmapDefaultBucketCount = 16;

template<typename Key, 
	typename Value, 
	typename Hash = std::hash<Key>,
	typename KeyEqual = std::equal_to<Key>,
	typename Allocator = mbase::allocator<mbase::pair<const Key, Value>>>
class unordered_map {
public:
	using key_type = Key;
	using mapped_type = Value;
	using value_type = mbase::pair<const Key, Value>;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using hasher = Hash;
	using key_equal = KeyEqual;
	using allocator_type = Allocator;
	using reference = typename value_type&;
	using const_reference = typename const value_type&;
	using pointer = typename allocator_type::pointer;
	using const_pointer = typename allocator_type::const_pointer;
	using iterator = I8;
	using const_iterator = I16;
	using local_iterator = I32;
	using const_local_iterator = I64;
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

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type max_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool empty() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type bucket_count() const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type max_bucket_count() const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR hasher hash_function() const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR key_equal key_eq() const;
 	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type bucket_size(size_type in_bucket) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type bucket(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR Value& at(const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR const Value& at(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type count(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR iterator find(const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR const_iterator find(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR bool contains(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR mbase::pair<iterator, iterator> equal_range(const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR mbase::pair<const_iterator, const_iterator> equal_range(const Key& in_key) const;
	MBASE_ND(MBASE_RESULT_IGNORE) Value& operator[](const Key& in_key);
	MBASE_ND(MBASE_RESULT_IGNORE) Value& operator[](Key&& in_key);
	/* ===== OBSERVATION METHODS END ===== */

	MBASE_INLINE_EXPR GENERIC clear() noexcept;
	MBASE_INLINE_EXPR iterator erase(iterator in_pos);
	MBASE_INLINE_EXPR iterator erase(const_iterator in_pos);
	MBASE_INLINE_EXPR iterator erase(const_iterator in_first, const_iterator in_end);
	MBASE_INLINE_EXPR size_type erase(const Key& in_key);
	MBASE_INLINE_EXPR GENERIC swap(unordered_map& in_rhs) noexcept;
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> insert(const value_type& in_value) noexcept;
	MBASE_INLINE_EXPR mbase::pair<iterator, bool> insert(value_type&& in_value) noexcept;
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
	mbase::vector<mbase::list<value_type>> mBucket;
};

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map() noexcept : mBucketCount(gUmapDefaultBucketCount), mHash(), mKeyEqual()
{
	
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(size_type in_bucket_count, const Hash& in_hash, const key_equal& in_equal, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(in_equal)
{
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(size_type in_bucket_count, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(), mKeyEqual()
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(size_type in_bucket_count, const Hash& in_hash, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual()
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(const Allocator& in_alloc) noexcept : mBucketCount(gUmapDefaultBucketCount), mHash(), mKeyEqual()
{

}
/* InputIt versions does not exist for now */
template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(const unordered_map& in_rhs) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(in_rhs.mBucket)
{
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(const unordered_map& in_rhs, const Allocator& in_alloc) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(in_rhs.mBucket)
{
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(unordered_map&& in_rhs) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(std::move(in_rhs.mBucket))
{
	
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(unordered_map&& in_rhs, const Allocator& in_alloc) noexcept : mBucketCount(in_rhs.mBucketCount), mHash(in_rhs.mHash), mKeyEqual(in_rhs.mKeyEqual), mBucket(std::move(in_rhs.mBucket))
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Hash& in_hash, const key_equal& in_equal, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual(in_equal)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count)
{

}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>::unordered_map(std::initializer_list<value_type> in_pairs, size_type in_bucket_count, const Hash& in_hash, const Allocator& in_alloc) noexcept : mBucketCount(in_bucket_count), mHash(in_hash), mKeyEqual()
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
	return *this;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>& unordered_map<Key, Value, Hash, KeyEqual, Allocator>::operator=(unordered_map&& in_rhs) noexcept
{
	return *this;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator>
MBASE_INLINE_EXPR unordered_map<Key, Value, Hash, KeyEqual, Allocator>& unordered_map<Key, Value, Hash, KeyEqual, Allocator>::operator=(std::initializer_list<value_type> in_pairs) noexcept
{
	return *this;
}


MBASE_STD_END

#endif // MBASE_UMAP_H