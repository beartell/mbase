#ifndef MBASE_FILESYSTEM_H
#define MBASE_FILESYSTEM_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>
#include <Windows.h>

MBASE_STD_BEGIN

// THIS INFORMATION STRUCT WILL BE EXTENDED LATER
struct FS_FILE_INFORMATION {
	mbase::string_view fileName;
	SIZE_T fileSize;
};

enum class FS_ERROR : I32{
	FS_SUCCESS = 0,
	FS_DIRECTORY_EXISTS = ERROR_ALREADY_EXISTS,
	FS_PATH_NOT_FOUND = ERROR_PATH_NOT_FOUND,
	FS_ACCESS_DENIED = ERROR_ACCESS_DENIED
};

MBASE_INLINE FS_ERROR create_directory(const mbase::string_view& in_path) noexcept {
	if(!CreateDirectoryA(in_path.c_str(), nullptr))
	{
		return (FS_ERROR)(GetLastError());
	}

	return FS_ERROR::FS_SUCCESS;
}

MBASE_INLINE FS_ERROR copy_file(const mbase::string_view& in_path, const mbase::string_view& in_copypath) noexcept {
	if(!CopyFileA(in_path.c_str(), in_copypath.c_str(), false))
	{
		return (FS_ERROR)(GetLastError());
	}

	return FS_ERROR::FS_SUCCESS;
}

MBASE_INLINE FS_ERROR delete_file(const mbase::string_view& in_path) noexcept {
	if(!DeleteFileA(in_path.c_str()))
	{
		return (FS_ERROR)(GetLastError());
	}

	return FS_ERROR::FS_SUCCESS;
}

USED_RETURN("temp path unused") MBASE_INLINE mbase::string get_temp_path() noexcept {
	IBYTE pathString[MAX_PATH + 1] = { 0 };
	GetTempPathA(MAX_PATH + 1, pathString);
	return mbase::string(pathString);
}

USED_RETURN("current path unused") MBASE_INLINE mbase::string get_current_path() noexcept {
	IBYTE pathString[MAX_PATH + 1] = { 0 };
	GetCurrentDirectoryA(MAX_PATH + 1, pathString);
	return mbase::string(pathString);
}

USED_RETURN("temp file unused") MBASE_INLINE mbase::string get_temp_file(const mbase::string_view& in_prefix) noexcept {
	IBYTE pathString[MAX_PATH + 1] = { 0 };
	GetTempFileNameA(".", in_prefix.c_str(), 0, pathString);
	return mbase::string(pathString);
}

MBASE_INLINE GENERIC get_directory(const mbase::string_view& in_path, mbase::vector<FS_FILE_INFORMATION>& out_files) noexcept {
	WIN32_FIND_DATAA findData;
	HANDLE findHandle = FindFirstFileA(in_path.c_str(), &findData);
	do {
		FS_FILE_INFORMATION ffi;
		ffi.fileName = findData.cFileName;
		ffi.fileSize = findData.nFileSizeHigh | findData.nFileSizeLow;
		out_files.push_back(ffi);
	} while (FindNextFileA(findHandle, &findData));
	FindClose(findHandle);
}

MBASE_STD_END

#endif // MBASE_FILESYSTEM_H