#ifndef MBASE_ALGORITHM_H
#define MBASE_ALGORITHM_H

#include <mbase/common.h>

MBASE_STD_BEGIN

template<typename InputIt, typename Type>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE InputIt find(InputIt in_first, InputIt in_last, const Type& in_value)
{
    for(; in_first != in_last; in_first++)
    {
        if(*in_first == in_value)
        {
            return in_first;
        }
    }
    return in_last;
}

#undef max
template<typename Type>
MBASE_INLINE_EXPR const Type& max(const Type& in_left, const Type& in_right)
{
    return in_left > in_right ? in_left : in_right;
}

#undef min
template<typename Type>
MBASE_INLINE_EXPR const Type& min(const Type& in_left, const Type& in_right)
{
    return in_left < in_right ? in_left : in_right;
}

template<typename Iterator>
MBASE_INLINE_EXPR Iterator max_element(Iterator in_first, Iterator in_last)
{
    if (in_first == in_last)
    {
        return in_last;
    }
 
    Iterator largest = in_first;
 
    while (++in_first != in_last)
    {
        if (*largest < *in_first)
        {
            largest = in_first;
        }
    }

    return largest;
}

template<typename Iterator>
MBASE_INLINE_EXPR Iterator min_element(Iterator in_first, Iterator in_last)
{
    if (in_first == in_last)
    {
        return in_last;
    }
 
    Iterator smallest = in_first;
 
    while (++in_first != in_last)
    {
        if (*smallest > *in_first)
        {
            smallest = in_first;
        }
    }

    return smallest;
}

template<typename Iterator>
MBASE_INLINE_EXPR typename Iterator::value_type average_element(Iterator in_first, Iterator in_last)
{
    typename Iterator::value_type avg = 0;
    SIZE_T elemCount = 0;
    for(; in_first != in_last; ++in_first)
    {
        avg += *in_first;
        ++elemCount;
    }

    if(!elemCount)
    {
        return 0;
    }

    return avg / elemCount;
}

MBASE_STD_END

#endif // MBASE_ALGORITHM_H