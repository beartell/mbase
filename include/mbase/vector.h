#ifndef MBASE_VECTOR_H
#define MBASE_VECTOR_H

/* DO NOT FORGET TO IMPLEMENT THE ERASE FUNCTION */

#include <mbase/common.h> // For data types and macros
#include <mbase/allocator.h> // For allocation routines
#include <mbase/safe_buffer.h> // For safe_buffer
#include <mbase/container_serializer_helper.h> // For serialize_helper
#include <mbase/sequence_iterator.h>

#include <initializer_list> // For std::initializer_list

MBASE_STD_BEGIN

/* --- OBJECT BEHAVIOURS --- */

/*
	Default Constructible
	Move Constructible
	Copy Constructible
	Destructible
	Copy Assignable
	Move Assignable
	Swappable
	Allocate Aware
	Equality Comparable
	Templated
	Serializable
	Type Aware
	Iterable
	Debuggable
*/

/* --- OBJECT BEHAVIOURS --- */

template<typename T, typename Allocator = mbase::allocator<T>>
class vector {
public:
	using value_type = T;
	using const_type = const T;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using reference = T&;
	using const_reference = const T&;
	using move_reference = T&&;
	using pointer = T*;
	using const_pointer = const T*;
	using allocator_type = Allocator;
	using iterator = typename sequence_iterator<T>;
	using const_iterator = typename const_sequence_iterator<T>;
	using reverse_iterator = typename reverse_sequence_iterator<T>;
	using const_reverse_iterator = typename const_reverse_sequence_iterator<T>;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR MBASE_EXPLICIT vector() noexcept;
	MBASE_INLINE_EXPR MBASE_EXPLICIT vector(const Allocator& in_alloc) noexcept;
	MBASE_INLINE_EXPR vector(size_type in_size, const T& in_value, const Allocator& in_alloc = Allocator());
	MBASE_INLINE_EXPR MBASE_EXPLICIT vector(size_type in_size, const Allocator& in_alloc = Allocator());
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<T, typename std::iterator_traits<InputIt>::value_type>>>
	MBASE_INLINE_EXPR vector(InputIt in_first, InputIt in_end, const Allocator& in_alloc = Allocator()) : raw_data(nullptr), mSize(0), mCapacity(4), externalAllocator(in_alloc) {
		raw_data = externalAllocator.allocate(mCapacity);
		for (in_first; in_first != in_end; in_first++)
		{
			push_back(*in_first);
		}
	}
	MBASE_INLINE_EXPR vector(std::initializer_list<value_type> in_list, const Allocator& in_alloc = Allocator()) noexcept;
	MBASE_INLINE_EXPR vector(const vector& in_rhs) noexcept;
	MBASE_INLINE_EXPR vector(const vector& in_rhs, const Allocator& in_alloc) noexcept;
	MBASE_INLINE_EXPR vector(vector&& in_rhs) noexcept;
	MBASE_INLINE_EXPR vector(vector&& in_rhs, const Allocator& in_alloc) noexcept;
	MBASE_INLINE_EXPR ~vector() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR vector& operator=(const vector& in_rhs) noexcept;
	MBASE_INLINE_EXPR vector& operator=(vector&& in_rhs) noexcept;
	MBASE_INLINE_EXPR vector& operator=(std::initializer_list<value_type> in_list) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== ITERATOR METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR iterator begin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR iterator end() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator cbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_iterator cend() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR reverse_iterator rbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR reverse_iterator rend() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_reverse_iterator crbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR const_reverse_iterator crend() const noexcept;
	/* ===== ITERATOR METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type max_size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type size() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type capacity() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool empty() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR allocator_type get_allocator() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference back() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference front() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference back() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference front() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR pointer data() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_pointer data() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference at(size_type in_pos) noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference at(size_type in_pos) const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) reference operator[](difference_type in_index) noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) const_reference operator[](difference_type in_index) const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	template<typename InputIt>
	MBASE_INLINE_EXPR GENERIC assign(InputIt in_begin, InputIt in_end) {
		clear();
		for(in_begin; in_begin != in_end; in_begin++)
		{
			push_back(*in_begin);
		}
	}
	MBASE_INLINE_EXPR GENERIC assign(size_type in_size, const T& in_value);
	MBASE_INLINE_EXPR GENERIC assign(std::initializer_list<value_type> in_list);
	MBASE_INLINE_EXPR GENERIC swap(mbase::vector<value_type, Allocator>& in_src) noexcept;
	MBASE_INLINE_EXPR GENERIC clear() noexcept;
	MBASE_INLINE_EXPR GENERIC resize(size_type in_size) noexcept;
	MBASE_INLINE_EXPR GENERIC resize(size_type in_size, const T& in_value) noexcept; // IMPLEMENT ONE DAY
	MBASE_INLINE_EXPR GENERIC shrink_to_fit();
	MBASE_INLINE_EXPR GENERIC reserve(size_type in_capacity) noexcept;
	MBASE_INLINE_EXPR iterator erase(iterator in_pos) noexcept;
	MBASE_INLINE_EXPR iterator erase(iterator in_begin, iterator in_end) noexcept;
	MBASE_INLINE_EXPR GENERIC push_back(const T& in_val) noexcept;
	MBASE_INLINE_EXPR GENERIC push_back(T&& in_val) noexcept;
	MBASE_INLINE_EXPR GENERIC pop_back() noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, const T& in_val) noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, T&& in_val) noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, size_type in_count, const T& in_val) noexcept;
	template<typename InputIt, typename = std::enable_if_t<std::is_constructible_v<T, typename std::iterator_traits<InputIt>::value_type>>>
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, InputIt in_begin, InputIt in_end) noexcept {
		if (in_pos == cend())
		{
			I32 awayFromEnd = 0;
			for(in_begin; in_begin != in_end; in_begin++)
			{
				push_back(*in_begin);
				++awayFromEnd;
			}
			
			return end() - awayFromEnd;
		}

		else
		{
			if (mSize)
			{
				vector vc(mCapacity);
				I32 iterIndex = 0;
				I32 foundIndex = 0;
				iterator mPos = const_cast<pointer>(in_pos.get());

				for (iterator It = begin(); It != end(); It++)
				{
					if (mPos == It)
					{
						foundIndex = iterIndex;
						for(in_begin; in_begin != in_end; in_begin++)
						{
							vc.push_back(*in_begin);
						}
					}
					else
					{
						vc.push_back(*It);
					}
					iterIndex++;
				}

				*this = std::move(vc);
				return iterator(raw_data + foundIndex);
			}
		}

		return begin();
	}
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, std::initializer_list<value_type> in_list) noexcept;
	template<typename ... Args>
	MBASE_INLINE_EXPR iterator emplace(const_iterator in_pos, Args&&... in_args) noexcept {
		return insert(in_pos, std::move(value_type(std::forward<Args>(in_args)...)));
	}
	template<typename ... Args>
	MBASE_INLINE_EXPR reference emplace_back(Args&& ... in_args) {
		push_back(std::move(value_type(std::forward<Args>(in_args)...)));
		return *(end() - 1);
	}
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== NON-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR GENERIC serialize(safe_buffer& out_buffer) noexcept;
	/* ===== NON-MODIFIER METHODS END ===== */

