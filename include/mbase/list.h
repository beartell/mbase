#ifndef MBASE_LIST_H
#define MBASE_LIST_H

#include <mbase/common.h>
#include <mbase/allocator.h> // mbase::allocator
#include <mbase/list_iterator.h> // mbase::bi_list_iterator, mbase::const_bi_list_iterator, mbase::reverse_bi_list_iterator, mbase::const_reverse_bi_list_iterator
#include <mbase/char_stream.h> // mbase::char_stream
#include <mbase/node_type.h> // mbase::list_node
#include <mbase/vector.h> // mbase::vector
#include <initializer_list> // std::initializer_list
#include <iterator> // std::iterator_traits
#include <type_traits> // std::is_constructible
#include <algorithm> // std::find

MBASE_STD_BEGIN

static const U32 gSerializedListBlockLength = 4;

/*

	--- CLASS INFORMATION ---
Identification: S0C19-OBJ-UD-ST

Name: list

Parent: None

Behaviour List:
- Default Constructible
- Move Constructible
- Copy Constructible
- Destructible
- Copy Assignable
- Move Assignable
- Arithmetic Operable
- Templated
- Type Aware
- Iterable
- Swappable
- Equality Comparable
- Sign Comparable
- Serializable

Description:

*/

template<typename T, typename Allocator = mbase::allocator<T>>
class list {
private:
	using node_type = typename list_node<T>;
	node_type* mFirstNode;
	node_type* mLastNode;
	SIZE_T mSize;
	
	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE GENERIC _push_back_node(node_type* in_node);
	/* ===== STATE-MODIFIER METHODS END ===== */

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

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE list() noexcept;
	MBASE_INLINE MBASE_EXPLICIT list(const Allocator& in_alloc);
	list(size_type in_count, const T& in_value, const Allocator& in_alloc = Allocator());
	MBASE_INLINE list(size_type in_count, const Allocator& in_alloc = Allocator());
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<T, typename std::iterator_traits<InputIt>::value_type>>>
	MBASE_INLINE list(InputIt in_begin, InputIt in_end, const Allocator& in_alloc = Allocator());
	list(const list& in_rhs);
	list(const list& in_rhs, const Allocator& in_alloc);
	list(list&& in_rhs);
	list(list&& in_rhs, const Allocator& in_alloc);
	MBASE_INLINE list(std::initializer_list<value_type> in_list) noexcept;
	MBASE_INLINE ~list() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	list& operator=(const list& in_rhs);
	list& operator=(list&& in_rhs) noexcept;
	list& operator=(std::initializer_list<value_type> in_vals);
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== ITERATOR METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR iterator begin() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR iterator end() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator cbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator cend() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE reverse_iterator rbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR reverse_iterator rend() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_reverse_iterator crbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_reverse_iterator crend() const noexcept;
	/* ===== ITERATOR METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR size_type get_serialized_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference front() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference front() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference back() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference back() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool empty() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type max_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR Allocator get_allocator() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE GENERIC assign(size_type in_count, const_reference in_value);
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<T, typename std::iterator_traits<InputIt>::value_type>>>
	MBASE_INLINE GENERIC assign(InputIt in_begin, InputIt in_end);
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
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, InputIt in_begin, InputIt in_end);
	MBASE_INLINE_EXPR iterator insert(difference_type in_index, const_reference in_object) noexcept;
	MBASE_INLINE_EXPR iterator insert(difference_type in_index, move_reference in_object) noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, std::initializer_list<value_type> in_vals);
	MBASE_INLINE GENERIC _insert_node(const_iterator in_it, node_type* in_node);
	template<typename ... Args>
	MBASE_INLINE iterator emplace(const_iterator in_pos, Args&& ... in_args);
	template<typename ... Args>
	MBASE_INLINE_EXPR GENERIC emplace_front(Args&& ... in_args);
	template<typename ... Args>
	MBASE_INLINE_EXPR GENERIC emplace_back(Args&& ... in_args);
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
	MBASE_INLINE GENERIC remove(const_reference in_value);
	template<typename UnaryPredicate>
	MBASE_INLINE GENERIC remove_if(UnaryPredicate in_p);
	MBASE_INLINE GENERIC reverse() noexcept;
	MBASE_INLINE GENERIC unique() noexcept;

	//template<typename BinaryPredicate>
	//MBASE_INLINE GENERIC unique(BinaryPredicate in_p) noexcept {
	//	list freshList;
	//	for(iterator It = begin(); It != end(); It++)
	//	{

	//	}
	//}
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== NON-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE GENERIC serialize(char_stream& out_buffer) noexcept;
	/* ===== NON-MODIFIER METHODS END ===== */

