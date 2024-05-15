#ifndef MBASE_IO_CONTEXT_H
#define MBASE_IO_CONTEXT_H

#include <mbase/common.h>
#include <mbase/io_base.h>
#include <mbase/char_stream.h>

MBASE_BEGIN

class async_io_context : public non_copymovable {
public:
	using size_type = SIZE_T;
	using difference_type = PTRDIFF;

	enum class flags : U32 {
		ASYNC_CTX_SUCCESS = 0,
		ASYNC_CTX_ERR_CONTEXT_ACTIVE = 1,
		ASYNC_CTX_ERR_ALREADY_HALTED = 2,
		ASYNC_IO_STAT_UNREGISTERED = 3,
		ASYNC_IO_STAT_IDLE = 4,
		ASYNC_IO_STAT_ABANDONED = 5,
		ASYNC_IO_STAT_FAILED = 6,
		ASYNC_IO_STAT_FLUSHED = 7,
		ASYNC_IO_STAT_OPERATING = 8,
		ASYNC_IO_STAT_FINISHED = 9,
		IO_CTX_DIRECTION_INPUT = 10,
		IO_CTX_DIRECTION_OUTPUT = 11
	};

	async_io_context(io_base& in_base, flags in_io_direction = flags::IO_CTX_DIRECTION_INPUT) noexcept :
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
		if(in_io_direction == flags::IO_CTX_DIRECTION_OUTPUT)
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
			//deep_char_stream* dcsTemp = static_cast<deep_char_stream*>(srcBuffer);
			delete srcBuffer;
		}
		
		ioHandle = nullptr;
		srcBuffer = nullptr;
	}

	flags ConstructContext(io_base& in_base, flags in_io_direction = flags::IO_CTX_DIRECTION_INPUT) {
		if (isActive)
		{
			return flags::ASYNC_CTX_ERR_CONTEXT_ACTIVE;
		}

		ioHandle = &in_base;
		if (!isBufferInternal)
		{
			//deep_char_stream* dcsTemp = static_cast<deep_char_stream*>(srcBuffer);
			delete srcBuffer;
		}

		srcBuffer = nullptr;

		if (in_io_direction == flags::IO_CTX_DIRECTION_OUTPUT)
		{
			srcBuffer = ioHandle->get_os();
		}
		else
		{
			srcBuffer = ioHandle->get_is();
		}

		return flags::ASYNC_CTX_SUCCESS;
	}

	MBASE_ND("io context observation ignored") size_type GetTotalTransferredBytes() const noexcept {
		return bytesTransferred;
	}

	MBASE_ND("io context observation ignored") size_type GetBytesOnEachIteration() const noexcept {
		return bytesOnEachIteration;
	}

	MBASE_ND("io context observation ignored") size_type GetRequestedByteCount() const noexcept {
		return targetBytes;
	}

	MBASE_ND("io context observation ignored") difference_type GetRemainingBytes() const noexcept {
		return targetBytes - bytesTransferred;
	}

	MBASE_ND("io context observation ignored") U32 GetCalculatedHopCount() const noexcept {
		return calculatedHop;
	}

	MBASE_ND("io context observation ignored") U32 GetHopCounter() const noexcept {
		return hopCounter;
	}

	MBASE_ND("io context observation ignored") flags GetIoDirection() const noexcept {
		return ioDirection;
	}

	MBASE_ND("io handle unused") io_base* GetIoHandle() noexcept {
		return ioHandle;
	}

	MBASE_ND("character stream unused") char_stream* GetCharacterStream() noexcept {
		return srcBuffer;
	}

	MBASE_ND("ignoring status") flags GetIoStatus() {
		return ais;
	}

	MBASE_ND("io context observation ignored") bool IsActive() const noexcept {
		return isActive;
	}

	GENERIC FlushContext() noexcept {
		srcBuffer->set_cursor_front();
		ioHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
		hopCounter = 0;
		bytesTransferred = 0;
		ais = flags::ASYNC_IO_STAT_FLUSHED;
	}

	flags HaltContext() noexcept {
		if(!isActive)
		{
			return flags::ASYNC_CTX_ERR_ALREADY_HALTED;
		}

		isActive = false;
		srcBuffer->set_cursor_front();
		ioHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
		return flags::ASYNC_CTX_SUCCESS;
	}

	flags ResumeContext() noexcept {
		if (isActive)
		{
			return flags::ASYNC_CTX_ERR_CONTEXT_ACTIVE;
		}

		isActive = true;
		srcBuffer->advance(bytesTransferred);
		ioHandle->set_file_pointer(bytesTransferred, mbase::io_base::move_method::MV_BEGIN);
		return flags::ASYNC_CTX_SUCCESS;
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
	flags ais;
	flags ioDirection;
};

MBASE_END

#endif // MBASE_IO_CONTEXT_H