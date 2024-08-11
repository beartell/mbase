#include <mbase/pc/pc_stream_manager.h>

MBASE_BEGIN

PcStreamManager::flags PcStreamManager::get_stream_by_handle(stream_handle& in_stream_handle, char_stream*& out_stream)
{
	// TODO: DO ERROR CHECK
	out_stream = &mStreams[in_stream_handle];

	return flags::STREAM_MNG_SUCCESS;
}

U32 PcStreamManager::get_stream_count()
{
	return mStreamCount;
}

U32 PcStreamManager::get_stream_size()
{
	return mStreamSize;
}

bool PcStreamManager::initialize(U32 in_stream_count, U32 in_stream_size)
{
	mStreamCount = in_stream_count;
	mStreamSize = in_stream_size;

	if (!mStreamCount)
	{
		mStreamCount = gDefaultStreamCount;
	}

	if (!mStreamSize)
	{
		mStreamSize = gDefaultStreamSize;
	}

	mStreams = std::move(mbase::vector<deep_char_stream>(in_stream_count));

	for (I32 i = 0; i < mStreamCount; i++)
	{
		mStreams.emplace_back(std::move(deep_char_stream(mStreamSize)));
	}

	for (I32 i = mStreamCount - 1; i > 0; i--)
	{
		mHandleStack.push(i);
	}

	return true;
}

PcStreamManager::flags PcStreamManager::acquire_stream(stream_handle& out_stream_handle)
{
	if (mHandleStack.empty())
	{
		return flags::STREAM_ERR_STREAMS_ARE_FULL;
	}
	// TODO: DO ERROR CHECK
	out_stream_handle = mHandleStack.top();
	mHandleStack.pop();
	mStreams[out_stream_handle].set_cursor_front();
	return flags::STREAM_MNG_SUCCESS;
}

PcStreamManager::flags PcStreamManager::acquire_stream(stream_handle& out_stream_handle, char_stream*& out_stream)
{
	if (mHandleStack.empty())
	{
		return flags::STREAM_ERR_STREAMS_ARE_FULL;
	}
	// TODO: DO ERROR CHECK
	out_stream_handle = mHandleStack.top();
	mHandleStack.pop();
	out_stream = &mStreams[out_stream_handle];
	out_stream->set_cursor_front();

	return flags::STREAM_MNG_SUCCESS;
}

PcStreamManager::flags PcStreamManager::release_stream(stream_handle& in_stream_handle)
{
	// TODO: DO ERROR CHECK
	if (in_stream_handle < 0)
	{
		return flags::STREAM_ERR_INVALID_HANDLE;
	}

	mHandleStack.push(in_stream_handle);
	return flags::STREAM_MNG_SUCCESS;
}

MBASE_END