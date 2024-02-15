#ifndef MBASE_IO_CONTEXT_H
#define MBASE_IO_CONTEXT_H

#include <mbase/common.h>
#include <mbase/io_base.h>
#include <mbase/char_stream.h>

MBASE_BEGIN

class async_io_context {
public:
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;

	enum class direction : U8 {
		IO_CTX_DIRECTION_INPUT = 0,
		IO_CTX_DIRECTION_OUTPUT = 1
	};

	enum class status : U8 {
		ASYNC_IO_STAT_UNREGISTERED = 0,
		ASYNC_IO_STAT_IDLE = 1,
		ASYNC_IO_STAT_INTERRUPTED = 2,
		ASYNC_IO_STAT_FAILED = 3,
		ASYNC_IO_STAT_FLUSHED = 4,
		ASYNC_IO_STAT_OPERATING = 5,
		ASYNC_IO_STAT_FINISHED = 6
	};

	async_io_context(io_base& in_base, direction in_io_direction = direction::IO_CTX_DIRECTION_INPUT) noexcept : 
		bytesTransferred(0),
		targetBytes(0),
		bytesOnEachIteration(0),
		lastFraction(0),
		calculatedHop(0),
		hopCounter(0),
		isActive(true),
		ioDirection(in_io_direction),
		isBufferInternal(true)
	{
		ioHandle = &in_base;
		if(in_io_direction == direction::IO_CTX_DIRECTION_OUTPUT)
		{
			srcBuffer = ioHandle->get_os();
		}
		else
		{
			srcBuffer = ioHandle->get_is();
		}
	}

	~async_io_context() noexcept {
		if(!isBufferInternal)
		{
			deep_char_stream* dcsTemp = static_cast<deep_char_stream*>(srcBuffer);
			delete dcsTemp;
		}
		ioHandle = nullptr;
		srcBuffer = nullptr;
	}

	USED_RETURN size_type GetTotalTransferredBytes() const noexcept {
		return bytesTransferred;
	}

	USED_RETURN size_type GetBytesOnEachIteration() const noexcept {
		return bytesOnEachIteration;
	}

	USED_RETURN size_type GetRequestedByteCount() const noexcept {
		return targetBytes;
	}

	USED_RETURN difference_type GetRemainingBytes() const noexcept {
		return targetBytes - bytesTransferred;
	}

	USED_RETURN U32 GetCalculatedHopCount() const noexcept {
		return calculatedHop;
	}

	USED_RETURN U32 GetHopCounter() const noexcept {
		return hopCounter;
	}

	USED_RETURN direction GetIoDirection() const noexcept {
		return ioDirection;
	}

	USED_RETURN io_base* GetIoHandle() noexcept {
		return ioHandle;
	}

	USED_RETURN char_stream* GetCharacterStream() noexcept {
		return srcBuffer;
	}

	USED_RETURN bool IsActive() const noexcept {
		return isActive;
	}

	GENERIC FlushContext() noexcept {
		srcBuffer->set_cursor_front();
		ioHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
		hopCounter = 0;
	}

	GENERIC HaltContext() noexcept {
		isActive = false;
		srcBuffer->set_cursor_front();
		ioHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
	}

	GENERIC ResumeContext() noexcept {
		isActive = true;
		srcBuffer->advance(bytesTransferred);
		ioHandle->set_file_pointer(bytesTransferred, mbase::io_base::move_method::MV_BEGIN);
	}

	friend class async_io_manager;

private:
	size_type bytesTransferred;
	size_type targetBytes;
	size_type bytesOnEachIteration;

	U32 lastFraction;
	U32 calculatedHop;
	U32 hopCounter;
	bool isActive;
	bool isBufferInternal; // true if io context use char_stream of io_base

	io_base* ioHandle;
	char_stream* srcBuffer;
	status ais;
	direction ioDirection;
};

MBASE_END

#endif // MBASE_IO_CONTEXT_H