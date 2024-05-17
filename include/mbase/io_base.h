#ifndef MBASE_IOBASE_H
#define MBASE_IOBASE_H

#include <mbase/common.h>
#include <mbase/char_stream.h>
#include <mbase/string.h>
#include <mbase/safe_buffer.h>
#include <Windows.h>

MBASE_STD_BEGIN

/*

	--- CLASS INFORMATION ---
Identification: S0C15-SAB-NA-ST

Name: io_context

Parent: None

Behaviour List:

Description:
it is used for storing the raw file handle and it's OS dependant 'context'.

*/

struct io_context {
	PTRGENERIC raw_handle = nullptr;
	PTRGENERIC context_body = nullptr;
};

/*

	--- CLASS INFORMATION ---
Identification: S0C16-SAB-UD-ST

Name: io_base

Parent: None

Behaviour List:
- Default Constructible
- Abstract Interfaceable
- Type Aware

Description:
io_base is the abstract class for providing a read/write operations for custom io scenarios.
Classes that implement the io_base interface, must implement the write_data and read_data
methods. As an example, in mbase library, classes that implement the io_base interface are:
io_file and io_tcp_client. 

Besides read/write methods, io_base provides a char_stream association which makes the io operations
much easier to maintain and track. To use stream association, refer to the Stream Association section.

*/

class io_base {
public:
	using size_type = SIZE_T;

	enum class move_method : U8 {
		MV_BEGIN = FILE_BEGIN,
		MV_CURRENT = FILE_CURRENT,
		MV_ENC = FILE_END
	};
	
	/* ===== BUILDER METHODS BEGIN ===== */
	io_base() : istream(nullptr), ostream(nullptr) {}
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) io_context& get_raw_context() noexcept { return rawContext; }
	MBASE_ND(MBASE_OBS_IGNORE) U32 get_last_error() const noexcept { return lastError; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) char_stream* get_is() noexcept { return istream; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) char_stream* get_os() noexcept { return ostream; }
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	virtual size_type write_data(IBYTEBUFFER in_src) = 0;
	virtual size_type write_data(IBYTEBUFFER in_src, size_type in_length) = 0;
	virtual size_type write_data(const mbase::string& in_src) = 0;
	virtual size_type write_data(char_stream& in_src) = 0;
	virtual size_type write_data(char_stream& in_src, size_type in_length) = 0;
	virtual size_type read_data(IBYTEBUFFER in_src, size_type in_length) = 0;
	virtual size_type read_data(char_stream& in_src) = 0;
	virtual size_type read_data(char_stream& in_src, size_type in_length) = 0;
	GENERIC associate_is(char_stream& in_stream) noexcept { istream = &in_stream; }
	GENERIC associate_os(char_stream& in_stream) noexcept { ostream = &in_stream; }
	size_type sync_is() noexcept { return read_data(*istream); }
	size_type sync_is(size_type in_length) noexcept { return read_data(*istream, in_length); }
	size_type sync_os() noexcept { return write_data(*ostream); }
	size_type sync_os(size_type in_length) noexcept { return write_data(*ostream, in_length); }
	// this is valid if the subclass is io_file,
	GENERIC set_file_pointer(size_type in_distance, move_method in_method) noexcept 
	{ 
		if(rawContext.raw_handle)
		{
			SetFilePointer(rawContext.raw_handle, in_distance, nullptr, (DWORD)in_method);
		}
	}
	/* ===== STATE-MODIFIER METHODS END ===== */

protected:
	GENERIC _set_raw_context(PTRGENERIC raw_handle) noexcept 
	{
		rawContext.raw_handle = raw_handle;
		rawContext.context_body = nullptr;
	}

	GENERIC _set_raw_context(PTRGENERIC raw_handle, PTRGENERIC context_body) noexcept 
	{
		rawContext.raw_handle = raw_handle;
		rawContext.context_body = context_body;
	}

	GENERIC _set_last_error(U32 in_errcode) noexcept 
	{
		lastError = in_errcode;
	}

	io_context rawContext;
	char_stream* istream;
	char_stream* ostream;
	U32 lastError;
};

MBASE_STD_END

#endif // !MBASE_IOBASE_H
