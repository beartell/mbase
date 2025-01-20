#ifndef MBASE_IOBASE_H
#define MBASE_IOBASE_H

#include <mbase/common.h>
#include <mbase/char_stream.h> // mbase::char_stream
#include <mbase/string.h> // mbase::string

#ifdef MBASE_PLATFORM_WINDOWS
#include <Windows.h> // FILE_BEGIN, FILE_CURRENT, FILE_END, SetFilePointer
#endif

#ifdef MBASE_PLATFORM_UNIX
#include <unistd.h>
#endif

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

template<typename OSFhandle>
struct io_context {
	OSFhandle raw_handle = 0;
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
	#ifdef MBASE_PLATFORM_WINDOWS
	using os_file_handle = PTRGENERIC;
	#endif
	#ifdef MBASE_PLATFORM_UNIX
	using os_file_handle = I32;
	#endif
	enum class move_method : U8 {
		#ifdef MBASE_PLATFORM_WINDOWS
		MV_BEGIN = FILE_BEGIN,
		MV_CURRENT = FILE_CURRENT,
		MV_END = FILE_END
		#endif
		#ifdef MBASE_PLATFORM_UNIX
		MV_BEGIN = SEEK_SET,
		MV_CURRENT = SEEK_CUR,
		MV_END = SEEK_END
		#endif
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	io_base() : mIstream(nullptr), mOstream(nullptr), mOperateReady(false), mLastError(0) {}
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) io_context<os_file_handle>& get_raw_context() noexcept { return mRawContext; }
	MBASE_ND(MBASE_OBS_IGNORE) U32 get_last_error() const noexcept { return mLastError; }
	MBASE_ND(MBASE_OBS_IGNORE) bool is_operate_ready() const noexcept { return mOperateReady; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) char_stream* get_is() noexcept { return mIstream; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) char_stream* get_os() noexcept { return mOstream; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) size_type get_file_pointer_pos() noexcept
	{
		if(mRawContext.raw_handle)
		{
			#ifdef MBASE_PLATFORM_WINDOWS
			return 0; // TODO: IMPLEMENT USING WIN32
			#endif

			#ifdef MBASE_PLATFORM_APPLE
			return lseek(mRawContext.raw_handle, 0, (I32)move_method::MV_CURRENT);
			#elif defined(MBASE_PLATFORM_UNIX)
			return lseek64(mRawContext.raw_handle, 0, (I32)move_method::MV_CURRENT);
			#endif
		}
		return 0;
	}
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	virtual size_type write_data(CBYTEBUFFER in_src) = 0;
	virtual size_type write_data(CBYTEBUFFER in_src, size_type in_length) = 0;
	virtual size_type write_data(const mbase::string& in_src) = 0;
	virtual size_type write_data(char_stream& in_src) = 0;
	virtual size_type write_data(char_stream& in_src, size_type in_length) = 0;
	virtual size_type read_data(IBYTEBUFFER in_src, size_type in_length) = 0;
	virtual size_type read_data(char_stream& in_src) = 0;
	virtual size_type read_data(char_stream& in_src, size_type in_length) = 0;
	GENERIC associate_is(char_stream& in_stream) noexcept { mIstream = &in_stream; }
	GENERIC associate_os(char_stream& in_stream) noexcept { mOstream = &in_stream; }
	GENERIC clear_is() noexcept { mIstream = nullptr; }
	GENERIC clear_os() noexcept { mOstream = nullptr; }
	size_type sync_is() noexcept { return read_data(*mIstream); }
	size_type sync_is(size_type in_length) noexcept { return read_data(*mIstream, in_length); }
	size_type sync_os() noexcept { return write_data(*mOstream); }
	size_type sync_os(size_type in_length) noexcept { return write_data(*mOstream, in_length); }
	// this is valid if the subclass is io_file,
	GENERIC set_file_pointer(size_type in_distance, move_method in_method) noexcept 
	{ 
		if(mRawContext.raw_handle)
		{
			#ifdef MBASE_PLATFORM_WINDOWS
			LARGE_INTEGER lint;
			lint.QuadPart = static_cast<LONGLONG>(in_distance);
			SetFilePointer(mRawContext.raw_handle, lint.LowPart, &lint.HighPart, (DWORD)in_method);
			#endif
			#ifdef MBASE_PLATFORM_APPLE
			lseek(mRawContext.raw_handle, in_distance, (I32)in_method);
			#elif defined(MBASE_PLATFORM_UNIX)
			lseek64(mRawContext.raw_handle, in_distance, (I32)in_method);
			#endif
		}
	}
	template<typename SerializableObject>
	size_type write_data(SerializableObject& in_src)
	{
		mbase::deep_char_stream dcs(in_src.get_serialized_size());
		in_src.serialize(dcs);
		return write_data(dcs.get_buffer(), dcs.buffer_length());
	}

	template<typename SerializableObject>
	size_type read_data(SerializableObject& in_target, IBYTEBUFFER in_src, size_type in_length)
	{
		mbase::char_stream cs(in_src, in_length);
		size_type readLength = read_data(in_src, in_length);
		in_target = std::move(in_target.deserialize(in_src, in_length));
		return readLength;
	}
	/* ===== STATE-MODIFIER METHODS END ===== */

protected:
	GENERIC _set_raw_context(os_file_handle raw_handle) noexcept 
	{
		mRawContext.raw_handle = raw_handle;
		mRawContext.context_body = nullptr;
	}

	GENERIC _set_raw_context(os_file_handle raw_handle, PTRGENERIC context_body) noexcept 
	{
		mRawContext.raw_handle = raw_handle;
		mRawContext.context_body = context_body;
	}

	GENERIC _set_last_error(U32 in_errcode) noexcept 
	{
		mLastError = in_errcode;
	}

	io_context<os_file_handle> mRawContext;
	char_stream* mIstream;
	char_stream* mOstream;
	bool mOperateReady;
	U32 mLastError;
};

MBASE_STD_END

#endif // !MBASE_IOBASE_H
