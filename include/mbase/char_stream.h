#ifndef MBASE_CHARSTREAM_H
#define MBASE_CHARSTREAM_H

#include <mbase/common.h>
#include <mbase/type_sequence.h>
//#include <mbase/string.h> // mbase::string

MBASE_STD_BEGIN

/*

	--- CLASS INFORMATION ---
Identification: S0C11-OBJ-DV-ST

Name: char_stream

Parent: NONE

Behaviour List:
- Default Constructible
- Copy Constructible
- Swappable
- Arithmetic Operable
- Equality Comparable
- Sign Comparable

Description:
char_stream allows user to store the byte buffer inside an object
and do pointer arithmetic operations in a managed way.

User will use methods such as advance, reverse, putc, put_data etc. to do
pointer arithmetic operations on the pointer.

Along with pointer arithmetic, the char_stream also stores the state of the pointer such as,
where the cursor is, and the size of the data our pointer points to. By storing these 
informations, developer will be able to do pointer arithmetic operations in much more safer environment.

Be aware that the char_stream class will not either copy, delete, or allocate new buffer. For this reason,
developer must be sure that the buffer that is supplied to the char_stream class will remain valid.
Otherwise, it will be undefined behavior.

Both char_stream and deep_char_stream should be used with utmost care.

+--------+--------+--------+--------+--------+
|        |        |        |        |        |
| byte 0 | byte 1 | byte 2 |  ....  | byte n |
|        |        |        |        |        |
+--------+--------+--------+--------+--------+


*/

class char_stream : protected type_sequence<IBYTE> {
public:
	using reference = IBYTE&;
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE char_stream() noexcept;
	MBASE_INLINE MBASE_STD_EXPLICIT char_stream(IBYTEBUFFER in_src) noexcept;
	MBASE_INLINE MBASE_STD_EXPLICIT char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept;
	MBASE_INLINE ~char_stream() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference front() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference front() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference back() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference back() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool is_cursor_end() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type buffer_length() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type get_pos() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR reference getc() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const_reference getc() const noexcept;
	template<typename T>
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const T& get_data() const noexcept;
	template<typename T>
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR T& get_data() noexcept;
	template<typename T>
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR T& get_datan(size_type in_length = sizeof(T)) noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER get_buffer() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER get_bufferc() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER data() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR CBYTEBUFFER get_buffer() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR CBYTEBUFFER get_bufferc() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR CBYTEBUFFER data() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== OPERATOR OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE IBYTEBUFFER operator*() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE CBYTEBUFFER operator*() const noexcept;
	/* ===== OPERATOR OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR GENERIC advance() noexcept;
	MBASE_INLINE_EXPR GENERIC advance(size_type in_length) noexcept;
	MBASE_INLINE_EXPR GENERIC reverse() noexcept;
	MBASE_INLINE_EXPR GENERIC reverse(size_type in_length) noexcept;
	MBASE_INLINE_EXPR GENERIC putc(IBYTE in_byte) noexcept;
	MBASE_INLINE_EXPR GENERIC putcn(IBYTE in_byte) noexcept;
	MBASE_INLINE_EXPR GENERIC put_buffer(CBYTEBUFFER in_data, size_type in_length) noexcept;
	MBASE_INLINE_EXPR GENERIC put_buffern(CBYTEBUFFER in_data, size_type in_length) noexcept;
	template<typename T>
	MBASE_INLINE_EXPR GENERIC put_data(const T& in_data, size_type in_length = sizeof(T)) noexcept;
	template<typename T>
	MBASE_INLINE_EXPR GENERIC put_datan(const T& in_data, size_type in_length = sizeof(T)) noexcept;
	MBASE_INLINE_EXPR GENERIC set_cursor_pos(size_type in_pos) noexcept;
	MBASE_INLINE_EXPR GENERIC set_cursor_front() noexcept;
	MBASE_INLINE_EXPR GENERIC set_cursor_end() noexcept;
	MBASE_INLINE_EXPR GENERIC zero_out_buffer() noexcept;
	MBASE_INLINE_EXPR GENERIC zero_out_buffern() noexcept;
	MBASE_INLINE virtual GENERIC _destroy_self() noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE IBYTEBUFFER operator+=(size_type in_rhs) noexcept;
	MBASE_INLINE IBYTEBUFFER operator++() noexcept;
	MBASE_INLINE IBYTEBUFFER operator++(I32) noexcept;
	MBASE_INLINE IBYTEBUFFER operator-=(size_type in_rhs) noexcept;
	MBASE_INLINE IBYTEBUFFER operator--() noexcept;
	MBASE_INLINE IBYTEBUFFER operator--(I32) noexcept;
	/* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

	/* ===== OPERATOR NON-MEMBER FUNCTIONS BEGIN ===== */
	MBASE_ND(MBASE_RESULT_IGNORE) friend bool operator==(const char_stream& in_lhs, const char_stream& in_rhs) noexcept 
	{
		return type_sequence<IBYTE>::is_equal(in_lhs.mSrcBuffer, in_rhs.mSrcBuffer, in_lhs.mBufferLength);
	}

