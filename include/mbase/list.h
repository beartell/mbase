#ifndef MBASE_LIST_H
#define MBASE_LIST_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <initializer_list>

MBASE_STD_BEGIN

/* CONTAINER REQUIREMENTS */
// default constructible
// copy constructible
// equality constructible
// swappable

/* TYPE REQUIREMENTS */
// copy insertable
// equality comparable
// destructible

template<typename T, typename Allocator = mbase::allocator_simple<T>>
class list {
public:
	using value_type = T;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using reference = value_type&;
	using const_reference = const reference;
	using move_reference = value_type&&;
	using pointer = value_type*;
	using const_pointer = const pointer;

	struct list_node {
		list_node* prev;
		list_node* next;
		pointer data;
	};

	list(std::initializer_list<value_type> in_list) noexcept {

	}

private:
	list_node* firstNode;
	size_type mSize;
	size_type mCapacity;
};

MBASE_STD_END

#endif // !MBASE_LIST_H