	/* ===== NON-MEMBER FUNCTIONS BEGIN ===== */
	MBASE_INLINE_EXPR static mbase::vector<T, Allocator> deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept;
	/* ===== NON-MEMBER FUNCTIONS END ===== */

private:
	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR GENERIC deep_clear() noexcept;
	MBASE_INLINE_EXPR GENERIC deep_clear_and_prepare() noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	Allocator externalAllocator;
	pointer raw_data;
	SIZE_T mSize;
	SIZE_T mCapacity;
};

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::vector() noexcept : raw_data(nullptr), mSize(0), mCapacity(4) 
{
	raw_data = externalAllocator.allocate(mCapacity); // default capacity
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::vector(const Allocator& in_alloc) noexcept : raw_data(nullptr), mSize(0), mCapacity(4), externalAllocator(in_alloc) 
{
	raw_data = externalAllocator.allocate(mCapacity); // default capacity
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::vector(size_type in_size, const T& in_value, const Allocator& in_alloc) : raw_data(nullptr), mSize(0), mCapacity(4), externalAllocator(in_alloc) 
{
	if (in_size <= 0)
	{
		raw_data = externalAllocator.allocate(mCapacity);
		return;
	}
	mCapacity = in_size * 2;
	raw_data = externalAllocator.allocate(mCapacity);
	for(difference_type i = 0; i < in_size; i++)
	{
		push_back(in_value);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::vector(size_type in_size, const Allocator& in_alloc) : raw_data(nullptr), mSize(0), mCapacity(4), externalAllocator(in_alloc) 
{
	if (in_size <= 0)
	{
		raw_data = externalAllocator.allocate(mCapacity);
		return;
	}
	mCapacity = in_size * 2;
	raw_data = externalAllocator.allocate(mCapacity);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::vector(std::initializer_list<value_type> in_list, const Allocator& in_alloc) noexcept
{
	mSize = 0;
	mCapacity = in_list.size() * 2;
	externalAllocator = in_alloc;

	if(!mCapacity)
	{
		mCapacity = 4;
		raw_data = externalAllocator.allocate(mCapacity);
		return;
	}

	const value_type* currentObj = in_list.begin();
	raw_data = externalAllocator.allocate(mCapacity);

	while (currentObj != in_list.end())
	{
		push_back(*currentObj);
		currentObj++;
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::vector(const vector& in_rhs) noexcept 
{
	mSize = in_rhs.mSize;
	mCapacity = in_rhs.mCapacity;
	// SELF NOTE = SHOULD I COPY THE ALLOCATOR ? such as "externalAllocator = in_rhs.externalAllocator";
	if (!mCapacity)
	{
		mCapacity = 4;
	}

	raw_data = externalAllocator.allocate(mCapacity);
	for (difference_type i = 0; i < mSize; i++)
	{
		externalAllocator.construct(raw_data + i, in_rhs[i]);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::vector(const vector& in_rhs, const Allocator& in_alloc) noexcept 
{
	mSize = in_rhs.mSize;
	mCapacity = in_rhs.mCapacity;
	externalAllocator = in_alloc;

	if (!mCapacity)
	{
		mCapacity = 4;
	}

	raw_data = externalAllocator.allocate(mCapacity);
	for (difference_type i = 0; i < mSize; i++)
	{
		externalAllocator.construct(raw_data + i, in_rhs[i]);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::vector(vector&& in_rhs) noexcept
{
	mSize = in_rhs.mSize;
	mCapacity = in_rhs.mCapacity;
	raw_data = in_rhs.raw_data;

	in_rhs.raw_data = nullptr;
	in_rhs.mSize = 0;
	in_rhs.mCapacity = 0;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::vector(vector&& in_rhs, const Allocator& in_alloc) noexcept
{
	mSize = in_rhs.mSize;
	mCapacity = in_rhs.mCapacity;
	raw_data = in_rhs.raw_data;
	externalAllocator = in_alloc;

	in_rhs.raw_data = nullptr;
	in_rhs.mSize = 0;
	in_rhs.mCapacity = 0;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>::~vector() noexcept
{
	deep_clear();
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>& vector<T, Allocator>::operator=(const vector& in_rhs) noexcept
{
	if (!in_rhs.mSize)
	{
		clear();
		shrink_to_fit();
		return *this;
	}

	deep_clear();
	mCapacity = 4;
	if(in_rhs.mCapacity)
	{
		mCapacity = in_rhs.mCapacity;
	}

	mSize = in_rhs.mSize;

	raw_data = externalAllocator.allocate(mCapacity);
	for (difference_type i = 0; i < mSize; i++)
	{
		externalAllocator.construct(raw_data + i, in_rhs[i]);
	}
	return *this;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>& vector<T, Allocator>::operator=(vector&& in_rhs) noexcept
{
	deep_clear();

	mSize = in_rhs.mSize;
	mCapacity = in_rhs.mCapacity;

	raw_data = in_rhs.raw_data;
	in_rhs.raw_data = nullptr;
	in_rhs.mSize = 0;
	in_rhs.mCapacity = 0;

	return *this;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR vector<T, Allocator>& vector<T, Allocator>::operator=(std::initializer_list<value_type> in_list) noexcept
{
	deep_clear();

	mSize = 0;
	mCapacity = in_list.size() * 2;

	if (!mCapacity)
	{
		mCapacity = 4;
		raw_data = externalAllocator.allocate(mCapacity);
		return;
	}

	const value_type* currentObj = in_list.begin();
	raw_data = externalAllocator.allocate(mCapacity);

	while (currentObj != in_list.end())
	{
		push_back(*currentObj);
		currentObj++;
	}
	
	return *this;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename vector<T, Allocator>::iterator vector<T, Allocator>::begin() const noexcept 
{
	return iterator(raw_data);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename vector<T, Allocator>::iterator vector<T, Allocator>::end() const noexcept 
{
	return iterator(raw_data + mSize);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename vector<T, Allocator>::const_iterator vector<T, Allocator>::cbegin() const noexcept 
{
	return const_iterator(raw_data);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename vector<T, Allocator>::const_iterator vector<T, Allocator>::cend() const noexcept 
{
	return const_iterator(raw_data + mSize);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename vector<T, Allocator>::reverse_iterator vector<T, Allocator>::rbegin() const noexcept
{
	return reverse_iterator(raw_data + (mSize - 1));
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename vector<T, Allocator>::reverse_iterator vector<T, Allocator>::rend() const noexcept 
{
	return reverse_iterator(raw_data - 1);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename vector<T, Allocator>::const_reverse_iterator vector<T, Allocator>::crbegin() const noexcept 
{
	return const_reverse_iterator(raw_data + (mSize - 1));
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR typename vector<T, Allocator>::const_reverse_iterator vector<T, Allocator>::crend() const noexcept
{
	return const_reverse_iterator(raw_data - 1);
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::size_type vector<T, Allocator>::max_size() const noexcept
{
	size_type result = (std::numeric_limits<difference_type>::max)();
	return result;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::size_type vector<T, Allocator>::size() const noexcept
{
	return mSize;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::size_type vector<T, Allocator>::capacity() const noexcept 
{
	return mCapacity;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool vector<T, Allocator>::empty() const noexcept
{
	return mSize == 0;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::allocator_type vector<T, Allocator>::get_allocator() const noexcept 
{
	return externalAllocator;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::reference vector<T, Allocator>::back() noexcept
{
	return raw_data[mSize - 1];
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::reference vector<T, Allocator>::front() noexcept
{
	return raw_data[0];
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::const_reference vector<T, Allocator>::back() const noexcept
{
	return raw_data[mSize - 1];
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::const_reference vector<T, Allocator>::front() const noexcept
{
	return raw_data[0];
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::pointer vector<T, Allocator>::data() noexcept
{
	return raw_data;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::const_pointer vector<T, Allocator>::data() const noexcept
{
	return raw_data;
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::reference vector<T, Allocator>::at(size_type in_pos) noexcept
{
	return raw_data[in_pos];
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename vector<T, Allocator>::const_reference vector<T, Allocator>::at(size_type in_pos) const noexcept
{
	return raw_data[in_pos];
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) typename vector<T, Allocator>::reference vector<T, Allocator>::operator[](difference_type in_index) noexcept
{
	return raw_data[in_index];
}

template<typename T, typename Allocator>
MBASE_ND(MBASE_OBS_IGNORE) typename vector<T, Allocator>::const_reference vector<T, Allocator>::operator[](difference_type in_index) const noexcept
{
	return raw_data[in_index];
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::assign(size_type in_size, const T& in_value) 
{
	clear();
	for (difference_type i = 0; i < in_size; i++)
	{
		push_back(in_value);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::assign(std::initializer_list<value_type> in_list) 
{
	clear();
	const value_type* currentObj = in_list.begin();
	for(currentObj; currentObj != in_list.end(); currentObj++)
	{
		push_back(*currentObj);
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::swap(mbase::vector<value_type, Allocator>& in_src) noexcept 
{
	std::swap(raw_data, in_src.raw_data);
	std::swap(mCapacity, in_src.mCapacity);
	std::swap(mSize, in_src.mSize);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::clear() noexcept 
{
	for(size_type i = 0; i < mSize; i++)
	{
		raw_data[i].~value_type();
	}
	mSize = 0;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::deep_clear() noexcept 
{
	if (raw_data)
	{
		for (difference_type i = 0; i < mSize; i++)
		{
			raw_data[i].~value_type();
		}
		externalAllocator.deallocate(raw_data, 0);
		raw_data = nullptr;
	}
	mSize = 0;
	mCapacity = 0;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::deep_clear_and_prepare() noexcept {
	deep_clear();
	mCapacity = 4;
	raw_data = externalAllocator.allocate(mCapacity);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::resize(size_type in_size) noexcept 
{
	if (in_size > mCapacity)
	{
		reserve(in_size * 2);
	}
	size_type sizeDifference = mSize - in_size;
	for(difference_type i = 0; i < sizeDifference; i++)
	{
		pop_back();
	}

	mSize = in_size;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::shrink_to_fit()
{
	if(mSize == mCapacity)
	{
		// DO NOTHING LITERALLY
		return;
	}

	if(!mSize)
	{
		deep_clear_and_prepare();
		return;
	}

	I32 sizeDivident = mSize / 4;
	I32 capacityDivident = mCapacity / 4;
	// IT MUST BE 1 IF THE CAPACITY IS ALREADY FIT
	// IF GREATER THAN 1, SHRINK IT
	I32 growthFactor = capacityDivident - sizeDivident;

	if(growthFactor == 1)
	{
		// DO NOTHING LITERALLY
		return;
	}
	else
	{
		I32 newCapacity = ((mSize + 3) / 4) * 8;
		pointer new_data = externalAllocator.allocate(newCapacity);
		
		for(difference_type i = 0; i < mSize; i++)
		{
			externalAllocator.construct(new_data + i, std::move(*(raw_data + i)));
			raw_data[i].~value_type();
		}
		externalAllocator.deallocate(raw_data, 0);
		raw_data = new_data;
		
		mCapacity = newCapacity;
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::reserve(size_type in_capacity) noexcept 
{
	if (in_capacity <= mCapacity)
	{
		return;
	}

	pointer new_data = externalAllocator.allocate(in_capacity);
	for (difference_type i = 0; i < mSize; i++)
	{
		externalAllocator.construct(new_data + i, std::move(*(raw_data + i)));
		raw_data[i].~value_type();
	}

	externalAllocator.deallocate(raw_data, 0);

	mCapacity = in_capacity;
	raw_data = new_data;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(iterator in_pos) noexcept
{
	if (in_pos == end())
	{
		pop_back();
		return end();
	}

	else
	{
		if (mSize)
		{
			vector vc(mCapacity);
			for(iterator It = begin(); It != end(); It++)
			{
				if(It == in_pos)
				{
					continue;
				}
				vc.push_back(std::move(*It));
			}
			*this = std::move(vc);
		}
	}
	return end();
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(iterator in_begin, iterator in_end) noexcept 
{
	if(in_begin == in_end)
	{
		return erase(in_begin);
	}
	else if(in_begin > in_end)
	{
		return end();
	}

	vector newVector;
	iterator itBegin = begin();

	for(itBegin; itBegin != in_begin; itBegin++)
	{
		newVector.push_back(std::move(*itBegin));
	}
	itBegin = in_end;
	for(itBegin; itBegin != end(); itBegin++)
	{
		newVector.push_back(std::move(*itBegin));
	}
	*this = std::move(newVector);
	return begin();
}


template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(const_iterator in_pos, const T& in_val) noexcept 
{
	return insert(in_pos, 1, in_val);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(const_iterator in_pos, T&& in_val) noexcept 
{
	return insert(in_pos, 1, std::move(in_val));
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(const_iterator in_pos, size_type in_count, const T& in_val) noexcept
{
	if(!in_count)
	{
		return begin();
	}

	if (in_pos == cend())
	{
		I32 awayFromEnd = 0;
		for (difference_type i = 0; i < in_count; i++)
		{
			push_back(in_val);
			++awayFromEnd;
		}

		return end() - awayFromEnd;
	}

	else
	{
		if (mSize)
		{
			vector vc(mCapacity);
			I32 iterIndex = 0;
			I32 foundIndex = 0;
			iterator mPos = const_cast<pointer>(in_pos.get());

			for (iterator It = begin(); It != end(); It++)
			{
				if (mPos == It)
				{
					foundIndex = iterIndex;
					for(I32 i = 0; i < in_count; i++)
					{
						vc.push_back(in_val);
					}
				}
				else
				{
					vc.push_back(*It);
				}
				++iterIndex;
			}

			*this = std::move(vc);
			return iterator(raw_data + foundIndex);
		}
	}

	return begin();
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(const_iterator in_pos, std::initializer_list<value_type> in_list) noexcept
{
	return insert(in_pos, in_list.begin(), in_list.end());
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::push_back(const T& in_val) noexcept
{
	if (mSize == mCapacity)
	{
		reserve(2 * mCapacity + 1);
	}
	pointer curObj = raw_data + mSize++;
	externalAllocator.construct(curObj, in_val);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::push_back(T&& in_val) noexcept 
{
	if (mSize == mCapacity)
	{
		reserve(2 * mCapacity + 1);
	}
	pointer curObj = raw_data + mSize++;
	externalAllocator.construct(curObj, std::move(in_val));
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::pop_back() noexcept 
{
	if (!mSize)
	{
		return;
	}

	pointer curObj = raw_data + --mSize;
	curObj->~value_type();
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::serialize(safe_buffer& out_buffer) noexcept
{
	if (mSize)
	{
		mbase::vector<safe_buffer> totalBuffer;

		serialize_helper<value_type> sl;

		safe_buffer tmpSafeBuffer;
		size_type totalLength = 0;

		for (iterator It = begin(); It != end(); It++)
		{
			sl.value = It.get();
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

template<typename T, typename Allocator>
MBASE_INLINE_EXPR mbase::vector<T, Allocator> mbase::vector<T, Allocator>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept 
{
	mbase::vector<T, Allocator> deserializedVec;
	if (in_length)
	{
		PTRU32 elemCount = reinterpret_cast<PTRU32>(in_src);
		serialize_helper<value_type> sl;

		IBYTEBUFFER tmpSrc = in_src + sizeof(U32);

		for (I32 i = 0; i < *elemCount; i++)
		{
			PTRU32 elemLength = reinterpret_cast<PTRU32>(tmpSrc);
			tmpSrc += sizeof(U32);
			deserializedVec.push_back(sl.deserialize(tmpSrc, *elemLength));
			tmpSrc += *elemLength;
		}
	}

	return deserializedVec;
}

MBASE_STD_END

#endif // !MBASE_VECTOR_H
