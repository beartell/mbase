#ifndef MBASE_PC_STREAM_MANAGER_H
#define MBASE_PC_STREAM_MANAGER_H

#include <mbase/common.h>
#include <mbase/char_stream.h>
#include <mbase/behaviors.h>
#include <mbase/vector.h>
#include <mbase/stack.h>

MBASE_BEGIN

static const U32 gDefaultStreamCount = 32;
static const U32 gDefaultStreamSize = 0xfffff; // 1MB

class PcStreamManager : public non_copymovable {
public:

	using stream_handle = I32;

	enum class flags : U8 {
		STREAM_MNG_SUCCESS = 0,
		STREAM_ERR_STREAMS_ARE_FULL,
		STREAM_ERR_INVALID_HANDLE
	};

	PcStreamManager() = default;
	~PcStreamManager() = default;

	flags get_stream_by_handle(stream_handle& in_stream_handle, char_stream*& out_stream);
	U32 get_stream_count();
	U32 get_stream_size();

	bool initialize(U32 in_stream_count = gDefaultStreamCount, U32 in_stream_size = gDefaultStreamSize);
	flags acquire_stream(stream_handle& out_stream_handle);
	flags acquire_stream(stream_handle& out_stream_handle, char_stream*& out_stream);
	flags release_stream(stream_handle& in_stream_handle);

private:
	mbase::vector<deep_char_stream> mStreams;
	mbase::stack<I32> mHandleStack;
	U32 mStreamCount;
	U32 mStreamSize;
};

MBASE_END

#endif // !MBASE_PC_STREAM_MANAGER_H