	/* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
	MBASE_INLINE static mbase::list<T, Allocator> deserialize(IBYTEBUFFER in_src, SIZE_T in_length);
	/* ===== NON-MEMBER FUNCTIONS END ===== */
};

template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::list() noexcept : mFirstNode(nullptr), mLastNode(nullptr), mSize(0) 
{
}

template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::list(const Allocator& in_alloc) : mFirstNode(nullptr), mLastNode(nullptr), mSize(0)
{
}

template<typename T, typename Allocator>
list<T, Allocator>::list(size_type in_count, const T& in_value, const Allocator& in_alloc) : mFirstNode(nullptr), mLastNode(nullptr), mSize(0)
{
	assign(in_count, in_value);
}

template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::list(size_type in_count, const Allocator& in_alloc) : mFirstNode(nullptr), mLastNode(nullptr), mSize(0)
{
	T defaultValue;
	assign(in_count, std::move(defaultValue));
}

template<typename T, typename Allocator>
template<typename InputIt, typename>
MBASE_INLINE list<T, Allocator>::list(InputIt in_begin, InputIt in_end, const Allocator& in_alloc) : mFirstNode(nullptr), mLastNode(nullptr), mSize(0) 
{
	for (in_begin; in_begin != in_end; in_begin++) {
		push_back(*in_begin);
	}
}

template<typename T, typename Allocator>
list<T, Allocator>::list(const list& in_rhs) : mFirstNode(nullptr), mLastNode(nullptr), mSize(0)
{
	clear();
	for(const_iterator cit = in_rhs.cbegin(); cit != in_rhs.cend(); cit++)
	{
		push_back(*cit);
	}
}

template<typename T, typename Allocator>
list<T, Allocator>::list(const list& in_rhs, const Allocator& in_alloc) : mFirstNode(nullptr), mLastNode(nullptr), mSize(0)
{
	clear();
	for (const_iterator cit = in_rhs.cbegin(); cit != in_rhs.cend(); cit++)
	{
		push_back(*cit);
	}
}

template<typename T, typename Allocator>
list<T, Allocator>::list(list&& in_rhs) : mFirstNode(in_rhs.mFirstNode), mLastNode(in_rhs.mLastNode), mSize(in_rhs.mSize) {
	in_rhs.mSize = 0;
	in_rhs.mFirstNode = nullptr;
	in_rhs.mLastNode = nullptr;
}

template<typename T, typename Allocator>
list<T, Allocator>::list(list&& in_rhs, const Allocator& in_alloc) : mFirstNode(in_rhs.mFirstNode), mLastNode(in_rhs.mLastNode), mSize(in_rhs.mSize)
{
	in_rhs.mSize = 0;
	in_rhs.mFirstNode = nullptr;
	in_rhs.mLastNode = nullptr;
}


template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::list(std::initializer_list<value_type> in_list) noexcept  : mFirstNode(nullptr), mLastNode(nullptr), mSize(0)
{
	assign(std::move(in_list));
}

template<typename T, typename Allocator>
MBASE_INLINE list<T, Allocator>::~list() noexcept
{
	clear();
}

