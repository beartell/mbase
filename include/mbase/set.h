#ifndef MBASE_SET_H
#define MBASE_SET_H

#include <mbase/common.h>
#include <mbase/allocator.h> // mbase::allocator
#include <mbase/node_type.h> // mbase::avl_node
#include <mbase/binary_iterator.h> // mbase::bst_iterator, mbase::const_bst_iterator, mbase::reverse_bst_iterator, mbase::const_reverse_bst_iterator
#include <mbase/traits.h> // mbase::serialize_helper
#include <mbase/safe_buffer.h> // mbase::safe_buffer
#include <functional> // std::less

MBASE_STD_BEGIN

static const SIZE_T gSerializedSetBlockLength = 8;

template<
	typename Key, 
	typename Compare = std::less<Key>, 
	typename Allocator = mbase::allocator<Key>
>
class set {
public:
	using key_type = Key;
	using value_type = Key;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using key_compare = Compare;
	using value_compare = Compare;
	using _node_type = avl_node<Key, key_compare>;
	using allocator_type = Allocator;
	using reference = Key&;
	using const_reference = const Key&;
	using pointer = Key*;
	using const_pointer = const Key*;
	using iterator = bst_iterator<_node_type, value_type>;
	using const_iterator = const_bst_iterator<_node_type, value_type>;
	using reverse_iterator = reverse_bst_iterator<_node_type, value_type>;
	using const_reverse_iterator = const_reverse_bst_iterator<_node_type, value_type>;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE set();
	MBASE_INLINE MBASE_EXPLICIT set(const Compare& in_comp, const Allocator& in_alloc = Allocator());
	MBASE_INLINE set(const set& in_rhs);
	MBASE_INLINE set(const set& in_rhs, const Allocator& in_alloc);
	MBASE_INLINE set(set&& in_rhs);
	MBASE_INLINE set(set&& in_rhs, const Allocator& in_alloc);
	MBASE_INLINE set(std::initializer_list<value_type> in_list, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<Key, typename std::iterator_traits<InputIt>::value_type>>>
	set(InputIt in_begin, InputIt in_end, const Allocator& in_alloc = Allocator()) : mRootNode(nullptr), mSize(0) {
		for (in_begin; in_begin != in_end; in_begin++)
		{
			insert(*in_begin);
		}
	}
	MBASE_INLINE ~set();
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	MBASE_INLINE set& operator=(const set& in_rhs);
	MBASE_INLINE set& operator=(set&& in_rhs) noexcept;
	MBASE_INLINE set& operator=(std::initializer_list<value_type> in_list);
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== ITERATOR METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE iterator begin() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator begin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator cbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE iterator end() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator end() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator cend() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE reverse_iterator rbegin() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reverse_iterator rbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reverse_iterator crbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE reverse_iterator rend() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reverse_iterator rend() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reverse_iterator crend() const noexcept;
	/* ===== ITERATOR METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE size_type get_serialized_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE allocator_type get_allocator() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE key_compare key_comp() const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE value_compare value_comp() const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool empty() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type max_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type count(const Key& in_key) const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE iterator find(const Key& in_key);
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const_iterator find(const Key& in_key) const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool contains(const Key& in_key) const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE std::pair<iterator, iterator> equal_range(const Key& in_key);
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE std::pair<const_iterator, const_iterator> equal_range(const Key& in_key) const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE iterator lower_bound(const Key& in_key);
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const_iterator lower_bound(const Key& in_key) const;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE iterator upper_bound(const Key& in_key);
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const_iterator upper_bound(const Key& in_key) const;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE std::pair<iterator, bool> insert(const value_type& in_value);
	MBASE_INLINE std::pair<iterator, bool> insert(value_type&& in_value);
	MBASE_INLINE iterator insert(const_iterator in_pos, const value_type& in_value);
	MBASE_INLINE iterator insert(const_iterator in_pos, value_type&& in_value);
	MBASE_INLINE GENERIC insert(std::initializer_list<value_type> in_list);
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<value_type, typename std::iterator_traits<InputIt>::value_type>>>
	MBASE_INLINE GENERIC insert(InputIt in_begin, InputIt in_end) {
		for (in_begin; in_begin != in_end; in_begin++)
		{
			insert(*in_begin);
		}
	}
	template<typename ... Args>
	MBASE_INLINE std::pair<iterator, bool> emplace(Args&& ... in_args) {
		return insert(std::move(std::forward<Args>(in_args)...));
	}
	template<typename ... Args>
	MBASE_INLINE std::pair<iterator, bool> emplace_hint(const_iterator in_hint, Args&& ... in_args) {
		return insert(in_hint, std::move(std::forward<Args>(in_args)...));
	}
	MBASE_INLINE iterator erase(const_iterator in_pos);
	MBASE_INLINE iterator erase(iterator in_pos);
	MBASE_INLINE iterator erase(iterator in_first, iterator in_last);
	MBASE_INLINE iterator erase(const_iterator in_first, const_iterator in_last);
	MBASE_INLINE size_type erase(const value_type& in_key);
	MBASE_INLINE GENERIC clear() noexcept;
	MBASE_INLINE GENERIC swap(set& in_rhs) noexcept;
	template<typename Compare2>
	MBASE_INLINE GENERIC merge(const set<Key, Compare2, Allocator>& in_src) {
		for(typename set<Key, Compare2, Allocator>::iterator It = in_src.begin(); It != in_src.end(); It++)
		{
			this->insert(*It);
		}
		in_src.clear();
	}
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== NON-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE GENERIC serialize(char_stream& out_buffer) const noexcept;
	/* ===== NON-MODIFIER METHODS END ===== */

	/* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
	MBASE_INLINE static mbase::set<Key, Compare, Allocator> deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed);
	/* ===== NON-MEMBER FUNCTIONS END ===== */

private:
	_node_type* mRootNode;
	size_type mSize;
};

/* ----- SET IMPLEMENTATION ----- */
template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>::set() : mRootNode(nullptr), mSize(0)
{
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>::set(const Compare& in_comp, const Allocator& in_alloc) : mRootNode(nullptr), mSize(0)
{
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>::set(const set& in_rhs) : mRootNode(nullptr), mSize(0) {
	iterator itBegin = in_rhs.begin();
	for (itBegin; itBegin != in_rhs.end(); itBegin++)
	{
		insert(*itBegin);
	}
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>::set(const set& in_rhs, const Allocator& in_alloc) : mRootNode(nullptr), mSize(0) {
	iterator itBegin = in_rhs.begin();
	for (itBegin; itBegin != in_rhs.end(); itBegin++)
	{
		insert(*itBegin);
	}
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>::set(set&& in_rhs) : mRootNode(in_rhs.mRootNode), mSize(in_rhs.mSize) {
	in_rhs.mRootNode = nullptr;
	in_rhs.mSize = 0;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>::set(set&& in_rhs, const Allocator& in_alloc) {
	mRootNode = in_rhs.mRootNode;
	mSize = in_rhs.mSize;

	in_rhs.mRootNode = nullptr;
	in_rhs.mSize = 0;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>::set(std::initializer_list<value_type> in_list, const Compare& in_comp, const Allocator& in_alloc) : mRootNode(nullptr), mSize(0) {
	insert(in_list.begin(), in_list.end());
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>::~set()
{
	clear();
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>& set<Key, Compare, Allocator>::operator=(const set& in_rhs) {
	clear();
	iterator itBegin = in_rhs.begin();
	for (itBegin; itBegin != in_rhs.end(); itBegin++)
	{
		insert(*itBegin);
	}
	return *this;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>& set<Key, Compare, Allocator>::operator=(set&& in_rhs) noexcept {
	clear();
	set rhs = std::move(in_rhs);
	iterator itBegin = in_rhs.begin();
	for (itBegin; itBegin != in_rhs.end(); itBegin++)
	{
		insert(*itBegin);
	}
	return *this;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE set<Key, Compare, Allocator>& set<Key, Compare, Allocator>::operator=(std::initializer_list<value_type> in_list) {
	clear();
	insert(in_list.begin(), in_list.end());
	return *this;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::begin() noexcept {
	return iterator(mRootNode);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::begin() const noexcept {
	return const_iterator(mRootNode);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::cbegin() const noexcept {
	return const_iterator(mRootNode);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::end() noexcept {
	return iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::end() const noexcept {
	return const_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::cend() const noexcept {
	return const_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::reverse_iterator set<Key, Compare, Allocator>::rbegin() noexcept {
	return reverse_iterator(mRootNode);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::const_reverse_iterator set<Key, Compare, Allocator>::rbegin() const noexcept {
	return const_reverse_iterator(mRootNode);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::const_reverse_iterator set<Key, Compare, Allocator>::crbegin() const noexcept {
	return const_reverse_iterator(mRootNode);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::reverse_iterator set<Key, Compare, Allocator>::rend() noexcept {
	return reverse_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::const_reverse_iterator set<Key, Compare, Allocator>::rend() const noexcept {
	return const_reverse_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename set<Key, Compare, Allocator>::const_reverse_iterator set<Key, Compare, Allocator>::crend() const noexcept {
	return const_reverse_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::size_type set<Key, Compare, Allocator>::get_serialized_size() const noexcept
{
	serialize_helper<value_type> sh;
	size_type totalSize = 0;
	for (const_iterator It = cbegin(); It != cend(); ++It)
	{
		bool isPrimitive = std::is_integral_v<value_type>;
		if (isPrimitive)
		{
			totalSize += mbase::get_serialized_size(*It);
		}
		else
		{
			totalSize += mbase::get_serialized_size(*It) + gSerializedSetBlockLength;
		}
	}
	return totalSize;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::allocator_type set<Key, Compare, Allocator>::get_allocator() const noexcept {
	return allocator_type();
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::key_compare set<Key, Compare, Allocator>::key_comp() const
{
	key_compare kc;
	return kc;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::value_compare set<Key, Compare, Allocator>::value_comp() const
{
	value_compare vc;
	return vc;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool set<Key, Compare, Allocator>::empty() const noexcept
{
	return mSize == 0;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::size_type set<Key, Compare, Allocator>::size() const noexcept
{
	return mSize;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::size_type set<Key, Compare, Allocator>::max_size() const noexcept
{
	size_type result = (std::numeric_limits<difference_type>::max)();
	return result;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::size_type set<Key, Compare, Allocator>::count(const Key& in_key) const {
	return static_cast<size_type>(this->contains(in_key));
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::find(const Key& in_key) {
	iterator itBegin(mRootNode);

	for (itBegin; itBegin != end(); itBegin++)
	{
		if (*itBegin == in_key)
		{
			return iterator(itBegin);
		}
	}

	return end();
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::find(const Key& in_key) const {
	const_iterator itBegin = const_iterator(mRootNode);
	for (itBegin; itBegin != cend(); itBegin++)
	{
		if (*itBegin == in_key)
		{
			return const_iterator(itBegin);
		}
	}
	return cend();
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool set<Key, Compare, Allocator>::contains(const Key& in_key) const {
	if (this->find(in_key) != cend())
	{
		return true;
	}
	return false;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE std::pair<typename set<Key, Compare, Allocator>::iterator, typename set<Key, Compare, Allocator>::iterator> set<Key, Compare, Allocator>::equal_range(const Key& in_key) {
	return std::make_pair(this->lower_bound(in_key), this->upper_bound(in_key));
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE std::pair<typename set<Key, Compare, Allocator>::const_iterator, typename set<Key, Compare, Allocator>::const_iterator> set<Key, Compare, Allocator>::equal_range(const Key& in_key) const {
	return std::make_pair(this->lower_bound(in_key), this->upper_bound(in_key));
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::lower_bound(const Key& in_key) {
	iterator itBegin = begin();
	for (itBegin; itBegin != end(); itBegin++)
	{
		if (*itBegin >= in_key)
		{
			return iterator(itBegin);
		}
	}

	return end();
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::lower_bound(const Key& in_key) const {
	const_iterator itBegin = begin();
	for (itBegin; itBegin != end(); itBegin++)
	{
		if (*itBegin >= in_key)
		{
			return const_iterator(itBegin);
		}
	}

	return cend();
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::upper_bound(const Key& in_key) {
	iterator itBegin = begin();
	for (itBegin; itBegin != end(); itBegin++)
	{
		if (*itBegin > in_key)
		{
			return iterator(itBegin);
		}
	}

	return end();
}

template<typename Key, typename Compare, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::upper_bound(const Key& in_key) const {
	const_iterator itBegin = begin();
	for (itBegin; itBegin != end(); itBegin++)
	{
		if (*itBegin > in_key)
		{
			return const_iterator(itBegin);
		}
	}

	return cend();
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE std::pair<typename set<Key, Compare, Allocator>::iterator, bool> set<Key, Compare, Allocator>::insert(const value_type& in_value) {
	std::pair<iterator, bool> insertResult = _node_type::insert_node<iterator>(mRootNode, in_value, mRootNode);
	if (insertResult.second)
	{
		++mSize;
	}

	return insertResult;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE std::pair<typename set<Key, Compare, Allocator>::iterator, bool> set<Key, Compare, Allocator>::insert(value_type&& in_value) {
	std::pair<iterator, bool> insertResult = _node_type::insert_node<iterator>(mRootNode, std::move(in_value), mRootNode);
	if (insertResult.second)
	{
		++mSize;
	}

	return insertResult;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::insert(const_iterator in_pos, const value_type& in_value) {
	// SELF-NOTE: IMPLEMENTATION IS INCOMPLETE, MAKE SURE TO COME BACK HERE
	// IT DOES NORMAL INSERTION AT THIS POINT
	return insert(in_value).first;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::insert(const_iterator in_pos, value_type&& in_value) {
	// SELF-NOTE: IMPLEMENTATION IS INCOMPLETE, MAKE SURE TO COME BACK HERE
	// IT DOES NORMAL INSERTION AT THIS POINT
	return insert(std::move(in_value)).first;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE GENERIC set<Key, Compare, Allocator>::insert(std::initializer_list<value_type> in_list) {
	insert(in_list.begin(), in_list.end());
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::erase(const_iterator in_pos) {
	if (!mSize)
	{
		return end();
	}

	if (in_pos == cend()) {
		return end();
	}

	iterator itBegin(in_pos._ptr, false);
	++itBegin;
	_node_type* nt = in_pos._ptr;
	if (mRootNode == nt)
	{
		mRootNode->remove_node(nt, *in_pos);
		mRootNode = nt;
	}
	else
	{
		mRootNode->remove_node(nt, *in_pos);
	}
	_node_type::balance(mRootNode); // FOR NOW, WE ARE BALANCING THE ENTIRE TREE ON EVERY DELETION, WE WILL GO BACK 

	--mSize;
	return itBegin;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::erase(iterator in_pos) {
	const_iterator cit = in_pos;
	return erase(cit);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::erase(iterator in_first, iterator in_last) {
	for (in_first; in_first != in_last; in_first++)
	{
		in_first = erase(in_first);
	}
	return in_first;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::erase(const_iterator in_first, const_iterator in_last)
{
	for (in_first; in_first != in_last; in_first++)
	{
		in_first = erase(in_first);
	}
	return in_first;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE typename set<Key, Compare, Allocator>::size_type set<Key, Compare, Allocator>::erase(const value_type& in_key)
{
	const_iterator ci = find(in_key);
	erase(ci);
	return 0;
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE GENERIC set<Key, Compare, Allocator>::clear() noexcept {
	for (iterator It = begin(); It != end();)
	{
		It = erase(It);
	}
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE GENERIC set<Key, Compare, Allocator>::swap(set& in_rhs) noexcept
{
	std::swap(mRootNode, in_rhs.mRootNode);
	std::swap(mSize, in_rhs.mSize);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE GENERIC set<Key, Compare, Allocator>::serialize(char_stream& out_buffer) const noexcept 
{
	if (mSize)
	{
		size_type serializedSize = this->get_serialized_size();
		if (out_buffer.buffer_length() < serializedSize)
		{
			// BUFFER LENGTH IS NOT ENOUGH TO HOLD SERIALIZED DATA
			return;
		}

		for (iterator It = begin(); It != end(); It++)
		{
			bool isPrimitive = std::is_integral_v<value_type>;
			if (!isPrimitive)
			{
				size_type blockLength = mbase::get_serialized_size(*It);
				out_buffer.put_datan<size_type>(blockLength);
			}
			//out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(&blockLength), sizeof(I32));
			mbase::serialize(*It, out_buffer);
		}
	}
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE mbase::set<Key, Compare, Allocator> set<Key, Compare, Allocator>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length, SIZE_T& bytes_processed)
{
	mbase::set<Key, Compare, Allocator> deserializedContainer;
	if (in_length)
	{
		char_stream inBuffer(in_src, in_length);

		inBuffer.set_cursor_end();
		inBuffer.advance();

		IBYTEBUFFER eofBuffer = inBuffer.get_bufferc();
		inBuffer.set_cursor_front();

		while (inBuffer.get_bufferc() < eofBuffer)
		{
			I32 blockLength = inBuffer.get_datan<I32>();
			bytes_processed += blockLength;
			IBYTEBUFFER blockData = inBuffer.get_bufferc();
			deserializedContainer.insert(std::move(mbase::deserialize<value_type>(blockData, blockLength, bytes_processed)));
			inBuffer.advance(blockLength);
		}
	}

	return deserializedContainer;
}

MBASE_STD_END

#endif // !MBASE_SET_H
