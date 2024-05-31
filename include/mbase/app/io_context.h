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
		totalBytesTransferred(0),
		bytesTransferred(0),
		targetBytes(0),
		bytesOnEachIteration(0),
		lastFraction(0),
		calculatedHop(0),
		hopCounter(0),
		isActive(false),
		ioDirection(in_io_direction),
		isBufferInternal(false)
	{
		ioHandle = &in_base;
		if(in_io_direction == flags::ASYNC_CTX_DIRECTION_OUTPUT)
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
			delete srcBuffer;
		}
		
		ioHandle = nullptr;
		srcBuffer = nullptr;
	}

	flags construct_context(io_base& in_base, flags in_io_direction = flags::ASYNC_CTX_DIRECTION_INPUT) {
		if (is_registered())
		{
			return flags::ASYNC_CTX_ERR_CONTEXT_ACTIVE;
		}

		if(srcBuffer)
		{
			if(!isBufferInternal)
			{
				delete srcBuffer;
			}
		}

		ioHandle = &in_base;

		if (in_io_direction == flags::ASYNC_CTX_DIRECTION_OUTPUT)
		{
			srcBuffer = ioHandle->get_os();
		}
		else
		{
			srcBuffer = ioHandle->get_is();
		}

		return flags::ASYNC_CTX_SUCCESS;
	}

	MBASE_ND("io context observation ignored") size_type get_bytes_transferred() const noexcept {
		return bytesTransferred;
	}

	MBASE_ND("io context observation ignored") size_type get_total_transferred_bytes() const noexcept {
		return totalBytesTransferred;
	}

	MBASE_ND("io context observation ignored") size_type get_bytes_on_each_iteration() const noexcept {
		return bytesOnEachIteration;
	}

	MBASE_ND("io context observation ignored") size_type get_requested_bytes_count() const noexcept {
		return targetBytes;
	}

	MBASE_ND("io context observation ignored") difference_type get_remaining_bytes() const noexcept {
		return targetBytes - bytesTransferred;
	}

	MBASE_ND("io context observation ignored") U32 get_calculated_hop_count() const noexcept {
		return calculatedHop;
	}

	MBASE_ND("io context observation ignored") U32 get_hop_counter() const noexcept {
		return hopCounter;
	}

	MBASE_ND("io context observation ignored") flags get_io_direction() const noexcept {
		return ioDirection;
	}

	MBASE_ND("io handle unused") io_base* get_io_handle() noexcept {
		return ioHandle;
	}

	MBASE_ND("character stream unused") char_stream* get_character_stream() noexcept {
		return srcBuffer;
	}

	MBASE_ND("ignoring status") flags get_io_status() {
		return ais;
	}

	MBASE_ND("io context observation ignored") bool is_active() const noexcept {
		return isActive;
	}

	MBASE_ND("io context observation ignored") bool is_registered() const noexcept {
		return (ais == flags::ASYNC_CTX_STAT_IDLE || ais == async_io_context::flags::ASYNC_CTX_STAT_OPERATING);
	}

	GENERIC flush_context() noexcept {
		srcBuffer->set_cursor_front();
		ioHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
		hopCounter = 0;
		bytesTransferred = 0;
		ais = flags::ASYNC_CTX_STAT_FLUSHED;
	}

	flags halt_context() noexcept {
		if(!isActive)
		{
			return flags::ASYNC_CTX_ERR_ALREADY_HALTED;
		}

		isActive = false;
		srcBuffer->set_cursor_front();
		ioHandle->set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
		return flags::ASYNC_CTX_SUCCESS;
	}

	flags resume_context() noexcept {
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
	GENERIC _setup_context(size_type in_bytes_to_write, size_type in_bytes_on_each) noexcept {
		ais = flags::ASYNC_CTX_STAT_IDLE;
		targetBytes = in_bytes_to_write;
		bytesOnEachIteration = in_bytes_on_each;
		calculatedHop = targetBytes / bytesOnEachIteration;
		lastFraction = targetBytes % bytesOnEachIteration;
		bytesTransferred = 0;
		hopCounter = 0;
		srcBuffer->set_cursor_front();
	}

	size_type totalBytesTransferred;
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