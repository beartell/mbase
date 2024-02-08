#ifndef MBASE_IO_CONTEXT_H
#define MBASE_IO_CONTEXT_H

#include <mbase/common.h>
#include <mbase/io_base.h>
#include <mbase/char_stream.h>

MBASE_BEGIN

class async_io_context {
public:
	enum class status : U8 {
		ASYNC_IO_STAT_UNREGISTERED = 0,
		ASYNC_IO_STAT_IDLE = 1,
		ASYNC_IO_STAT_INTERRUPTED = 2,
		ASYNC_IO_STAT_FAILED = 3,
		ASYNC_IO_STAT_FLUSHED = 4,
		ASYNC_IO_STAT_OPERATING = 5,
		ASYNC_IO_STAT_FINISHED = 6
	};

	async_io_context(io_base& in_base) noexcept : 
		bytesTransferred(0),
		targetBytes(0),
		lastFraction(0),
		calculatedHop(0),
		hopCounter(0),
		isActive(true) 
	{
		ioHandle = &in_base;
		srcBuffer = ioHandle->get_os();
	}

	GENERIC SetIoHandle(io_base& in_handle) noexcept {
		ioHandle = &in_handle;
		srcBuffer = ioHandle->get_os();
	}

	USED_RETURN U64 GetTotalTransferredBytes() const noexcept {
		return bytesTransferred;
	}

	USED_RETURN U64 GetRequestedByteCount() const noexcept {
		return targetBytes;
	}

	USED_RETURN U64 GetRemainingBytes() const noexcept {
		return targetBytes - bytesTransferred;
	}

	USED_RETURN U32 GetCalculatedHopCount() const noexcept {
		return calculatedHop;
	}

	USED_RETURN U32 GetHopCounter() const noexcept {
		return hopCounter;
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
		bytesTransferred = 0;
		srcBuffer->set_cursor_front();
		hopCounter = 0;
		// !!!!! do not forget to reposition the file pointer too !!!!!
	}

	GENERIC HaltContext() noexcept {
		isActive = false;
		srcBuffer->set_cursor_front();
	}

	GENERIC ResumeContext() noexcept {
		isActive = true;
		srcBuffer->advance(bytesTransferred);
	}

	friend class async_io_manager;

private:
	U64 bytesTransferred;
	U64 targetBytes;
	U32 lastFraction;
	U32 calculatedHop;
	U32 hopCounter;
	io_base* ioHandle;
	bool isActive;

	char_stream* srcBuffer;
	status ais;
};

MBASE_END

#endif // MBASE_IO_CONTEXT_H