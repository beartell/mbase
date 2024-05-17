#ifndef MBASE_BINARY_ITERATOR
#define MBASE_BINARY_ITERATOR

#include <mbase/common.h>

MBASE_STD_BEGIN

/*

    --- CLASS INFORMATION ---
Identification: S0C7-OBJ-NA-ST

Name: bst_iterator

Parent: None

Behaviour List:
- Default Constructible
- Copy Constructible
- Swappable
- Arithmetic Operable
- Equality Comparable
- Sign Comparable
- Templated
- Type Aware

Description:
This class provides an interface for iteration of binary search trees(bst).
It's interface is mostly identical to the standard library iterators.
For this reason, <algorithm> library can utilize this class.

*/

template<typename T, typename DataT>
class bst_iterator;

/*

    --- CLASS INFORMATION ---
Identification: S0C8-OBJ-NA-ST

Name: const_bst_iterator

Parent: None

Behaviour List:
- Default Constructible
- Copy Constructible
- Swappable
- Arithmetic Operable
- Equality Comparable
- Sign Comparable
- Templated
- Type Aware

Description:
This class provides an interface for constant iteration of binary search trees(bst).
It's interface is mostly identical to the standard library iterators.
For this reason, <algorithm> library can utilize this class.

*/


template<typename T, typename DataT>
class const_bst_iterator;

/*

    --- CLASS INFORMATION ---
Identification: S0C9-OBJ-NA-ST

Name: reverse_bst_iterator

Parent: None

Behaviour List:
- Default Constructible
- Copy Constructible
- Swappable
- Arithmetic Operable
- Equality Comparable
- Sign Comparable
- Templated
- Type Aware

Description:
This class provides an interface for reverse iteration of binary search trees(bst).
It's interface is mostly identical to the standard library iterators.
For this reason, <algorithm> library can utilize this class.

*/

template<typename T, typename DataT>
class reverse_bst_iterator;

/*

    --- CLASS INFORMATION ---
Identification: S0C10-OBJ-NA-ST

Name: const_reverse_bst_iterator

Parent: None

Behaviour List:
- Default Constructible
- Copy Constructible
- Swappable
- Arithmetic Operable
- Equality Comparable
- Sign Comparable
- Templated
- Type Aware

Description:
This class provides an interface for constant reverse iteration of binary search trees(bst).
It's interface is mostly identical to the standard library iterators.
For this reason, <algorithm> library can utilize this class.

*/

template<typename T, typename DataT>
class const_reverse_bst_iterator;

template<typename T, typename DataT>
class bst_iterator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;
    using iterator_category = std::bidirectional_iterator_tag;

    /* ===== BUILDER METHODS BEGIN ===== */
    MBASE_INLINE bst_iterator(pointer in_ptr) noexcept;
    MBASE_INLINE bst_iterator(pointer in_ptr, bool in_noTraverse) noexcept;
    MBASE_INLINE bst_iterator(const bst_iterator& in_rhs) noexcept;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE pointer get() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE DataT& operator*() const noexcept;
    MBASE_INLINE DataT* operator->() const noexcept;
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_INLINE bst_iterator& operator++() noexcept;
    MBASE_INLINE bst_iterator& operator++(I32) noexcept;
    MBASE_INLINE bst_iterator& operator--() noexcept;
    MBASE_INLINE bst_iterator& operator--(I32) noexcept;
    /* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

    /* ===== OPERATOR NON-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const bst_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const bst_iterator& in_rhs) const noexcept;
    /* ===== OPERATOR NON-MODIFIER METHODS END ===== */

    pointer _ptr;
};

template<typename T, typename DataT>
class const_bst_iterator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;
    using dt = DataT;
    using iterator_category = std::bidirectional_iterator_tag;

    /* ===== BUILDER METHODS BEGIN ===== */
    MBASE_INLINE const_bst_iterator(pointer in_ptr) noexcept;
    MBASE_INLINE const_bst_iterator(const const_bst_iterator& in_rhs) noexcept;
    MBASE_INLINE const_bst_iterator(const bst_iterator<T, DataT>& in_rhs) noexcept;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_pointer get() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const DataT& operator*() const noexcept;
    MBASE_INLINE const DataT* operator->() const noexcept;
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_INLINE const_bst_iterator& operator++() noexcept;
    MBASE_INLINE const_bst_iterator& operator++(I32) noexcept;
    MBASE_INLINE const_bst_iterator& operator--() noexcept;
    MBASE_INLINE const_bst_iterator& operator--(I32) noexcept;
    /* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

    /* ===== OPERATOR NON-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const const_bst_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const const_bst_iterator& in_rhs) const noexcept;
    /* ===== OPERATOR NON-MODIFIER METHODS END ===== */

    pointer _ptr;
};

