#ifndef MBASE_IOFILE_H
#define MBASE_IOFILE_H

#include <mbase/io_base.h> // mbase::io_base
#include <mbase/string.h> // mbase::string
#include <mbase/behaviors.h> // mbase::non_copymovable
#include <mbase/rng.h>
#include <Windows.h> // CreateFileA, SetFilePointer, WriteFile, ReadFile, CreateIoCompletionPort

MBASE_STD_BEGIN

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

	io_file()  noexcept : rawHandle(nullptr) {}

	io_file(const mbase::string& in_filename, access_mode in_accmode, disposition in_disp = disposition::OVERWRITE, bool isAsync = false) noexcept {
		DWORD fileAttrs = FILE_ATTRIBUTE_NORMAL;
		if(isAsync)
		{
			fileAttrs |= FILE_FLAG_OVERLAPPED;
			ov.hEvent = CreateEventA(nullptr, false, true, nullptr);
			ov.Offset = 0;
			ov.OffsetHigh = 0;
		}

		rawHandle = CreateFileA(in_filename.c_str(), (DWORD)in_accmode, FILE_SHARE_READ, nullptr, (DWORD)in_disp, fileAttrs, nullptr);
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

			if(isAsync)
			{
				iocpId = mbase::gen_random_64();
				iocp = CreateIoCompletionPort(rawHandle, nullptr, iocpId, 0);
			}
		}
	}

	~io_file() noexcept {
		CloseHandle(rawHandle);
	}

	MBASE_INLINE PTRGENERIC open_file(const mbase::string& in_filename, access_mode in_accmode, disposition in_disp = disposition::OVERWRITE, bool isAsync = false) noexcept {
		CloseHandle(rawHandle);
		DWORD fileAttrs = FILE_ATTRIBUTE_NORMAL;
		if (isAsync)
		{
			fileAttrs |= FILE_FLAG_OVERLAPPED;
		}

		rawHandle = CreateFileA(in_filename.c_str(), (DWORD)in_accmode, FILE_SHARE_READ, nullptr, (DWORD)in_disp, fileAttrs, nullptr);
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
		return rawHandle;
	}

	MBASE_INLINE GENERIC close_file() noexcept {
		CloseHandle(rawHandle);
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

	size_type awrite_data(IBYTEBUFFER in_src, size_type in_length)
	{
		DWORD dataWritten = 0;
		WriteFile(rawHandle, in_src, in_length, &dataWritten, &ov);
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

	LPOVERLAPPED getov() {
		return &ov;
	}

	HANDLE getiocp() {
		return iocp;
	}

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

	USED_RETURN MBASE_INLINE mbase::string get_file_name() const noexcept {
		return fileName;
	}

	USED_RETURN MBASE_INLINE SIZE_T get_file_size() const noexcept {
		LARGE_INTEGER lInt;
		GetFileSizeEx(rawHandle, &lInt);
		return lInt.QuadPart;
	}
	
private:
	HANDLE iocp;
	U64 iocpId;
	OVERLAPPED ov;
	mbase::string fileName;
	HANDLE rawHandle;
};

MBASE_STD_END

#endif // MBASE_IOFILE_H