#ifndef MBASE_SET_H
#define MBASE_SET_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <mbase/node_type.h>
#include <mbase/binary_iterator.h>
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
	using node_type = I32;
	using insert_return_type = I32;

	set() : rootNode(nullptr), mSize(0) {}
	MBASE_EXPLICIT set(const Compare& in_comp, const Allocator& in_alloc = Allocator());
	//MBASE_EXPLICIT set(const Allocator& in_alloc = Allocator());
	template<typename InputIt>
	set(InputIt in_begin, InputIt in_end, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	template<typename InputIt>
	set(InputIt in_begin, InputIt in_end, const Allocator& in_alloc = Allocator());
	set(const set& in_rhs);
	set(const set& in_rhs, const Allocator& in_alloc);
	set(set&& in_rhs);
	set(set&& in_rhs, const Allocator& in_alloc);
	set(std::initializer_list<value_type> in_list, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	set(std::initializer_list<value_type> in_list, const Allocator& in_alloc);

	~set(){}

	set& operator=(const set& in_rhs);
	set& operator=(set&& in_rhs) noexcept;
	set& operator=(std::initializer_list<value_type> in_list);

	allocator_type get_allocator() const noexcept;
	key_compare key_comp() const 
	{ 
		key_compare kc;
		return kc; 
	}
	value_compare value_comp() const
	{
		value_compare vc;
		return vc;
	}
	bool empty() const noexcept
	{
		return mSize == 0;
	}
	size_type size() const noexcept
	{
		return mSize;
	}
	size_type max_size() const noexcept
	{
		return std::numeric_limits<difference_type>::max();
	}
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

	iterator begin() noexcept {
		return iterator(rootNode);
	}
	const_iterator begin() const noexcept {
		return const_iterator(rootNode);
	}
	const_iterator cbegin() const noexcept {
		return const_iterator(rootNode);
	}
	iterator end() noexcept {
		return iterator(nullptr);
	}
	const_iterator end() const noexcept {
		return const_iterator(nullptr);
	}
	const_iterator cend() const noexcept {
		return const_iterator(nullptr);
	}
	reverse_iterator rbegin() noexcept {
		return reverse_iterator(rootNode);
	}
	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(rootNode);
	}
	const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(rootNode);
	}
	reverse_iterator rend() noexcept {
		return reverse_iterator(nullptr);
	}
	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(nullptr);
	}
	const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(nullptr);
	}

	GENERIC clear() noexcept;
	iterator insert(const value_type& in_value) {
		_node_type::insert_node(rootNode, in_value, rootNode);
		return iterator(rootNode);
	}
	iterator insert(value_type&& in_value) {
		_node_type::insert_node(rootNode, std::move(in_value), rootNode);
		return iterator(rootNode);
	}
	//iterator insert(const_iterator in_pos, const value_type& in_value);
	//iterator insert(const_iterator in_pos, value_type&& in_value);
	template<typename InputIt>
	GENERIC insert(InputIt in_begin, InputIt in_end);
	GENERIC insert(std::initializer_list<value_type> in_list);
	//insert_return_type insert(node_type&& in_node);
	iterator insert(const_iterator in_pos, node_type&& in_node);
	template<typename ... Args>
	std::pair<iterator, bool> emplace(Args&& ... args);
	template<typename ... Args>
	// emplace_hint(const_iterator in_hint, Args&& ... args);
	//iterator erase(iterator in_pos);
	//iterator erase(const_iterator in_pos);
	//iterator erase(iterator in_first, iterator in_last);
	//iterator erase(const_iterator in_first, const_iterator in_last);
	//size_type erase(const Key& in_key);
	GENERIC swap(set& in_rhs) noexcept;
	//node_type extract(const_iterator in_pos);
	//node_type extract(const Key& in_key);
	/*template<typename Compare2>
	GENERIC merge(set<Key, Compare2, Allocator>& in_src);
	template<typename Compare2>
	GENERIC merge(set<Key, Compare2, Allocator>&& in_src);*/
	_node_type* rootNode;
private:
	
	size_type mSize;
};

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