template<typename T, typename DataT>
class reverse_bst_iterator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;
    using iterator_category = std::bidirectional_iterator_tag;

    /* ===== BUILDER METHODS BEGIN ===== */
    MBASE_INLINE reverse_bst_iterator(pointer in_ptr) noexcept;
    MBASE_INLINE reverse_bst_iterator(const reverse_bst_iterator& in_rhs) noexcept;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE pointer get() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE DataT& operator*() const noexcept;
    MBASE_INLINE DataT* operator->() const noexcept;
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_INLINE reverse_bst_iterator& operator++() noexcept;
    MBASE_INLINE reverse_bst_iterator& operator++(I32) noexcept;
    MBASE_INLINE reverse_bst_iterator& operator--() noexcept;
    MBASE_INLINE reverse_bst_iterator& operator--(I32) noexcept;
    /* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

    /* ===== OPERATOR NON-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const reverse_bst_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const reverse_bst_iterator& in_rhs) const noexcept;
    /* ===== OPERATOR NON-MODIFIER METHODS END ===== */

    pointer _ptr;
};

template<typename T, typename DataT>
class const_reverse_bst_iterator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using size_type = SIZE_T;
    using difference_type = PTRDIFF;
    using iterator_category = std::bidirectional_iterator_tag;

    /* ===== BUILDER METHODS BEGIN ===== */
    MBASE_INLINE const_reverse_bst_iterator(pointer in_ptr) noexcept;
    MBASE_INLINE const_reverse_bst_iterator(const const_reverse_bst_iterator& in_rhs) noexcept;
    MBASE_INLINE const_reverse_bst_iterator(const reverse_bst_iterator<T, DataT>& in_rhs) noexcept;
    /* ===== BUILDER METHODS END ===== */

    /* ===== OBSERVATION METHODS BEGIN ===== */
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_pointer get() const noexcept;
    MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const DataT& operator*() const noexcept;
    MBASE_INLINE const DataT* operator->() const noexcept;
    /* ===== OBSERVATION METHODS END ===== */

    /* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
    MBASE_INLINE const_reverse_bst_iterator& operator++() noexcept;
    MBASE_INLINE const_reverse_bst_iterator& operator++(I32) noexcept;
    MBASE_INLINE const_reverse_bst_iterator& operator--() noexcept;
    MBASE_INLINE const_reverse_bst_iterator& operator--(I32) noexcept;
    /* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

    /* ===== OPERATOR NON-MODIFIER METHODS BEGIN ===== */
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator==(const const_reverse_bst_iterator& in_rhs) const noexcept;
    MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool operator!=(const const_reverse_bst_iterator& in_rhs) const noexcept;
    /* ===== OPERATOR NON-MODIFIER METHODS END ===== */

    pointer _ptr;
};


template<typename T, typename DataT>
MBASE_INLINE bst_iterator<T, DataT>::bst_iterator(pointer in_ptr) noexcept : _ptr(in_ptr)
{
    while (in_ptr)
    {
        _ptr = in_ptr;
        in_ptr = in_ptr->left;
    }
}

template<typename T, typename DataT>
MBASE_INLINE bst_iterator<T, DataT>::bst_iterator(pointer in_ptr, bool in_noTraverse) noexcept : _ptr(in_ptr) {

}


