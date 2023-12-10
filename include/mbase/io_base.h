#ifndef MBASE_IOBASE_H
#define MBASE_IOBASE_H

#include <mbase/common.h>
#include <mbase/char_stream.h>

MBASE_STD_BEGIN

// IO_BASE IS ABSTRACT CLASS
struct io_context {
	PTRGENERIC raw_handle;
	PTRGENERIC context_body;
};

class io_base {
public:
	using size_type = SIZE_T;

	virtual size_type write_data(IBYTEBUFFER in_src) = 0;
	virtual size_type write_data(IBYTEBUFFER in_src, size_type in_length) = 0;
	virtual size_type write_data(const mbase::string& in_src) = 0;
	virtual size_type write_data(const char_stream& in_src) = 0;

	virtual size_type read_data(IBYTEBUFFER in_src, size_type in_length) = 0;
	virtual size_type read_data(const char_stream& in_src) = 0;

	// ASSOCIATE INPUT CHARACTER STREAM
	GENERIC associate_is(char_stream& in_stream) noexcept {
		istream = &in_stream;
	}

	// ASSOCIATE OUTPUT CHARACTER STREAM
	GENERIC associate_os(char_stream& in_stream) noexcept {
		ostream = &in_stream;
	}

	size_type sync_is() noexcept {
		return write_data(*istream);
	}

	size_type sync_os() noexcept {
		return read_data(*ostream);
	}

	USED_RETURN io_context& get_raw_context() noexcept {
		return rawContext;
	}

	U32 get_last_error() const noexcept {
		return lastError;
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
