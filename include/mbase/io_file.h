#ifndef MBASE_IOFILE_H
#define MBASE_IOFILE_H

#include <mbase/io_base.h>
#include <mbase/string.h>
#include <Windows.h>

MBASE_STD_BEGIN

// TODO
// DO NOT FORGET OT IMPLEMENT io_file_async

class io_file : public io_base {
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

	io_file()  noexcept : rawHandle(nullptr) {

	}

	io_file(const mbase::string& in_filename, access_mode in_accmode, disposition in_disp = disposition::OPEN) noexcept {
		rawHandle = CreateFileA(in_filename.c_str(), (DWORD)in_accmode, FILE_SHARE_READ, nullptr, (DWORD)in_disp, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(!rawHandle)
		{
			_set_last_error(GetLastError());
		}
		else
		{
			_set_raw_context(rawHandle);
			if(in_disp == disposition::APPEND)
			{
				SetFilePointer(rawHandle, 0, nullptr, FILE_END);
			}
		}
	}

	~io_file() {
		CloseHandle(rawHandle);
	}

	GENERIC open_file(const mbase::string& in_filename, access_mode in_accmode, disposition in_disp = disposition::OVERWRITE) noexcept {
		CloseHandle(rawHandle);
		rawHandle = CreateFileA(in_filename.c_str(), (DWORD)in_accmode, FILE_SHARE_READ, nullptr, (DWORD)in_disp, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (!rawHandle)
		{
			_set_last_error(GetLastError());
		}
		else
		{
			_set_raw_context(rawHandle);
			if (in_disp == disposition::APPEND)
			{
				SetFilePointer(rawHandle, 0, nullptr, FILE_END);
			}
		}
	}

	size_type write_data(IBYTEBUFFER in_src) override 
	{
		DWORD dataWritten = 0;
		SIZE_T dataLength = type_sequence<IBYTE>::length(in_src);
		WriteFile(rawHandle, in_src, dataLength, &dataWritten, nullptr);
		_set_last_error(GetLastError());
		return dataWritten;
	}
	size_type write_data(IBYTEBUFFER in_src, size_type in_length) override
	{
		DWORD dataWritten = 0;
		WriteFile(rawHandle, in_src, in_length, &dataWritten, nullptr);
		_set_last_error(GetLastError());
		return dataWritten;
	}
	size_type write_data(const mbase::string& in_src) override
	{
		DWORD dataWritten = 0;
		WriteFile(rawHandle, in_src.c_str(), in_src.size(), &dataWritten, nullptr);
		_set_last_error(GetLastError());
		return dataWritten;
	}
	size_type write_data(const char_stream& in_src) override
	{
		DWORD dataWritten = 0;
		PTRDIFF cursorPos = in_src.get_pos();
		IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
		WriteFile(rawHandle, tmpBuffer, in_src.buffer_length() - cursorPos, &dataWritten, nullptr);
		_set_last_error(GetLastError());
		return dataWritten;
	}

	size_type read_data(IBYTEBUFFER in_src, size_type in_length) override
	{
		DWORD dataRead = 0;
		ReadFile(rawHandle, in_src, in_length, &dataRead, nullptr);
		_set_last_error(GetLastError());
		return dataRead;
	}

	size_type read_data(const char_stream& in_src) override
	{
		DWORD dataRead = 0;
		PTRDIFF cursorPos = in_src.get_pos();
		IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
		ReadFile(rawHandle, tmpBuffer, in_src.buffer_length() - cursorPos, &dataRead, nullptr);
		_set_last_error(GetLastError());
		return dataRead;
	}

	mbase::string get_file_name() {
		return fileName;
	}

	SIZE_T get_file_size() {
		LARGE_INTEGER lInt;
		GetFileSizeEx(rawHandle, &lInt);
		return lInt.QuadPart;
	}
	
private:
	mbase::string fileName;
	HANDLE rawHandle;
};

MBASE_STD_END

#endif // MBASE_IOFILE_H