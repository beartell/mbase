#ifndef MBASE_STD_COMMON_H
#define MBASE_STD_COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <limits>

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#endif

#endif

/* LANGUAGE VERSION CONTROL */

#ifndef __cplusplus
#error Missing C++ compiler
#endif // !__cplusplus

#ifdef __cpp_pack_indexing
#define MBASE_CPP_VERSION 26
#elif __cpp_multidimensional_subscript
#define MBASE_CPP_VERSION 23
#elif __cpp_concepts
#define MBASE_CPP_VERSION 20
#elif __cpp_fold_expressions
#define MBASE_CPP_VERSION 17
#else
#error Invalid C++ version. MBASE require at least C++17.
#endif // !__cpp_pack_indexing

/* LANGUAGE VERSION CONTROL */

#if MBASE_CPP_VERSION >= 20
	#define MBASE_ND(in_reason) [[nodiscard(in_reason)]]
	#define MBASE_INLINE inline
	#define MBASE_INLINE_EXPR constexpr inline
#else
#if MBASE_CPP_VERSION < 17
	#define MBASE_ND(in_reason)
	#define MBASE_INLINE
	#define MBASE_INLINE_EXPR
#else
	#define MBASE_ND(in_reason) [[nodiscard]]
	#define MBASE_INLINE inline
	#define MBASE_INLINE_EXPR inline
#endif
#endif

#define MBASE_STD_LIB_COMPATIBLE

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

#define MBASE_OBS_IGNORE "observation return value ignored"
#define MBASE_IGNORE_NONTRIVIAL "non-trivial observation ignored"
#define MBASE_RESULT_IGNORE "result of the operation is ignored"

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

#define MBASE_FS_FLAGS_MIN 100
#define MBASE_FS_FLAGS_MAX 120
#define MBAES_FS_FLAGS_CONTROL_START 115

#define MBASE_IO_FLAGS_MIN 121
#define MBASE_IO_FLAGS_MAX 170
#define MBASE_IO_FLAGS_CONTROL_START 145

#define MBASE_AIO_MNG_FLAGS_MIN 171
#define MBAES_AIO_MNG_FLAGS_MAX 220
#define MBASE_AIO_MNG_FLAGS_CONTROL_START 200

#define MBASE_ASYNC_CTX_FLAGS_MIN 221
#define MBASE_ASYNC_CTX_FLAGS_MAX 270
#define MBASE_ASYNC_CTX_FLAGS_CONTROL_START 250

#define MBASE_TIMER_HANDLER_FLAGS_MIN 271
#define MBASE_TIMER_HANDLER_FLAGS_MAX 300
#define MBASE_TIMER_HANDLER_FLAGS_CONTROL_START 285

#define MBASE_EVENT_HANDLER_FLAGS_MIN 301
#define MBASE_EVENT_HANDLER_FLAGS_MAX 330
#define MBASE_EVENT_HANDLER_FLAGS_CONTROL_START 315

#define MBASE_TPOOL_FLAGS_MIN 331
#define MBASE_TPOOL_FLAGS_MAX 360
#define MBASE_TPOOL_FLAGS_CONTROL_START 340

#define MBASE_TIMER_LOOP_FLAGS_MIN 361
#define MBASE_TIMER_LOOP_FLAGS_MAX 390
#define MBASE_TIMER_LOOP_FLAGS_CONTROL_START 375

#define MBASE_EVENT_MANAGER_FLAGS_MIN 391
#define MBASE_EVENT_MANAGER_FLAGS_MAX 420
#define MBASE_EVENT_MANAGER_FLAGS_CONTROL_START 405

MBASE_STD_BEGIN

using I8 = char;
using I16 = int16_t;
using I32 = int32_t;
using I64 = int64_t;

using C8 = char;
using C16 = char16_t;
using C32 = char32_t;
using WIDE = wchar_t;

using U8 = uint8_t;
using U16 = uint16_t;
using U32 = uint32_t;
using U64 = uint64_t;

using F32 = float;
using F64 = double;

using SIZE_T = U64;
using PTRDIFF = I64;

using PTR8 = I8*;
using PTR16 = I16*;
using PTR32 = I32*;
using PTR64 = I64*;

using PTRC8 = C8*;
using PTRC16 = C16*;
using PTRC32 = C32*;

using PTRU8 = U8*;
using PTRU16 = U16*;
using PTRU32 = U32*;
using PTRU64 = U64*;

using PTRF32 = F32*;
using PTRF64 = F64*;

using IBYTE = I8;
using UBYTE = U8;

using IBYTEPTR = PTR8;
using UBYTEPTR = PTRU8;

using IBYTEBUFFER = IBYTEPTR;
using UBYTEBUFFER = UBYTEPTR;
using CBYTEBUFFER = const I8*;

using MSTRING = const I8*;
using MSTRING16 = const C16*;
using MSTRING32 = const C32*;
using MWSTRING = const WIDE*;

using GENERIC = void;
using PTRGENERIC = GENERIC*;
using CPTRGENERIC = const PTRGENERIC;

using VBUFF = PTRGENERIC;
using PTRVBUFF = PTRGENERIC*;

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