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
		ASYNC_CTX_STAT_FLUSHED,
		ASYNC_CTX_STAT_OPERATING,
		ASYNC_CTX_STAT_FINISHED,
		ASYNC_CTX_DIRECTION_INPUT,
		ASYNC_CTX_DIRECTION_OUTPUT,
		ASYNC_CTX_ERR_UNKNOWN = MBASE_ASYNC_CTX_FLAGS_MAX
	};

	async_io_context(io_base& in_base, flags in_io_direction = flags::ASYNC_CTX_DIRECTION_INPUT) noexcept :
		mTotalBytesTransferred(0),
		mBytesTransferred(0),
		mTargetBytes(0),
		mBytesOnEachIteration(0),
		mLastFraction(0),
		mCalculatedHop(0),
		mHopCounter(0),
		mIsActive(false),
		mIoDirection(in_io_direction),
		mIsBufferInternal(false),
		mIoHandle(&in_base)
	{
		if(in_io_direction == flags::ASYNC_CTX_DIRECTION_OUTPUT)
		{
			mSrcBuffer = mIoHandle->get_os();
		}
		else
		{
			mSrcBuffer = mIoHandle->get_is();
		}
	}

	~async_io_context() noexcept {
		if(!mIsBufferInternal)
		{
			delete mSrcBuffer;
		}
		
		mIoHandle = nullptr;
		mSrcBuffer = nullptr;
	}

	flags construct_context(io_base& in_base, flags in_io_direction = flags::ASYNC_CTX_DIRECTION_INPUT) {
		if (is_registered())
		{
			return flags::ASYNC_CTX_ERR_CONTEXT_ACTIVE;
		}

		if(mSrcBuffer)
		{
			if(!mIsBufferInternal)
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

	MBASE_ND("io context observation ignored") size_type get_bytes_transferred() const noexcept {
		return mBytesTransferred;
	}

	MBASE_ND("io context observation ignored") size_type get_total_transferred_bytes() const noexcept {
		return mTotalBytesTransferred;
	}

	MBASE_ND("io context observation ignored") size_type get_bytes_on_each_iteration() const noexcept {
		return mBytesOnEachIteration;
	}

	MBASE_ND("io context observation ignored") size_type get_requested_bytes_count() const noexcept {
		return bTargetBytes;
	}

	MBASE_ND("io context observation ignored") difference_type get_remaining_bytes() const noexcept {
		return mTargetBytes - mBytesTransferred;
	}

	MBASE_ND("io context observation ignored") U32 get_calculated_hop_count() const noexcept {
		return mCalculatedHop;
	}

	MBASE_ND("io context observation ignored") U32 get_hop_counter() const noexcept {
		return mHopCounter;
	}

	MBASE_ND("io context observation ignored") flags get_io_direction() const noexcept {
		return mIoDirection;
	}

	MBASE_ND("io handle unused") io_base* get_io_handle() noexcept {
		return mIoHandle;
	}

	MBASE_ND("character stream unused") char_stream* get_character_stream() noexcept {
		return mSrcBuffer;
	}

	MBASE_ND("ignoring status") flags get_io_status() {
		return mContextState;
	}

	MBASE_ND("io context observation ignored") bool is_active() const noexcept {
		return mIsActive;
	}

	MBASE_ND("io context observation ignored") bool is_registered() const noexcept {
		return (get_io_status() == flags::ASYNC_CTX_STAT_IDLE || get_io_status() == async_io_context::flags::ASYNC_CTX_STAT_OPERATING);
	}

	GENERIC flush_context() noexcept {
		mSrcBuffer->set_cursor_front();
		mIoHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
		mHopCounter = 0;
		mBytesTransferred = 0;
		mContextState = flags::ASYNC_CTX_STAT_FLUSHED;
	}

	flags halt_context() noexcept {
		if(!mIsActive)
		{
			return flags::ASYNC_CTX_ERR_ALREADY_HALTED;
		}

		mIsActive = false;
		mSrcBuffer->set_cursor_front();
		mIoHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
		return flags::ASYNC_CTX_SUCCESS;
	}

	flags resume_context() noexcept {
		if (mIsActive)
		{
			return flags::ASYNC_CTX_ERR_CONTEXT_ACTIVE;
		}

		mIsActive = true;
		mSrcBuffer->advance(mBytesTransferred);
		mIoHandle->set_file_pointer(mBytesTransferred, mbase::io_base::move_method::MV_BEGIN);
		return flags::ASYNC_CTX_SUCCESS;
	}

	friend class async_io_manager;

private:
	GENERIC _setup_context(size_type in_bytes_to_write, size_type in_bytes_on_each) noexcept {
		mContextState = flags::ASYNC_CTX_STAT_IDLE;
		mTargetBytes = in_bytes_to_write;
		mBytesOnEachIteration = in_bytes_on_each;
		mCalculatedHop = mTargetBytes / mBytesOnEachIteration;
		mLastFraction = mTargetBytes % mBytesOnEachIteration;
		mBytesTransferred = 0;
		mHopCounter = 0;
		mSrcBuffer->set_cursor_front();
	}

	size_type mTotalBytesTransferred;
	size_type mBytesTransferred;
	size_type mTargetBytes;
	size_type mBytesOnEachIteration;
	U32 mLastFraction;
	U32 mCalculatedHop;
	U32 mHopCounter;
	bool mIsActive;
	bool mIsBufferInternal; // true if io context use char_stream of io_base
	io_base* mIoHandle;
	char_stream* mSrcBuffer;
	flags mContextState;
	flags mIoDirection;
};

MBASE_END

#endif // MBASE_IO_CONTEXT_H