template<typename T, typename DataT>
MBASE_INLINE bst_iterator<T, DataT>::bst_iterator(const bst_iterator& in_rhs) noexcept
{
    _ptr = in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename bst_iterator<T, DataT>::pointer bst_iterator<T, DataT>::get() const noexcept
{
    return _ptr;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE DataT& bst_iterator<T, DataT>::operator*() const noexcept
{
    return _ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE DataT* bst_iterator<T, DataT>::operator->() const noexcept {
    return &_ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE bst_iterator<T, DataT>& bst_iterator<T, DataT>::operator++() noexcept 
{
    if(!_ptr)
    {
        return *this;
    }

    if (_ptr->right)
    {
        pointer leftTraverse = _ptr->right;
        while (leftTraverse->left)
        {
            leftTraverse = leftTraverse->left;
        }
        _ptr = leftTraverse;
    }
    else
    {
        if (_ptr->parent)
        {
            if (_ptr == _ptr->parent->left)
            {
                _ptr = _ptr->parent;
            }
            else if (_ptr == _ptr->parent->right)
            {
                typename value_type::key_compare kc;
                pointer parentTraversal = _ptr->parent;
                while (parentTraversal && (!kc(_ptr->data, parentTraversal->data)))
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

template<typename T, typename DataT>
MBASE_INLINE bst_iterator<T, DataT>& bst_iterator<T, DataT>::operator++(I32) noexcept 
{
    bst_iterator bi(*this);
    ++(*this);
    return bi;
}

template<typename T, typename DataT>
MBASE_INLINE bst_iterator<T, DataT>& bst_iterator<T, DataT>::operator--() noexcept 
{
    if (!_ptr)
    {
        return *this;
    }

    if (_ptr->left)
    {

        pointer lrTraverse = _ptr->left;
        while (lrTraverse->right)
        {
            lrTraverse = lrTraverse->right;
        }
        _ptr = lrTraverse;
    }
    else
    {
        if (_ptr->parent)
        {
            if (_ptr == _ptr->parent->left)
            {
                typename value_type::key_compare kc;
                pointer parentTraverse = _ptr;
                while (parentTraverse->parent && kc(_ptr->data, parentTraverse->parent->data))
                {
                    parentTraverse = parentTraverse->parent;
                }

                _ptr = parentTraverse->parent;
            }
            else if (_ptr == _ptr->parent->right)
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

template<typename T, typename DataT>
MBASE_INLINE bst_iterator<T, DataT>& bst_iterator<T, DataT>::operator--(I32) noexcept
{
    bst_iterator bi(*this);
    --(*this);
    return bi;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool bst_iterator<T, DataT>::operator==(const bst_iterator& in_rhs) const noexcept
{
    return _ptr == in_rhs.get();
}

template<typename T, typename DataT>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool bst_iterator<T, DataT>::operator!=(const bst_iterator& in_rhs) const noexcept
{
    return _ptr != in_rhs.get();
}

/* ---- CONST IMPLEMENTATION ---- */

template<typename T, typename DataT>
MBASE_INLINE const_bst_iterator<T, DataT>::const_bst_iterator(pointer in_ptr) noexcept : _ptr(in_ptr)
{
    while (in_ptr)
    {
        _ptr = in_ptr;
        in_ptr = in_ptr->left;
    }
}

template<typename T, typename DataT>
MBASE_INLINE const_bst_iterator<T, DataT>::const_bst_iterator(const const_bst_iterator& in_rhs) noexcept
{
    _ptr = in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_INLINE const_bst_iterator<T, DataT>::const_bst_iterator(const bst_iterator<T, DataT>& in_rhs) noexcept 
{
    _ptr = in_rhs.get();
}


template<typename T, typename DataT>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename const_bst_iterator<T, DataT>::const_pointer const_bst_iterator<T, DataT>::get() const noexcept
{
    return _ptr;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const DataT& const_bst_iterator<T, DataT>::operator*() const noexcept
{
    return _ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE const DataT* const_bst_iterator<T, DataT>::operator->() const noexcept {
    return &_ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE const_bst_iterator<T, DataT>& const_bst_iterator<T, DataT>::operator++() noexcept
{
    if (!_ptr)
    {
        return *this;
    }

    if (_ptr->right)
    {
        pointer leftTraverse = _ptr->right;
        while (leftTraverse->left)
        {
            leftTraverse = leftTraverse->left;
        }
        _ptr = leftTraverse;
    }
    else
    {
        if (_ptr->parent)
        {
            if (_ptr == _ptr->parent->left)
            {
                _ptr = _ptr->parent;
            }
            else if (_ptr == _ptr->parent->right)
            {
                typename value_type::key_compare kc;
                pointer parentTraversal = _ptr->parent;
                while (parentTraversal && (!kc(_ptr->data, parentTraversal->data)))
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

template<typename T, typename DataT>
MBASE_INLINE const_bst_iterator<T, DataT>& const_bst_iterator<T, DataT>::operator++(I32) noexcept
{
    const_bst_iterator bi(*this);
    ++(*this);
    return bi;
}

template<typename T, typename DataT>
MBASE_INLINE const_bst_iterator<T, DataT>& const_bst_iterator<T, DataT>::operator--() noexcept
{
    if (!_ptr)
    {
        return *this;
    }

    if (_ptr->left)
    {

        pointer lrTraverse = _ptr->left;
        while (lrTraverse->right)
        {
            lrTraverse = lrTraverse->right;
        }
        _ptr = lrTraverse;
    }
    else
    {
        if (_ptr->parent)
        {
            if (_ptr == _ptr->parent->left)
            {
                typename value_type::key_compare kc;
                pointer parentTraverse = _ptr;
                while (parentTraverse->parent && kc(_ptr->data, parentTraverse->parent->data))
                {
                    parentTraverse = parentTraverse->parent;
                }

                _ptr = parentTraverse->parent;
            }
            else if (_ptr == _ptr->parent->right)
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

template<typename T, typename DataT>
MBASE_INLINE const_bst_iterator<T, DataT>& const_bst_iterator<T, DataT>::operator--(I32) noexcept
{
    const_bst_iterator bi(*this);
    --(*this);
    return bi;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool const_bst_iterator<T, DataT>::operator==(const const_bst_iterator& in_rhs) const noexcept
{
    return _ptr == in_rhs.get();
}

template<typename T, typename DataT>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool const_bst_iterator<T, DataT>::operator!=(const const_bst_iterator& in_rhs) const noexcept
{
    return _ptr != in_rhs.get();
}

/* ----- REVERSE IMPLEMENTATION ----- */

template<typename T, typename DataT>
MBASE_INLINE reverse_bst_iterator<T, DataT>::reverse_bst_iterator(pointer in_ptr) noexcept : _ptr(in_ptr)
{
    while (in_ptr)
    {
        _ptr = in_ptr;
        in_ptr = in_ptr->right;
    }
}

template<typename T, typename DataT>
MBASE_INLINE reverse_bst_iterator<T, DataT>::reverse_bst_iterator(const reverse_bst_iterator& in_rhs) noexcept
{
    _ptr = in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename reverse_bst_iterator<T, DataT>::pointer reverse_bst_iterator<T, DataT>::get() const noexcept
{
    return _ptr;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE DataT& reverse_bst_iterator<T, DataT>::operator*() const noexcept
{
    return _ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE DataT* reverse_bst_iterator<T, DataT>::operator->() const noexcept {
    return &_ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE reverse_bst_iterator<T, DataT>& reverse_bst_iterator<T, DataT>::operator++() noexcept
{
    if (!_ptr)
    {
        return *this;
    }

    if (_ptr->left)
    {

        pointer lrTraverse = _ptr->left;
        while (lrTraverse->right)
        {
            lrTraverse = lrTraverse->right;
        }
        _ptr = lrTraverse;
    }
    else
    {
        if (_ptr->parent)
        {
            if (_ptr == _ptr->parent->left)
            {
                typename value_type::key_compare kc;
                pointer parentTraverse = _ptr;
                while (parentTraverse->parent && kc(_ptr->data, parentTraverse->parent->data))
                {
                    parentTraverse = parentTraverse->parent;
                }

                _ptr = parentTraverse->parent;
            }
            else if (_ptr == _ptr->parent->right)
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

template<typename T, typename DataT>
MBASE_INLINE reverse_bst_iterator<T, DataT>& reverse_bst_iterator<T, DataT>::operator++(I32) noexcept
{
    reverse_bst_iterator bi(*this);
    ++(*this);
    return bi;
}

template<typename T, typename DataT>
MBASE_INLINE reverse_bst_iterator<T, DataT>& reverse_bst_iterator<T, DataT>::operator--() noexcept
{
    if (!_ptr)
    {
        return *this;
    }

    if (_ptr->right)
    {
        pointer leftTraverse = _ptr->right;
        while (leftTraverse->left)
        {
            leftTraverse = leftTraverse->left;
        }
        _ptr = leftTraverse;
    }
    else
    {
        if (_ptr->parent)
        {
            if (_ptr == _ptr->parent->left)
            {
                _ptr = _ptr->parent;
            }
            else if (_ptr == _ptr->parent->right)
            {
                typename value_type::key_compare kc;
                pointer parentTraversal = _ptr->parent;
                while (parentTraversal && (!kc(_ptr->data, parentTraversal->data)))
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

template<typename T, typename DataT>
MBASE_INLINE reverse_bst_iterator<T, DataT>& reverse_bst_iterator<T, DataT>::operator--(I32) noexcept
{
    reverse_bst_iterator bi(*this);
    --(*this);
    return bi;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool reverse_bst_iterator<T, DataT>::operator==(const reverse_bst_iterator& in_rhs) const noexcept
{
    return _ptr == in_rhs.get();
}

template<typename T, typename DataT>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool reverse_bst_iterator<T, DataT>::operator!=(const reverse_bst_iterator& in_rhs) const noexcept
{
    return _ptr != in_rhs.get();
}

/* ----- CONST REVERSE IMPLEMENTATION ----- */ 

template<typename T, typename DataT>
MBASE_INLINE const_reverse_bst_iterator<T, DataT>::const_reverse_bst_iterator(pointer in_ptr) noexcept : _ptr(in_ptr)
{
    while (in_ptr)
    {
        _ptr = in_ptr;
        in_ptr = in_ptr->right;
    }
}

template<typename T, typename DataT>
MBASE_INLINE const_reverse_bst_iterator<T, DataT>::const_reverse_bst_iterator(const const_reverse_bst_iterator& in_rhs) noexcept
{
    _ptr = in_rhs._ptr;
}

template<typename T, typename DataT>
MBASE_INLINE const_reverse_bst_iterator<T, DataT>::const_reverse_bst_iterator(const reverse_bst_iterator<T, DataT>& in_rhs) noexcept
{
    _ptr = in_rhs.get();
}


template<typename T, typename DataT>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename const_reverse_bst_iterator<T, DataT>::const_pointer const_reverse_bst_iterator<T, DataT>::get() const noexcept
{
    return _ptr;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const DataT& const_reverse_bst_iterator<T, DataT>::operator*() const noexcept
{
    return _ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE const DataT* const_reverse_bst_iterator<T, DataT>::operator->() const noexcept {
    return &_ptr->data;
}

template<typename T, typename DataT>
MBASE_INLINE const_reverse_bst_iterator<T, DataT>& const_reverse_bst_iterator<T, DataT>::operator++() noexcept
{
    if (!_ptr)
    {
        return *this;
    }

    if (_ptr->left)
    {

        pointer lrTraverse = _ptr->left;
        while (lrTraverse->right)
        {
            lrTraverse = lrTraverse->right;
        }
        _ptr = lrTraverse;
    }
    else
    {
        if (_ptr->parent)
        {
            if (_ptr == _ptr->parent->left)
            {
                typename value_type::key_compare kc;
                pointer parentTraverse = _ptr;
                while (parentTraverse->parent && kc(_ptr->data, parentTraverse->parent->data))
                {
                    parentTraverse = parentTraverse->parent;
                }

                _ptr = parentTraverse->parent;
            }
            else if (_ptr == _ptr->parent->right)
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

template<typename T, typename DataT>
MBASE_INLINE const_reverse_bst_iterator<T, DataT>& const_reverse_bst_iterator<T, DataT>::operator++(I32) noexcept
{
    const_reverse_bst_iterator bi(*this);
    ++(*this);
    return bi;
}

template<typename T, typename DataT>
MBASE_INLINE const_reverse_bst_iterator<T, DataT>& const_reverse_bst_iterator<T, DataT>::operator--() noexcept
{
    if (!_ptr)
    {
        return *this;
    }

    if (_ptr->right)
    {
        pointer leftTraverse = _ptr->right;
        while (leftTraverse->left)
        {
            leftTraverse = leftTraverse->left;
        }
        _ptr = leftTraverse;
    }
    else
    {
        if (_ptr->parent)
        {
            if (_ptr == _ptr->parent->left)
            {
                _ptr = _ptr->parent;
            }
            else if (_ptr == _ptr->parent->right)
            {
                typename value_type::key_compare kc;
                pointer parentTraversal = _ptr->parent;
                while (parentTraversal && (!kc(_ptr->data, parentTraversal->data)))
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

template<typename T, typename DataT>
MBASE_INLINE const_reverse_bst_iterator<T, DataT>& const_reverse_bst_iterator<T, DataT>::operator--(I32) noexcept
{
    const_reverse_bst_iterator bi(*this);
    --(*this);
    return bi;
}

template<typename T, typename DataT>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool const_reverse_bst_iterator<T, DataT>::operator==(const const_reverse_bst_iterator& in_rhs) const noexcept
{
    return _ptr == in_rhs.get();
}

template<typename T, typename DataT>
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE bool const_reverse_bst_iterator<T, DataT>::operator!=(const const_reverse_bst_iterator& in_rhs) const noexcept
{
    return _ptr != in_rhs.get();
}


MBASE_STD_END

#endif // !MBASE_BINARY_ITERATOR
