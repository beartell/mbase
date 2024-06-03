#ifndef MBASE_IOFILE_H
#define MBASE_IOFILE_H

#include <mbase/io_base.h> // mbase::io_base
#include <mbase/string.h> // mbase::string
#include <mbase/behaviors.h> // mbase::non_copymovable
#include <mbase/char_stream.h> // mbase::char_stream
#include <Windows.h> // CreateFileA, SetFilePointer, WriteFile, ReadFile, CreateIoCompletionPort

MBASE_STD_BEGIN

/*

	--- CLASS INFORMATION ---
Identification: S0C17-OBJ-UD-ST

Name: io_file

Parent: S0C16-SAB-UD-ST, S0C6-STR-NA-ST 

Behaviour List:
- Default Constructible
- Destructible

Description:
io_file is a class that implements the io_base abstract class.
It is providing file io operations through it's corresponding methods such as:
open_file, close_file, get_file_size, get_file_name etc.

The read/write operation which are through read_data/write_data methods are synchronized,
blocking operations. Attempting to write to an invalid file will result in a OS dependant behavior.
io_file does not make further control on read/write operations whether the file handle is valid or not.

To achieve async io behavior, refer to the section Async I/O in MBASE.

*/

class io_file : public io_base, public non_copymovable {
public:
	enum class access_mode : U32 {
		READ_ACCESS = GENERIC_READ,
		WRITE_ACCESS = GENERIC_WRITE,
		RW_ACCESS = READ_ACCESS | WRITE_ACCESS
	};

	enum class disposition : U32 {
		OVERWRITE = CREATE_ALWAYS, // always succeeds
		APPEND = OPEN_EXISTING, // sometimes fail
		OPEN = OPEN_ALWAYS // always succeeds
	};

	io_file() noexcept;
	io_file(const mbase::string& in_filename, access_mode in_accmode = access_mode::RW_ACCESS, disposition in_disp = disposition::OVERWRITE) noexcept;
	~io_file() noexcept;

	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_file_open() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE mbase::string get_file_name() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type get_file_size() const noexcept;

	MBASE_INLINE PTRGENERIC open_file(const mbase::string& in_filename, access_mode in_accmode = access_mode::RW_ACCESS, disposition in_disp = disposition::OVERWRITE) noexcept;
	MBASE_INLINE GENERIC close_file() noexcept;
	size_type write_data(IBYTEBUFFER in_src) override;
	size_type write_data(IBYTEBUFFER in_src, size_type in_length) override;
	size_type write_data(const mbase::string& in_src) override;
	size_type write_data(char_stream& in_src) override;
	size_type write_data(char_stream& in_src, size_type in_length) override;
	size_type read_data(IBYTEBUFFER in_src, size_type in_length) override;
	size_type read_data(char_stream& in_src) override;
	size_type read_data(char_stream& in_src, size_type in_length) override;

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

private:
	bool mIsFileOpen;
	mbase::string mFileName;
};

io_file::io_file() noexcept : mIsFileOpen(false), mFileName()
{
}

io_file::io_file(const mbase::string& in_filename, access_mode in_accmode, disposition in_disp) noexcept : mIsFileOpen(false), mFileName(in_filename)
{
	DWORD fileAttrs = FILE_ATTRIBUTE_NORMAL;
	PTRGENERIC rawHandle = CreateFileA(mFileName.c_str(), (DWORD)in_accmode, FILE_SHARE_READ, nullptr, (DWORD)in_disp, fileAttrs, nullptr);
	if (!mRawContext.raw_handle)
	{
		_set_last_error(GetLastError());
	}
	else
	{
		mOperateReady = true;
		mIsFileOpen = true;
		_set_raw_context(rawHandle);
		if (in_disp == disposition::APPEND)
		{
			SetFilePointer(mRawContext.raw_handle, 0, nullptr, FILE_END);
		}
	}
}

