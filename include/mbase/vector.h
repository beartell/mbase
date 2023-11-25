#ifndef MBASE_VECTOR_H
#define MBASE_VECTOR_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <iostream>

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
class vector {
public:
	class vector_iterator : public sequence_iterator<T> {
	public:
		vector_iterator(pointer in_ptr) noexcept : sequence_iterator(in_ptr){}

		MBASE_INLINE pointer operator->() const noexcept {
			return _ptr;
		}
		USED_RETURN MBASE_INLINE bool operator==(const vector_iterator& in_rhs) const noexcept {
			return *_ptr == *in_rhs._ptr;
		}

		USED_RETURN MBASE_INLINE bool operator!=(const vector_iterator& in_rhs) const noexcept {
			return !(*_ptr == *in_rhs._ptr);
		}
	};

	using value_type = T;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using reference = value_type&;
	using const_reference = const reference;
	using move_reference = value_type&&;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using iterator = vector_iterator;
	using const_iterator = const vector_iterator;

	explicit vector() noexcept : raw_data(nullptr), mSize(0), mCapacity(0) {
		// DO NOTHING LITERALLY
	}

	explicit vector(size_type in_size) noexcept : raw_data(nullptr), mSize(0), mCapacity(0) {
		if(in_size < 0)
		{
			return;
		}
		mCapacity = in_size;
		raw_data = Allocator::allocate(mCapacity);
	}

	vector(std::initializer_list<value_type> in_list) noexcept {
		mSize = 0;
		mCapacity = in_list.size() * 2;

		const value_type* currentObj = in_list.begin();
		raw_data = new value_type[mCapacity];

		while(currentObj != in_list.end())
		{
			raw_data[mSize++] = *currentObj;
			currentObj++;
		}
	}

	vector(const vector& in_rhs) noexcept {
		Allocator::destroy_all(raw_data);

		mSize = in_rhs.mSize;
		mCapacity = in_rhs.mCapacity;
		if(mCapacity != 0)
		{
			raw_data = new value_type[mCapacity];
			for (I32 i = 0; i < mCapacity; i++)
			{
				new_data[i] = in_rhs.raw_data[i];
			}
			raw_data = new_data;
		}
	}
	
	~vector() noexcept {
		Allocator::destroy_all(raw_data);
	}

	MBASE_INLINE_EXPR GENERIC clear() noexcept {
		Allocator::destroy_all(raw_data);
		mSize = 0;
		mCapacity = 0;
		raw_data = nullptr;
	}

	MBASE_INLINE_EXPR GENERIC clear(size_type in_capacity) noexcept {
		Allocator::destroy_all(raw_data);
		mSize = 0;
		mCapacity = in_capacity;
	}

	MBASE_INLINE_EXPR GENERIC resize(size_type in_size) noexcept {
		if(in_size > mCapacity)
		{
			reserve(in_size * 2);
		}
		
		mSize = in_size;
	}

	MBASE_INLINE_EXPR GENERIC reserve(size_type in_capacity) noexcept {
		if(in_capacity < mSize)
		{
			return;
		}
		pointer new_data = Allocator::allocate(in_capacity);
		for(I32 i = 0; i < mSize; i++)
		{
			new_data[i] = std::move(raw_data[i]);
		}

		mCapacity = in_capacity;
		Allocator::destroy_all(raw_data);
		raw_data = new_data;
	}

	MBASE_INLINE_EXPR GENERIC push_back(const_reference in_data) noexcept {
		if(mSize == mCapacity)
		{
			reserve(2 * mCapacity + 1);
		}
		raw_data[mSize++] = in_data;
	}

	MBASE_INLINE_EXPR GENERIC push_back(move_reference in_data) noexcept {
		if (mSize == mCapacity)
		{
			reserve(2 * mCapacity + 1);
		}
		raw_data[mSize++] = std::move(in_data);
	}

	MBASE_INLINE_EXPR GENERIC pop_back() noexcept {
		--mSize;
	}

	MBASE_INLINE_EXPR iterator begin() noexcept {
		return iterator(raw_data);
	}

	MBASE_INLINE_EXPR iterator end() noexcept {
		return iterator(raw_data + mSize);
	}

	MBASE_INLINE_EXPR const_iterator cbegin() const noexcept {
		return const_iterator(raw_data);
	}

	MBASE_INLINE_EXPR const_iterator cend() const noexcept {
		return const_iterator(raw_data + mSize);
	}

	MBASE_INLINE_EXPR reference back() noexcept {
		return raw_data[mSize - 1];
	}

	MBASE_INLINE_EXPR reference front() noexcept {
		return raw_data[0];
	}

	MBASE_INLINE_EXPR const_reference back() const noexcept {
		return raw_data[mSize - 1];
	}

	MBASE_INLINE_EXPR const_reference front() const noexcept {
		return raw_data[0];
	}

	MBASE_INLINE_EXPR pointer data() noexcept {
		return raw_data;
	}

	MBASE_INLINE_EXPR const_pointer data() const noexcept {
		return raw_data;
	}

private:
	pointer raw_data;
	SIZE_T mSize;
	SIZE_T mCapacity;
};

MBASE_STD_END

#endif // !MBASE_VECTOR_H
