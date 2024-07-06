#ifndef MBASE_IO_CONTEXT_H
#define MBASE_IO_CONTEXT_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/io_base.h>
#include <mbase/char_stream.h>

MBASE_BEGIN

class async_io_context : public non_copymovable {
public:
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;

	enum class flags : U32 {
		ASYNC_CTX_SUCCESS = 0,
		ASYNC_CTX_ERR_CONTEXT_ACTIVE = MBASE_ASYNC_CTX_FLAGS_MIN,
		ASYNC_CTX_ERR_ALREADY_HALTED = MBASE_ASYNC_CTX_FLAGS_CONTROL_START,
		ASYNC_CTX_STAT_UNREGISTERED,
		ASYNC_CTX_STAT_IDLE,
		ASYNC_CTX_STAT_ABANDONED,
		ASYNC_CTX_STAT_FAILED,
		ASYNC_CTX_STAT_HALTED,
		ASYNC_CTX_STAT_OPERATING,
		ASYNC_CTX_STAT_FINISHED,
		ASYNC_CTX_STAT_FLUSHED,
		ASYNC_CTX_DIRECTION_INPUT,
		ASYNC_CTX_DIRECTION_OUTPUT,
		ASYNC_CTX_ERR_UNKNOWN = MBASE_ASYNC_CTX_FLAGS_MAX
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE async_io_context(io_base& in_base, flags in_io_direction = flags::ASYNC_CTX_DIRECTION_INPUT) noexcept;
	MBASE_INLINE ~async_io_context() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type get_bytes_transferred() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type get_total_transferred_bytes() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type get_bytes_transferred_last_iteration() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type get_bytes_on_each_iteration() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type get_requested_bytes_count() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE size_type get_aio_manager_id() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE difference_type get_remaining_bytes() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 get_calculated_hop_count() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 get_hop_counter() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE flags get_io_direction() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE flags get_io_status() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_active() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool is_registered() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE io_base* get_io_handle() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE char_stream* get_character_stream() noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE flags construct_context(io_base& in_base, flags in_io_direction = flags::ASYNC_CTX_DIRECTION_INPUT);
	MBASE_INLINE GENERIC flush_context() noexcept;
	MBASE_INLINE flags halt_context() noexcept;
	MBASE_INLINE flags resume_context() noexcept;
	MBASE_INLINE flags set_stream(mbase::char_stream* in_stream) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	friend class async_io_manager;

private:
	GENERIC _setup_context(size_type in_bytes_to_write, size_type in_bytes_on_each, size_type in_manager_id) noexcept {
		mContextState = flags::ASYNC_CTX_STAT_IDLE;
		mTargetBytes = in_bytes_to_write;
		mBytesOnEachIteration = in_bytes_on_each;
		mCalculatedHop = mTargetBytes / mBytesOnEachIteration;
		mLastFraction = mTargetBytes % mBytesOnEachIteration;
		mBytesTransferred = 0;
		mHopCounter = 0;
		mSrcBuffer->set_cursor_front();
		mIsActive = true;
	}

	using aio_ctx_element = mbase::list<async_io_context*>::iterator;

