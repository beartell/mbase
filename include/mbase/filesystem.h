#ifndef MBASE_FILESYSTEM_H
#define MBASE_FILESYSTEM_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <Windows.h>

MBASE_STD_BEGIN

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

USED_RETURN MBASE_INLINE mbase::string get_temp_path() noexcept {
	IBYTE pathString[MAX_PATH + 1] = { 0 };
	GetTempPathA(MAX_PATH + 1, pathString);
	return mbase::string(pathString);
}

USED_RETURN MBASE_INLINE mbase::string get_current_path() noexcept {
	IBYTE pathString[MAX_PATH + 1] = { 0 };
	GetCurrentDirectoryA(MAX_PATH + 1, pathString);
	return mbase::string(pathString);
}

MBASE_STD_END

#endif // MBASE_FILESYSTEM_H