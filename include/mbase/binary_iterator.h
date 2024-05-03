#ifndef MBASE_BINARY_ITERATOR
#define MBASE_BINARY_ITERATOR

#include <mbase/common.h>

MBASE_STD_BEGIN

template<typename T, typename DataT>
class bst_iterator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;
    //using iterator_category = std::bidirectional_iterator_tag;

    MBASE_INLINE bst_iterator(pointer in_ptr) noexcept : _ptr(in_ptr)
    {
        while(in_ptr)
        {
            _ptr = in_ptr;
            in_ptr = in_ptr->left;
        }
    }

    MBASE_INLINE bst_iterator(const bst_iterator& in_rhs) noexcept
    {
        _ptr = in_rhs._ptr;
    }

    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE pointer get() const noexcept {
        return _ptr;
    }

    MBASE_ND("internal data that is access through the iterator should be used") MBASE_INLINE DataT& operator*() const noexcept {
        return _ptr->data;
    }

    MBASE_INLINE DataT* operator->() const noexcept {
        return &_ptr->data;
    }

    MBASE_INLINE bst_iterator& operator++() noexcept {
        if(_ptr->right)
        {
            pointer leftTraverse = _ptr->right;
            while(leftTraverse->left)
            {
                leftTraverse = leftTraverse->left;
            }
            _ptr = leftTraverse;
        }
        else
        {
            if(_ptr->parent)
            {
                if (_ptr == _ptr->parent->left)
                {
                    _ptr = _ptr->parent;
                }
                else if (_ptr == _ptr->parent->right)
                {
                    pointer parentTraversal = _ptr->parent;
                    while (parentTraversal && (_ptr->data > parentTraversal->data))
                    {
                        parentTraversal = parentTraversal->parent;
                    }
                    _ptr = parentTraversal;
                }
            }
            else
            {
                _ptr = nullptr;
            }
        }

        return *this;
    }

    MBASE_INLINE bst_iterator& operator++(int) noexcept {
        bst_iterator bi(_ptr);
        ++(*this);
        return bi;
    }

    MBASE_INLINE bst_iterator& operator--() noexcept {
        if(_ptr->left)
        {
            
            pointer lrTraverse = _ptr->left;
            while(lrTraverse->right)
            {
                lrTraverse = lrTraverse->right;
            }
            _ptr = lrTraverse;
        }
        else
        {
            if(_ptr->parent)
            {
                if(_ptr == _ptr->parent->left)
                {
                    pointer parentTraverse = _ptr;
                    while(parentTraverse->parent && (_ptr->data < parentTraverse->parent->data))
                    {
                        parentTraverse = parentTraverse->parent;
                    }

                    _ptr = parentTraverse->parent;
                }
                else if(_ptr == _ptr->parent->right)
                {
                    _ptr = _ptr->parent;
                }
            }
            else
            {
                _ptr = nullptr;
            }
        }
        return *this;
    }

private:
    pointer _ptr;
};

MBASE_STD_END

#endif // !MBASE_BINARY_ITERATOR
