#ifndef MBASE_SET_H
#define MBASE_SET_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <mbase/node_type.h>
#include <mbase/binary_iterator.h>
#include <mbase/container_serializer_helper.h>
#include <mbase/safe_buffer.h>
#include <functional>

MBASE_STD_BEGIN

template<
	typename Key, 
	typename Compare = std::less<Key>, 
	typename Allocator = mbase::allocator<Key>
>
class set {
public:
	using key_type = typename Key;
	using value_type = typename Key;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using key_compare = typename Compare;
	using value_compare = typename Compare;
	using _node_type = avl_node<Key, typename key_compare>;
	using allocator_type = typename Allocator;
	using reference = Key&;
	using const_reference = const Key&;
	using pointer = Key*;
	using const_pointer = const Key*;
	using iterator = typename bst_iterator<_node_type, value_type>;
	using const_iterator = typename const_bst_iterator<_node_type, value_type>;
	using reverse_iterator = typename reverse_bst_iterator<_node_type, value_type>;
	using const_reverse_iterator = typename const_reverse_bst_iterator<_node_type, value_type>;
	//using node_type = typename exposed_set_node<Key, Allocator>;
	//using insert_return_type = I32;

	set();
	MBASE_EXPLICIT set(const Compare& in_comp, const Allocator& in_alloc = Allocator());
	set(const set& in_rhs);
	set(const set& in_rhs, const Allocator& in_alloc);
	set(set&& in_rhs);
	set(set&& in_rhs, const Allocator& in_alloc);
	set(std::initializer_list<value_type> in_list, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<Key, typename std::iterator_traits<InputIt>::value_type>>>
	set(InputIt in_begin, InputIt in_end, const Allocator& in_alloc = Allocator()) : rootNode(nullptr), mSize(0) {
		for (in_begin; in_begin != in_end; in_begin++)
		{
			insert(*in_begin);
		}
	}
	~set();

	set& operator=(const set& in_rhs);
	set& operator=(set&& in_rhs) noexcept;
	set& operator=(std::initializer_list<value_type> in_list);

	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;
	reverse_iterator rbegin() noexcept;
	const_reverse_iterator rbegin() const noexcept;
	const_reverse_iterator crbegin() const noexcept;
	reverse_iterator rend() noexcept;
	const_reverse_iterator rend() const noexcept;
	const_reverse_iterator crend() const noexcept;

	allocator_type get_allocator() const noexcept;
	key_compare key_comp() const;
	value_compare value_comp() const;
	bool empty() const noexcept;
	size_type size() const noexcept;
	size_type max_size() const noexcept;
	size_type count(const Key& in_key) const;
	iterator find(const Key& in_key);
	const_iterator find(const Key& in_key) const;
	bool contains(const Key& in_key) const;
	std::pair<iterator, iterator> equal_range(const Key& in_key);
	std::pair<const_iterator, const_iterator> equal_range(const Key& in_key) const;
	iterator lower_bound(const Key& in_key);
	const_iterator lower_bound(const Key& in_key) const;
	iterator upper_bound(const Key& in_key);
	const_iterator upper_bound(const Key& in_key) const;
	
	std::pair<iterator, bool> insert(const value_type& in_value);
	std::pair<iterator, bool> insert(value_type&& in_value);
	iterator insert(const_iterator in_pos, const value_type& in_value);
	iterator insert(const_iterator in_pos, value_type&& in_value);
	GENERIC insert(std::initializer_list<value_type> in_list);
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<value_type, typename std::iterator_traits<InputIt>::value_type>>>
	GENERIC insert(InputIt in_begin, InputIt in_end) {
		for (in_begin; in_begin != in_end; in_begin++)
		{
			insert(*in_begin);
		}
	}
	//insert_return_type insert(node_type&& in_node);
	template<typename ... Args>
	std::pair<iterator, bool> emplace(Args&& ... in_args) {
		return insert(std::move(std::forward<Args>(in_args)...));
	}
	template<typename ... Args>
	std::pair<iterator, bool> emplace_hint(const_iterator in_hint, Args&& ... in_args) {
		return insert(in_hint, std::move(std::forward<Args>(in_args)...));
	}

	iterator erase(const_iterator in_pos);
	iterator erase(iterator in_pos);
	iterator erase(iterator in_first, iterator in_last);
	iterator erase(const_iterator in_first, const_iterator in_last);
	size_type erase(const value_type& in_key);
	GENERIC clear() noexcept;
	GENERIC swap(set& in_rhs) noexcept;
	template<typename Compare2>
	GENERIC merge(const set<Key, Compare2, Allocator>& in_src) {
		for(typename set<Key, Compare2, Allocator>::iterator It = in_src.begin(); It != in_src.end(); It++)
		{
			this->insert(*It);
		}
		in_src.clear();
	}

	MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept;
	MBASE_INLINE static mbase::set<Key, Compare, Allocator> deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept;

private:
	_node_type* rootNode;
	size_type mSize;
};

/* ----- SET IMPLEMENTATION ----- */
template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::set() : rootNode(nullptr), mSize(0) 
{
}

template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::set(const Compare& in_comp, const Allocator& in_alloc) : rootNode(nullptr), mSize(0) 
{
}

template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::set(const set& in_rhs) : rootNode(nullptr), mSize(0) {
	iterator itBegin = in_rhs.begin();
	for (itBegin; itBegin != in_rhs.end(); itBegin++)
	{
		insert(*itBegin);
	}
}

template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::set(const set& in_rhs, const Allocator& in_alloc) : rootNode(nullptr), mSize(0) {
	iterator itBegin = in_rhs.begin();
	for (itBegin; itBegin != in_rhs.end(); itBegin++)
	{
		insert(*itBegin);
	}
}

template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::set(set&& in_rhs) {
	rootNode = in_rhs.rootNode;
	mSize = in_rhs.mSize;

	in_rhs.rootNode = nullptr;
	in_rhs.mSize = 0;
}

template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::set(set&& in_rhs, const Allocator& in_alloc) {
	rootNode = in_rhs.rootNode;
	mSize = in_rhs.mSize;

	in_rhs.rootNode = nullptr;
	in_rhs.mSize = 0;
}

template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::set(std::initializer_list<value_type> in_list, const Compare& in_comp, const Allocator& in_alloc) : rootNode(nullptr), mSize(0) {
	insert(in_list.begin(), in_list.end());
}

template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::~set()
{
	clear();
}

template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>& set<Key, Compare, Allocator>::operator=(const set& in_rhs) {
	clear();
	iterator itBegin = in_rhs.begin();
	for (itBegin; itBegin != in_rhs.end(); itBegin++)
	{
		insert(*itBegin);
	}
	return *this;
}

template<typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>& set<Key, Compare, Allocator>::operator=(set&& in_rhs) noexcept {
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
set<Key, Compare, Allocator>& set<Key, Compare, Allocator>::operator=(std::initializer_list<value_type> in_list) {
	clear();
	insert(in_list.begin(), in_list.end());
	return *this;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::begin() noexcept {
	return iterator(rootNode);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::begin() const noexcept {
	return const_iterator(rootNode);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::cbegin() const noexcept {
	return const_iterator(rootNode);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::end() noexcept {
	return iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::end() const noexcept {
	return const_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::cend() const noexcept {
	return const_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::reverse_iterator set<Key, Compare, Allocator>::rbegin() noexcept {
	return reverse_iterator(rootNode);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::const_reverse_iterator set<Key, Compare, Allocator>::rbegin() const noexcept {
	return const_reverse_iterator(rootNode);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::const_reverse_iterator set<Key, Compare, Allocator>::crbegin() const noexcept {
	return const_reverse_iterator(rootNode);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::reverse_iterator set<Key, Compare, Allocator>::rend() noexcept {
	return reverse_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::const_reverse_iterator set<Key, Compare, Allocator>::rend() const noexcept {
	return const_reverse_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::const_reverse_iterator set<Key, Compare, Allocator>::crend() const noexcept {
	return const_reverse_iterator(nullptr);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::allocator_type set<Key, Compare, Allocator>::get_allocator() const noexcept {
	return allocator_type();
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::key_compare set<Key, Compare, Allocator>::key_comp() const
{
	key_compare kc;
	return kc;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::value_compare set<Key, Compare, Allocator>::value_comp() const
{
	value_compare vc;
	return vc;
}

template<typename Key, typename Compare, typename Allocator>
bool set<Key, Compare, Allocator>::empty() const noexcept
{
	return mSize == 0;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::size_type set<Key, Compare, Allocator>::size() const noexcept
{
	return mSize;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::size_type set<Key, Compare, Allocator>::max_size() const noexcept
{
	size_type result = (std::numeric_limits<difference_type>::max)();
	return result;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::size_type set<Key, Compare, Allocator>::count(const Key& in_key) const {
	return static_cast<size_type>(this->contains(in_key));
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::find(const Key& in_key) {
	iterator itBegin(rootNode);

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
typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::find(const Key& in_key) const {
	const_iterator itBegin = const_iterator(rootNode);
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
bool set<Key, Compare, Allocator>::contains(const Key& in_key) const {
	if (this->find(in_key) != cend())
	{
		return true;
	}
	return false;
}

template<typename Key, typename Compare, typename Allocator>
std::pair<typename set<Key, Compare, Allocator>::iterator, typename set<Key, Compare, Allocator>::iterator> set<Key, Compare, Allocator>::equal_range(const Key& in_key) {
	return std::make_pair(this->lower_bound(in_key), this->upper_bound(in_key));
}

template<typename Key, typename Compare, typename Allocator>
std::pair<typename set<Key, Compare, Allocator>::const_iterator, typename set<Key, Compare, Allocator>::const_iterator> set<Key, Compare, Allocator>::equal_range(const Key& in_key) const {
	return std::make_pair(this->lower_bound(in_key), this->upper_bound(in_key));
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::lower_bound(const Key& in_key) {
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
typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::lower_bound(const Key& in_key) const {
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
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::upper_bound(const Key& in_key) {
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
typename set<Key, Compare, Allocator>::const_iterator set<Key, Compare, Allocator>::upper_bound(const Key& in_key) const {
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
std::pair<typename set<Key, Compare, Allocator>::iterator, bool> set<Key, Compare, Allocator>::insert(const value_type& in_value) {
	std::pair<iterator, bool> insertResult = _node_type::insert_node<iterator>(rootNode, in_value, rootNode);
	if (insertResult.second)
	{
		++mSize;
	}

	return insertResult;
}

template<typename Key, typename Compare, typename Allocator>
std::pair<typename set<Key, Compare, Allocator>::iterator, bool> set<Key, Compare, Allocator>::insert(value_type&& in_value) {
	std::pair<iterator, bool> insertResult = _node_type::insert_node<iterator>(rootNode, std::move(in_value), rootNode);
	if (insertResult.second)
	{
		++mSize;
	}

	return insertResult;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::insert(const_iterator in_pos, const value_type& in_value) {
	// SELF-NOTE: IMPLEMENTATION IS INCOMPLETE, MAKE SURE TO COME BACK HERE
	// IT DOES NORMAL INSERTION AT THIS POINT
	return insert(in_value).first;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::insert(const_iterator in_pos, value_type&& in_value) {
	// SELF-NOTE: IMPLEMENTATION IS INCOMPLETE, MAKE SURE TO COME BACK HERE
	// IT DOES NORMAL INSERTION AT THIS POINT
	return insert(std::move(in_value)).first;
}

template<typename Key, typename Compare, typename Allocator>
GENERIC set<Key, Compare, Allocator>::insert(std::initializer_list<value_type> in_list) {
	insert(in_list.begin(), in_list.end());
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::erase(const_iterator in_pos) {
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
	if (rootNode == nt)
	{
		rootNode->remove_node(nt, *in_pos);
		rootNode = nt;
	}
	else
	{
		rootNode->remove_node(nt, *in_pos);
	}
	_node_type::balance(rootNode); // FOR NOW, WE ARE BALANCING THE ENTIRE TREE ON EVERY DELETION, WE WILL GO BACK 

	--mSize;
	return itBegin;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::erase(iterator in_pos) {
	const_iterator cit = in_pos;
	return erase(cit);
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::erase(iterator in_first, iterator in_last) {
	for (in_first; in_first != in_last; in_first++)
	{
		in_first = erase(in_first);
	}
	return in_first;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::iterator set<Key, Compare, Allocator>::erase(const_iterator in_first, const_iterator in_last)
{
	for (in_first; in_first != in_last; in_first++)
	{
		in_first = erase(in_first);
	}
	return in_first;
}

template<typename Key, typename Compare, typename Allocator>
typename set<Key, Compare, Allocator>::size_type set<Key, Compare, Allocator>::erase(const value_type& in_key)
{
	const_iterator ci = find(in_key);
	erase(ci);
	return 0;
}

template<typename Key, typename Compare, typename Allocator>
GENERIC set<Key, Compare, Allocator>::clear() noexcept {
	for (iterator It = begin(); It != end();)
	{
		It = erase(It);
	}
}

template<typename Key, typename Compare, typename Allocator>
GENERIC set<Key, Compare, Allocator>::swap(set& in_rhs) noexcept 
{
	std::swap(rootNode, in_rhs.rootNode);
	std::swap(mSize, in_rhs.mSize);
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE GENERIC set<Key, Compare, Allocator>::serialize(safe_buffer& out_buffer) noexcept {
	if (mSize)
	{
		mbase::vector<safe_buffer> totalBuffer;

		serialize_helper<Key> sl;
		safe_buffer tmpSafeBuffer;
		size_type totalLength = 0;

		for (iterator It = begin(); It != end(); It++)
		{
			sl.value = &It.get()->data;
			sl.serialize(tmpSafeBuffer);
			if (tmpSafeBuffer.bfLength)
			{
				totalLength += tmpSafeBuffer.bfLength;
				totalBuffer.push_back(std::move(tmpSafeBuffer));
			}
		}

		if (totalBuffer.size())
		{
			SIZE_T totalBufferLength = totalLength + (totalBuffer.size() * sizeof(U32)) + sizeof(U32);

			mbase::vector<safe_buffer>::iterator It = totalBuffer.begin();
			MB_SET_SAFE_BUFFER(out_buffer, totalBufferLength);

			IBYTEBUFFER _bfSource = out_buffer.bfSource;
			PTRU32 elemCount = reinterpret_cast<PTRU32>(_bfSource);
			*elemCount = totalBuffer.size();

			_bfSource += sizeof(U32);

			size_type totalIterator = 0;
			for (It; It != totalBuffer.end(); It++)
			{
				elemCount = reinterpret_cast<PTRU32>(_bfSource);
				*elemCount = It->bfLength;
				_bfSource += sizeof(U32);
				for (I32 i = 0; i < It->bfLength; i++)
				{
					_bfSource[i] = It->bfSource[i];
				}

				_bfSource += It->bfLength;
			}
		}
	}
}

template<typename Key, typename Compare, typename Allocator>
MBASE_INLINE mbase::set<Key, Compare, Allocator> set<Key, Compare, Allocator>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
	mbase::set<Key, Compare, Allocator> deserializedContainer;
	if (in_length)
	{
		PTRU32 elemCount = reinterpret_cast<PTRU32>(in_src);
		serialize_helper<value_type> sl;

		IBYTEBUFFER tmpSrc = in_src + sizeof(U32);

		for (I32 i = 0; i < *elemCount; i++)
		{
			PTRU32 elemLength = reinterpret_cast<PTRU32>(tmpSrc);
			tmpSrc += sizeof(U32);
			deserializedContainer.insert(sl.deserialize(tmpSrc, *elemLength));
			tmpSrc += *elemLength;
		}
	}

	return deserializedContainer;
}

template<
	typename Key,
	typename Compare = std::less<Key>,
	typename Allocator = mbase::allocator<Key>
>
class multiset {
private:

public:
	using key_type = typename Key;
	using value_type = typename Key;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using key_compare = typename Compare;
	using value_compare = typename Compare;
	using allocator_type = typename Allocator;
	using reference = Key&;
	using const_reference = const Key&;
	using pointer = Key*;
	using const_pointer = const Key*;
	using iterator = I32;
	using const_iterator = I64;
	using reverse_iterator = F32;
	using const_reverse_iterator = F64;
	using node_type = I32;
	using insert_return_type = I32;

	multiset();
	MBASE_EXPLICIT multiset(const Compare& in_comp, const Allocator& in_alloc = Allocator());
	MBASE_EXPLICIT multiset(const Allocator& in_alloc = Allocator());
	template<typename InputIt>
	multiset(InputIt in_begin, InputIt in_end, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	template<typename InputIt>
	multiset(InputIt in_begin, InputIt in_end, const Allocator& in_alloc = Allocator());
	multiset(const multiset& in_rhs);
	multiset(const multiset& in_rhs, const Allocator& in_alloc);
	multiset(multiset&& in_rhs);
	multiset(multiset&& in_rhs, const Allocator& in_alloc);
	multiset(std::initializer_list<value_type> in_list, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	multiset(std::initializer_list<value_type> in_list, const Allocator& in_alloc);

	~multiset();

	multiset& operator=(const multiset& in_rhs);
	multiset& operator=(multiset&& in_rhs) noexcept;
	multiset& operator=(std::initializer_list<value_type> in_list);

	allocator_type get_allocator() const noexcept;
	key_compare key_comp() const;
	value_compare value_comp() const;
	bool empty() const noexcept;
	size_type size() const noexcept;
	size_type max_size() const noexcept;
	size_type count(const Key& in_key) const;
	template<typename K>
	size_type count(const K& in_key) const;
	iterator find(const Key& in_key);
	const_iterator find(const Key& in_key) const;
	template<typename K>
	iterator find(const K& in_key);
	template<typename K>
	const_iterator find(const K& in_key) const;
	bool contains(const Key& in_key) const;
	template<typename K>
	bool contains(const K& in_key) const;
	std::pair<iterator, iterator> equal_range(const Key& in_key);
	std::pair<const_iterator, const_iterator> equal_range(const Key& in_key) const;
	template<typename K>
	std::pair<iterator, iterator> equal_range(const K& in_key);
	template<typename K>
	std::pair<const_iterator, const_iterator> equal_range(const K& in_key) const;
	iterator lower_bound(const Key& in_key);
	const_iterator lower_bound(const Key& in_key) const;
	template<typename K>
	iterator lower_bound(const K& in_key);
	template<typename K>
	const_iterator lower_bound(const K& in_key) const;
	iterator upper_bound(const Key& in_key);
	const_iterator upper_bound(const Key& in_key) const;
	template<typename K>
	iterator upper_bound(const K& in_key);
	template<typename K>
	const_iterator upper_bound(const K& in_key) const;

	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;
	reverse_iterator rbegin() noexcept;
	const_reverse_iterator rbegin() const noexcept;
	const_reverse_iterator crbegin() const noexcept;
	reverse_iterator rend() noexcept;
	const_reverse_iterator rend() const noexcept;
	const_reverse_iterator crend() const noexcept;

	GENERIC clear() noexcept;
	iterator insert(const value_type& in_value);
	iterator insert(value_type&& in_value);
	iterator insert(const_iterator in_pos, const value_type& in_value);
	iterator insert(const_iterator in_pos, value_type&& in_value);
	template<typename InputIt>
	GENERIC insert(InputIt in_begin, InputIt in_end);
	GENERIC insert(std::initializer_list<value_type> in_list);
	insert_return_type insert(node_type&& in_node);
	iterator insert(const_iterator in_pos, node_type&& in_node);
	template<typename ... Args>
	iterator emplace(Args&& ... args);
	template<typename ... Args>
	iterator emplace_hint(const_iterator in_hint, Args&& ... args);
	iterator erase(iterator in_pos);
	iterator erase(const_iterator in_pos);
	iterator erase(iterator in_first, iterator in_last);
	iterator erase(const_iterator in_first, const_iterator in_last);
	size_type erase(const Key& in_key);
	GENERIC swap(multiset& in_rhs) noexcept;
	node_type extract(const_iterator in_pos);
	node_type extract(const Key& in_key);
	template<typename Compare2>
	GENERIC merge(multiset<Key, Compare2, Allocator>& in_src);
	template<typename Compare2>
	GENERIC merge(multiset<Key, Compare2, Allocator>&& in_src);
	
};


MBASE_STD_END

#endif // !MBASE_SET_H
