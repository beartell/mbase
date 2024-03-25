#ifndef MBASE_VECTOR_H
#define MBASE_VECTOR_H

/* DO NOT FORGET TO IMPLEMENT THE ERASE FUNCTION */

#include <mbase/common.h> // For data types and macros
#include <mbase/allocator.h> // For allocation routines
#include <mbase/safe_buffer.h> // For safe_buffer
#include <mbase/container_serializer_helper.h> // For serialize_helper

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

	using iterator = sequence_iterator<T>;
	using const_iterator = const_sequence_iterator<T>;
	using reverse_iterator = reverse_sequence_iterator<T>;
	using const_reverse_iterator = const_reverse_sequence_iterator<T>;

	MBASE_EXPLICIT vector() noexcept;
	MBASE_EXPLICIT vector(size_type in_size) noexcept;
	vector(std::initializer_list<value_type> in_list) noexcept;
	vector(const vector& in_rhs) noexcept;
	vector(vector&& in_rhs) noexcept;
	~vector() noexcept;

	vector& operator=(const vector& in_rhs) noexcept;
	vector& operator=(vector&& in_rhs) noexcept;

	USED_RETURN("last element being ignored") MBASE_INLINE_EXPR reference back() noexcept;
	USED_RETURN("first element being ignored") MBASE_INLINE_EXPR reference front() noexcept;
	USED_RETURN("last element being ignored") MBASE_INLINE_EXPR const_reference back() const noexcept;
	USED_RETURN("first element being ignored") MBASE_INLINE_EXPR const_reference front() const noexcept;
	USED_RETURN("raw data being ignored") MBASE_INLINE_EXPR pointer data() noexcept;
	USED_RETURN("raw string data being ignored") MBASE_INLINE_EXPR const_pointer data() const noexcept;

	USED_RETURN("unused element") reference operator[](difference_type in_index) noexcept;
	USED_RETURN("unused element") const_reference operator[](difference_type in_index) const noexcept;

	USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR iterator begin() const noexcept;
	USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR iterator end() const noexcept;
	USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR const_iterator cbegin() const noexcept;
	USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR const_iterator cend() const noexcept;
	USED_RETURN("reverse iterator being ignored") MBASE_INLINE_EXPR reverse_iterator rbegin() const noexcept;
	USED_RETURN("reverse iterator being ignored") MBASE_INLINE_EXPR reverse_iterator rend() const noexcept;
	USED_RETURN("const reverse iterator being ignored") MBASE_INLINE_EXPR const_reverse_iterator crbegin() const noexcept;
	USED_RETURN("const reverse iterator being ignored") MBASE_INLINE_EXPR const_reverse_iterator crend() const noexcept;

	USED_RETURN("container observation ignored") MBASE_INLINE_EXPR size_type size() const noexcept;
	USED_RETURN("container observation ignored") MBASE_INLINE_EXPR size_type capacity() const noexcept;
	USED_RETURN("container observation ignored") MBASE_INLINE_EXPR bool empty() const noexcept;

	MBASE_INLINE_EXPR GENERIC swap(mbase::vector<value_type, Allocator>& in_src) noexcept;
	MBASE_INLINE_EXPR GENERIC clear() noexcept;
	MBASE_INLINE_EXPR GENERIC deep_clear() noexcept;
	MBASE_INLINE_EXPR GENERIC resize(size_type in_size) noexcept;
	MBASE_INLINE_EXPR GENERIC reserve(size_type in_capacity) noexcept;
	MBASE_INLINE_EXPR GENERIC erase(iterator in_pos) noexcept;
	MBASE_INLINE_EXPR GENERIC push_back(const T& in_val) noexcept;
	MBASE_INLINE_EXPR GENERIC push_back(T&& in_val) noexcept;
	MBASE_INLINE_EXPR GENERIC pop_back() noexcept;
	MBASE_INLINE_EXPR iterator insert(const_iterator in_pos, const T& in_val) noexcept;

	MBASE_INLINE_EXPR GENERIC serialize(safe_buffer& out_buffer) noexcept;
	MBASE_INLINE_EXPR static mbase::vector<T, Allocator> deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept;

private:
	Allocator externalAllocator;
	pointer raw_data;
	SIZE_T mSize;
	SIZE_T mCapacity;
};

