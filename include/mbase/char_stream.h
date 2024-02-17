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

	char_stream() noexcept : bufferLength(0), streamCursor(0), srcBuffer(nullptr) {}

	MBASE_STD_EXPLICIT char_stream(IBYTEBUFFER in_src) noexcept {
		bufferLength = length(in_src) + 1; // CHAR STREAM WILL INCLUDE NULL TERMINATOR
		streamCursor = 0;
		srcBuffer = in_src;
	}

	MBASE_STD_EXPLICIT char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept : bufferLength(in_length), streamCursor(0), srcBuffer(in_src) {}

	MBASE_INLINE_EXPR GENERIC advance() noexcept {
		++streamCursor;
	}

	MBASE_INLINE_EXPR GENERIC advance(difference_type in_length) noexcept {
		streamCursor += in_length;
	}

	MBASE_INLINE_EXPR GENERIC reverse() noexcept {
		--streamCursor;
	}

	MBASE_INLINE_EXPR GENERIC reverse(difference_type in_length) noexcept {
		streamCursor -= in_length;
	}

	MBASE_INLINE_EXPR GENERIC putc(IBYTE in_byte) noexcept {
		*(srcBuffer + streamCursor) = in_byte;
	}

	MBASE_INLINE_EXPR GENERIC putcn(IBYTE in_byte) noexcept {
		*(srcBuffer + streamCursor++) = in_byte;
	}

	MBASE_INLINE_EXPR GENERIC put_data(IBYTEBUFFER in_data, size_type in_length) noexcept {
		difference_type tempCursorPos = streamCursor;
		for(size_type i = 0; i < in_length; i++)
		{
			*(srcBuffer + tempCursorPos++) = *(in_data + i);
		}
	}

	MBASE_INLINE_EXPR GENERIC put_datan(IBYTEBUFFER in_data, size_type in_length) noexcept {
		for (size_type i = 0; i < in_length; i++)
		{
			*(srcBuffer + streamCursor++) = *(in_data + i);
		}
	}

	USED_RETURN MBASE_INLINE_EXPR IBYTE front() const noexcept {
		return *srcBuffer;
	}

	USED_RETURN MBASE_INLINE_EXPR IBYTE back() const noexcept {
		return *(srcBuffer + (bufferLength - 1));
	}

	USED_RETURN MBASE_INLINE_EXPR size_type buffer_length() const noexcept {
		return bufferLength;
	}

	USED_RETURN MBASE_INLINE_EXPR IBYTEBUFFER get_buffer() const noexcept {
		return srcBuffer;
	}

	USED_RETURN MBASE_INLINE_EXPR IBYTEBUFFER get_bufferc() const noexcept {
		return srcBuffer + streamCursor;
	}

	USED_RETURN MBASE_INLINE_EXPR difference_type get_pos() const noexcept {
		return streamCursor;
	}

	USED_RETURN MBASE_INLINE_EXPR IBYTE getc() const noexcept {
		return *(srcBuffer + streamCursor);
	}

	USED_RETURN MBASE_INLINE_EXPR IBYTE getcn() noexcept {
		return *(srcBuffer + streamCursor--);
	}

	USED_RETURN MBASE_INLINE_EXPR IBYTEBUFFER data() const noexcept {
		return srcBuffer;
	}

	MBASE_INLINE_EXPR GENERIC set_cursor_pos(difference_type in_pos) noexcept {
		if(in_pos < 0)
		{
			streamCursor = 0;
			return;
		}

		streamCursor = in_pos;
	}

	MBASE_INLINE_EXPR GENERIC set_cursor_front() noexcept {
		streamCursor = 0;
	}

	MBASE_INLINE_EXPR GENERIC set_cursor_end() noexcept {
		streamCursor = bufferLength - 1;
	}

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
	MBASE_EXPLICIT deep_char_stream(IBYTEBUFFER in_src) noexcept : char_stream(in_src) {
		IBYTEBUFFER freshBuffer = static_cast<IBYTEBUFFER>(malloc(bufferLength));
		copy_bytes(freshBuffer, srcBuffer, bufferLength);
		srcBuffer = freshBuffer;
	}

	MBASE_EXPLICIT deep_char_stream(IBYTEBUFFER in_src, size_type in_length) noexcept : char_stream(in_src, in_length) {
		IBYTEBUFFER freshBuffer = static_cast<IBYTEBUFFER>(malloc(bufferLength));
		copy_bytes(freshBuffer, srcBuffer, bufferLength);
		srcBuffer = freshBuffer;
	}

	MBASE_EXPLICIT deep_char_stream(size_type in_length) : char_stream() {
		IBYTEBUFFER freshBuffer = static_cast<IBYTEBUFFER>(malloc(in_length));
		bufferLength = in_length;
		srcBuffer = freshBuffer;
		fill(srcBuffer, 0, bufferLength);
	}

	deep_char_stream(const deep_char_stream& in_rhs) noexcept : char_stream() {
		bufferLength = in_rhs.bufferLength;
		streamCursor = in_rhs.streamCursor;
		srcBuffer = static_cast<IBYTEBUFFER>(malloc(bufferLength));
		copy_bytes(srcBuffer, in_rhs.srcBuffer, in_rhs.bufferLength);
	}

	deep_char_stream(deep_char_stream&& in_rhs) noexcept {
		bufferLength = in_rhs.bufferLength;
		srcBuffer = in_rhs.srcBuffer;
		in_rhs.srcBuffer = nullptr;
	}

	deep_char_stream& operator=(const deep_char_stream& in_rhs) noexcept {
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

	deep_char_stream& operator=(deep_char_stream&& in_rhs) noexcept {
		free(srcBuffer);
		bufferLength = in_rhs.bufferLength;
		srcBuffer = in_rhs.srcBuffer;
		in_rhs.srcBuffer = nullptr;
		return *this;
	}

	~deep_char_stream() {
		if(!srcBuffer)
		{
			return;
		}

		free(srcBuffer);
	}
};

MBASE_STD_END

#endif // MBASE_CHARSTREAM_H