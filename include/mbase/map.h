#ifndef MBASE_MAP_H
#define MBASE_MAP_H

#include <mbase/common.h>
#include <mbase/allocator.h> // mbase::allocator
#include <functional> // std::less

MBASE_STD_BEGIN

/*

	--- CLASS INFORMATION ---
Identification: S0C28-OBJ-NA-NU

Name: map

Parent: None

Behaviour List:
- Default Constructible
- Copy Constructible
- Move Constructible
- Copy Assignable
- Move Assignable
- Arithmetic Operable
- Templated
- Serializable
- Swappable
- Type Aware

Description:

*/


template<typename Key,
	typename T,
	typename Compare = std::less<Key>,
	typename Allocator = mbase::allocator<std::pair<const Key, T>>
>
class map {
private:

public:
	using key_type = typename Key;
	using mapped_type = typename T;
	using value_type = std::pair<const Key, T>;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using key_compare = typename Compare;
	using allocator_type = typename Allocator;
	using reference = typename value_type&;
	using const_reference = typename const value_type&;
	using pointer = typename Allocator::pointer;
	using const_pointer = typename Allocator::const_pointer;
	using iterator = I32;
	using const_iterator = I64;
	using reverse_iterator = F32;
	using const_reverse_iterator = F64;
	using node_type = I32;
	using insert_return_type = I32;

