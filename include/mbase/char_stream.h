#ifndef MBASE_CHARSTREAM_H
#define MBASE_CHARSTREAM_H

#include <mbase/common.h>
#include <mbase/string.h>

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

*/

class char_stream : protected mbase::type_sequence<IBYTE> {
public:
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE char_stream() noexcept;
	MBASE_INLINE MBASE_STD_EXPLICIT char_stream(IBYTEBUFFER in_src) noexcept;
	MBASE_INLINE MBASE_STD_EXPLICIT char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept;
	MBASE_INLINE ~char_stream() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR IBYTE front() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR IBYTE back() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool is_cursor_end() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR size_type buffer_length() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER get_buffer() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER get_bufferc() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR difference_type get_pos() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR IBYTE getc() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER data() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== OPERATOR OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE IBYTEBUFFER operator*() noexcept;
	/* ===== OPERATOR OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE_EXPR GENERIC advance() noexcept;
	MBASE_INLINE_EXPR GENERIC advance(difference_type in_length) noexcept;
	MBASE_INLINE_EXPR GENERIC reverse() noexcept;
	MBASE_INLINE_EXPR GENERIC reverse(difference_type in_length) noexcept;
	MBASE_INLINE_EXPR GENERIC putc(IBYTE in_byte) noexcept;
	MBASE_INLINE_EXPR GENERIC putcn(IBYTE in_byte) noexcept;
	MBASE_INLINE_EXPR GENERIC put_data(IBYTEBUFFER in_data, size_type in_length) noexcept;
	MBASE_INLINE_EXPR GENERIC put_datan(IBYTEBUFFER in_data, size_type in_length) noexcept;
	MBASE_INLINE_EXPR GENERIC set_cursor_pos(difference_type in_pos) noexcept;
	MBASE_INLINE_EXPR GENERIC set_cursor_front() noexcept;
	MBASE_INLINE_EXPR GENERIC set_cursor_end() noexcept;
	MBASE_INLINE_EXPR GENERIC zero_out_buffer() noexcept;
	MBASE_INLINE_EXPR GENERIC zero_out_buffern() noexcept;
	MBASE_INLINE virtual GENERIC _destroy_self() noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== OPERATOR STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE IBYTEBUFFER operator+=(difference_type in_rhs) noexcept;
	MBASE_INLINE IBYTEBUFFER operator++() noexcept;
	MBASE_INLINE IBYTEBUFFER operator++(I32) noexcept;
	MBASE_INLINE IBYTEBUFFER operator-=(difference_type in_rhs) noexcept;
	MBASE_INLINE IBYTEBUFFER operator--() noexcept;
	MBASE_INLINE IBYTEBUFFER operator--(I32) noexcept;
	/* ===== OPERATOR STATE-MODIFIER METHODS END ===== */

	/* ===== OPERATOR NON-MEMBER FUNCTIONS BEGIN ===== */
	MBASE_ND(MBASE_RESULT_IGNORE) friend bool operator==(const char_stream& in_lhs, const char_stream& in_rhs) noexcept {
		return mbase::type_sequence<IBYTE>::is_equal(in_lhs.srcBuffer, in_rhs.srcBuffer, in_lhs.bufferLength);
	}

