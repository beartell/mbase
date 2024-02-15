#ifndef MBASE_VECTOR_H
#define MBASE_VECTOR_H

/* DO NOT FORGET TO IMPLEMENT THE ERASE FUNCTION */

#include <mbase/common.h> // For data types and macros
#include <mbase/allocator.h> // For allocation routines
#include <mbase/type_sequence.h> // For sequence iterator
#include <mbase/safe_buffer.h> // For safe buffer
#include <mbase/container_serializer_helper.h> // For serialize_helper

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

template<typename T>
class vector_iterator : public mbase::sequence_iterator<T> {
public:
	vector_iterator(pointer in_ptr) noexcept : sequence_iterator(in_ptr) {}
	vector_iterator(const vector_iterator& in_rhs) noexcept : sequence_iterator(in_rhs._ptr) {}

	MBASE_INLINE pointer operator->() const noexcept {
		return _ptr;
	}

	MBASE_INLINE pointer get() const noexcept {
		return _ptr;
	}

	USED_RETURN MBASE_INLINE bool operator==(const vector_iterator& in_rhs) const noexcept {
		return _ptr == in_rhs._ptr;
	}

	USED_RETURN MBASE_INLINE bool operator!=(const vector_iterator& in_rhs) const noexcept {
		return _ptr != in_rhs._ptr;
	}
};

template <typename T>
class const_vector_iterator : public mbase::sequence_iterator<T> {
public:
	const_vector_iterator(pointer in_ptr) noexcept : sequence_iterator(in_ptr) {}
	const_vector_iterator(const const_vector_iterator& in_rhs) noexcept : sequence_iterator(in_rhs._ptr) {}

	USED_RETURN MBASE_INLINE const T& operator*() const noexcept {
		return *_ptr;
	}

	MBASE_INLINE const T* operator->() const noexcept {
		return _ptr;
	}

	MBASE_INLINE const T* get() const noexcept {
		return _ptr;
	}

	USED_RETURN MBASE_INLINE bool operator==(const const_vector_iterator& in_rhs) const noexcept {
		return _ptr == in_rhs._ptr;
	}

	USED_RETURN MBASE_INLINE bool operator!=(const const_vector_iterator& in_rhs) const noexcept {
		return _ptr != in_rhs._ptr;
	}
};