template<typename T, typename Allocator>
list<T, Allocator>& list<T, Allocator>::operator=(const list& in_rhs) 
{
	clear();
	for(const_iterator cit = in_rhs.cbegin(); cit != in_rhs.cend(); cit++)
	{
		push_back(*cit);
	}

	return *this;
}

template<typename T, typename Allocator>
list<T, Allocator>& list<T, Allocator>::operator=(list&& in_rhs) noexcept 
{
	clear();

	mSize = in_rhs.mSize;
	mFirstNode = in_rhs.mFirstNode;
	mLastNode = in_rhs.mLastNode;

	in_rhs.mSize = 0;
	in_rhs.mFirstNode = nullptr;
	in_rhs.mLastNode = nullptr;

	return *this;
}

template<typename T, typename Allocator>
list<T, Allocator>& list<T, Allocator>::operator=(std::initializer_list<value_type> in_vals)
{
	assign(std::move(in_vals));
	return *this;
}


template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::begin() noexcept 
{
	return iterator(mFirstNode);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::end() noexcept 
{
	return iterator(nullptr);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename list<T, Allocator>::const_iterator list<T, Allocator>::cbegin() const noexcept
{
	return const_iterator(mFirstNode);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename list<T, Allocator>::const_iterator list<T, Allocator>::cend() const noexcept
{
	return const_iterator(nullptr);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename list<T, Allocator>::reverse_iterator list<T, Allocator>::rbegin() const noexcept
{
	return reverse_iterator(mLastNode);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename list<T, Allocator>::reverse_iterator list<T, Allocator>::rend() const noexcept
{
	return reverse_iterator(nullptr);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename list<T, Allocator>::const_reverse_iterator list<T, Allocator>::crbegin() const noexcept
{
	return const_reverse_iterator(mLastNode);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename list<T, Allocator>::const_reverse_iterator list<T, Allocator>::crend() const noexcept
{
	return const_reverse_iterator(nullptr);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE_EXPR typename list<T, Allocator>::size_type list<T, Allocator>::get_serialized_size() const noexcept
{
	serialize_helper<value_type> sh;
	size_type totalSize = 0;
	for (iterator It = begin(); It != end(); It++)
	{
		sh.value = It.get();
		totalSize += sh.get_serialized_size() + gSerializedListBlockLength; // 4 is block length indicator
	}
	return totalSize;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename list<T, Allocator>::reference list<T, Allocator>::front() noexcept 
{
	return *mFirstNode->data;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename list<T, Allocator>::const_reference list<T, Allocator>::front() const noexcept
{
	return *mFirstNode->data;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename list<T, Allocator>::reference list<T, Allocator>::back() noexcept
{
	return *mLastNode->data;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename list<T, Allocator>::const_reference list<T, Allocator>::back() const noexcept
{
	return *mLastNode->data;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool list<T, Allocator>::empty() const noexcept
{
	return mSize == 0;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename list<T, Allocator>::size_type list<T, Allocator>::size() const noexcept
{
	return mSize;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename list<T, Allocator>::size_type list<T, Allocator>::max_size() const noexcept
{
	size_type result = (std::numeric_limits<difference_type>::max)();
	return result;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR Allocator list<T, Allocator>::get_allocator() const noexcept 
{
	return Allocator();
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::assign(size_type in_count, const_reference in_value) 
{
	clear();
	for(size_type i = 0; i < in_count; i++)
	{
		push_back(in_value);
	}
}

template<typename T, typename Allocator>
template<typename InputIt, typename>
MBASE_INLINE GENERIC list<T, Allocator>::assign(InputIt in_begin, InputIt in_end)
{
	clear();
	for (in_begin; in_begin != in_end; in_begin++)
	{
		push_back(*in_begin);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::assign(std::initializer_list<value_type> in_vals) 
{
	if(!in_vals.size())
	{
		clear();
		return;
	}

	auto iListIter = in_vals.begin();
	for(iListIter; iListIter != in_vals.end(); iListIter++)
	{
		push_back(*iListIter);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::clear() noexcept 
{
	while (mSize)
	{
		pop_back();
	}
	mFirstNode = nullptr;
	mLastNode = nullptr;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::push_back(const_reference in_data) noexcept
{
	node_type* newNode = new node_type(in_data);
	newNode->prev = mLastNode;
	if (mLastNode)
	{
		mLastNode->next = newNode;
	}
	else
	{
		mLastNode = newNode;
		mFirstNode = mLastNode;
	}
	mLastNode = newNode;
	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::push_back(move_reference in_data) noexcept 
{
	node_type* newNode = new node_type(std::move(in_data));
	newNode->prev = mLastNode;
	if (mLastNode)
	{
		mLastNode->next = newNode;
	}
	else
	{
		mLastNode = newNode;
		mFirstNode = mLastNode;
	}
	mLastNode = newNode;
	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::push_front(const_reference in_data) noexcept 
{
	node_type* newNode = new node_type(in_data);
	newNode->next = mFirstNode;
	if (mFirstNode)
	{
		mFirstNode->prev = newNode;
	}
	else
	{
		mFirstNode = newNode;
		mLastNode = mFirstNode;
	}

	mFirstNode = newNode;
	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::push_front(move_reference in_data) noexcept 
{
	node_type* newNode = new node_type(std::move(in_data));
	newNode->next = mFirstNode;
	if (mFirstNode)
	{
		mFirstNode->prev = newNode;
	}
	else
	{
		mFirstNode = newNode;
		mLastNode = mFirstNode;
	}

	mFirstNode = newNode;
	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::pop_back() noexcept 
{
	if (!(--mSize))
	{
		delete mLastNode;
		mLastNode = nullptr;
		mFirstNode = nullptr;
		return;
	}
	node_type* newmLastNode = mLastNode->prev;
	newmLastNode->next = nullptr;
	delete mLastNode;
	mLastNode = newmLastNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::pop_front() noexcept 
{
	if (!(--mSize))
	{
		delete mFirstNode;
		mLastNode = nullptr;
		mFirstNode = nullptr;
		return;
	}

	node_type* newmFirstNode = mFirstNode->next;
	newmFirstNode->prev = nullptr;
	delete mFirstNode;
	mFirstNode = newmFirstNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(const_iterator in_pos, const_reference in_object) noexcept
{
	if(in_pos == cend())
	{
		push_back(in_object);
		return iterator(mLastNode);
	}

	node_type* mNode = const_cast<node_type*>(in_pos._get_node());
	node_type* newNode = new node_type(in_object);
	newNode->prev = mNode->prev;
	if (mNode->prev)
	{
		mNode->prev->next = newNode;
	}

	else
	{
		mFirstNode = newNode;
	}

	mNode->prev = newNode;
	newNode->next = mNode;
	++mSize;
	return iterator(newNode);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(const_iterator in_pos, move_reference in_object) noexcept 
{
	if (in_pos == cend())
	{
		push_back(std::move(in_object));
		return iterator(mLastNode);
	}

	node_type* mNode = const_cast<node_type*>(in_pos._get_node());
	node_type* newNode = new node_type(std::move(in_object));
	newNode->prev = mNode->prev;
	if (mNode->prev)
	{
		mNode->prev->next = newNode;
	}

	else
	{
		mFirstNode = newNode;
	}

	mNode->prev = newNode;
	newNode->next = mNode;
	++mSize;
	return iterator(newNode);
}

template<typename T, typename Allocator>
template<typename InputIt, typename>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(const_iterator in_pos, InputIt in_begin, InputIt in_end) 
{
	iterator inLast(mLastNode);
	for (in_begin; in_begin != in_end; in_begin++)
	{
		inLast = insert(in_pos, *in_begin);
	}

	return inLast;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(const_iterator in_pos, size_type in_count, const_reference in_object) 
{
	if(!in_count)
	{
		return end();
	}

	iterator activeIt(mFirstNode);
	for (size_type i = 0; i < in_count; i++) 
	{
		activeIt = insert(in_pos, in_object);
	}
	return activeIt;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(difference_type in_index, const_reference in_object) noexcept 
{
	iterator in_pos = begin();
	in_pos += in_index;

	return insert(in_pos, in_object);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(difference_type in_index, move_reference in_object) noexcept 
{
	iterator in_pos = begin();
	in_pos += in_index;

	return insert(in_pos, std::move(in_object));
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::insert(const_iterator in_pos, std::initializer_list<value_type> in_vals)
{
	return insert(in_pos, in_vals.begin(), in_vals.end());
}

template<typename T, typename Allocator>
template<typename ... Args>
MBASE_INLINE typename list<T, Allocator>::iterator list<T, Allocator>::emplace(const_iterator in_pos, Args&& ... in_args)
{
	return insert(in_pos, std::move(value_type(std::forward<Args>(in_args)...)));
}

template<typename T, typename Allocator>
template<typename ... Args>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::emplace_front(Args&& ... in_args) 
{
	push_front(std::move(value_type(std::forward<Args>(in_args)...)));
}

template<typename T, typename Allocator>
template<typename ... Args>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::emplace_back(Args&& ... in_args) 
{
	push_back(std::move(value_type(std::forward<Args>(in_args)...)));
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::erase(iterator in_pos) noexcept 
{
	node_type* mNode = in_pos._get_node();
	node_type* returnedNode = mNode->next;

	if (mNode == mFirstNode)
	{
		pop_front();
	}

	else if (mNode == mLastNode)
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
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::erase(const_iterator in_pos) 
{
	node_type* mNode = const_cast<node_type*>(in_pos._get_node());
	node_type* returnedNode = mNode->next;

	if (mNode == mFirstNode)
	{
		pop_front();
	}

	else if (mNode == mLastNode)
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
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::erase(iterator in_begin, iterator in_end) 
{
	iterator removedNode(mLastNode);
	for(in_begin; in_begin != in_end; in_begin++) {
		in_begin = erase(in_begin);
		removedNode = in_begin;
	}
	return removedNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename list<T, Allocator>::iterator list<T, Allocator>::erase(const_iterator in_begin, const_iterator in_end) 
{
	iterator removedNode(mLastNode);
	for (in_begin; in_begin != in_end; in_begin++) {
		in_begin = erase(in_begin);
		removedNode = in_begin;
	}
	return removedNode;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list& in_rhs) 
{
	iterator in_begin = in_rhs.begin();
	for(in_begin; in_begin != in_rhs.end();)
	{
		node_type* nt = in_begin._get_node();
		in_begin++;
		_insert_node(in_pos, nt);
	}

	list newList(std::move(in_rhs));

	in_rhs.mSize = 0;
	in_rhs.mFirstNode = nullptr;
	in_rhs.mLastNode = nullptr;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list&& in_rhs) 
{
	iterator in_begin = in_rhs.begin();
	for (in_begin; in_begin != in_rhs.end();)
	{
		node_type* nt = in_begin._get_node();
		in_begin++;
		_insert_node(in_pos, nt);
	}

	list newList(std::move(in_rhs));

	in_rhs.mSize = 0;
	in_rhs.mFirstNode = nullptr;
	in_rhs.mLastNode = nullptr;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list& in_rhs, const_iterator in_it) 
{
	insert(in_pos, *in_it);
	in_rhs.erase(in_it);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list&& in_rhs, const_iterator in_it) 
{
	insert(in_pos, *in_it);
	in_rhs.erase(in_it);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::splice(const_iterator in_pos, list&& in_rhs, const_iterator in_begin, const_iterator in_end) 
{
	for(in_begin; in_begin != in_end; in_begin++)
	{
		insert(in_pos, *in_begin);
	}

	in_rhs.erase(in_begin, in_end);
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::remove(const_reference in_value) 
{
	const_iterator in_begin = cbegin();
	for(in_begin; in_begin != cend(); in_begin++)
	{
		if (*in_begin == in_value)
		{
			erase(in_begin);
			return;
		}
	}
}

template<typename T, typename Allocator>
template<typename UnaryPredicate>
MBASE_INLINE GENERIC list<T, Allocator>::remove_if(UnaryPredicate in_p) 
{
	const_iterator in_begin = cbegin();
	for (in_begin; in_begin != cend(); in_begin++)
	{
		if (in_p(*in_begin))
		{
			erase(in_begin);
			return;
		}
	}
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::reverse() noexcept
{
	if (!mSize)
	{
		return;
	}

	node_type* current = mFirstNode;
	node_type* temp = nullptr;

	while (current != nullptr) {
		temp = current->prev;
		current->prev = current->next;
		current->next = temp;
		current = current->prev;
	}

	if (temp != nullptr) {
		mFirstNode = temp->prev;
		mLastNode = temp->next;
	}
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::unique() noexcept
{
	if (!mSize)
	{
		return;
	}

	list freshList;
	for (iterator It = begin(); It != end(); It++)
	{
		if (std::find(freshList.begin(), freshList.end(), *It) == freshList.end())
		{
			freshList.push_back(std::move(*It));
		}
	}
	*this = std::move(freshList);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC list<T, Allocator>::swap(list& in_src) noexcept 
{
	std::swap(mFirstNode, in_src.mFirstNode);
	std::swap(mLastNode, in_src.mLastNode);
	std::swap(mSize, in_src.mSize);
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::serialize(char_stream& out_buffer) noexcept 
{
	if (mSize)
	{
		size_type serializedSize = get_serialized_size();
		if (out_buffer.buffer_length() < serializedSize)
		{
			// BUFFER LENGTH IS NOT ENOUGH TO HOLD SERIALIZED DATA
			return;
		}

		serialize_helper<value_type> serHelper;

		for (iterator It = begin(); It != end(); It++)
		{
			serHelper.value = It.get();

			I32 blockLength = serHelper.get_serialized_size();
			out_buffer.put_datan(reinterpret_cast<IBYTEBUFFER>(&blockLength), sizeof(I32));
			serHelper.serialize(out_buffer);
		}

		out_buffer.set_cursor_front();
	}
}

template<typename T, typename Allocator>
MBASE_INLINE mbase::list<T, Allocator> list<T, Allocator>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length) 
{
	mbase::list<T, Allocator> deserializedContainer;
	if (in_length)
	{
		serialize_helper<value_type> serHelper;
		char_stream inBuffer(in_src, in_length);

		inBuffer.set_cursor_end();
		inBuffer.advance();

		IBYTEBUFFER eofBuffer = inBuffer.get_bufferc();
		inBuffer.set_cursor_front();

		while (inBuffer.get_bufferc() < eofBuffer)
		{
			I32 blockLength = *inBuffer.get_bufferc();
			inBuffer.advance(sizeof(I32));
			IBYTEBUFFER blockData = inBuffer.get_bufferc();
			deserializedContainer.push_back(std::move(serHelper.deserialize(blockData, blockLength)));
			inBuffer.advance(blockLength);
		}
	}

	return deserializedContainer;
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::_push_back_node(node_type* in_node) 
{
	in_node->prev = mLastNode;
	if(mLastNode)
	{
		mLastNode->next = in_node;
	}
	else
	{
		mLastNode = in_node;
		mFirstNode = mLastNode;
	}
	in_node->next = nullptr;
	mLastNode = in_node;
	++mSize;
}

template<typename T, typename Allocator>
MBASE_INLINE GENERIC list<T, Allocator>::_insert_node(const_iterator in_it, node_type* in_node) 
{
	if(in_it == cend())
	{
		_push_back_node(in_node);
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
		mFirstNode = in_node;
	}

	mNode->prev = in_node;
	in_node->next = mNode;
	++mSize;
}

MBASE_STD_END

#endif // !MBASE_LIST_H