	MBASE_ND(MBASE_RESULT_IGNORE) friend bool operator!=(const char_stream& in_lhs, const char_stream& in_rhs) noexcept {
		return !mbase::type_sequence<IBYTE>::is_equal(in_lhs.srcBuffer, in_rhs.srcBuffer, in_lhs.bufferLength);
	}
	/* ===== OPERATOR NON-MEMBER FUNCTIONS END ===== */

protected:
	size_type bufferLength;
	difference_type streamCursor;
	IBYTEBUFFER srcBuffer;
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

MBASE_INLINE char_stream::char_stream() noexcept : bufferLength(0), streamCursor(0), srcBuffer(nullptr)
{
}

MBASE_INLINE char_stream::char_stream(IBYTEBUFFER in_src) noexcept {
	bufferLength = length_bytes(in_src) + 1; // CHAR STREAM WILL INCLUDE NULL TERMINATOR
	streamCursor = 0;
	srcBuffer = in_src;
}

MBASE_INLINE char_stream::char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept : bufferLength(in_length), streamCursor(0), srcBuffer(in_src)
{
}

MBASE_INLINE char_stream::~char_stream() noexcept
{
	_destroy_self();
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE IBYTEBUFFER char_stream::operator*() noexcept {
	return srcBuffer + streamCursor;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR IBYTE char_stream::front() const noexcept {
	return *srcBuffer;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR IBYTE char_stream::back() const noexcept {
	return *(srcBuffer + (bufferLength - 1));
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR bool char_stream::is_cursor_end() const noexcept
{
	return streamCursor == bufferLength;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR char_stream::size_type char_stream::buffer_length() const noexcept {
	return bufferLength;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER char_stream::get_buffer() const noexcept {
	return srcBuffer;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER char_stream::get_bufferc() const noexcept {
	return srcBuffer + streamCursor;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR char_stream::difference_type char_stream::get_pos() const noexcept {
	return streamCursor;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE_EXPR IBYTE char_stream::getc() const noexcept {
	return *(srcBuffer + streamCursor);
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE_EXPR IBYTEBUFFER char_stream::data() const noexcept {
	return srcBuffer;
}

MBASE_INLINE_EXPR GENERIC char_stream::advance() noexcept {
	++streamCursor;
}

MBASE_INLINE_EXPR GENERIC char_stream::advance(difference_type in_length) noexcept {
	streamCursor += in_length;
}

MBASE_INLINE_EXPR GENERIC char_stream::reverse() noexcept {
	--streamCursor;
}

MBASE_INLINE_EXPR GENERIC char_stream::reverse(difference_type in_length) noexcept {
	streamCursor -= in_length;
}

MBASE_INLINE_EXPR GENERIC char_stream::putc(IBYTE in_byte) noexcept {
	*(srcBuffer + streamCursor) = in_byte;
}

MBASE_INLINE_EXPR GENERIC char_stream::putcn(IBYTE in_byte) noexcept {
	*(srcBuffer + streamCursor++) = in_byte;
}

MBASE_INLINE_EXPR GENERIC char_stream::put_data(IBYTEBUFFER in_data, size_type in_length) noexcept {
	difference_type tempCursorPos = streamCursor;
	for (size_type i = 0; i < in_length; i++)
	{
		*(srcBuffer + tempCursorPos++) = *(in_data + i);
	}
}

MBASE_INLINE_EXPR GENERIC char_stream::put_datan(IBYTEBUFFER in_data, size_type in_length) noexcept {
	for (size_type i = 0; i < in_length; i++)
	{
		*(srcBuffer + streamCursor++) = *(in_data + i);
	}
}

MBASE_INLINE_EXPR GENERIC char_stream::set_cursor_pos(difference_type in_pos) noexcept {
	if (in_pos < 0)
	{
		streamCursor = 0;
		return;
	}

	streamCursor = in_pos;
}

MBASE_INLINE_EXPR GENERIC char_stream::set_cursor_front() noexcept {
	streamCursor = 0;
}

MBASE_INLINE_EXPR GENERIC char_stream::set_cursor_end() noexcept {
	streamCursor = bufferLength - 1;
}

MBASE_INLINE_EXPR GENERIC char_stream::zero_out_buffer() noexcept 
{
	difference_type oldCursorPos = streamCursor;
	set_cursor_front();
	putcn(0);
	set_cursor_pos(oldCursorPos);
}

MBASE_INLINE_EXPR GENERIC char_stream::zero_out_buffern() noexcept
{
	putcn(0);
}

MBASE_INLINE GENERIC char_stream::_destroy_self() noexcept {
	// do nothing
}

MBASE_INLINE IBYTEBUFFER char_stream::operator+=(difference_type in_rhs) noexcept {
	advance(in_rhs);
	return srcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator++() noexcept {
	advance();
	return srcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator++(I32) noexcept {
	advance();
	return srcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator-=(difference_type in_rhs) noexcept {
	reverse(in_rhs);
	return srcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator--() noexcept {
	reverse();
	return srcBuffer;
}

MBASE_INLINE IBYTEBUFFER char_stream::operator--(I32) noexcept {
	reverse();
	return srcBuffer;
}

MBASE_INLINE deep_char_stream::deep_char_stream(IBYTEBUFFER in_src) noexcept : char_stream(in_src) {
	IBYTEBUFFER freshBuffer = static_cast<IBYTEBUFFER>(malloc(bufferLength));
	copy_bytes(freshBuffer, srcBuffer, bufferLength);
	srcBuffer = freshBuffer;
}

MBASE_INLINE deep_char_stream::deep_char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept : char_stream(in_src, in_length) {
	IBYTEBUFFER freshBuffer = static_cast<IBYTEBUFFER>(malloc(bufferLength));
	copy_bytes(freshBuffer, srcBuffer, bufferLength);
	srcBuffer = freshBuffer;
}

MBASE_INLINE deep_char_stream::deep_char_stream(size_type in_length) : char_stream() {
	IBYTEBUFFER freshBuffer = static_cast<IBYTEBUFFER>(malloc(in_length));
	bufferLength = in_length;
	srcBuffer = freshBuffer;
	fill(srcBuffer, 0, bufferLength);
}

MBASE_INLINE deep_char_stream::deep_char_stream(const deep_char_stream& in_rhs) noexcept : char_stream() {
	bufferLength = in_rhs.bufferLength;
	streamCursor = in_rhs.streamCursor;
	srcBuffer = static_cast<IBYTEBUFFER>(malloc(bufferLength));
	copy_bytes(srcBuffer, in_rhs.srcBuffer, in_rhs.bufferLength);
}

MBASE_INLINE deep_char_stream::deep_char_stream(deep_char_stream&& in_rhs) noexcept {
	bufferLength = in_rhs.bufferLength;
	srcBuffer = in_rhs.srcBuffer;
	in_rhs.srcBuffer = nullptr;
}

MBASE_INLINE deep_char_stream::~deep_char_stream() {
	_destroy_self();
}

MBASE_INLINE deep_char_stream& deep_char_stream::operator=(const deep_char_stream& in_rhs) noexcept {
	if (in_rhs.bufferLength == bufferLength)
	{
		streamCursor = in_rhs.streamCursor;
		fill(srcBuffer, 0, bufferLength);
		copy_bytes(srcBuffer, in_rhs.srcBuffer, in_rhs.bufferLength);
	}
	else {
		free(srcBuffer);
		bufferLength = in_rhs.bufferLength;
		streamCursor = in_rhs.streamCursor;
		srcBuffer = static_cast<IBYTEBUFFER>(malloc(bufferLength));
		fill(srcBuffer, 0, bufferLength);
		copy_bytes(srcBuffer, in_rhs.srcBuffer, in_rhs.bufferLength);
	}

	return *this;
}

MBASE_INLINE deep_char_stream& deep_char_stream::operator=(deep_char_stream&& in_rhs) noexcept {
	free(srcBuffer);
	bufferLength = in_rhs.bufferLength;
	srcBuffer = in_rhs.srcBuffer;
	in_rhs.srcBuffer = nullptr;
	return *this;
}

MBASE_INLINE GENERIC deep_char_stream::_destroy_self() noexcept {
	if (!srcBuffer)
	{
		return;
	}

	free(srcBuffer);
	srcBuffer = nullptr;
}

MBASE_STD_END

#endif // MBASE_CHARSTREAM_H