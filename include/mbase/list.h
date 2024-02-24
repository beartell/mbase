#ifndef MBASE_LIST_H
#define MBASE_LIST_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <mbase/list_iterator.h>
#include <iterator>
#include <initializer_list>

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

template<typename T, typename Allocator = mbase::allocator_simple<T>>
class list {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using move_reference = T&&;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;

	struct list_node {
		list_node* prev;
		list_node* next;
		pointer data;

		list_node(const_reference in_object) noexcept : prev(nullptr), next(nullptr) {
			data = Allocator::allocate(1);
			Allocator::construct(data, in_object);
		}

		list_node(move_reference in_object) noexcept : prev(nullptr), next(nullptr) {
			data = Allocator::allocate(1);
			Allocator::construct(data, std::move(in_object));
		}

		~list_node() noexcept {
			// data is guaranteed to be present
			// so no need to null check
			data->~value_type();
			Allocator::deallocate(data);
		}
	};

	template<typename T, typename DataT>
	class bi_list_iterator : public forward_list_iterator<T, DataT> {
	public:
		using iterator_category = std::bidirectional_iterator_tag;

		bi_list_iterator(pointer in_ptr) noexcept : forward_list_iterator<T, DataT>(in_ptr) {}
		bi_list_iterator(const bi_list_iterator& in_rhs) noexcept : forward_list_iterator<T, DataT>(in_rhs._ptr) {}

		MBASE_INLINE bi_list_iterator& operator-=(difference_type in_rhs) noexcept {
			for (size_type i = 0; i < in_rhs; i++)
			{
				_ptr = _ptr->prev;
			}
			return *this;
		}

		MBASE_INLINE bi_list_iterator& operator--() noexcept {
			_ptr = _ptr->prev;
			return *this;
		}

		MBASE_INLINE bi_list_iterator& operator--(int) noexcept {
			_ptr = _ptr->prev;
			return *this;
		}

		friend class list;
	};

	template<typename T, typename DataT>
	class const_bi_list_iterator : public const_forward_list_iterator<T, DataT> {
	public:
		using iterator_category = std::bidirectional_iterator_tag;

		const_bi_list_iterator(pointer in_ptr) noexcept : const_forward_list_iterator<T, DataT>(in_ptr) {}
		const_bi_list_iterator(const const_bi_list_iterator& in_rhs) noexcept : const_forward_list_iterator<T, DataT>(in_rhs._ptr) {}
		const_bi_list_iterator(const bi_list_iterator<T, DataT>& in_rhs) noexcept : const_forward_list_iterator<T, DataT>(in_rhs.get()) {}

		MBASE_INLINE const_bi_list_iterator& operator-=(difference_type in_rhs) noexcept {
			for (size_type i = 0; i < in_rhs; i++)
			{
				_ptr = _ptr->prev;
			}
			return *this;
		}

		MBASE_INLINE const_bi_list_iterator& operator--() noexcept {
			_ptr = _ptr->prev;
			return *this;
		}

		MBASE_INLINE const_bi_list_iterator& operator--(int) noexcept {
			_ptr = _ptr->prev;
			return *this;
		}

		friend class list;
	};

	template<typename T, typename DataT>
	class reverse_bi_list_iterator : public backward_list_iterator<T, DataT> {
	public:
		using iterator_category = std::bidirectional_iterator_tag;

		reverse_bi_list_iterator(pointer in_ptr) noexcept : backward_list_iterator<T, DataT>(in_ptr) {}
		reverse_bi_list_iterator(const reverse_bi_list_iterator& in_rhs) noexcept : backward_list_iterator<T, DataT>(in_rhs._ptr) {}

		MBASE_INLINE reverse_bi_list_iterator& operator-=(difference_type in_rhs) noexcept {
			for (size_type i = 0; i < in_rhs; i++)
			{
				_ptr = _ptr->next;
			}
			return *this;
		}

		MBASE_INLINE reverse_bi_list_iterator& operator--() noexcept {
			_ptr = _ptr->next;
			return *this;
		}

		MBASE_INLINE reverse_bi_list_iterator& operator--(int) noexcept {
			_ptr = _ptr->next;
			return *this;
		}

		friend class list;
	};

