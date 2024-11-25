#ifndef MBASE_ARGUMENT_GET_VALUE_H
#define MBASE_ARGUMENT_GET_VALUE_H

#include <mbase/common.h>
#include <mbase/smart_conversion.h>
#include <utility>

MBASE_BEGIN

template<typename Type>
struct argument_get_numeric {
    static bool value(const I32& in_index, const I32& in_argc, IBYTEBUFFER* in_argv, Type& out_value)
    {
        if (in_index >= in_argc || in_index + 1 == in_argc || in_index < 0)
        {
            // If index is bigger or equal to argc, invocation is invalid
            // If index + 1 is equal to argc, it means that it does not have a value to set
            // If index is less than 0, invocation is invalid
            return false;
        }

        out_value = mbase::smart_conversion<Type>::apply(in_argv[in_index + 1]);
        return true;
    }
};

struct argument_get_string {
    static bool value(const I32& in_index, const I32& in_argc, IBYTEBUFFER* in_argv, mbase::string& out_value)
    {
        if (in_index >= in_argc || in_index + 1 == in_argc)
        {
            // If index is bigger or equal to argc, invocation is invalid
            // If index + 1 is equal to argc, it means that it does not have a value to set
            return false;
        }

        out_value = in_argv[in_index + 1];
        return true;
    }
};

template<typename Type>
struct argument_get {
    using get_type = std::conditional_t<std::is_same_v<Type, mbase::string>, argument_get_string, argument_get_numeric<Type>>;
    static bool value(const I32& in_index, const I32& in_argc, IBYTEBUFFER* in_argv, Type& out_value)
    {
        return get_type::value(in_index, in_argc, in_argv, out_value);
    }
};

MBASE_END

#endif // !MBASE_ARGUMENT_GET_VALUE_H