	MBASE_ND(MBASE_RESULT_IGNORE) friend bool operator!=(const char_stream& in_lhs, const char_stream& in_rhs) noexcept 
	{
		return !type_sequence<IBYTE>::is_equal(in_lhs.mSrcBuffer, in_rhs.mSrcBuffer, in_lhs.mBufferLength);
	}
	/* ===== OPERATOR NON-MEMBER FUNCTIONS END ===== */

protected:
	size_type mBufferLength;
	size_type mStreamCursor;
	IBYTEBUFFER mSrcBuffer;
};

/*

	--- CLASS INFORMATION ---
Identification: S0C12-OBJ-UD-ST

Name: deep_char_stream

Parent: S0C11-OBJ-UD-ST

Behaviour List:
- Default Constructible
- Copy Constructible
- Move Constructible
- Destructible
- Arithmetic Operable
- Equality Comparable
- Sign Comparable

Description:
deep_char_stream is all identical with char_stream with a little exception that,
the deep_char_stream will have it's own copy of the byte buffer and handle the allocation and destruction of it.

Both char_stream and deep_char_stream should be used with utmost care.

*/


class deep_char_stream : public char_stream {
public:
	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE MBASE_EXPLICIT deep_char_stream(IBYTEBUFFER in_src) noexcept;
	MBASE_INLINE MBASE_EXPLICIT deep_char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept;
	MBASE_INLINE MBASE_EXPLICIT deep_char_stream(size_type in_length);
	MBASE_INLINE deep_char_stream(const deep_char_stream& in_rhs) noexcept;
	MBASE_INLINE deep_char_stream(deep_char_stream&& in_rhs) noexcept;
	MBASE_INLINE ~deep_char_stream();
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	MBASE_INLINE deep_char_stream& operator=(const deep_char_stream& in_rhs) noexcept;
	MBASE_INLINE deep_char_stream& operator=(deep_char_stream&& in_rhs) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE GENERIC _destroy_self() noexcept override;
	/* ===== STATE-MODIFIER METHODS END ===== */
};

MBASE_INLINE char_stream::char_stream() noexcept : mBufferLength(0), mStreamCursor(0), mSrcBuffer(nullptr)
{
}

MBASE_INLINE char_stream::char_stream(IBYTEBUFFER in_src) noexcept {
	mBufferLength = this->length_bytes(in_src) + 1; // CHAR STREAM WILL INCLUDE NULL TERMINATOR
	mStreamCursor = 0;
	mSrcBuffer = in_src;
}

MBASE_INLINE char_stream::char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept : mBufferLength(in_length), mStreamCursor(0), mSrcBuffer(in_src)
{
}

