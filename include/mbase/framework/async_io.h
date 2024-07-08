#ifndef MBASE_ASYNC_IO_H
#define MBASE_ASYNC_IO_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/list.h>
#include <mbase/framework/io_context.h>

MBASE_BEGIN

static SIZE_T gAioMngIdCounter = 0;

class async_io_manager : public non_copymovable {
public:
	using size_type = SIZE_T;

	enum class flags : U32 {
		AIO_MNG_SUCCESS = 0,
		AIO_MNG_ERR_MISSING_CONTEXT = MBASE_AIO_MNG_FLAGS_MIN,
		AIO_MNG_ERR_ALREADY_REGISTERED,
		AIO_MNG_ERR_INVALID_PARAMS,
		AIO_MNG_ERR_LIST_BLOATED,
		AIO_MNG_ERR_MISSING_STREAM,
		AIO_MNG_ERR_INVALID_IO_DIRECTION,
		AIO_MNG_ERR_EMPTY_BUFFER,
		AIO_MNG_ERR_INVALID_INTERNAL_STREAM_SIZE,
		AIO_MNG_WARN_INTERNAL_STREAM_ALTERED,
		AIO_MNG_ERR_PARAMETER_AMBIGUITY,
		AIO_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER,
		AIO_MNG_ERR_INACTIVE_CONTEXT,
		AIO_MNG_ERR_INTERNAL_ERROR,
		AIO_MNG_ERR_UNKNOWN = MBAES_AIO_MNG_FLAGS_MAX
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE async_io_manager() noexcept;
	MBASE_INLINE async_io_manager(U32 in_max_write_context, U32 in_max_read_context) noexcept;
	MBASE_INLINE ~async_io_manager() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const mbase::list<async_io_context*>& get_write_context_list() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const mbase::list<async_io_context*>& get_read_context_list() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 get_allowed_write_context() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 get_allowed_read_context() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE SIZE_T get_manager_id() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_write_context_available() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_read_context_available() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE flags enqueue_write_context(async_io_context& in_ctx, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE flags enqueue_write_context(async_io_context& in_ctx, IBYTEBUFFER in_data, size_type in_bytes_to_write, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE flags enqueue_read_context(async_io_context& in_ctx, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE flags enqueue_read_context(async_io_context& in_ctx, size_type in_bytes_to_read, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE GENERIC flush_read_contexts() noexcept;
	MBASE_INLINE GENERIC flush_write_contexts() noexcept;
	MBASE_INLINE GENERIC clear_read_contexts() noexcept;
	MBASE_INLINE GENERIC clear_write_contexts() noexcept;
	MBASE_INLINE flags run_target_write_context(async_io_context& in_ctx) noexcept;
	MBASE_INLINE flags run_target_read_context(async_io_context& in_ctx) noexcept;
	MBASE_INLINE GENERIC run_write_contexts() noexcept;
	MBASE_INLINE GENERIC run_read_contexts() noexcept;
	MBASE_INLINE GENERIC run_both_contexts(bool in_writefirst = true) noexcept;
	MBASE_INLINE GENERIC halt_write_contexts() noexcept;
	MBASE_INLINE GENERIC halt_read_contexts() noexcept;
	MBASE_INLINE GENERIC set_allowed_write_context(U32 in_max_write_context) noexcept;
	MBASE_INLINE GENERIC set_allowed_read_context(U32 in_max_read_context) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

private:
	SIZE_T mAioId;
	U32 mMaximumAllowedWriteContext;
	U32 mMaximumAllowedReadContext;
	mbase::list<async_io_context*> mWriteContextList;
	mbase::list<async_io_context*> mReadContextList;

	#ifdef MBASE_ASYNC_IO_THREAD_SAFE
		mbase::mutex mWriteQueueMutex;
		mbase::mutex mReadQueueMutex;
	#endif
};

MBASE_INLINE async_io_manager::async_io_manager() noexcept
{
	mAioId = ++gAioMngIdCounter;
	mMaximumAllowedWriteContext = 32;
	mMaximumAllowedReadContext = 32;
}

MBASE_INLINE async_io_manager::async_io_manager(U32 in_max_write_context, U32 in_max_read_context) noexcept
{
	// BOUNDARIES WILL BE CONTROLLED
	mAioId = ++gAioMngIdCounter;
	mMaximumAllowedWriteContext = in_max_write_context;
	mMaximumAllowedReadContext = in_max_read_context;
}

MBASE_INLINE async_io_manager::~async_io_manager() noexcept 
{
	clear_write_contexts();
	clear_read_contexts();
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const mbase::list<async_io_context*>& async_io_manager::get_write_context_list() const noexcept
{
	return mWriteContextList;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE const mbase::list<async_io_context*>& async_io_manager::get_read_context_list() const noexcept
{
	return mReadContextList;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 async_io_manager::get_allowed_write_context() const noexcept
{
	return mMaximumAllowedWriteContext;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 async_io_manager::get_allowed_read_context() const noexcept
{
	return mMaximumAllowedReadContext;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE SIZE_T async_io_manager::get_manager_id() const noexcept
{
	return mAioId;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool async_io_manager::is_write_context_available() const noexcept
{
	if (mWriteContextList.size() > mMaximumAllowedWriteContext)
	{
		return false;
	}

	return true;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool async_io_manager::is_read_context_available() const noexcept
{
	if (mReadContextList.size() > mMaximumAllowedReadContext)
	{
		return false;
	}

	return true;
}

MBASE_INLINE async_io_manager::flags async_io_manager::enqueue_write_context(async_io_context& in_ctx, size_type in_bytes_on_each) noexcept 
{
	if (mWriteContextList.size() > mMaximumAllowedWriteContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	char_stream* srcBuffer = in_ctx.get_character_stream();
	MBASE_NULL_CHECK_RETURN_VAL(srcBuffer, flags::AIO_MNG_ERR_MISSING_STREAM);
	
	if (in_ctx.get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (in_ctx.is_registered())
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}

	if(!srcBuffer->buffer_length())
	{
		return flags::AIO_MNG_ERR_EMPTY_BUFFER;
	}
	
	if(srcBuffer->buffer_length() < in_bytes_on_each)
	{
		return flags::AIO_MNG_ERR_PARAMETER_AMBIGUITY;
	}

	in_ctx._setup_context(in_ctx.mSrcBuffer->buffer_length(), in_bytes_on_each, get_manager_id());

	MBASE_TS_LOCK(mWriteQueueMutex)
		mWriteContextList.push_back(&in_ctx);
		in_ctx.mSelfIter = mWriteContextList.end_node();
	MBASE_TS_UNLOCK(mWriteQueueMutex)

	return flags::AIO_MNG_SUCCESS;
}

MBASE_INLINE async_io_manager::flags async_io_manager::enqueue_write_context(async_io_context& in_ctx, IBYTEBUFFER in_data, size_type in_bytes_to_write, size_type in_bytes_on_each) noexcept 
{
	if (mWriteContextList.size() > mMaximumAllowedWriteContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	MBASE_NULL_CHECK_RETURN_VAL(in_data, flags::AIO_MNG_ERR_EMPTY_BUFFER);

	if (in_ctx.get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (in_ctx.is_registered())
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}
	
	if (!in_bytes_to_write)
	{
		return flags::AIO_MNG_ERR_EMPTY_BUFFER;
	}

	if(in_bytes_to_write < in_bytes_on_each)
	{
		return flags::AIO_MNG_ERR_PARAMETER_AMBIGUITY;
	}

	flags err = flags::AIO_MNG_SUCCESS;

	if (in_ctx.mSrcBuffer)
	{
		err = flags::AIO_MNG_WARN_INTERNAL_STREAM_ALTERED;
		if (in_ctx.mSrcBuffer->buffer_length() == in_bytes_to_write)
		{
			in_ctx.mSrcBuffer->zero_out_buffer();
		}
		else
		{
			if(in_ctx.mIsBufferInternal)
			{
				return flags::AIO_MNG_ERR_INVALID_INTERNAL_STREAM_SIZE;
			}

			delete in_ctx.mSrcBuffer;
			in_ctx.mSrcBuffer = new deep_char_stream(in_data, in_bytes_to_write);
			in_ctx.mIsBufferInternal = false;
		}
	}
	else
	{
		in_ctx.mSrcBuffer = new deep_char_stream(in_data, in_bytes_to_write);
		in_ctx.mIsBufferInternal = false;
	}

	in_ctx._setup_context(in_ctx.mSrcBuffer->buffer_length(), in_bytes_on_each, get_manager_id());

	MBASE_TS_LOCK(mWriteQueueMutex)
		mWriteContextList.push_back(&in_ctx);
		in_ctx.mSelfIter = mWriteContextList.end_node();
	MBASE_TS_UNLOCK(mWriteQueueMutex)

	return err;
}

MBASE_INLINE async_io_manager::flags async_io_manager::enqueue_read_context(async_io_context& in_ctx, size_type in_bytes_on_each) noexcept 
{
	if (mReadContextList.size() > mMaximumAllowedReadContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	char_stream* srcBuffer = in_ctx.get_character_stream();
	MBASE_NULL_CHECK_RETURN_VAL(srcBuffer, flags::AIO_MNG_ERR_MISSING_STREAM);

	if (in_ctx.get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (in_ctx.is_registered())
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}

	if (!srcBuffer->buffer_length())
	{
		return flags::AIO_MNG_ERR_EMPTY_BUFFER;
	}

	if(srcBuffer->buffer_length() < in_bytes_on_each)
	{
		return flags::AIO_MNG_ERR_PARAMETER_AMBIGUITY;
	}

	in_ctx._setup_context(in_ctx.mSrcBuffer->buffer_length(), in_bytes_on_each, get_manager_id());

	MBASE_TS_LOCK(mReadQueueMutex)
		mReadContextList.push_back(&in_ctx);
		in_ctx.mSelfIter = mReadContextList.end_node();
	MBASE_TS_UNLOCK(mReadQueueMutex);

	return flags::AIO_MNG_SUCCESS;
}

MBASE_INLINE async_io_manager::flags async_io_manager::enqueue_read_context(async_io_context& in_ctx, size_type in_bytes_to_read, size_type in_bytes_on_each) noexcept 
{
	if (mReadContextList.size() > mMaximumAllowedReadContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	if (in_ctx.get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (in_ctx.is_registered())
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}

	if (!in_bytes_to_read)
	{
		return flags::AIO_MNG_ERR_EMPTY_BUFFER;
	}

	if(in_bytes_to_read < in_bytes_on_each)
	{
		return flags::AIO_MNG_ERR_PARAMETER_AMBIGUITY;
	}

	flags err = flags::AIO_MNG_SUCCESS;

	if (in_ctx.mSrcBuffer)
	{
		err = flags::AIO_MNG_WARN_INTERNAL_STREAM_ALTERED;
		if (in_ctx.mSrcBuffer->buffer_length() == in_bytes_to_read)
		{
			in_ctx.mSrcBuffer->zero_out_buffer();
		}
		else
		{
			if (in_ctx.mIsBufferInternal)
			{
				return flags::AIO_MNG_ERR_INVALID_INTERNAL_STREAM_SIZE;
			}
			in_ctx.mSrcBuffer = new deep_char_stream(in_bytes_to_read);
			in_ctx.mIsBufferInternal = false;
		}
	}
	else
	{
		in_ctx.mSrcBuffer = new deep_char_stream(in_bytes_to_read);
		in_ctx.mIsBufferInternal = false;
	}

	in_ctx._setup_context(in_ctx.mSrcBuffer->buffer_length(), in_bytes_on_each, get_manager_id());

	MBASE_TS_LOCK(mReadQueueMutex);
		mReadContextList.push_back(&in_ctx);
		in_ctx.mSelfIter = mReadContextList.end_node();
	MBASE_TS_UNLOCK(mReadQueueMutex);

	return err;
}

MBASE_INLINE GENERIC async_io_manager::flush_read_contexts() noexcept 
{
	MBASE_TS_LOCK(mReadQueueMutex);
	for (mbase::list<async_io_context*>::iterator It = mReadContextList.begin(); It != mReadContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		iCtx->flush_context();
	}
	MBASE_TS_UNLOCK(mReadQueueMutex);
}

MBASE_INLINE GENERIC async_io_manager::flush_write_contexts() noexcept 
{
	MBASE_TS_LOCK(mWriteQueueMutex)
		for (mbase::list<async_io_context*>::iterator It = mWriteContextList.begin(); It != mWriteContextList.end(); It++)
		{
			async_io_context* iCtx = *It;
			iCtx->flush_context();
		}
	MBASE_TS_UNLOCK(mWriteQueueMutex)
}

MBASE_INLINE GENERIC async_io_manager::clear_read_contexts() noexcept 
{
	MBASE_TS_LOCK(mReadQueueMutex);
	for (mbase::list<async_io_context*>::iterator It = mReadContextList.begin(); It != mReadContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		async_io_context::flags st = iCtx->mContextState;
		iCtx->flush_context();
		iCtx->mIsActive = false;
		if (st != async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
		{
			iCtx->mContextState = async_io_context::flags::ASYNC_CTX_STAT_ABANDONED;
		}

		It = mReadContextList.erase(It);
	}
	MBASE_TS_UNLOCK(mReadQueueMutex);
}

MBASE_INLINE GENERIC async_io_manager::clear_write_contexts() noexcept 
{
	MBASE_TS_LOCK(mWriteQueueMutex)
		for (mbase::list<async_io_context*>::iterator It = mWriteContextList.begin(); It != mWriteContextList.end(); It++)
		{
			async_io_context* iCtx = *It;
			async_io_context::flags st = iCtx->mContextState;
			iCtx->flush_context();
			iCtx->mIsActive = false;
			if (st != async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
			{
				iCtx->mContextState = async_io_context::flags::ASYNC_CTX_STAT_ABANDONED;
			}
			It = mWriteContextList.erase(It);
		}
	MBASE_TS_UNLOCK(mWriteQueueMutex)
}

MBASE_INLINE async_io_manager::flags async_io_manager::run_target_write_context(async_io_context& in_ctx) noexcept
{
	MBASE_TS_LOCK(mWriteQueueMutex)

	if(in_ctx.get_aio_manager_id() != get_manager_id())
	{
		return flags::AIO_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER;
	}

	if(!in_ctx.is_active())
	{
		return flags::AIO_MNG_ERR_INACTIVE_CONTEXT;
	}

	if(in_ctx.get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	// TODO: Check if io base is operable

	async_io_context::flags tempFlag = async_io_context::flags::ASYNC_CTX_STAT_IDLE;

	in_ctx.mIsActive = true;
	in_ctx.mContextState = async_io_context::flags::ASYNC_CTX_STAT_OPERATING;

	size_type bytesToWrite = in_ctx.get_bytes_on_each_iteration();
	io_base* rawHandle = in_ctx.get_io_handle();
	char_stream* rawStream = in_ctx.get_character_stream();

	in_ctx.mHopCounter++;
	if (in_ctx.get_hop_counter() == in_ctx.get_calculated_hop_count())
	{
		bytesToWrite += in_ctx.mLastFraction;
		tempFlag = async_io_context::flags::ASYNC_CTX_STAT_FINISHED;
	}

	size_type writtenBytes = rawHandle->write_data(*rawStream, bytesToWrite);

	in_ctx.mBytesTransferred += writtenBytes;
	in_ctx.mTotalBytesTransferred += writtenBytes;
	in_ctx.mBytesTransferredLastIteration = writtenBytes;
	in_ctx.mContextState = tempFlag;

	if(!writtenBytes)
	{
		in_ctx.mContextState = async_io_context::flags::ASYNC_CTX_STAT_ABANDONED;
		mWriteContextList.erase(in_ctx.mSelfIter);
		MBASE_TS_UNLOCK(mWriteQueueMutex)
		return flags::AIO_MNG_ERR_INTERNAL_ERROR;
	}

	if(in_ctx.mContextState == async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
	{
		mWriteContextList.erase(in_ctx.mSelfIter);
		MBASE_TS_UNLOCK(mWriteQueueMutex)
		return flags::AIO_MNG_SUCCESS;
	}

	MBASE_TS_UNLOCK(mWriteQueueMutex);
	return flags::AIO_MNG_SUCCESS;
}

MBASE_INLINE async_io_manager::flags async_io_manager::run_target_read_context(async_io_context& in_ctx) noexcept
{
	MBASE_TS_LOCK(mReadQueueMutex)

	if (in_ctx.get_aio_manager_id() != get_manager_id())
	{
		return flags::AIO_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER;
	}

	if (!in_ctx.is_active())
	{
		return flags::AIO_MNG_ERR_INACTIVE_CONTEXT;
	}

	if (in_ctx.get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	// TODO: Check if io base is operable

	async_io_context::flags tempFlag = async_io_context::flags::ASYNC_CTX_STAT_IDLE;

	in_ctx.mIsActive = true;
	in_ctx.mContextState = async_io_context::flags::ASYNC_CTX_STAT_OPERATING;

	size_type bytesToRead = in_ctx.get_bytes_on_each_iteration();
	io_base* rawHandle = in_ctx.get_io_handle();
	char_stream* rawStream = in_ctx.get_character_stream();

	in_ctx.mHopCounter++;
	if (in_ctx.get_hop_counter() == in_ctx.get_calculated_hop_count())
	{
		bytesToRead += in_ctx.mLastFraction;
		tempFlag = async_io_context::flags::ASYNC_CTX_STAT_FINISHED;
	}

	size_type readBytes = rawHandle->read_data(*rawStream, bytesToRead);

	in_ctx.mBytesTransferred += readBytes;
	in_ctx.mTotalBytesTransferred += readBytes;
	in_ctx.mBytesTransferredLastIteration = readBytes;
	in_ctx.mContextState = tempFlag;

	if (!readBytes)
	{
		in_ctx.mContextState = async_io_context::flags::ASYNC_CTX_STAT_ABANDONED;
		mReadContextList.erase(in_ctx.mSelfIter);
		MBASE_TS_UNLOCK(mReadQueueMutex)
		return flags::AIO_MNG_ERR_INTERNAL_ERROR;
	}

	if (in_ctx.mContextState == async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
	{
		mReadContextList.erase(in_ctx.mSelfIter);
		MBASE_TS_UNLOCK(mReadQueueMutex)
		return flags::AIO_MNG_SUCCESS;
	}

	MBASE_TS_UNLOCK(mReadQueueMutex)
	return flags::AIO_MNG_SUCCESS;
}

MBASE_INLINE GENERIC async_io_manager::run_write_contexts() noexcept 
{
	for (mbase::list<async_io_context*>::iterator It = mWriteContextList.begin(); It != mWriteContextList.end();)
	{
		async_io_context* iCtx = *It;
		++It;
		run_target_write_context(*iCtx);
	}
}

MBASE_INLINE GENERIC async_io_manager::run_read_contexts() noexcept 
{
	for (mbase::list<async_io_context*>::iterator It = mReadContextList.begin(); It != mReadContextList.end();)
	{
		async_io_context* iCtx = *It;
		++It;
		run_target_read_context(*iCtx);
	}
}

MBASE_INLINE GENERIC async_io_manager::run_both_contexts(bool in_writefirst) noexcept 
{
	if (in_writefirst)
	{
		run_write_contexts();
		run_read_contexts();
	}
	else
	{
		run_read_contexts();
		run_write_contexts();
	}
}

MBASE_INLINE GENERIC async_io_manager::halt_write_contexts() noexcept 
{
	MBASE_TS_LOCK(mWriteQueueMutex)

	for (mbase::list<async_io_context*>::iterator It = mWriteContextList.begin(); It != mWriteContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		iCtx->halt_context();
	}

	MBASE_TS_UNLOCK(mWriteQueueMutex)
}

MBASE_INLINE GENERIC async_io_manager::halt_read_contexts() noexcept 
{
	MBASE_TS_LOCK(mReadQueueMutex);

	for (mbase::list<async_io_context*>::iterator It = mReadContextList.begin(); It != mReadContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		iCtx->halt_context();
	}

	MBASE_TS_UNLOCK(mReadQueueMutex);
}

MBASE_INLINE GENERIC async_io_manager::set_allowed_write_context(U32 in_max_write_context) noexcept
{
	mMaximumAllowedWriteContext = in_max_write_context;
}

MBASE_INLINE GENERIC async_io_manager::set_allowed_read_context(U32 in_max_read_context) noexcept
{
	mMaximumAllowedReadContext = in_max_read_context;
}


MBASE_END

#endif // MBASE_ASYNC_IO_H