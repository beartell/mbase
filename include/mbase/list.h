#ifndef MBASE_LIST_H
#define MBASE_LIST_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <mbase/list_iterator.h>
#include <mbase/safe_buffer.h>
#include <iterator>
#include <initializer_list>
#include <type_traits>

// TODO: Reconsider list_node structure's destructor and constructor
// TODO: Consider implementing a list_pool
// TODO: Do not forget to implement copy constructor

MBASE_STD_BEGIN

/* CONTAINER REQUIREMENTS */
// default constructible
// copy constructible
// equality constructible
// swappable
// serializable

/* TYPE REQUIREMENTS */
// copy insertable
// equality comparable
// destructible

template<typename T, typename Allocator = mbase::allocator<T>>
class list {
private:
	using node_type = typename list_node<T, Allocator>;
	node_type* firstNode;
	node_type* lastNode;
	SIZE_T mSize;
	
	GENERIC clear_and_prepare(const T& in_value) {
		clear();
		mSize = 1;

		firstNode = new node_type(in_value);
		lastNode = firstNode;
	}

	GENERIC clear_and_prepare(const T& in_value, const Allocator& in_alloc) {
		clear();
		mSize = 1;

		firstNode = new node_type(in_value, in_alloc);
		lastNode = firstNode;
	}

	MBASE_INLINE GENERIC push_back_node(node_type* in_node);

public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using move_reference = T&&;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;

	using iterator = typename bi_list_iterator<node_type, value_type>;
	using const_iterator = typename const_bi_list_iterator<node_type, value_type>;
	using reverse_iterator = typename reverse_bi_list_iterator<node_type, value_type>;
	using const_reverse_iterator = typename const_reverse_bi_list_iterator<node_type, value_type>;

	MBASE_INLINE list() noexcept;
	MBASE_INLINE MBASE_EXPLICIT list(const Allocator& in_alloc);
	list(size_type in_count, const T& in_value, const Allocator& in_alloc = Allocator());
	MBASE_INLINE list(size_type in_count, const Allocator& in_alloc = Allocator());
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<T, typename std::iterator_traits<InputIt>::value_type>>>
	list(InputIt in_begin, InputIt in_end, const Allocator& in_alloc = Allocator()) {
		mSize = 0;
		if(in_begin == in_end)
		{
			firstNode = nullptr;
			lastNode = nullptr;
			return;
		}
		mSize = 1;
		firstNode = new node_type(*(in_begin++), in_alloc);
		lastNode = firstNode;
		for (in_begin; in_begin != in_end; in_begin++) {
			push_back(*in_begin);
		}
	}
	list(const list& in_rhs);
	list(const list& in_rhs, const Allocator& in_alloc);
	list(list&& in_rhs);
	list(list&& in_rhs, const Allocator& in_alloc);
	MBASE_INLINE list(std::initializer_list<value_type> in_list) noexcept;
	MBASE_INLINE ~list() noexcept;

	list& operator=(const list& in_rhs);
	list& operator=(list&& in_rhs) noexcept;
	list& operator=(std::initializer_list<value_type> in_vals);

	USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR iterator begin() noexcept;
	USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR iterator end() noexcept;
	USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR const_iterator cbegin() const noexcept;
	USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR const_iterator cend() const noexcept;
	USED_RETURN("reverse iterator being ignored") MBASE_INLINE reverse_iterator rbegin() const noexcept;
	USED_RETURN("reverse iterator being ignored") MBASE_INLINE_EXPR reverse_iterator rend() const noexcept;
	USED_RETURN("const reverse iterator being ignored") MBASE_INLINE const_reverse_iterator crbegin() const noexcept;
	USED_RETURN("const reverse iterator being ignored") MBASE_INLINE_EXPR const_reverse_iterator crend() const noexcept;

	USED_RETURN("first element being ignored") MBASE_INLINE_EXPR reference front() noexcept;
	USED_RETURN("first element being ignored") MBASE_INLINE_EXPR const_reference front() const noexcept;
	USED_RETURN("last element being ignored") MBASE_INLINE_EXPR reference back() noexcept;
	USED_RETURN("last element being ignored") MBASE_INLINE_EXPR const_reference back() const noexcept;
	USED_RETURN("container observation ignored") MBASE_INLINE_EXPR bool empty() const noexcept;
	USED_RETURN("container observation ignored") MBASE_INLINE_EXPR size_type size() const noexcept;
	USED_RETURN("ignoring max size") MBASE_INLINE size_type max_size() const noexcept;
	USED_RETURN("ignoring allocator") MBASE_INLINE_EXPR Allocator get_allocator() const noexcept;

