#ifndef MBASE_IOBASE_H
#define MBASE_IOBASE_H

#include <mbase/common.h>
#include <mbase/char_stream.h>
#include <mbase/safe_buffer.h>

MBASE_STD_BEGIN

// IO_BASE IS ABSTRACT CLASS
struct io_context {
	PTRGENERIC raw_handle;
	PTRGENERIC context_body; // NOT USED OFTEN
};

class io_base {
public:
	io_base() : istream(nullptr), ostream(nullptr) {}

	using size_type = SIZE_T;

	virtual size_type write_data(IBYTEBUFFER in_src) = 0;
	virtual size_type write_data(IBYTEBUFFER in_src, size_type in_length) = 0;
	virtual size_type write_data(const mbase::string& in_src) = 0;
	virtual size_type write_data(char_stream& in_src) = 0;
	virtual size_type write_data(char_stream& in_src, size_type in_length) = 0;


	virtual size_type read_data(IBYTEBUFFER in_src, size_type in_length) = 0;
	virtual size_type read_data(char_stream& in_src) = 0;
	virtual size_type read_data(char_stream& in_src, size_type in_length) = 0;

	/*template<typename T>
	size_type write_data(T& in_src) {
		safe_buffer mBuffer;
		in_src.serialize(&mBuffer);
		return write_data(mBuffer.bfSource, mBuffer.bfLength);
	}

	template<typename T>
	size_type read_data(T& in_target, IBYTEBUFFER in_src, size_type in_length) {
		size_type readLength = read_data(in_src, in_length);
		in_target.deserialize(in_src, in_length);
		return readLength;
	}*/

	// ASSOCIATE INPUT CHARACTER STREAM
	GENERIC associate_is(char_stream& in_stream) noexcept {
		istream = &in_stream;
	}

	// ASSOCIATE OUTPUT CHARACTER STREAM
	GENERIC associate_os(char_stream& in_stream) noexcept {
		ostream = &in_stream;
	}

	size_type sync_is() noexcept {
		return read_data(*istream);
	}

	size_type sync_is(size_type in_length) noexcept {
		return read_data(*istream, in_length);
	}

	size_type sync_os() noexcept {
		return write_data(*ostream);
	}

	size_type sync_os(size_type in_length) noexcept {
		return write_data(*ostream, in_length);
	}

	USED_RETURN io_context& get_raw_context() noexcept {
		return rawContext;
	}

	USED_RETURN U32 get_last_error() const noexcept {
		return lastError;
	}

	char_stream* get_is() noexcept {
		return istream;
	}

	char_stream* get_os() noexcept {
		return ostream;
	}

protected:
	GENERIC _set_raw_context(PTRGENERIC raw_handle) noexcept {
		rawContext.raw_handle = raw_handle;
		rawContext.context_body = nullptr;
	}

	GENERIC _set_raw_context(PTRGENERIC raw_handle, PTRGENERIC context_body) noexcept {
		rawContext.raw_handle = raw_handle;
		rawContext.context_body = context_body;
	}

	GENERIC _set_last_error(U32 in_errcode) noexcept {
		lastError = in_errcode;
	}

	io_context rawContext;
	char_stream* istream;
	char_stream* ostream;
	U32 lastError;
};

MBASE_STD_END

#endif // !MBASE_IOBASE_H