io_file::~io_file() noexcept {
	close_file();
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool io_file::is_file_open() const noexcept 
{
	return mIsFileOpen;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE mbase::string io_file::get_file_name() const noexcept 
{
	return mFileName;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename io_file::size_type io_file::get_file_size() const noexcept 
{
	if(!mIsFileOpen)
	{
		return 0;
	}
	LARGE_INTEGER lInt;
	GetFileSizeEx(mRawContext.raw_handle, &lInt);
	return lInt.QuadPart;
}

MBASE_INLINE PTRGENERIC io_file::open_file(const mbase::string& in_filename, access_mode in_accmode, disposition in_disp) noexcept
{
	close_file();
	DWORD fileAttrs = FILE_ATTRIBUTE_NORMAL;
	PTRGENERIC rawHandle = CreateFileA(in_filename.c_str(), (DWORD)in_accmode, FILE_SHARE_READ, nullptr, (DWORD)in_disp, fileAttrs, nullptr);
	mFileName = in_filename;
	if (!mRawContext.raw_handle)
	{
		_set_last_error(GetLastError());
	}
	else
	{
		mOperateReady = true;
		mIsFileOpen = true;
		_set_raw_context(rawHandle);
		if (in_disp == disposition::APPEND)
		{
			SetFilePointer(mRawContext.raw_handle, 0, nullptr, FILE_END);
		}
	}
	return mRawContext.raw_handle;
}

MBASE_INLINE GENERIC io_file::close_file() noexcept 
{
	if (mRawContext.raw_handle)
	{
		CloseHandle(mRawContext.raw_handle);
	}
	mIsFileOpen = false;
	mOperateReady = false;
	mRawContext.raw_handle = nullptr;
}

typename io_file::size_type io_file::write_data(IBYTEBUFFER in_src)
{
	DWORD dataWritten = 0;
	SIZE_T dataLength = type_sequence<IBYTE>::length_bytes(in_src);
	WriteFile(mRawContext.raw_handle, in_src, dataLength, &dataWritten, nullptr);
	_set_last_error(GetLastError());
	return dataWritten;
}

typename io_file::size_type io_file::write_data(IBYTEBUFFER in_src, size_type in_length)
{
	DWORD dataWritten = 0;
	WriteFile(mRawContext.raw_handle, in_src, in_length, &dataWritten, nullptr);
	_set_last_error(GetLastError());
	return dataWritten;
}

typename io_file::size_type io_file::write_data(const mbase::string& in_src)
{
	DWORD dataWritten = 0;
	WriteFile(mRawContext.raw_handle, in_src.c_str(), in_src.size(), &dataWritten, nullptr);
	_set_last_error(GetLastError());
	return dataWritten;
}

typename io_file::size_type io_file::write_data(char_stream& in_src)
{
	DWORD dataWritten = 0;
	PTRDIFF cursorPos = in_src.get_pos();
	IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
	WriteFile(mRawContext.raw_handle, tmpBuffer, in_src.buffer_length() - cursorPos, &dataWritten, nullptr);
	_set_last_error(GetLastError());
	return dataWritten;
}

typename io_file::size_type io_file::write_data(char_stream& in_src, size_type in_length)
{
	DWORD dataWritten = 0;
	IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
	WriteFile(mRawContext.raw_handle, tmpBuffer, in_length, &dataWritten, nullptr);
	in_src.advance(dataWritten);
	_set_last_error(GetLastError());
	return dataWritten;
}

typename io_file::size_type io_file::read_data(IBYTEBUFFER in_src, size_type in_length)
{
	DWORD dataRead = 0;
	ReadFile(mRawContext.raw_handle, in_src, in_length, &dataRead, nullptr);
	_set_last_error(GetLastError());
	return dataRead;
}

typename io_file::size_type io_file::read_data(char_stream& in_src)
{
	DWORD dataRead = 0;
	PTRDIFF cursorPos = in_src.get_pos();
	IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
	ReadFile(mRawContext.raw_handle, tmpBuffer, in_src.buffer_length() - cursorPos, &dataRead, nullptr);
	_set_last_error(GetLastError());
	return dataRead;
}

typename io_file::size_type io_file::read_data(char_stream& in_src, size_type in_length)
{
	DWORD dataRead = 0;
	IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
	ReadFile(mRawContext.raw_handle, tmpBuffer, in_length, &dataRead, nullptr);
	in_src.advance(dataRead);
	_set_last_error(GetLastError());
	return dataRead;
}

MBASE_STD_END

#endif // MBASE_IOFILE_H