	MBASE_INLINE GENERIC assign(size_type in_count, const_reference in_value);
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<T, typename std::iterator_traits<InputIt>::value_type>>>
	MBASE_INLINE GENERIC assign(InputIt in_begin, InputIt in_end) {
		if(in_begin == in_end)
		{
			clear();
		}
		clear_and_prepare(*(in_begin++));
		for(in_begin; in_begin != in_end; in_begin++)
		{
			push_back(*in_begin);
		}
	}
	MBASE_INLINE GENERIC assign(std::initializer_list<value_type> in_vals);
	MBASE_INLINE_EXPR GENERIC clear() noexcept;
	MBASE_INLINE_EXPR GENERIC push_back(const_reference in_data) noexcept;
	MBASE_INLINE_EXPR GENERIC push_back(move_reference in_data) noexcept;
	MBASE_INLINE_EXPR GENERIC push_front(const_reference in_data) noexcept;
	MBASE_INLINE_EXPR GENERIC push_front(move_reference in_data) noexcept;
	MBASE_INLINE_EXPR GENERIC pop_back() noexcept;
	MBASE_INLINE_EXPR GENERIC pop_front() noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, const_reference in_object) noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, move_reference in_object) noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, size_type in_count, const_reference in_object);
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<T, typename std::iterator_traits<InputIt>::value_type>>>
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, InputIt in_begin, InputIt in_end) {
		iterator inLast(lastNode);
		for(in_begin; in_begin != in_end; in_begin++)
		{
			inLast = insert(in_pos, *in_begin);
		}

		return in_last;
	}
	MBASE_INLINE_EXPR iterator insert(difference_type in_index, const_reference in_object) noexcept;
	MBASE_INLINE_EXPR iterator insert(difference_type in_index, move_reference in_object) noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, std::initializer_list<value_type> in_vals);
	MBASE_INLINE GENERIC _insert_node(const_iterator in_it, node_type* in_node);
	template<typename ... Args>
	MBASE_INLINE iterator emplace(const_iterator in_pos, Args&& ... in_args) {
		return insert(in_pos, std::move(value_type(std::forward<Args>(in_args)...)));
	}
	template<typename ... Args>
	MBASE_INLINE_EXPR GENERIC emplace_front(Args&& ... in_args) {
		push_front(std::move(value_type(std::forward<Args>(in_args)...)));
	}
	template<typename ... Args>
	MBASE_INLINE_EXPR GENERIC emplace_back(Args&& ... in_args) {
		push_back(std::move(value_type(std::forward<Args>(in_args)...)));
	}
	MBASE_INLINE_EXPR iterator erase(iterator in_pos) noexcept;
	MBASE_INLINE_EXPR iterator erase(const_iterator in_pos);
	MBASE_INLINE_EXPR iterator erase(iterator in_begin, iterator in_end); // POSSIBLE PROBLEM
	MBASE_INLINE_EXPR iterator erase(const_iterator in_begin, const_iterator in_end); // POSSIBLE PROBLEM
	MBASE_INLINE_EXPR GENERIC swap(list& in_src) noexcept;
	MBASE_INLINE_EXPR GENERIC merge(list& in_rhs); // IMPLEMENT
	MBASE_INLINE_EXPR GENERIC merge(list&& in_rhs); // IMPLEMENT
	template<typename Compare>
	MBASE_INLINE_EXPR GENERIC merge(list& in_rhs, Compare in_comp); // IMPLEMENT
	template<typename Compare>
	MBASE_INLINE_EXPR GENERIC merge(list&& in_rhs, Compare in_comp); // IMPLEMENT
	MBASE_INLINE_EXPR GENERIC splice(const_iterator in_pos, list& in_rhs);
	MBASE_INLINE_EXPR GENERIC splice(const_iterator in_pos, list&& in_rhs);
	MBASE_INLINE_EXPR GENERIC splice(const_iterator in_pos, list& in_rhs, const_iterator in_it);
	MBASE_INLINE_EXPR GENERIC splice(const_iterator in_pos, list&& in_rhs, const_iterator in_it);
	MBASE_INLINE_EXPR GENERIC splice(const_iterator in_pos, list&& in_rhs, const_iterator in_begin, const_iterator in_end);
	MBASE_INLINE GENERIC serialize(safe_buffer& out_buffer) noexcept;
	MBASE_INLINE static mbase::list<T, Allocator> deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept;
};