	size_type mTotalBytesTransferred;
	size_type mBytesTransferred;
	size_type mTargetBytes;
	size_type mBytesOnEachIteration;
	size_type mBytesTransferredLastIteration;
	size_type mAioManagerId;
	U32 mLastFraction;
	U32 mCalculatedHop;
	U32 mHopCounter;
	bool mIsActive;
	bool mIsBufferInternal; // true if io context use char_stream of io_base
	io_base* mIoHandle;
	char_stream* mSrcBuffer;
	flags mContextState;
	flags mIoDirection;
	aio_ctx_element mSelfIter;
};

MBASE_INLINE async_io_context::async_io_context(io_base& in_base, flags in_io_direction) noexcept :
	mTotalBytesTransferred(0),
	mBytesTransferred(0),
	mTargetBytes(0),
	mBytesOnEachIteration(0),
	mBytesTransferredLastIteration(0),
	mAioManagerId(0),
	mLastFraction(0),
	mCalculatedHop(0),
	mHopCounter(0),
	mIsActive(false),
	mIoDirection(in_io_direction),
	mIsBufferInternal(false),
	mIoHandle(&in_base),
	mSelfIter(nullptr)
{
	if (in_io_direction == flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		mSrcBuffer = mIoHandle->get_os();
	}
	else
	{
		mSrcBuffer = mIoHandle->get_is();
	}
}

MBASE_INLINE async_io_context::~async_io_context() noexcept 
{
	if (!mIsBufferInternal)
	{
		delete mSrcBuffer;
	}

	mIoHandle = nullptr;
	mSrcBuffer = nullptr;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename async_io_context::size_type async_io_context::get_bytes_transferred() const noexcept 
{
	return mBytesTransferred;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename async_io_context::size_type async_io_context::get_total_transferred_bytes() const noexcept 
{
	return mTotalBytesTransferred;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename async_io_context::size_type async_io_context::get_bytes_transferred_last_iteration() const noexcept
{
	return mBytesTransferredLastIteration;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename async_io_context::size_type async_io_context::get_bytes_on_each_iteration() const noexcept 
{
	return mBytesOnEachIteration;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename async_io_context::size_type async_io_context::get_requested_bytes_count() const noexcept 
{
	return mTargetBytes;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename async_io_context::size_type async_io_context::get_aio_manager_id() const noexcept
{
	return mAioManagerId;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename async_io_context::difference_type async_io_context::get_remaining_bytes() const noexcept 
{
	return mTargetBytes - mBytesTransferred;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 async_io_context::get_calculated_hop_count() const noexcept 
{
	return mCalculatedHop;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 async_io_context::get_hop_counter() const noexcept 
{
	return mHopCounter;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE async_io_context::flags async_io_context::get_io_direction() const noexcept 
{
	return mIoDirection;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE async_io_context::flags async_io_context::get_io_status() const noexcept
{
	return mContextState;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool async_io_context::is_active() const noexcept 
{
	return mIsActive;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE bool async_io_context::is_registered() const noexcept 
{
	return (get_io_status() == flags::ASYNC_CTX_STAT_IDLE || get_io_status() == async_io_context::flags::ASYNC_CTX_STAT_OPERATING);
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE io_base* async_io_context::get_io_handle() noexcept 
{
	return mIoHandle;
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE char_stream* async_io_context::get_character_stream() noexcept 
{
	return mSrcBuffer;
}

MBASE_INLINE async_io_context::flags async_io_context::construct_context(io_base& in_base, flags in_io_direction) 
{
	if (is_registered())
	{
		return flags::ASYNC_CTX_ERR_CONTEXT_ACTIVE;
	}

	if (mSrcBuffer)
	{
		if (!mIsBufferInternal)
		{
			delete mSrcBuffer;
		}
	}

	mIoHandle = &in_base;

	if (in_io_direction == flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		mSrcBuffer = mIoHandle->get_os();
	}
	else
	{
		mSrcBuffer = mIoHandle->get_is();
	}

	return flags::ASYNC_CTX_SUCCESS;
}

MBASE_INLINE GENERIC async_io_context::flush_context() noexcept 
{
	mSrcBuffer->set_cursor_front();
	//mIoHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
	mHopCounter = 0;
	mBytesTransferred = 0;
	mContextState = flags::ASYNC_CTX_STAT_FLUSHED;
}

MBASE_INLINE async_io_context::flags async_io_context::halt_context() noexcept 
{
	if (!mIsActive)
	{
		return flags::ASYNC_CTX_ERR_ALREADY_HALTED;
	}

	mIsActive = false;
	//mSrcBuffer->set_cursor_front();
	//mIoHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
	return flags::ASYNC_CTX_SUCCESS;
}

MBASE_INLINE async_io_context::flags async_io_context::resume_context() noexcept 
{
	if (mIsActive)
	{
		return flags::ASYNC_CTX_ERR_CONTEXT_ACTIVE;
	}

	mIsActive = true;
	mSrcBuffer->advance(mBytesTransferred);
	//mIoHandle->set_file_pointer(mBytesTransferred, mbase::io_base::move_method::MV_BEGIN);
	return flags::ASYNC_CTX_SUCCESS;
}

MBASE_INLINE async_io_context::flags async_io_context::set_stream(mbase::char_stream* in_stream) noexcept
{
	if(mSrcBuffer)
	{
		if(!mIsBufferInternal)
		{
			delete mSrcBuffer;
		}
	}
	mSrcBuffer = in_stream;
	mIsBufferInternal = true;
	return flags::ASYNC_CTX_SUCCESS;
}

MBASE_END

#endif // MBASE_IO_CONTEXT_H