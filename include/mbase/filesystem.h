#ifndef MBASE_FILESYSTEM_H
#define MBASE_FILESYSTEM_H

#include <mbase/common.h>
#include <mbase/string.h> // mbase::string
#include <mbase/vector.h> // mbase::vector
#include <iostream>
#include <string>

#ifdef MBASE_PLATFORM_WINDOWS
#include <Windows.h>
/*
ERROR_ALREADY_EXISTS,
ERROR_PATH_NOT_FOUND,
ERROR_ACCESS_DENIED,
CreateDirectoryA,
FindFirstFileA,
FindNextFileA,
FindClose,
CopyFileA,
DeleteFileA,
GetTempPathA,
GetCurrentDirectoryA,
GetTempFileNameA
*/
#endif

#ifdef MBASE_PLATFORM_UNIX
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#endif

MBASE_STD_BEGIN

// THIS INFORMATION STRUCT WILL BE EXTENDED LATER
struct FS_FILE_INFORMATION {
	mbase::wstring fileName;
	SIZE_T fileSize = 0;
};

struct FS_FILE_INFORMATIONA {
	mbase::string fileName;
	SIZE_T fileSize = 0;
};

enum class FS_ERROR : I32{
	FS_SUCCESS = 0,
	FS_DIRECTORY_EXISTS = MBASE_FS_FLAGS_MIN,
	FS_PATH_NOT_FOUND,
	FS_ACCESS_DENIED,
	FS_UNKNOWN_ERROR = MBASE_FS_FLAGS_MAX
};

MBASE_INLINE FS_ERROR err_convert(I16 in_err) noexcept;
MBASE_INLINE FS_ERROR create_directory(const mbase::string& in_path) noexcept;
MBASE_INLINE FS_ERROR create_directory(const mbase::wstring& in_path) noexcept;
MBASE_INLINE FS_ERROR copy_file(const mbase::string& in_path, const mbase::string& in_copypath) noexcept;
MBASE_INLINE FS_ERROR delete_file(const mbase::wstring& in_path) noexcept;
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE mbase::wstring get_temp_path() noexcept;
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE mbase::wstring get_current_path() noexcept;
MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE mbase::wstring get_temp_file(const mbase::wstring& in_prefix = L"") noexcept;
template<typename ContainerType = mbase::vector<FS_FILE_INFORMATION>>
MBASE_INLINE GENERIC get_directory(const mbase::wstring& in_path, ContainerType& out_files) noexcept
{
#ifdef MBASE_PLATFORM_WINDOWS
	WIN32_FIND_DATAW findData;
	in_path += '*';
	HANDLE findHandle = FindFirstFileW(in_path.c_str(), &findData);
	do {
		FS_FILE_INFORMATION ffi;
		ffi.fileName = findData.cFileName;
		ffi.fileSize = findData.nFileSizeHigh | findData.nFileSizeLow;
		if (ffi.fileName == L"." || ffi.fileName == L"..") 
		{
			continue;
		}
		out_files.push_back(ffi);
	} while (FindNextFileW(findHandle, &findData));
	FindClose(findHandle);
#endif
#ifdef MBASE_PLATFORM_UNIX
	DIR *d = nullptr;
	struct dirent* dir = nullptr;
	d = opendir(mbase::to_utf8(in_path).c_str());
	if(d)
	{
		while((dir = readdir(d)) != NULL)
		{
			FS_FILE_INFORMATION ffi;
			struct stat st = {0};
			mbase::string fileName = dir->d_name;
			if (fileName == "." || fileName == "..") 
			{
				continue;
			}
			ffi.fileName = std::move(mbase::from_utf8(fileName));

			if(!stat(dir->d_name, &st))
			{
				ffi.fileSize = st.st_size;
			}
			out_files.push_back(ffi);
		}
		closedir(d);
	}
#endif
}

template<typename ContainerType = mbase::vector<FS_FILE_INFORMATIONA>>
MBASE_INLINE GENERIC get_directory(const mbase::string& in_path, ContainerType& out_files) noexcept
{
#ifdef MBASE_PLATFORM_WINDOWS
	WIN32_FIND_DATAA findData;
	in_path += '*';
	HANDLE findHandle = FindFirstFileA(in_path.c_str(), &findData);
	do {
		FS_FILE_INFORMATIONA ffi;
		ffi.fileName = findData.cFileName;
		ffi.fileSize = findData.nFileSizeHigh | findData.nFileSizeLow;
		if (ffi.fileName == "." || ffi.fileName == "..")
		{
			continue;
		}
		out_files.push_back(ffi);
	} while (FindNextFileA(findHandle, &findData));
	FindClose(findHandle);
#endif
#ifdef MBASE_PLATFORM_UNIX
	DIR* d = nullptr;
	struct dirent* dir = nullptr;
	d = opendir(in_path.c_str());
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			FS_FILE_INFORMATION ffi;
			struct stat st = { 0 };
			mbase::string fileName = dir->d_name;
			if (fileName == "." || fileName == "..")
			{
				continue;
			}
			ffi.fileName = std::move(mbase::from_utf8(fileName));

			if (!stat(dir->d_name, &st))
			{
				ffi.fileSize = st.st_size;
			}
			out_files.push_back(ffi);
		}
		closedir(d);
	}