template<typename T, typename Allocator = mbase::allocator_simple<T>>
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

	using iterator = vector_iterator<T>;
	using const_iterator = const_vector_iterator<T>;

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

	USED_RETURN reference operator[](difference_type in_index) noexcept {
		return raw_data[in_index];
	}

	USED_RETURN const_reference operator[](difference_type in_index) const noexcept {
		return raw_data[in_index];
	}

	MBASE_INLINE_EXPR GENERIC clear() noexcept {
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
			Allocator::deallocate(raw_data);
			raw_data = nullptr;
		}
		mSize = 0;
		mCapacity = 0;
	}
	
	MBASE_INLINE_EXPR GENERIC deep_clear() noexcept {
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
		if(in_capacity <= mCapacity)
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
		if(in_pos == end())
		{
			pop_back();
		}
		else
		{
			if(mSize)
			{
				pointer new_data = Allocator::allocate(in_capacity);
				difference_type i = 0;
				for (iterator It = begin(); It != end(); It++)
				{
					if (It == in_pos)
					{
						It->~value_type();
					}
					else
					{
						raw_data[i].~value_type();
						Allocator::construct(new_data + i, std::move(*(raw_data + i)));
					}
					++i;
				}
				Allocator::deallocate(raw_data);
				raw_data = new_data;
				--mSize;
			}
		}
	}

	MBASE_INLINE_EXPR GENERIC push_back(const T& in_data) noexcept {
		if(mSize == mCapacity)
		{
			reserve(2 * mCapacity + 1);
		}
		pointer curObj = raw_data + mSize++;
		Allocator::construct(curObj, in_data);
	}

	MBASE_INLINE_EXPR GENERIC push_back(T&& in_data) noexcept {
		if (mSize == mCapacity)
		{
			reserve(2 * mCapacity + 1);
		}
		pointer curObj = raw_data + mSize++;
		Allocator::construct(curObj, std::move(in_data));
	}

	MBASE_INLINE_EXPR GENERIC pop_back() noexcept {
		if(!mSize)
		{
			return;
		}

		pointer curObj = raw_data + --mSize;
		curObj->~value_type();
	}

	MBASE_INLINE GENERIC serialize(safe_buffer* out_buffer) noexcept {
		if(mSize)
		{	
			mbase::vector<safe_buffer> totalBuffer;

			serialize_helper<value_type> sl;

			safe_buffer tmpSafeBuffer;
			size_type totalLength = 0;

			for(iterator It = begin(); It != end(); It++)
			{
				sl.value = It.get();
				sl.serialize(&tmpSafeBuffer);
				if(tmpSafeBuffer.bfLength)
				{
					totalLength += tmpSafeBuffer.bfLength;
					totalBuffer.push_back(std::move(tmpSafeBuffer));
				}
			}

			if(totalBuffer.size())
			{
				SIZE_T totalBufferLength = totalLength + (totalBuffer.size() * sizeof(U32)) + sizeof(U32);

				mbase::vector<safe_buffer>::iterator It = totalBuffer.begin();
				out_buffer->bfLength = totalBufferLength;
				out_buffer->bfSource = new IBYTE[totalBufferLength];

				IBYTEBUFFER _bfSource = out_buffer->bfSource;
				PTRU32 elemCount = reinterpret_cast<PTRU32>(_bfSource);
				*elemCount = totalBuffer.size();

				_bfSource += sizeof(U32);
				
				size_type totalIterator = 0;
				for(It; It != totalBuffer.end(); It++)
				{
					elemCount = reinterpret_cast<PTRU32>(_bfSource);
					*elemCount = It->bfLength;
					_bfSource += sizeof(U32);
					for(I32 i = 0; i < It->bfLength; i++)
					{
						_bfSource[i] = It->bfSource[i];
					}

					_bfSource += It->bfLength;
				}
			}
		}
	}

	MBASE_INLINE static mbase::vector<T, Allocator> deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
		mbase::vector<T, Allocator> deserializedVec;
		if(in_length)
		{
			PTRU32 elemCount = reinterpret_cast<PTRU32>(in_src);
			serialize_helper<value_type> sl;

			IBYTEBUFFER tmpSrc = in_src + sizeof(U32);

			for(I32 i = 0; i < *elemCount; i++)
			{
				PTRU32 elemLength = reinterpret_cast<PTRU32>(tmpSrc);
				tmpSrc += sizeof(U32);
				deserializedVec.push_back(sl.deserialize(tmpSrc, *elemLength));
				tmpSrc += *elemLength;
			}
		}

		return deserializedVec;
	}

	MBASE_INLINE_EXPR GENERIC swap(mbase::vector<value_type>& in_src) noexcept {
		std::swap(raw_data, in_src.raw_data);
		std::swap(mCapacity, in_src.mCapacity);
		std::swap(mSize, in_src.mSize);
	}

	USED_RETURN MBASE_INLINE_EXPR iterator begin() noexcept {
		return iterator(raw_data);
	}

	USED_RETURN MBASE_INLINE_EXPR iterator end() noexcept {
		return iterator(raw_data + mSize);
	}

	USED_RETURN MBASE_INLINE_EXPR const_iterator cbegin() const noexcept {
		return const_iterator(raw_data);
	}

	USED_RETURN MBASE_INLINE_EXPR const_iterator cend() const noexcept {
		return const_iterator(raw_data + mSize);
	}

	USED_RETURN MBASE_INLINE_EXPR reference back() noexcept {
		return raw_data[mSize - 1];
	}

	USED_RETURN MBASE_INLINE_EXPR reference front() noexcept {
		return raw_data[0];
	}

	USED_RETURN MBASE_INLINE_EXPR const_reference back() const noexcept {
		return raw_data[mSize - 1];
	}

	USED_RETURN MBASE_INLINE_EXPR const_reference front() const noexcept {
		return raw_data[0];
	}

	USED_RETURN MBASE_INLINE_EXPR pointer data() noexcept {
		return raw_data;
	}

	USED_RETURN MBASE_INLINE_EXPR const_pointer data() const noexcept {
		return raw_data;
	}

	USED_RETURN MBASE_INLINE_EXPR size_type size() const noexcept {
		return mSize;
	}

	USED_RETURN MBASE_INLINE_EXPR size_type capacity() const noexcept {
		return mCapacity;
	}

	USED_RETURN MBASE_INLINE_EXPR bool empty() const noexcept {
		return mSize == 0;
	}

private:
	pointer raw_data;
	SIZE_T mSize;
	SIZE_T mCapacity;
};

MBASE_STD_END

#endif // !MBASE_VECTOR_H
