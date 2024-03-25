#ifndef MBASE_CHARSTREAM_H
#define MBASE_CHARSTREAM_H

#include <mbase/common.h>
#include <mbase/string.h>

MBASE_STD_BEGIN

// NOTE THAT THE CONSTRUCTOR DOES NOT COPY THE IBYTEBUFFER

class char_stream : protected mbase::type_sequence<IBYTE> {
public:
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;

	MBASE_INLINE char_stream() noexcept;
	MBASE_INLINE MBASE_STD_EXPLICIT char_stream(IBYTEBUFFER in_src) noexcept;
	MBASE_INLINE MBASE_STD_EXPLICIT char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept;

	MBASE_INLINE IBYTEBUFFER operator*() noexcept;

	USED_RETURN("first byte being ignored") MBASE_INLINE_EXPR IBYTE front() const noexcept;
	USED_RETURN("last byte being ignored") MBASE_INLINE_EXPR IBYTE back() const noexcept;
	USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR size_type buffer_length() const noexcept;
	USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR IBYTEBUFFER get_buffer() const noexcept;
	USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR IBYTEBUFFER get_bufferc() const noexcept;
	USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR difference_type get_pos() const noexcept;
	USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR IBYTE getc() const noexcept;
	USED_RETURN("stream data ignored") MBASE_INLINE_EXPR IBYTEBUFFER data() const noexcept;

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

	MBASE_INLINE IBYTEBUFFER operator+=(difference_type in_rhs) noexcept;
	MBASE_INLINE IBYTEBUFFER operator++() noexcept;
	MBASE_INLINE IBYTEBUFFER operator++(I32) noexcept;
	MBASE_INLINE IBYTEBUFFER operator-=(difference_type in_rhs) noexcept;
	MBASE_INLINE IBYTEBUFFER operator--() noexcept;
	MBASE_INLINE IBYTEBUFFER operator--(I32) noexcept;

	friend bool operator==(const char_stream& in_lhs, const char_stream& in_rhs) noexcept {
		return mbase::type_sequence<IBYTE>::is_equal(in_lhs.srcBuffer, in_rhs.srcBuffer, in_lhs.bufferLength);
	}

	friend bool operator!=(const char_stream& in_lhs, const char_stream& in_rhs) noexcept {
		return !mbase::type_sequence<IBYTE>::is_equal(in_lhs.srcBuffer, in_rhs.srcBuffer, in_lhs.bufferLength);
	}

protected:
	size_type bufferLength;
	difference_type streamCursor;
	IBYTEBUFFER srcBuffer;
};

class deep_char_stream : public char_stream {
public:
	MBASE_INLINE MBASE_EXPLICIT deep_char_stream(IBYTEBUFFER in_src) noexcept;
	MBASE_INLINE MBASE_EXPLICIT deep_char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept;
	MBASE_INLINE MBASE_EXPLICIT deep_char_stream(size_type in_length);
	MBASE_INLINE deep_char_stream(const deep_char_stream& in_rhs) noexcept;
	MBASE_INLINE deep_char_stream(deep_char_stream&& in_rhs) noexcept;
	MBASE_INLINE ~deep_char_stream();

	MBASE_INLINE deep_char_stream& operator=(const deep_char_stream& in_rhs) noexcept;
	MBASE_INLINE deep_char_stream& operator=(deep_char_stream&& in_rhs) noexcept;
	
};

MBASE_INLINE char_stream::char_stream() noexcept : bufferLength(0), streamCursor(0), srcBuffer(nullptr)
{
}

MBASE_INLINE char_stream::char_stream(IBYTEBUFFER in_src) noexcept {
	bufferLength = length(in_src) + 1; // CHAR STREAM WILL INCLUDE NULL TERMINATOR
	streamCursor = 0;
	srcBuffer = in_src;
}

MBASE_INLINE char_stream::char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept : bufferLength(in_length), streamCursor(0), srcBuffer(in_src)
{
}

MBASE_INLINE IBYTEBUFFER char_stream::operator*() noexcept {
	return srcBuffer + streamCursor;
}

USED_RETURN("first byte being ignored") MBASE_INLINE_EXPR IBYTE char_stream::front() const noexcept {
	return *srcBuffer;
}

USED_RETURN("last byte being ignored") MBASE_INLINE_EXPR IBYTE char_stream::back() const noexcept {
	return *(srcBuffer + (bufferLength - 1));
}

USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR char_stream::size_type char_stream::buffer_length() const noexcept {
	return bufferLength;
}

USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR IBYTEBUFFER char_stream::get_buffer() const noexcept {
	return srcBuffer;
}

USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR IBYTEBUFFER char_stream::get_bufferc() const noexcept {
	return srcBuffer + streamCursor;
}

USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR char_stream::difference_type char_stream::get_pos() const noexcept {
	return streamCursor;
}

USED_RETURN("stream observation ignored") MBASE_INLINE_EXPR IBYTE char_stream::getc() const noexcept {
	return *(srcBuffer + streamCursor);
}

USED_RETURN("stream data ignored") MBASE_INLINE_EXPR IBYTEBUFFER char_stream::data() const noexcept {
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
	if (!srcBuffer)
	{
		return;
	}

	free(srcBuffer);
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

MBASE_STD_END

#endif // MBASE_CHARSTREAM_H