#endif
}

/* IMPLEMENTATIONS */

MBASE_INLINE FS_ERROR err_convert(I16 in_err) noexcept
{
#ifdef MBASE_PLATFORM_WINDOWS
	switch (in_err)
	{
	case ERROR_ALREADY_EXISTS:
		return FS_ERROR::FS_DIRECTORY_EXISTS;
	case ERROR_PATH_NOT_FOUND:
		return FS_ERROR::FS_PATH_NOT_FOUND;
	case ERROR_ACCESS_DENIED:
		return FS_ERROR::FS_ACCESS_DENIED;
	default:
		return FS_ERROR::FS_UNKNOWN_ERROR;
	}
#endif
#ifdef MBASE_PLATFORM_LINUX
	switch (in_err)
	{
	case EEXIST:
		return FS_ERROR::FS_DIRECTORY_EXISTS;
	case ENOENT:
		return FS_ERROR::FS_PATH_NOT_FOUND;
	case EACCES:
		return FS_ERROR::FS_ACCESS_DENIED;
	case EPERM:
		return FS_ERROR::FS_ACCESS_DENIED;
	default:
		return FS_ERROR::FS_UNKNOWN_ERROR;
	}
#endif
	return FS_ERROR::FS_SUCCESS;
}

MBASE_INLINE FS_ERROR create_directory(const mbase::string& in_path) noexcept 
{
#ifdef MBASE_PLATFORM_WINDOWS
	if (!CreateDirectoryA(in_path.c_str(), nullptr))
	{
		return err_convert(GetLastError());
	}
#endif
#ifdef MBASE_PLATFORM_UNIX
	if(mkdir(in_path.c_str(), 0744))
	{

	}
#endif
	return FS_ERROR::FS_SUCCESS;
}

MBASE_INLINE FS_ERROR create_directory(const mbase::wstring& in_path) noexcept
{
#ifdef MBASE_PLATFORM_WINDOWS
	if (!CreateDirectoryW(in_path.c_str(), nullptr))
	{
		return err_convert(GetLastError());
	}
#endif
#ifdef MBASE_PLATFORM_UNIX
	if (mkdir(mbase::to_utf8(in_path).c_str(), 0744))
	{

	}
#endif
	return FS_ERROR::FS_SUCCESS;
}

MBASE_INLINE FS_ERROR copy_file(const mbase::string& in_path, const mbase::string& in_copypath) noexcept 
{
#ifdef MBASE_PLATFORM_WINDOWS
	if (!CopyFileA(in_path.c_str(), in_copypath.c_str(), false))
	{
		return err_convert(GetLastError());
	}
#endif
#ifdef MBASE_PLATFORM_UNIX

#endif
	return FS_ERROR::FS_SUCCESS;
}

MBASE_INLINE FS_ERROR delete_file(const mbase::wstring& in_path) noexcept 
{
#ifdef MBASE_PLATFORM_WINDOWS
	if (!DeleteFileW(in_path.c_str()))
	{
		return err_convert(GetLastError());
	}
#endif
#ifdef MBASE_PLATFORM_UNIX
	remove(mbase::to_utf8(in_path).c_str());
#endif
	return FS_ERROR::FS_SUCCESS;
}

MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE mbase::wstring get_temp_path() noexcept 
{
#ifdef MBASE_PLATFORM_WINDOWS
	wchar_t pathString[MAX_PATH + 1] = { 0 };
	GetTempPathW(MAX_PATH + 1, pathString);
	return mbase::wstring(pathString);
#endif
#ifdef MBASE_PLATFORM_UNIX
	return mbase::wstring(L"/tmp/");
#endif
}

MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE mbase::wstring get_current_path() noexcept 
{
#ifdef MBASE_PLATFORM_WINDOWS
	wchar_t pathString[MAX_PATH + 1] = { 0 };
	GetCurrentDirectoryW(MAX_PATH + 1, pathString);
	return mbase::wstring(pathString);
#endif
#ifdef MBASE_PLATFORM_UNIX
	return mbase::wstring(mbase::from_utf8(get_current_dir_name()));
#endif
}

MBASE_ND(MBASE_RESULT_IGNORE) MBASE_INLINE mbase::wstring get_temp_file(const mbase::wstring& in_prefix) noexcept 
{
#ifdef MBASE_PLATFORM_WINDOWS
	wchar_t pathString[MAX_PATH + 1] = { 0 };
	wchar_t tempPathString[MAX_PATH + 1] = {0};
	GetTempPath2W(MAX_PATH + 1, tempPathString);
	GetTempFileNameW(tempPathString, in_prefix.c_str(), 0, pathString);
	return mbase::wstring(pathString);
#endif
#ifdef MBASE_PLATFORM_UNIX
	mbase::wstring tempName = in_prefix + L"XXXXXX";
	mkstemp(mbase::to_utf8(tempName).data());
	return tempName;
#endif
}


MBASE_STD_END

#endif // MBASE_FILESYSTEM_H