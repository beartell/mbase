#ifndef MBASE_STD_COMMON_H
#define MBASE_STD_COMMON_H

#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#endif


#endif

#define MBASE_INLINE inline
#define MBASE_CONSTEXPR constexpr
#define MBASE_INLINE_EXPR inline constexpr

#define USED_RETURN [[nodiscard]]

#ifndef MBASE_STD_API
	#if defined(_MSC_VER)
		#define MBASE_STD_API __declspec(dllexport)
		#define MBASE_API MBASE_STD_API
	#endif // _WIN32
	// MAKE SURE TO IMPL THE CYGWIN AND GNUC
#endif

#define MBASE_STD_EXPLICIT explicit
#define MBASE_EXPLICIT explicit

#define MBASE_STD_NAME "MBASESTD"
#define MBASE_STD_VERSION "1.0.0"
#define MBASE_STD_VERSION_N 100

#define MBASE_STD_BEGIN namespace mbase{
#define MBASE_STD_END }

#define MBASE_BEGIN MBASE_STD_BEGIN
#define MBASE_END MBASE_STD_END

#define MBASE_NULL_CHECK(in_arg) (in_arg == nullptr)
#define MBASE_NULL_CHECK_RETURN(in_arg) \
if(MBASE_NULL_CHECK(in_arg))\
{\
	return;\
}

#define MBASE_NULL_CHECK_RETURN_VAL(in_arg, in_val) \
if(MBASE_NULL_CHECK(in_arg))\
{\
	return in_val;\
}

#ifdef MBASE_ASYNC_IO_THREAD_SAFE
	#define MBASE_TS_LOCK(in_mutex) in_mutex.acquire();
	#define MBASE_TS_UNLOCK(in_mutex) in_mutex.release();
#else
	#define MBASE_TS_LOCK(in_mutex)
	#define MBASE_TS_UNLOCK(in_mutex)
#endif // MBASE_ASYNC_IO_THREAD_SAFE


MBASE_STD_BEGIN

using I8		  = char;
using I16		  = int16_t;
using I32		  = int32_t;
using I64		  = int64_t;

using U8		  = uint8_t;
using U16		  = uint16_t;
using U32		  = uint32_t;
using U64		  = uint64_t;

using F32		  = float;
using F64		  = double;

using SIZE_T	  = U64;
using PTRDIFF	  = I64;

using PTR8	      = I8*;
using PTR16		  = I16*;
using PTR32		  = I32*;
using PTR64		  = I64*;

using PTRU8		  = U8*;
using PTRU16	  = U16*;
using PTRU32	  = U32*;
using PTRU64      = U64*;

using PTRF32	  = F32*;
using PTRF64	  = F64*;

using IBYTE		  = I8;
using UBYTE		  = U8;

using IBYTEPTR    = PTR8;
using UBYTEPTR    = PTRU8;

using IBYTEBUFFER = IBYTEPTR;
using UBYTEBUFFER = UBYTEPTR;

using MSTRING     = const IBYTEBUFFER;

using GENERIC     = void;
using PTRGENERIC  = GENERIC*;
using CPTRGENERIC = const PTRGENERIC;

using VBUFF       = PTRGENERIC;
using PTRVBUFF    = PTRGENERIC*;

MBASE_STD_END

#define I8_MIN INT8_MIN
#define I16_MIN INT16_MIN
#define I32_MIN INT32_MIN
#define I64_MIN INT64_MIN

#define I8_MAX INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX

#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#endif // MBASE_STD_COMMON_H