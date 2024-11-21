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

MBASE_STD_END

#endif // MBASE_ALGORITHM_H