	MBASE_INLINE map();
	MBASE_INLINE MBASE_EXPLICIT map(const Compare& in_comp, const Allocator& alloc = Allocator());
	MBASE_INLINE MBASE_EXPLICIT map(const Allocator& in_alloc);
	template<typename InputIt>
	MBASE_INLINE map(InputIt in_begin, InputIt in_end, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	template<typename InputIt>
	MBASE_INLINE map(InputIt in_begin, InputIt in_end, const Allocator& in_alloc);
	MBASE_INLINE map(const map& in_rhs);
	MBASE_INLINE map(const map& in_rhs, const Allocator& in_alloc);
	MBASE_INLINE map(map&& in_rhs);
	MBASE_INLINE map(map&& in_rhs, const Allocator& in_alloc);
	MBASE_INLINE map(std::initializer_list<value_type> in_list, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	MBASE_INLINE map(std::initializer_list<value_type> in_list, const Allocator& in_alloc);

	~map();

	MBASE_INLINE map& operator=(const map& in_rhs);
	MBASE_INLINE map& operator=(map&& in_rhs) noexcept;
	MBASE_INLINE map& operator=(std::initializer_list<value_type> in_list);

	MBASE_INLINE allocator_type get_allocator() const noexcept;
	MBASE_INLINE bool empty() const noexcept;
	MBASE_INLINE size_type size() const noexcept;
	MBASE_INLINE size_type max_size() const noexcept;
	MBASE_INLINE size_type count(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE size_type count(const K& in_key) const;
	MBASE_INLINE iterator find(const Key& in_key);
	MBASE_INLINE const_iterator find(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE iterator find(const K& in_key);
	template<typename K>
	MBASE_INLINE const_iterator find(const K& in_key) const;
	MBASE_INLINE bool contains(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE bool contains(const K& in_key) const;
	MBASE_INLINE std::pair<iterator, iterator> equal_range(const Key& key);
	MBASE_INLINE std::pair<const_iterator, const_iterator> equal_range(const Key& key);
	template<typename K>
	MBASE_INLINE std::pair<iterator, iterator> equal_range(const K& in_key);
	template<typename K>
	MBASE_INLINE std::pair<const_iterator, const_iterator> equal_range(const K& in_key);
	MBASE_INLINE iterator lower_bound(const Key& in_key);
	MBASE_INLINE const_iterator lower_bound(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE iterator lower_bound(const K& in_key);
	template<typename K>
	MBASE_INLINE const_iterator lower_bound(const K& in_key) const;
	MBASE_INLINE iterator upper_bound(const Key& in_key);
	MBASE_INLINE const_iterator upper_bound(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE iterator upper_bound(const K& in_key);
	template<typename K>
	MBASE_INLINE const_iterator upper_bound(const K& in_key) const;
	MBASE_INLINE key_compare key_comp() const;
	// VALUE COMP LATER

	MBASE_INLINE iterator begin() noexcept;
	MBASE_INLINE const_iterator begin() const noexcept;
	MBASE_INLINE const_iterator cbegin() const noexcept;
	MBASE_INLINE iterator end() noexcept;
	MBASE_INLINE const_iterator end() const noexcept;
	MBASE_INLINE const_iterator cend() const noexcept;
	MBASE_INLINE reverse_iterator rbegin() noexcept;
	MBASE_INLINE const_reverse_iterator rbegin() const noexcept;
	MBASE_INLINE const_reverse_iterator crbegin() const noexcept;
	MBASE_INLINE reverse_iterator rend() noexcept;
	MBASE_INLINE const_reverse_iterator rend() const noexcept;
	MBASE_INLINE const_reverse_iterator crend() const noexcept;

	MBASE_INLINE GENERIC clear() noexcept;
	MBASE_INLINE std::pair<iterator, bool> insert(const value_type& in_value);
	MBASE_INLINE std::pair<iterator, bool> insert(value_type&& in_value);
	MBASE_INLINE iterator insert(const_iterator in_pos, const value_type& in_value);
	MBASE_INLINE iterator insert(const_iterator in_pos, value_type&& in_value);
	template<typename InputIt>
	MBASE_INLINE GENERIC insert(InputIt in_begin, InputIt in_end);
	MBASE_INLINE GENERIC insert(std::initializer_list<value_type> in_list);
	MBASE_INLINE insert_return_type insert(node_type&& in_node);
	MBASE_INLINE iterator insert(const_iterator in_pos, node_type&& in_node);
	template<typename K>
	MBASE_INLINE std::pair<iterator, bool> insert(K&& in_key);
	template<typename K>
	MBASE_INLINE iterator insert(const_iterator in_pos, K&& in_key);
	template<typename M>
	MBASE_INLINE std::pair<iterator, bool> insert_or_assign(const Key& in_key, M&& in_obj);
	template<typename M>
	MBASE_INLINE std::pair<iterator, bool> insert_or_assign(Key&& in_key, M&& in_obj);
	template<typename M>
	MBASE_INLINE iterator insert_or_assign(const_iterator in_hint, const Key& in_key, M&& in_obj);
	template<typename M>
	MBASE_INLINE iterator insert_or_assign(const_iterator in_hing, Key&& in_key, M&& in_obj);
	template<typename ... Args>
	std::pair<iterator, bool> emplace(Args&& ... in_args);
	template<typename ... Args>
	iterator emplace_hint(const_iterator in_hint, Args&& ... in_args);
	template<typename ... Args>
	std::pair<iterator, bool> try_emplace(const Key& in_key, Args&&... in_args);
	template<typename ... Args>
	std::pair<iterator, bool> try_emplace(Key&& in_key, Args&&... in_args);
	template<typename ... Args>
	iterator try_emplace(const_iterator in_hint, const Key& in_key, Args&&... in_args);
	template<typename ... Args>
	iterator try_emplace(const_iterator in_hint, Key&& in_key, Args&&... in_args);
	iterator erase(iterator in_pos);
	iterator erase(const_iterator in_pos);
	iterator erase(iterator in_begin, iterator in_end);
	iterator erase(const_iterator in_begin, iterator in_end);
	size_type erase(const Key& in_key);
	GENERIC swap(map& in_rhs) noexcept;
	node_type extract(const_iterator in_pos);
	node_type extract(const Key& in_key);
	template<typename Compare2>
	GENERIC merge(map<Key, T, Compare2, Allocator>& in_src);
	template<typename Compare2>
	GENERIC merge(map<Key, T, Compare2, Allocator>&& in_src);
}; 

template<typename Key,
	typename T,
	typename Compare = std::less<Key>,
	typename Allocator = mbase::allocator<std::pair<const Key, T>>
>
class multimap {
private:

public:
	using key_type = typename Key;
	using mapped_type = typename T;
	using value_type = std::pair<const Key, T>;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using key_compare = typename Compare;
	using allocator_type = typename Allocator;
	using reference = typename value_type&;
	using const_reference = typename const value_type&;
	using pointer = typename Allocator::pointer;
	using const_pointer = typename Allocator::const_pointer;
	using iterator = I32;
	using const_iterator = I64;
	using reverse_iterator = F32;
	using const_reverse_iterator = F64;
	using node_type = I32;
	using insert_return_type = I32;

	MBASE_INLINE multimap();
	MBASE_INLINE MBASE_EXPLICIT multimap(const Compare& in_comp, const Allocator& alloc = Allocator());
	MBASE_INLINE MBASE_EXPLICIT multimap(const Allocator& in_alloc);
	template<typename InputIt>
	MBASE_INLINE multimap(InputIt in_begin, InputIt in_end, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	template<typename InputIt>
	MBASE_INLINE multimap(InputIt in_begin, InputIt in_end, const Allocator& in_alloc);
	MBASE_INLINE multimap(const multimap& in_rhs);
	MBASE_INLINE multimap(const multimap& in_rhs, const Allocator& in_alloc);
	MBASE_INLINE multimap(multimap&& in_rhs);
	MBASE_INLINE multimap(multimap&& in_rhs, const Allocator& in_alloc);
	MBASE_INLINE multimap(std::initializer_list<value_type> in_list, const Compare& in_comp = Compare(), const Allocator& in_alloc = Allocator());
	MBASE_INLINE multimap(std::initializer_list<value_type> in_list, const Allocator& in_alloc);

	~multimap();

	MBASE_INLINE multimap& operator=(const multimap& in_rhs);
	MBASE_INLINE multimap& operator=(multimap&& in_rhs) noexcept;
	MBASE_INLINE multimap& operator=(std::initializer_list<value_type> in_list);

	MBASE_INLINE allocator_type get_allocator() const noexcept;
	MBASE_INLINE bool empty() const noexcept;
	MBASE_INLINE size_type size() const noexcept;
	MBASE_INLINE size_type max_size() const noexcept;
	MBASE_INLINE size_type count(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE size_type count(const K& in_key) const;
	MBASE_INLINE iterator find(const Key& in_key);
	MBASE_INLINE const_iterator find(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE iterator find(const K& in_key);
	template<typename K>
	MBASE_INLINE const_iterator find(const K& in_key) const;
	MBASE_INLINE bool contains(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE bool contains(const K& in_key) const;
	MBASE_INLINE std::pair<iterator, iterator> equal_range(const Key& key);
	MBASE_INLINE std::pair<const_iterator, const_iterator> equal_range(const Key& key);
	template<typename K>
	MBASE_INLINE std::pair<iterator, iterator> equal_range(const K& in_key);
	template<typename K>
	MBASE_INLINE std::pair<const_iterator, const_iterator> equal_range(const K& in_key);
	MBASE_INLINE iterator lower_bound(const Key& in_key);
	MBASE_INLINE const_iterator lower_bound(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE iterator lower_bound(const K& in_key);
	template<typename K>
	MBASE_INLINE const_iterator lower_bound(const K& in_key) const;
	MBASE_INLINE iterator upper_bound(const Key& in_key);
	MBASE_INLINE const_iterator upper_bound(const Key& in_key) const;
	template<typename K>
	MBASE_INLINE iterator upper_bound(const K& in_key);
	template<typename K>
	MBASE_INLINE const_iterator upper_bound(const K& in_key) const;
	MBASE_INLINE key_compare key_comp() const;
	// VALUE COMP LATER

	MBASE_INLINE iterator begin() noexcept;
	MBASE_INLINE const_iterator begin() const noexcept;
	MBASE_INLINE const_iterator cbegin() const noexcept;
	MBASE_INLINE iterator end() noexcept;
	MBASE_INLINE const_iterator end() const noexcept;
	MBASE_INLINE const_iterator cend() const noexcept;
	MBASE_INLINE reverse_iterator rbegin() noexcept;
	MBASE_INLINE const_reverse_iterator rbegin() const noexcept;
	MBASE_INLINE const_reverse_iterator crbegin() const noexcept;
	MBASE_INLINE reverse_iterator rend() noexcept;
	MBASE_INLINE const_reverse_iterator rend() const noexcept;
	MBASE_INLINE const_reverse_iterator crend() const noexcept;

	MBASE_INLINE GENERIC clear() noexcept;
	MBASE_INLINE iterator insert(const value_type& in_value);
	MBASE_INLINE iterator insert(value_type&& in_value);
	MBASE_INLINE iterator insert(const_iterator in_pos, const value_type& in_value);
	MBASE_INLINE iterator insert(const_iterator in_pos, value_type&& in_value);
	template<typename InputIt>
	MBASE_INLINE GENERIC insert(InputIt in_begin, InputIt in_end);
	MBASE_INLINE GENERIC insert(std::initializer_list<value_type> in_list);
	MBASE_INLINE insert_return_type insert(node_type&& in_node);
	MBASE_INLINE iterator insert(const_iterator in_pos, node_type&& in_node);
	template<typename K>
	MBASE_INLINE iterator insert(K&& in_key);
	template<typename K>
	MBASE_INLINE iterator insert(const_iterator in_pos, K&& in_key);
	template<typename M>
	MBASE_INLINE iterator insert_or_assign(const Key& in_key, M&& in_obj);
	template<typename M>
	MBASE_INLINE iterator insert_or_assign(Key&& in_key, M&& in_obj);
	template<typename M>
	MBASE_INLINE iterator insert_or_assign(const_iterator in_hint, const Key& in_key, M&& in_obj);
	template<typename M>
	MBASE_INLINE iterator insert_or_assign(const_iterator in_hing, Key&& in_key, M&& in_obj);
	template<typename ... Args>
	iterator emplace(Args&& ... in_args);
	template<typename ... Args>
	iterator emplace_hint(const_iterator in_hint, Args&& ... in_args);
	template<typename ... Args>
	iterator try_emplace(const Key& in_key, Args&&... in_args);
	template<typename ... Args>
	iterator try_emplace(Key&& in_key, Args&&... in_args);
	template<typename ... Args>
	iterator try_emplace(const_iterator in_hint, const Key& in_key, Args&&... in_args);
	template<typename ... Args>
	iterator try_emplace(const_iterator in_hint, Key&& in_key, Args&&... in_args);
	iterator erase(iterator in_pos);
	iterator erase(const_iterator in_pos);
	iterator erase(iterator in_begin, iterator in_end);
	iterator erase(const_iterator in_begin, iterator in_end);
	size_type erase(const Key& in_key);
	GENERIC swap(multimap& in_rhs) noexcept;
	node_type extract(const_iterator in_pos);
	node_type extract(const Key& in_key);
	template<typename Compare2>
	GENERIC merge(multimap<Key, T, Compare2, Allocator>& in_src);
	template<typename Compare2>
	GENERIC merge(multimap<Key, T, Compare2, Allocator>&& in_src);
};

MBASE_STD_END

#endif // !MBASE_MAP_H