template<typename T, typename Allocator>
vector<T, Allocator>::vector() noexcept : raw_data(nullptr), mSize(0), mCapacity(4) {
	raw_data = externalAllocator.allocate(mCapacity); // default capacity
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(size_type in_size) noexcept : raw_data(nullptr), mSize(0), mCapacity(0) {
	if (in_size < 0)
	{
		return;
	}
	mCapacity = in_size;
	raw_data = externalAllocator.allocate(mCapacity);
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(std::initializer_list<value_type> in_list) noexcept {
	mSize = 0;
	mCapacity = in_list.size() * 2;

	const value_type* currentObj = in_list.begin();
	raw_data = externalAllocator.allocate(mCapacity);

	while (currentObj != in_list.end())
	{
		externalAllocator.construct(raw_data + mSize, *currentObj);
		currentObj++;
		mSize++;
	}
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(const vector& in_rhs) noexcept {
	mSize = in_rhs.mSize;
	mCapacity = in_rhs.mCapacity;

	if (!mCapacity)
	{
		mCapacity = 4;
	}

	raw_data = externalAllocator.allocate(mCapacity);
	for (I32 i = 0; i < mCapacity; i++)
	{
		externalAllocator.construct(raw_data + i, in_rhs[i]);
	}
}

template<typename T, typename Allocator>
vector<T, Allocator>::vector(vector&& in_rhs) noexcept {
	mSize = in_rhs.mSize;
	mCapacity = in_rhs.mCapacity;

	raw_data = in_rhs.raw_data;
	in_rhs.raw_data = nullptr;
}

template<typename T, typename Allocator>
vector<T, Allocator>::~vector() noexcept {
	clear();
}

template<typename T, typename Allocator>
vector<T, Allocator>& vector<T, Allocator>::operator=(const vector& in_rhs) noexcept {
	clear();

	if (!in_rhs.mCapacity)
	{
		mSize = 0;
		mCapacity = 0;
		return *this;
	}

	mSize = in_rhs.mSize;
	mCapacity = in_rhs.mCapacity;

	raw_data = externalAllocator.allocate(mCapacity);
	for (I32 i = 0; i < mCapacity; i++)
	{
		externalAllocator.construct(raw_data + i, in_rhs[i]);
	}
	return *this;
}

template<typename T, typename Allocator>
vector<T, Allocator>& vector<T, Allocator>::operator=(vector&& in_rhs) noexcept {
	clear();

	mSize = in_rhs.mSize;
	mCapacity = in_rhs.mCapacity;

	raw_data = in_rhs.raw_data;
	in_rhs.raw_data = nullptr;

	return *this;
}

template<typename T, typename Allocator>
USED_RETURN("last element being ignored") MBASE_INLINE_EXPR T& vector<T, Allocator>::back() noexcept {
	return raw_data[mSize - 1];
}

template<typename T, typename Allocator>
USED_RETURN("last element being ignored") MBASE_INLINE_EXPR T& vector<T, Allocator>::front() noexcept {
	return raw_data[0];
}

template<typename T, typename Allocator>
USED_RETURN("last element being ignored") MBASE_INLINE_EXPR const T& vector<T, Allocator>::back() const noexcept {
	return raw_data[mSize - 1];
}

template<typename T, typename Allocator>
USED_RETURN("last element being ignored") MBASE_INLINE_EXPR const T& vector<T, Allocator>::front() const noexcept {
	return raw_data[0];
}

template<typename T, typename Allocator>
USED_RETURN("raw data being ignored") MBASE_INLINE_EXPR T* vector<T, Allocator>::data() noexcept {
	return raw_data;
}

template<typename T, typename Allocator>
USED_RETURN("raw data being ignored") MBASE_INLINE_EXPR const T* vector<T, Allocator>::data() const noexcept {
	return raw_data;
}

template<typename T, typename Allocator>
USED_RETURN("unused element") T& vector<T, Allocator>::operator[](difference_type in_index) noexcept {
	return raw_data[in_index];
}

template<typename T, typename Allocator>
USED_RETURN("unused element") const T& vector<T, Allocator>::operator[](difference_type in_index) const noexcept {
	return raw_data[in_index];
}

template<typename T, typename Allocator>
USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR sequence_iterator<T> vector<T, Allocator>::begin() const noexcept {
	return iterator(raw_data);
}

template<typename T, typename Allocator>
USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR mbase::sequence_iterator<T> vector<T, Allocator>::end() const noexcept {
	return iterator(raw_data + mSize);
}

template<typename T, typename Allocator>
USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR mbase::const_sequence_iterator<T> vector<T, Allocator>::cbegin() const noexcept {
	return const_iterator(raw_data);
}

template<typename T, typename Allocator>
USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR mbase::const_sequence_iterator<T> vector<T, Allocator>::cend() const noexcept {
	return const_iterator(raw_data + mSize);
}

template<typename T, typename Allocator>
USED_RETURN("reverse iterator being ignored") MBASE_INLINE_EXPR mbase::reverse_sequence_iterator<T> vector<T, Allocator>::rbegin() const noexcept {
	return reverse_iterator(raw_data + (mSize - 1));
}

template<typename T, typename Allocator>
USED_RETURN("reverse iterator being ignored") MBASE_INLINE_EXPR mbase::reverse_sequence_iterator<T> vector<T, Allocator>::rend() const noexcept {
	return reverse_iterator(raw_data - 1);
}

template<typename T, typename Allocator>
USED_RETURN("const reverse iterator being ignored") MBASE_INLINE_EXPR mbase::const_reverse_sequence_iterator<T> vector<T, Allocator>::crbegin() const noexcept {
	return const_reverse_iterator(raw_data + (mSize - 1));
}

template<typename T, typename Allocator>
USED_RETURN("const reverse iterator being ignored") MBASE_INLINE_EXPR mbase::const_reverse_sequence_iterator<T> vector<T, Allocator>::crend() const noexcept {
	return const_reverse_iterator(raw_data - 1);
}

template<typename T, typename Allocator>
USED_RETURN("container observation ignored") MBASE_INLINE_EXPR SIZE_T vector<T, Allocator>::size() const noexcept {
	return mSize;
}

template<typename T, typename Allocator>
USED_RETURN("container observation ignored") MBASE_INLINE_EXPR SIZE_T vector<T, Allocator>::capacity() const noexcept {
	return mCapacity;
}

template<typename T, typename Allocator>
USED_RETURN("container observation ignored") MBASE_INLINE_EXPR bool vector<T, Allocator>::empty() const noexcept {
	return mSize == 0;
}


template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::clear() noexcept {
	// TODO:
		// POP_BACK UNTIL THE VECTOR IS EMPTY

		/* PROBLEMS WITH THIS IMPLEMENTATION:
			IT DEALLOCATES THE VECTOR CAPACITY IN WHICH SHOULD BE OPTIONAL.
			USER SHOULD MANUALLY CALL "SHRINK_TO_FIT" MEMBER FUNCTION TO ACHIEVE THAT BEHAVIOUR
		*/
	if (raw_data)
	{
		for (size_type i = 0; i < mSize; i++)
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
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::deep_clear() noexcept {
	if (raw_data)
	{
		for (size_type i = 0; i < mSize; i++)
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
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::resize(SIZE_T in_size) noexcept {
	if (in_size > mCapacity)
	{
		reserve(in_size * 2);
	}
	mSize = in_size;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::reserve(SIZE_T in_capacity) noexcept {
	if (in_capacity <= mCapacity)
	{
		return;
	}

	pointer new_data = externalAllocator.allocate(in_capacity);
	difference_type i = 0;
	for (i; i < mSize; i++)
	{
		externalAllocator.construct(new_data + i, std::move(*(raw_data + i)));
		raw_data[i].~value_type();
	}
	externalAllocator.deallocate(raw_data, 0);
	raw_data = nullptr;

	mSize = i;
	mCapacity = in_capacity;
	raw_data = new_data;
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::erase(iterator in_pos) noexcept {
	if (in_pos == end() - 1)
	{
		pop_back();
	}
	else
	{
		if (mSize)
		{
			pointer new_data = externalAllocator.allocate(mCapacity);
			difference_type i = 0;
			difference_type j = 0;
			for (iterator It = begin(); It != end(); It++)
			{
				if (It == in_pos)
				{
					raw_data[j].~value_type();
					++j;
				}
				else
				{
					raw_data[j].~value_type();
				}
				externalAllocator.construct(new_data + i, std::move(*(raw_data + j)));
				++i;
				++j;
			}
			externalAllocator.deallocate(raw_data, 0);
			raw_data = new_data;
			--mSize;
		}
	}
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR sequence_iterator<T> vector<T, Allocator>::insert(const_iterator in_pos, const T& in_val) noexcept {
	return iterator(raw_data);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::push_back(const T& in_val) noexcept {
	if (mSize == mCapacity)
	{
		reserve(2 * mCapacity + 1);
	}
	pointer curObj = raw_data + mSize++;
	externalAllocator.construct(curObj, in_val);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::push_back(T&& in_val) noexcept {
	if (mSize == mCapacity)
	{
		reserve(2 * mCapacity + 1);
	}
	pointer curObj = raw_data + mSize++;
	externalAllocator.construct(curObj, std::move(in_val));
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::pop_back() noexcept {
	if (!mSize)
	{
		return;
	}

	pointer curObj = raw_data + --mSize;
	curObj->~value_type();
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::serialize(safe_buffer& out_buffer) noexcept {
	if (mSize)
	{
		mbase::vector<safe_buffer> totalBuffer;

		serialize_helper<value_type> sl;

		safe_buffer tmpSafeBuffer;
		size_type totalLength = 0;

		for (iterator It = begin(); It != end(); It++)
		{
			sl.value = It.get();
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
MBASE_INLINE_EXPR GENERIC vector<T, Allocator>::swap(mbase::vector<value_type, Allocator>& in_src) noexcept {
	std::swap(raw_data, in_src.raw_data);
	std::swap(mCapacity, in_src.mCapacity);
	std::swap(mSize, in_src.mSize);
}

template<typename T, typename Allocator>
MBASE_INLINE_EXPR mbase::vector<T, Allocator> mbase::vector<T, Allocator>::deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
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