template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::list() noexcept : firstNode(nullptr), lastNode(nullptr), mSize(0) 
{
}

template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::list(const Allocator& in_alloc) : firstNode(nullptr), lastNode(nullptr), mSize(0)
{
}

template<typename T, typename Allocator>
list<T, Allocator>::list(size_type in_count, const T& in_value, const Allocator& in_alloc)
{
	mSize = in_count;
	if(!mSize)
	{
		firstNode = nullptr;
		lastNode = nullptr;
		return;
	}
	mSize = 1;

	firstNode = new node_type(in_value, in_alloc);
	lastNode = firstNode;
	for(I32 i = 0; i < in_count - 1; i++)
	{
		push_back(in_value);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::list(size_type in_count, const Allocator& in_alloc)
{
	mSize = in_count;
	if (!mSize)
	{
		firstNode = nullptr;
		lastNode = nullptr;
		return;
	}
	mSize = 1;

	T defaultValue;
	firstNode = new node_type(defaultValue, in_alloc);
	lastNode = firstNode;
	for (I32 i = 0; i < in_count - 1; i++)
	{
		push_back(defaultValue);
	}
}

template<typename T, typename Allocator>
list<T, Allocator>::list(const list& in_rhs) {
	mSize = in_rhs.mSize;
	if(!mSize)
	{
		firstNode = nullptr;
		lastNode = nullptr;
		return;
	}
	mSize = 1;

	const_iterator cit = in_rhs.cbegin();
	firstNode = new node_type(*(cit++));
	lastNode = firstNode;
	for(cit; cit != in_rhs.cend(); cit++)
	{
		push_back(*cit);
	}
}

template<typename T, typename Allocator>
list<T, Allocator>::list(const list& in_rhs, const Allocator& in_alloc) {
	mSize = in_rhs.mSize;
	if (!mSize)
	{
		firstNode = nullptr;
		lastNode = nullptr;
		return;
	}
	mSize = 1;

	const_iterator cit = in_rhs.cbegin();
	firstNode = new node_type(*(cit++), in_alloc);
	lastNode = firstNode;
	for (cit; cit != in_rhs.cend(); cit++)
	{
		push_back(*cit);
	}
}

template<typename T, typename Allocator>
list<T, Allocator>::list(list&& in_rhs) {
	mSize = in_rhs.mSize;
	firstNode = in_rhs.firstNode;
	lastNode = in_rhs.lastNode;

	in_rhs.mSize = 0;
	in_rhs.firstNode = nullptr;
	in_rhs.lastNode = nullptr;
}

template<typename T, typename Allocator>
list<T, Allocator>::list(list&& in_rhs, const Allocator& in_alloc) {
	mSize = in_rhs.mSize;
	firstNode = in_rhs.firstNode;
	lastNode = in_rhs.lastNode;

	in_rhs.mSize = 0;
	in_rhs.firstNode = nullptr;
	in_rhs.lastNode = nullptr;
}


template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::list(std::initializer_list<value_type> in_list) noexcept {
	mSize = in_list.size();
	if (!mSize)
	{
		firstNode = nullptr;
		lastNode = nullptr;
		return;
	}
	mSize = 1;

	auto currentObj = in_list.begin();
	firstNode = new node_type(*(currentObj++));
	lastNode = firstNode;

	for(currentObj; currentObj != in_list.end(); currentObj++)
	{
		push_back(*currentObj);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::~list() noexcept
{
	clear();
}

template<typename T, typename Allocator>
list<T, Allocator>& list<T, Allocator>::operator=(const list& in_rhs) { // TODO: IMPLEMENT
	const_iterator cit = in_rhs.cbegin();
	if(cit == in_rhs.cend())
	{
		clear();
		return *this;
	}

	clear_and_prepare(*(cit++));
	for(cit; cit != in_rhs.cend(); cit++)
	{
		push_back(*cit);
	}

	return *this;
}

template<typename T, typename Allocator>
list<T, Allocator>& list<T, Allocator>::operator=(list&& in_rhs) noexcept { // TODO: IMPLEMENT
	clear();

	mSize = in_rhs.mSize;
	firstNode = in_rhs.firstNode;
	lastNode = in_rhs.lastNode;

	in_rhs.mSize = 0;
	in_rhs.firstNode = nullptr;
	in_rhs.lastNode = nullptr;

	return *this;
}

template<typename T, typename Allocator>
list<T, Allocator>& list<T, Allocator>::operator=(std::initializer_list<value_type> in_vals) { // TODO: IMPLEMENT
	if(!in_vals.size())
	{
		clear();
		*this;
	}

	auto inListIter = in_vals.begin();
	clear_and_prepare(*(inListIter++));
	for(inListIter; inListIter != in_vals.end(); inListIter++)
	{
		push_back(*inListIter);
	}

	return *this;
}


template<typename T, typename Allocator>
USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::begin() noexcept {
	return iterator(firstNode);
}

template<typename T, typename Allocator>
USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::end() noexcept {
	return iterator(nullptr);
}

template<typename T, typename Allocator>
USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::const_iterator list<T, Allocator>::cbegin() const noexcept {
	return const_iterator(firstNode);
}

template<typename T, typename Allocator>
USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::const_iterator list<T, Allocator>::cend() const noexcept {
	return const_iterator(nullptr);
}

template<typename T, typename Allocator>
USED_RETURN("reverse iterator being ignored") MBASE_INLINE typename list<T, Allocator>::reverse_iterator list<T, Allocator>::rbegin() const noexcept {
	return reverse_iterator(lastNode);
}

template<typename T, typename Allocator>
USED_RETURN("reverse iterator being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::reverse_iterator list<T, Allocator>::rend() const noexcept {
	return reverse_iterator(nullptr);
}

template<typename T, typename Allocator>
USED_RETURN("const reverse iterator being ignored") MBASE_INLINE typename list<T, Allocator>::const_reverse_iterator list<T, Allocator>::crbegin() const noexcept {
	return const_reverse_iterator(lastNode);
}

template<typename T, typename Allocator>
USED_RETURN("const reverse iterator being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::const_reverse_iterator list<T, Allocator>::crend() const noexcept {
	return const_reverse_iterator(nullptr);
}

template<typename T, typename Allocator>
USED_RETURN("first element being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::reference list<T, Allocator>::front() noexcept {
	return *firstNode->data;
}

template<typename T, typename Allocator>
USED_RETURN("first element being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::const_reference list<T, Allocator>::front() const noexcept {
	return *firstNode->data;
}

template<typename T, typename Allocator>
USED_RETURN("last element being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::reference list<T, Allocator>::back() noexcept {
	return *lastNode->data;
}

template<typename T, typename Allocator>
USED_RETURN("last element being ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::const_reference list<T, Allocator>::back() const noexcept {
	return *lastNode->data;
}

template<typename T, typename Allocator>
USED_RETURN("container observation ignored") MBASE_INLINE_EXPR bool list<T, Allocator>::empty() const noexcept {
	return mSize == 0;
}

template<typename T, typename Allocator>
USED_RETURN("container observation ignored") MBASE_INLINE_EXPR typename list<T, Allocator>::size_type list<T, Allocator>::size() const noexcept {
	return mSize;
}

template<typename T, typename Allocator>
USED_RETURN("ignoring max size") MBASE_INLINE typename list<T, Allocator>::size_type list<T, Allocator>::max_size() const noexcept {
	return std::numeric_limits<difference_type>::max();
}

template<typename T, typename Allocator>
USED_RETURN("ignoring allocator") MBASE_INLINE_EXPR Allocator list<T, Allocator>::get_allocator() const noexcept {
	return Allocator();
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::assign(size_type in_count, const_reference in_value) {
	clear_and_prepare(in_value);
	for(I32 i = 0; i < in_count - 1; i++)
	{
		push_back(in_value);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::assign(std::initializer_list<value_type> in_vals) {
	if(!in_vals.size())
	{
		clear();
		return;
	}

	auto iListIter = in_vals.begin();
	clear_and_prepare(*(iListIter++));
	for(iListIter; iListIter != in_vals.end(); iListIter++)
	{
		push_back(*iListIter);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::clear() noexcept {
	while (mSize)
	{
		pop_back();
	}
}


template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::push_back(const_reference in_data) noexcept {
	
	node_type* newNode = new node_type(in_data);
	newNode->prev = lastNode;
	if (lastNode)
	{
		// in case of list being empty
		lastNode->next = newNode;
	}
	else
	{
		lastNode = newNode;
		firstNode = lastNode;
	}
	lastNode = newNode;
	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::push_back(move_reference in_data) noexcept {
	node_type* newNode = new node_type(std::move(in_data));
	newNode->prev = lastNode;
	if (lastNode)
	{
		// in case of list being empty
		lastNode->next = newNode;
	}
	else
	{
		lastNode = newNode;
		firstNode = lastNode;
	}
	lastNode = newNode;
	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::push_front(const_reference in_data) noexcept {
	node_type* newNode = new node_type(in_data);
	newNode->next = firstNode;
	if (firstNode)
	{
		firstNode->prev = newNode;
	}
	else
	{
		firstNode = newNode;
		lastNode = firstNode;
	}

	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::push_front(move_reference in_data) noexcept {
	node_type* newNode = new node_type(std::move(in_data));
	newNode->next = firstNode;
	if (firstNode)
	{
		firstNode->prev = newNode;
	}
	else
	{
		firstNode = newNode;
		lastNode = firstNode;
	}

	firstNode = newNode;
	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::pop_back() noexcept {
	if (!(--mSize))
	{
		delete lastNode;
		lastNode = nullptr;
		firstNode = nullptr;
		return;
	}
	node_type* newLastNode = lastNode->prev;
	newLastNode->next = nullptr;
	delete lastNode;
	lastNode = newLastNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::pop_front() noexcept {
	if (!(--mSize))
	{
		delete firstNode;
		lastNode = nullptr;
		firstNode = nullptr;
		return;
	}

	node_type* newFirstNode = firstNode->next;
	newFirstNode->prev = nullptr;
	delete firstNode;
	firstNode = newFirstNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(const_iterator in_pos, const_reference in_object) noexcept {
	if(in_pos == cend())
	{
		push_back(in_object);
		return iterator(lastNode);
	}

	node_type* mNode = const_cast<node_type*>(in_pos.get());
	node_type* newNode = new node_type(in_object);
	newNode->prev = mNode->prev;
	if (mNode->prev)
	{
		mNode->prev->next = newNode;
	}

	else
	{
		firstNode = newNode;
	}

	mNode->prev = newNode;
	newNode->next = mNode;
	++mSize;
	return iterator(newNode);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(const_iterator in_pos, move_reference in_object) noexcept {
	if (in_pos == cend())
	{
		push_back(std::move(in_object));
		return iterator(lastNode);
	}

	node_type* mNode = const_cast<node_type*>(in_pos.get());
	node_type* newNode = new node_type(std::move(in_object));
	newNode->prev = mNode->prev;
	if (mNode->prev)
	{
		mNode->prev->next = newNode;
	}

	else
	{
		firstNode = newNode;
	}

	mNode->prev = newNode;
	newNode->next = mNode;
	++mSize;
	return iterator(newNode);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(const_iterator in_pos, size_type in_count, const_reference in_object) {
	if(!in_count)
	{
		return end();
	}

	iterator activeIt(firstNode);
	for (I32 i = 0; i < in_count; i++) 
	{
		activeIt = insert(in_pos, in_object);
	}
	return activeIt;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(difference_type in_index, const_reference in_object) noexcept {
	iterator in_pos = begin();
	in_pos += in_index;

	return insert(in_pos, in_object);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(difference_type in_index, move_reference in_object) noexcept {
	iterator in_pos = begin();
	in_pos += in_index;

	return insert(in_pos, std::move(in_object));
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::erase(iterator in_pos) noexcept {
	node_type* mNode = in_pos.get();
	node_type* returnedNode = mNode->next;

	if (mNode == firstNode)
	{
		pop_front();
	}

	else if (mNode == lastNode)
	{
		returnedNode = mNode->prev;
		pop_back();
	}

	else
	{
		mNode->prev->next = mNode->next;
		mNode->next->prev = mNode->prev;
		delete mNode;
		--mSize;
	}
	return returnedNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::erase(const_iterator in_pos) {
	node_type* mNode = const_cast<node_type*>in_pos.get();
	node_type* returnedNode = mNode->next;

	if (mNode == firstNode)
	{
		pop_front();
	}

	else if (mNode == lastNode)
	{
		returnedNode = mNode->prev;
		pop_back();
	}

	else
	{
		mNode->prev->next = mNode->next;
		mNode->next->prev = mNode->prev;
		delete mNode;
		--mSize;
	}
	return returnedNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::erase(iterator in_begin, iterator in_end) {
	iterator removedNode(lastNode);
	for(in_begin; in_begin != in_end; in_begin++) {
		in_begin = erase(in_begin);
		removedNode = in_begin;
	}
	return removedNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::erase(const_iterator in_begin, const_iterator in_end) {
	iterator removedNode(lastNode);
	for (in_begin; in_begin != in_end; in_begin++) {
		in_begin = erase(in_begin);
		removedNode = in_begin;
	}
	return removedNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list& in_rhs) {
	iterator in_begin = in_rhs.begin();
	for(in_begin; in_begin != in_rhs.end();)
	{
		node_type* nt = in_begin.get();
		in_begin++;
		_insert_node(in_pos, nt);
	}

	list newList(std::move(in_rhs));

	in_rhs.mSize = 0;
	in_rhs.firstNode = nullptr;
	in_rhs.lastNode = nullptr;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list&& in_rhs) {
	iterator in_begin = in_rhs.begin();
	for (in_begin; in_begin != in_rhs.end();)
	{
		node_type* nt = in_begin.get();
		in_begin++;
		_insert_node(in_pos, nt);
	}

	list newList(std::move(in_rhs));

	in_rhs.mSize = 0;
	in_rhs.firstNode = nullptr;
	in_rhs.lastNode = nullptr;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list& in_rhs, const_iterator in_it) {
	insert(in_pos, *in_it);
	in_rhs.erase(in_it);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list&& in_rhs, const_iterator in_it) {
	insert(in_pos, *in_it);
	in_rhs.erase(in_it);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list&& in_rhs, const_iterator in_begin, const_iterator in_end) {
	// EVEN IF THE IMPLEMENTATION IS CORRECT,
	// MAKE SURE TO ERASE ELEMENTS OF 'RHS' ON THE FLY (IN THE LOOP)

	for(in_begin; in_begin != in_end; in_begin++)
	{
		insert(in_pos, *in_begin);
	}

	in_rhs.erase(in_begin, in_end);
}


template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::swap(list& in_src) noexcept {
	std::swap(firstNode, in_src.firstNode);
	std::swap(lastNode, in_src.lastNode);
	std::swap(mSize, in_src.mSize);
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::serialize(safe_buffer& out_buffer) noexcept {
	if (mSize)
	{
		mbase::vector<safe_buffer> totalBuffer;

		serialize_helper<value_type> sl;

		safe_buffer tmpSafeBuffer;
		size_type totalLength = 0;

		for (iterator It = begin(); It != end(); It++)
		{
			sl.value = It.get()->data;
			sl.serialize(&tmpSafeBuffer);
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
			out_buffer.bfLength = totalBufferLength;
			out_buffer.bfSource = new IBYTE[totalBufferLength];

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

template<typename T, typename Allocator>
MBASE_INLINE mbase::list<T, Allocator> list<T, Allocator>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
	mbase::list<T, Allocator> deserializedContainer;
	if (in_length)
	{
		PTRU32 elemCount = reinterpret_cast<PTRU32>(in_src);
		serialize_helper<value_type> sl;

		IBYTEBUFFER tmpSrc = in_src + sizeof(U32);

		for (I32 i = 0; i < *elemCount; i++)
		{
			PTRU32 elemLength = reinterpret_cast<PTRU32>(tmpSrc);
			tmpSrc += sizeof(U32);
			deserializedContainer.push_back(sl.deserialize(tmpSrc, *elemLength));
			tmpSrc += *elemLength;
		}
	}

	return deserializedContainer;
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::push_back_node(node_type* in_node) {
	in_node->prev = lastNode;
	if(lastNode)
	{
		lastNode->next = in_node;
	}
	else
	{
		lastNode = in_node;
		firstNode = lastNode;
	}
	in_node->next = nullptr;
	lastNode = in_node;
	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::_insert_node(const_iterator in_it, node_type* in_node) {
	if(in_it == cend())
	{
		push_back_node(in_node);
		return;
	}

	node_type* mNode = const_cast<node_type*>(in_it.get());
	in_node->prev = mNode->prev;
	if(mNode->prev)
	{
		mNode->prev->next = in_node;
	}

	else
	{
		firstNode = in_node;
	}

	mNode->prev = in_node;
	in_node->next = mNode;
	++mSize;
}

MBASE_STD_END

#endif // !MBASE_LIST_H
