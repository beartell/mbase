#ifndef MBASE_VECTOR_H
#define MBASE_VECTOR_H

#include <mbase/common.h> // For data types and macros
#include <mbase/allocator.h> // For allocation routines

#include <initializer_list> // For std::initializer_list

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
	using value_type = T;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;
	using reference = value_type&;
	using const_reference = const reference;
	using move_reference = value_type&&;
	using pointer = value_type*;
	using const_pointer = const pointer;

	class vector_iterator : public sequence_iterator<T> {
	public:
		vector_iterator(pointer in_ptr) noexcept : sequence_iterator(in_ptr) {}

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

	using iterator = vector_iterator;
	using const_iterator = const vector_iterator;

	explicit vector() noexcept : raw_data(nullptr), mSize(0), mCapacity(4) {
		raw_data = Allocator::allocate(mCapacity); // default capacity
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
		raw_data = Allocator::allocate(mCapacity);

		while(currentObj != in_list.end())
		{
			Allocator::construct(raw_data + mSize, *currentObj);
			currentObj++;
			mSize++;
		}
	}

	vector(const vector& in_rhs) noexcept {
		clear();
		
		if(!in_rhs.mCapacity)
		{
			mSize = 0;
			mCapacity = 0;
			return;
		}

		mSize = in_rhs.mSize;
		mCapacity = in_rhs.mCapacity;

		raw_data = Allocator::allocate(mCapacity);
		for (I32 i = 0; i < mCapacity; i++)
		{
			Allocator::construct(raw_data + i, in_rhs[i]);
		}
	}
	
	vector(vector&& in_rhs) noexcept {
		clear();

		mSize = in_rhs.mSize;
		mCapacity = in_rhs.mCapacity;

		raw_data = in_rhs.raw_data;
		in_rhs.raw_data = nullptr;
	}

	~vector() noexcept {
		clear();
	}

	vector& operator=(const vector& in_rhs) noexcept {
		clear();

		if (!in_rhs.mCapacity)
		{
			mSize = 0;
			mCapacity = 0;
			return *this;
		}

		mSize = in_rhs.mSize;
		mCapacity = in_rhs.mCapacity;

		raw_data = Allocator::allocate(mCapacity);
		for (I32 i = 0; i < mCapacity; i++)
		{
			Allocator::construct(raw_data + i, in_rhs[i]);
		}
		return *this;
	}

	vector& operator=(vector&& in_rhs) noexcept {
		clear();

		mSize = in_rhs.mSize;
		mCapacity = in_rhs.mCapacity;

		raw_data = in_rhs.raw_data;
		in_rhs.raw_data = nullptr;

		return *this;
	}

	reference operator[](difference_type in_index) noexcept {
		return raw_data[in_index];
	}

	const_reference operator[](difference_type in_index) const noexcept {
		return raw_data[in_index];
	}

	MBASE_INLINE_EXPR GENERIC clear() noexcept {
		if (raw_data)
		{
			for (size_type i = 0; i < mSize; i++)
			{
				raw_data[i].~value_type();
			}
			Allocator::deallocate(raw_data);
			raw_data = nullptr;
		}
		mSize = 0;
		mCapacity = 0;
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
		difference_type i = 0;
		for(i; i < mSize; i++)
		{
			Allocator::construct(new_data + i, std::move(*(raw_data + i)));
		}
		clear();
		mSize = i;
		mCapacity = in_capacity;
		raw_data = new_data;
	}

	MBASE_INLINE_EXPR GENERIC erase(iterator in_pos) noexcept {
		// NOT WORKING RN
		if (in_pos != end()) {
			for (iterator iter = in_pos; iter != end() - 1; ++iter) {
				*iter = *(iter + 1);
			}

			--mSize;
		}
	}

	MBASE_INLINE_EXPR GENERIC push_back(const_reference in_data) noexcept {
		if(mSize == mCapacity)
		{
			reserve(2 * mCapacity + 1);
		}
		pointer curObj = raw_data + mSize++;
		Allocator::construct(curObj, in_data);
	}

	MBASE_INLINE_EXPR GENERIC push_back(move_reference in_data) noexcept {
		if (mSize == mCapacity)
		{
			reserve(2 * mCapacity + 1);
		}
		pointer curObj = raw_data + mSize++;
		Allocator::construct(curObj, std::move(in_data));
	}

	MBASE_INLINE_EXPR GENERIC pop_back() noexcept {
		pointer curObj = raw_data + --mSize;
		curObj->~value_type();
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