	template<typename T, typename DataT>
	class const_reverse_bi_list_iterator : public const_backward_list_iterator<T, DataT> {
	public:
		using iterator_category = std::bidirectional_iterator_tag;

		const_reverse_bi_list_iterator(pointer in_ptr) noexcept : const_backward_list_iterator<T, DataT>(in_ptr) {}
		const_reverse_bi_list_iterator(const const_reverse_bi_list_iterator& in_rhs) noexcept : const_backward_list_iterator<T, DataT>(in_rhs._ptr) {}
		const_reverse_bi_list_iterator(const reverse_bi_list_iterator<T, DataT>& in_rhs) noexcept : const_backward_list_iterator<T, DataT>(in_rhs.get()) {}


		MBASE_INLINE const_reverse_bi_list_iterator& operator-=(difference_type in_rhs) noexcept {
			for (size_type i = 0; i < in_rhs; i++)
			{
				_ptr = _ptr->next;
			}
			return *this;
		}

		MBASE_INLINE const_pointer get() const noexcept {
			return _ptr;
		}

		MBASE_INLINE const_reverse_bi_list_iterator& operator--() noexcept {
			_ptr = _ptr->next;
			return *this;
		}

		MBASE_INLINE const_reverse_bi_list_iterator& operator--(int) noexcept {
			_ptr = _ptr->next;
			return *this;
		}

		friend class list;
	};

	using iterator = bi_list_iterator<list_node, value_type>;
	using const_iterator = const_bi_list_iterator<list_node, value_type>;
	using reverse_iterator = reverse_bi_list_iterator<list_node, value_type>;
	using const_reverse_iterator = const_reverse_bi_list_iterator<list_node, value_type>;

	list() noexcept : firstNode(nullptr), lastNode(nullptr), mSize(0) {}

	list(std::initializer_list<value_type> in_list) noexcept {
		mSize = in_list.size();
		const value_type* currentObj = in_list.begin();
		firstNode = new list_node(*currentObj);
		list_node* activeNode = firstNode;

		currentObj++;

		while (currentObj != in_list.end())
		{
			list_node* freshNode = new list_node(*currentObj);
			
			activeNode->next = freshNode;
			freshNode->prev = activeNode;
			activeNode = freshNode;

			currentObj++;
		}

		lastNode = activeNode;
	}

	~list() noexcept {
		clear();
	}

	MBASE_INLINE_EXPR GENERIC swap(mbase::list<value_type>& in_src) noexcept {
		std::swap(firstNode, in_src.firstNode);
		std::swap(lastNode, in_src.lastNode);
		std::swap(mSize, in_src.mSize);
	}

	USED_RETURN("first element being ignored") MBASE_INLINE_EXPR reference front() noexcept {
		return *firstNode->data;
	}
	
	USED_RETURN("first element being ignored") MBASE_INLINE_EXPR const_reference front() const noexcept {
		return *firstNode->data;
	}

	USED_RETURN("last element being ignored") MBASE_INLINE_EXPR reference back() noexcept {
		return *lastNode->data;
	}

	USED_RETURN("last element being ignored") MBASE_INLINE_EXPR const_reference back() const noexcept {
		return *lastNode->data;
	}

	USED_RETURN("container observation ignored") MBASE_INLINE_EXPR bool empty() const noexcept {
		return mSize == 0;
	}

	USED_RETURN("container observation ignored") MBASE_INLINE_EXPR size_type size() const noexcept {
		return mSize;
	}

	USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR iterator begin() const noexcept {
		return iterator(firstNode);
	}

	USED_RETURN("iterator being ignored") MBASE_INLINE_EXPR iterator end() const noexcept {
		if(!lastNode)
		{
			return iterator(lastNode);
		}

		return iterator(lastNode->next);
	}

	USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR const_iterator cbegin() const noexcept {
		return const_iterator(firstNode);
	}

	USED_RETURN("const iterator being ignored") MBASE_INLINE_EXPR const_iterator cend() const noexcept {
		if (!lastNode)
		{
			return const_iterator(lastNode);
		}

		return const_iterator(lastNode->next);
	}

	USED_RETURN("reverse iterator being ignored") MBASE_INLINE reverse_iterator rbegin() const noexcept {
		return reverse_iterator(lastNode);
	}