MBASE_INLINE char_stream::~char_stream() noexcept
{
	_destroy_self();
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE IBYTEBUFFER char_stream::operator*() noexcept 
{
	return mSrcBuffer + mStreamCursor;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE CBYTEBUFFER char_stream::operator*() const noexcept
{
	return mSrcBuffer + mStreamCursor;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename char_stream::reference char_stream::front() noexcept
{
	return *mSrcBuffer;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename char_stream::const_reference char_stream::front() const noexcept
{
	return *mSrcBuffer;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename char_stream::reference char_stream::back() noexcept
{
	return *(mSrcBuffer + (mBufferLength - 1));
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename char_stream::const_reference char_stream::back() const noexcept
{
	return *(mSrcBuffer + (mBufferLength - 1));
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool char_stream::is_cursor_end() const noexcept
{
	return mStreamCursor == mBufferLength;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR char_stream::size_type char_stream::buffer_length() const noexcept 
{
	return mBufferLength;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR char_stream::size_type char_stream::get_pos() const noexcept
{
	return mStreamCursor;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename char_stream::reference char_stream::getc() noexcept
{
	return *(mSrcBuffer + mStreamCursor);
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR typename char_stream::const_reference char_stream::getc() const noexcept
{
	return *(mSrcBuffer + mStreamCursor);
}

template<typename T>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR const T& char_stream::get_data() const noexcept
{
	return *(reinterpret_cast<T*>(mSrcBuffer));
}
template<typename T>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR T& char_stream::get_data() noexcept
{
	return *(reinterpret_cast<T*>(mSrcBuffer));
}
template<typename T>
MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR T& char_stream::get_datan(size_type in_length) noexcept
{
	T* iData = reinterpret_cast<T*>(mSrcBuffer + mStreamCursor);
	advance(in_length);
	return *iData;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER char_stream::get_buffer() noexcept
{
	return mSrcBuffer;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER char_stream::get_bufferc() noexcept
{
	return mSrcBuffer + mStreamCursor;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER char_stream::data() noexcept
{
	return mSrcBuffer;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR CBYTEBUFFER char_stream::get_buffer() const noexcept
{
	return mSrcBuffer;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR CBYTEBUFFER char_stream::get_bufferc() const noexcept
{
	return mSrcBuffer + mStreamCursor;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR CBYTEBUFFER char_stream::data() const noexcept
{
	return mSrcBuffer;
}

MBASE_INLINE_EXPR GENERIC char_stream::advance() noexcept 
{
	++mStreamCursor;
}

MBASE_INLINE_EXPR GENERIC char_stream::advance(size_type in_length) noexcept
{
	mStreamCursor += in_length;
}

MBASE_INLINE_EXPR GENERIC char_stream::reverse() noexcept 
{
	--mStreamCursor;
}

MBASE_INLINE_EXPR GENERIC char_stream::reverse(size_type in_length) noexcept
{
	mStreamCursor -= in_length;
}

MBASE_INLINE_EXPR GENERIC char_stream::putc(IBYTE in_byte) noexcept 
{
	*(mSrcBuffer + mStreamCursor) = in_byte;
}

MBASE_INLINE_EXPR GENERIC char_stream::putcn(IBYTE in_byte) noexcept 
{
	*(mSrcBuffer + mStreamCursor++) = in_byte;
}

MBASE_INLINE_EXPR GENERIC char_stream::put_buffer(CBYTEBUFFER in_data, size_type in_length) noexcept 
{
	size_type tempCursorPos = mStreamCursor;
	for (size_type i = 0; i < in_length; i++)
	{
		*(mSrcBuffer + tempCursorPos++) = *(in_data + i);
	}
}

MBASE_INLINE_EXPR GENERIC char_stream::put_buffern(CBYTEBUFFER in_data, size_type in_length) noexcept
{
	for (size_type i = 0; i < in_length; i++)
	{
		*(mSrcBuffer + mStreamCursor++) = *(in_data + i);
	}
}

template<typename T>
MBASE_INLINE_EXPR GENERIC char_stream::put_data(const T& in_data, size_type in_length) noexcept
{
	put_buffer(reinterpret_cast<CBYTEBUFFER>(&in_data), in_length);
}

template<typename T>
MBASE_INLINE_EXPR GENERIC char_stream::put_datan(const T& in_data, size_type in_length) noexcept
{
	put_buffern(reinterpret_cast<CBYTEBUFFER>(&in_data), in_length);
}

MBASE_INLINE_EXPR GENERIC char_stream::set_cursor_pos(size_type in_pos) noexcept
{
	if (in_pos < 0)
	{
		mStreamCursor = 0;
		return;
	}

	mStreamCursor = in_pos;
}

MBASE_INLINE_EXPR GENERIC char_stream::set_cursor_front() noexcept 
{
	mStreamCursor = 0;
}

MBASE_INLINE_EXPR GENERIC char_stream::set_cursor_end() noexcept 
{
	mStreamCursor = mBufferLength - 1;
}

MBASE_INLINE_EXPR GENERIC char_stream::zero_out_buffer() noexcept 
{
	size_type oldCursorPos = mStreamCursor;
	set_cursor_front();
	putcn(0);
	set_cursor_pos(oldCursorPos);
}

MBASE_INLINE_EXPR GENERIC char_stream::zero_out_buffern() noexcept
{
	putcn(0);
}

MBASE_INLINE GENERIC char_stream::_destroy_self() noexcept 
{
	// do nothing
}

MBASE_INLINE IBYTEBUFFER char_stream::operator+=(size_type in_rhs) noexcept
{
	advance(in_rhs);
	return mSrcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator++() noexcept 
{
	advance();
	return mSrcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator++(I32) noexcept 
{
	advance();
	return mSrcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator-=(size_type in_rhs) noexcept
{
	reverse(in_rhs);
	return mSrcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator--() noexcept 
{
	reverse();
	return mSrcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator--(I32) noexcept 
{
	reverse();
	return mSrcBuffer;
}

MBASE_INLINE deep_char_stream::deep_char_stream(IBYTEBUFFER in_src) noexcept : char_stream(in_src) 
{
	if (!mBufferLength)
	{
		return;
	}

	IBYTEBUFFER freshBuffer = static_cast<IBYTEBUFFER>(malloc(mBufferLength));
	this->copy_bytes(freshBuffer, mSrcBuffer, mBufferLength);
	mSrcBuffer = freshBuffer;
}

MBASE_INLINE deep_char_stream::deep_char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept : char_stream(in_src, in_length) 
{
	if(!mBufferLength)
	{
		return;
	}

	IBYTEBUFFER freshBuffer = static_cast<IBYTEBUFFER>(malloc(mBufferLength));
	this->copy_bytes(freshBuffer, mSrcBuffer, mBufferLength);
	mSrcBuffer = freshBuffer;
}

MBASE_INLINE deep_char_stream::deep_char_stream(size_type in_length) : char_stream() 
{
	if(!in_length)
	{
		return;
	}

	IBYTEBUFFER freshBuffer = static_cast<IBYTEBUFFER>(malloc(in_length));
	mBufferLength = in_length;
	mSrcBuffer = freshBuffer;
	this->fill(mSrcBuffer, 0, mBufferLength);
}

MBASE_INLINE deep_char_stream::deep_char_stream(const deep_char_stream& in_rhs) noexcept : char_stream() 
{
	mBufferLength = in_rhs.mBufferLength;
	mStreamCursor = in_rhs.mStreamCursor;
	if(!mBufferLength)
	{
		return;
	}

	mSrcBuffer = static_cast<IBYTEBUFFER>(malloc(mBufferLength));
	this->copy_bytes(mSrcBuffer, in_rhs.mSrcBuffer, in_rhs.mBufferLength);
}

MBASE_INLINE deep_char_stream::deep_char_stream(deep_char_stream&& in_rhs) noexcept 
{
	mBufferLength = in_rhs.mBufferLength;
	mSrcBuffer = in_rhs.mSrcBuffer;
	in_rhs.mSrcBuffer = nullptr;
	in_rhs.mBufferLength = 0;
	in_rhs.mStreamCursor = 0;
}

MBASE_INLINE deep_char_stream::~deep_char_stream() 
{
	_destroy_self();
}

MBASE_INLINE deep_char_stream& deep_char_stream::operator=(const deep_char_stream& in_rhs) noexcept 
{
	if (in_rhs.mBufferLength == mBufferLength)
	{
		mStreamCursor = in_rhs.mStreamCursor;
		this->fill(mSrcBuffer, 0, mBufferLength);
		this->copy_bytes(mSrcBuffer, in_rhs.mSrcBuffer, in_rhs.mBufferLength);
	}
	else {
		_destroy_self();
		mBufferLength = in_rhs.mBufferLength;
		mStreamCursor = in_rhs.mStreamCursor;
		mSrcBuffer = static_cast<IBYTEBUFFER>(malloc(mBufferLength));
		this->fill(mSrcBuffer, 0, mBufferLength);
		this->copy_bytes(mSrcBuffer, in_rhs.mSrcBuffer, in_rhs.mBufferLength);
	}

	return *this;
}

MBASE_INLINE deep_char_stream& deep_char_stream::operator=(deep_char_stream&& in_rhs) noexcept 
{
	_destroy_self();
	mBufferLength = in_rhs.mBufferLength;
	mSrcBuffer = in_rhs.mSrcBuffer;
	in_rhs.mSrcBuffer = nullptr;
	in_rhs.mBufferLength = 0;
	in_rhs.mStreamCursor = 0;
	return *this;
}

MBASE_INLINE GENERIC deep_char_stream::_destroy_self() noexcept 
{
	if (!mSrcBuffer)
	{
		return;
	}

	free(mSrcBuffer);
	mSrcBuffer = nullptr;
	mBufferLength = 0;
	mStreamCursor = 0;
}

MBASE_STD_END

#endif // MBASE_CHARSTREAM_H