	USED_RETURN("reverse iterator being ignored") MBASE_INLINE_EXPR reverse_iterator rend() const noexcept {
		if(!firstNode)
		{
			return reverse_iterator(firstNode);
		}

		return reverse_iterator(firstNode->prev);
	}

	USED_RETURN("const reverse iterator being ignored") MBASE_INLINE const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(lastNode);
	}

	USED_RETURN("const reverse iterator being ignored") MBASE_INLINE_EXPR const_reverse_iterator crend() const noexcept {
		if (!firstNode)
		{
			return const_reverse_iterator(firstNode);
		}

		return const_reverse_iterator(firstNode->prev);
	}

	MBASE_INLINE_EXPR GENERIC clear() noexcept {
		while(mSize)
		{
			pop_back();
		}
	}

	MBASE_INLINE_EXPR GENERIC push_back(const_reference in_data) noexcept {
		list_node* newNode = new list_node(in_data);
		newNode->prev = lastNode;
		if(lastNode)
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

	MBASE_INLINE_EXPR GENERIC push_back(move_reference in_data) noexcept {
		list_node* newNode = new list_node(std::move(in_data));
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

	MBASE_INLINE_EXPR GENERIC push_front(const_reference in_data) noexcept {
		list_node* newNode = new list_node(in_data);
		newNode->next = firstNode;
		if(firstNode)
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

	MBASE_INLINE_EXPR GENERIC push_front(move_reference in_data) noexcept {
		list_node* newNode = new list_node(std::move(in_data));
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

	MBASE_INLINE_EXPR GENERIC pop_back() noexcept {
		if(!(--mSize))
		{
			delete lastNode;
			lastNode = nullptr;
			firstNode = nullptr;
			return;
		}
		list_node* newLastNode = lastNode->prev;
		newLastNode->next = nullptr;
		delete lastNode;
		lastNode = newLastNode;
	}

	MBASE_INLINE_EXPR GENERIC pop_front() noexcept {
		if (!(--mSize))
		{
			delete firstNode;
			lastNode = nullptr;
			firstNode = nullptr;
			return;
		}

		list_node* newFirstNode = firstNode->next;
		newFirstNode->prev = nullptr;
		delete firstNode;
		firstNode = newFirstNode;
	}

	MBASE_INLINE_EXPR GENERIC insert(const_iterator in_pos, const_reference in_object) noexcept {
		list_node* mNode = in_pos._ptr;
		list_node* newNode = new list_node(in_object);
		newNode->prev = mNode->prev;
		if(mNode->prev)
		{
			mNode->prev->next = newNode;
		}

		mNode->prev = newNode;
		newNode->next = mNode;
		++mSize;
	}

	MBASE_INLINE_EXPR GENERIC insert(const_iterator in_pos, move_reference in_object) noexcept {
		list_node* mNode = in_pos._ptr;
		list_node* newNode = new list_node(std::move(in_object));
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
	}

	MBASE_INLINE_EXPR GENERIC insert(difference_type in_index, const_reference in_object) noexcept {
		iterator in_pos = begin();
		in_pos += in_index;

		insert(in_pos, in_object);
	}

	MBASE_INLINE_EXPR GENERIC insert(difference_type in_index, move_reference in_object) noexcept {
		iterator in_pos = begin();
		in_pos += in_index;

		insert(in_pos, std::move(in_object));
	}

	MBASE_INLINE_EXPR iterator erase(iterator in_pos) noexcept {
		list_node* mNode = in_pos._ptr;
		list_node* returnedNode = mNode->next;

		if(mNode == firstNode)
		{
			pop_front();
		}

		else if(mNode == lastNode)
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

	MBASE_INLINE GENERIC serialize(safe_buffer* out_buffer) noexcept {
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
				out_buffer->bfLength = totalBufferLength;
				out_buffer->bfSource = new IBYTE[totalBufferLength];

				IBYTEBUFFER _bfSource = out_buffer->bfSource;
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

	MBASE_INLINE static mbase::list<T, Allocator> deserialize(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
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

private:
	list_node* firstNode;
	list_node* lastNode;
	size_type mSize;
};

MBASE_STD_END

#endif // !MBASE_LIST_H
