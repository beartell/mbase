#ifndef MBASE_ASYNC_IO_H
#define MBASE_ASYNC_IO_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/list.h>
#include <mbase/app/io_context.h>

MBASE_BEGIN

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
		AIO_MNG_ERR_UNKNOWN = MBAES_AIO_MNG_FLAGS_MAX
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE async_io_manager(U32 in_max_write_context, U32 in_max_read_context) noexcept;
	MBASE_INLINE ~async_io_manager() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE mbase::list<async_io_context*>* get_write_context_list() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE mbase::list<async_io_context*>* get_read_context_list() noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 get_allowed_write_context() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 get_allowed_read_context() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE flags enqueue_write_context(async_io_context* in_ctx, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE flags enqueue_write_context(async_io_context* in_ctx, IBYTEBUFFER in_data, size_type in_bytes_to_write, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE flags enqueue_read_context(async_io_context* in_ctx, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE flags enqueue_read_context(async_io_context* in_ctx, size_type in_bytes_to_read, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE GENERIC flush_read_contexts() noexcept;
	MBASE_INLINE GENERIC flush_write_contexts() noexcept;
	MBASE_INLINE GENERIC clear_read_contexts() noexcept;
	MBASE_INLINE GENERIC clear_write_contexts() noexcept;
	MBASE_INLINE GENERIC run_write_contexts() noexcept;
	MBASE_INLINE GENERIC run_read_contexts() noexcept;
	MBASE_INLINE GENERIC run_both_contexts(bool in_writefirst = true) noexcept;
	MBASE_INLINE GENERIC halt_write_contexts() noexcept;
	MBASE_INLINE GENERIC halt_read_contexts() noexcept;
	MBASE_INLINE GENERIC set_allowed_write_context(U32 in_max_write_context) noexcept;
	MBASE_INLINE GENERIC set_allowed_read_context(U32 in_max_read_context) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

private:
	U32 maximumAllowedWriteContext;
	U32 maximumAllowedReadContext;
	mbase::list<async_io_context*> writeContextList;
	mbase::list<async_io_context*> readContextList;

	#ifdef MBASE_ASYNC_IO_THREAD_SAFE
		mbase::mutex writeQueueMutex;
		mbase::mutex readQueueMutex;
	#endif
};

MBASE_INLINE async_io_manager::async_io_manager(U32 in_max_write_context, U32 in_max_read_context) noexcept 
{
	maximumAllowedWriteContext = in_max_write_context;
	maximumAllowedReadContext = in_max_read_context;
}

MBASE_INLINE async_io_manager::~async_io_manager() noexcept 
{
	clear_write_contexts();
	clear_read_contexts();
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE mbase::list<async_io_context*>* async_io_manager::get_write_context_list() noexcept
{
	return &writeContextList;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE mbase::list<async_io_context*>* async_io_manager::get_read_context_list() noexcept
{
	return &readContextList;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 async_io_manager::get_allowed_write_context() const noexcept
{
	return maximumAllowedWriteContext;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 async_io_manager::get_allowed_read_context() const noexcept
{
	return maximumAllowedReadContext;
}

MBASE_INLINE async_io_manager::flags async_io_manager::enqueue_write_context(async_io_context* in_ctx, size_type in_bytes_on_each) noexcept 
{
	MBASE_NULL_CHECK_RETURN_VAL(in_ctx, flags::AIO_MNG_ERR_MISSING_CONTEXT);
	char_stream* srcBuffer = in_ctx->get_character_stream();
	MBASE_NULL_CHECK_RETURN_VAL(srcBuffer, flags::AIO_MNG_ERR_MISSING_STREAM);

	if (in_ctx->get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (writeContextList.size() > maximumAllowedWriteContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	if (in_ctx->is_registered())
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}

	if(!srcBuffer->buffer_length())
	{
		return flags::AIO_MNG_ERR_EMPTY_BUFFER;
	}

	in_ctx->_setup_context(in_ctx->srcBuffer->buffer_length(), in_bytes_on_each);

	MBASE_TS_LOCK(writeQueueMutex)
		writeContextList.push_back(in_ctx);
	MBASE_TS_UNLOCK(writeQueueMutex)

	return flags::AIO_MNG_SUCCESS;
}

MBASE_INLINE async_io_manager::flags async_io_manager::enqueue_write_context(async_io_context* in_ctx, IBYTEBUFFER in_data, size_type in_bytes_to_write, size_type in_bytes_on_each) noexcept 
{
	MBASE_NULL_CHECK_RETURN_VAL(in_data, flags::AIO_MNG_ERR_EMPTY_BUFFER);
	MBASE_NULL_CHECK_RETURN_VAL(in_ctx, flags::AIO_MNG_ERR_MISSING_CONTEXT);
	if(!in_bytes_to_write)
	{
		return flags::AIO_MNG_ERR_EMPTY_BUFFER;
	}

	if (in_ctx->get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (writeContextList.size() > maximumAllowedWriteContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	if (in_ctx->is_registered())
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}

	flags err = flags::AIO_MNG_SUCCESS;

	if (in_ctx->srcBuffer)
	{
		err = flags::AIO_MNG_WARN_INTERNAL_STREAM_ALTERED;
		if (in_ctx->srcBuffer->buffer_length() == in_bytes_to_write)
		{
			in_ctx->srcBuffer->zero_out_buffer();
		}
		else
		{
			if(in_ctx->isBufferInternal)
			{
				return flags::AIO_MNG_ERR_INVALID_INTERNAL_STREAM_SIZE;
			}

			delete in_ctx->srcBuffer;
			in_ctx->srcBuffer = new deep_char_stream(in_data, in_bytes_to_write);
			in_ctx->isBufferInternal = false;
		}
	}
	else
	{
		in_ctx->srcBuffer = new deep_char_stream(in_data, in_bytes_to_write);
		in_ctx->isBufferInternal = false;
	}

	in_ctx->_setup_context(in_ctx->srcBuffer->buffer_length(), in_bytes_on_each);

	MBASE_TS_LOCK(writeQueueMutex)
		writeContextList.push_back(in_ctx);
	MBASE_TS_UNLOCK(writeQueueMutex)

	return err;
}

MBASE_INLINE async_io_manager::flags async_io_manager::enqueue_read_context(async_io_context* in_ctx, size_type in_bytes_on_each) noexcept 
{
	MBASE_NULL_CHECK_RETURN_VAL(in_ctx, flags::AIO_MNG_ERR_MISSING_CONTEXT);
	char_stream* srcBuffer = in_ctx->get_character_stream();
	MBASE_NULL_CHECK_RETURN_VAL(srcBuffer, flags::AIO_MNG_ERR_MISSING_STREAM);

	if (in_ctx->get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (in_ctx->is_registered())
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}

	if (!srcBuffer->buffer_length())
	{
		return flags::AIO_MNG_ERR_EMPTY_BUFFER;
	}

	if (readContextList.size() > maximumAllowedReadContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	in_ctx->_setup_context(in_ctx->srcBuffer->buffer_length(), in_bytes_on_each);

	MBASE_TS_LOCK(readQueueMutex)
		readContextList.push_back(in_ctx);
	MBASE_TS_UNLOCK(readQueueMutex);

	return flags::AIO_MNG_SUCCESS;
}

MBASE_INLINE async_io_manager::flags async_io_manager::enqueue_read_context(async_io_context* in_ctx, size_type in_bytes_to_read, size_type in_bytes_on_each) noexcept 
{
	MBASE_NULL_CHECK_RETURN_VAL(in_ctx, flags::AIO_MNG_ERR_MISSING_CONTEXT);

	if (!in_bytes_to_read)
	{
		return flags::AIO_MNG_ERR_EMPTY_BUFFER;
	}

	if (in_ctx->get_io_direction() != async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (in_ctx->is_registered())
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}

	if (readContextList.size() > maximumAllowedReadContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	flags err = flags::AIO_MNG_SUCCESS;

	if (in_ctx->srcBuffer)
	{
		err = flags::AIO_MNG_WARN_INTERNAL_STREAM_ALTERED;
		if (in_ctx->srcBuffer->buffer_length() == in_bytes_to_read)
		{
			in_ctx->srcBuffer->zero_out_buffer();
		}
		else
		{
			if (in_ctx->isBufferInternal)
			{
				return flags::AIO_MNG_ERR_INVALID_INTERNAL_STREAM_SIZE;
			}
			//deep_char_stream* dcsTemp = static_cast<deep_char_stream*>(in_ctx->srcBuffer);
			delete in_ctx->srcBuffer;
			in_ctx->srcBuffer = nullptr;
			in_ctx->srcBuffer = new deep_char_stream(in_bytes_to_read);
			in_ctx->isBufferInternal = false;
		}
	}
	else
	{
		in_ctx->srcBuffer = new deep_char_stream(in_bytes_to_read);
		in_ctx->isBufferInternal = false;
	}

	in_ctx->_setup_context(in_ctx->srcBuffer->buffer_length(), in_bytes_on_each);

	MBASE_TS_LOCK(readQueueMutex);
		readContextList.push_back(in_ctx);
	MBASE_TS_UNLOCK(readQueueMutex);

	return err;
}

MBASE_INLINE GENERIC async_io_manager::flush_read_contexts() noexcept 
{
	MBASE_TS_LOCK(readQueueMutex);
	for (mbase::list<async_io_context*>::iterator It = readContextList.begin(); It != readContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		iCtx->flush_context();
	}
	MBASE_TS_UNLOCK(readQueueMutex);
}

MBASE_INLINE GENERIC async_io_manager::flush_write_contexts() noexcept 
{
	MBASE_TS_LOCK(writeQueueMutex)
		for (mbase::list<async_io_context*>::iterator It = writeContextList.begin(); It != writeContextList.end(); It++)
		{
			async_io_context* iCtx = *It;
			iCtx->flush_context();
		}
	MBASE_TS_UNLOCK(writeQueueMutex)
}

MBASE_INLINE GENERIC async_io_manager::clear_read_contexts() noexcept 
{
	MBASE_TS_LOCK(readQueueMutex);
	for (mbase::list<async_io_context*>::iterator It = readContextList.begin(); It != readContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		async_io_context::flags st = iCtx->ais;
		iCtx->flush_context();
		iCtx->isActive = false;
		if (st != async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
		{
			iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_ABANDONED;
		}

		It = readContextList.erase(It);
	}
	MBASE_TS_UNLOCK(readQueueMutex);
}

MBASE_INLINE GENERIC async_io_manager::clear_write_contexts() noexcept 
{
	MBASE_TS_LOCK(writeQueueMutex)
		for (mbase::list<async_io_context*>::iterator It = writeContextList.begin(); It != writeContextList.end(); It++)
		{
			async_io_context* iCtx = *It;
			async_io_context::flags st = iCtx->ais;
			iCtx->flush_context();
			iCtx->isActive = false;
			if (st != async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
			{
				iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_ABANDONED;
			}
			It = writeContextList.erase(It);
		}
	MBASE_TS_UNLOCK(writeQueueMutex)
}

MBASE_INLINE GENERIC async_io_manager::run_write_contexts() noexcept 
{
	MBASE_TS_LOCK(writeQueueMutex)

		for (mbase::list<async_io_context*>::iterator It = writeContextList.begin(); It != writeContextList.end(); It++)
		{
			async_io_context* iCtx = *It;
			if (!iCtx->isActive)
			{
				continue;
			}
			iCtx->isActive = true;
			iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_OPERATING;

			size_type bytesToWrite = iCtx->bytesOnEachIteration;
			iCtx->hopCounter++;
			if (iCtx->hopCounter == iCtx->calculatedHop)
			{
				iCtx->isActive = false;
				iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_FINISHED;
				bytesToWrite += iCtx->lastFraction; // if the result of (targetBytes % calculatedHop) is not zero, add that value to the end
			}

			io_base* rawHandle = iCtx->GetIoHandle();
			char_stream* mStream = iCtx->get_character_stream();
			size_type writtenBytes = rawHandle->write_data(*mStream, bytesToWrite);
			iCtx->bytesTransferred += writtenBytes;
			iCtx->totalBytesTransferred += iCtx->bytesTransferred;
			if (iCtx->ais == async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
			{
				iCtx->flush_context();
				iCtx->isActive = false;
				It = writeContextList.erase(It);
			}
			else
			{
				if (!writtenBytes)
				{
					iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_FAILED; // MEANS, ERROR OCCURED
					iCtx->halt_context();
					It = writeContextList.erase(It);
				}
				else
				{
					iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_IDLE;
				}
			}
		}

	MBASE_TS_UNLOCK(writeQueueMutex)
}

MBASE_INLINE GENERIC async_io_manager::run_read_contexts() noexcept 
{
	MBASE_TS_LOCK(readQueueMutex);

	for (mbase::list<async_io_context*>::iterator It = readContextList.begin(); It != readContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		if (!iCtx->isActive)
		{
			continue;
		}

		iCtx->isActive = true;
		iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_OPERATING;
		size_type bytesToRead = iCtx->bytesOnEachIteration;
		iCtx->hopCounter++;
		if (iCtx->hopCounter == iCtx->calculatedHop)
		{
			iCtx->isActive = false;
			iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_FINISHED;
			bytesToRead += iCtx->lastFraction; // if the result of (targetBytes % calculatedHop) is not zero, add that value to the end
		}

		io_base* rawHandle = iCtx->GetIoHandle();
		char_stream* mStream = iCtx->get_character_stream();
		size_type readBytes = rawHandle->read_data(*mStream, bytesToRead);
		iCtx->bytesTransferred += readBytes;
		iCtx->totalBytesTransferred += iCtx->bytesTransferred;
		if (iCtx->ais == async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
		{
			iCtx->flush_context();
			iCtx->isActive = false;
			It = readContextList.erase(It);
		}
		else
		{
			if (!readBytes)
			{
				iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_FAILED;
				iCtx->halt_context();
				It = writeContextList.erase(It);
			}
			else
			{
				iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_IDLE;
			}
		}
	}

	MBASE_TS_UNLOCK(readQueueMutex);
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
	MBASE_TS_LOCK(writeQueueMutex)

		for (mbase::list<async_io_context*>::iterator It = writeContextList.begin(); It != writeContextList.end(); It++)
		{
			async_io_context* iCtx = *It;
			iCtx->halt_context();
		}

	MBASE_TS_UNLOCK(writeQueueMutex)
}

MBASE_INLINE GENERIC async_io_manager::halt_read_contexts() noexcept 
{
	MBASE_TS_LOCK(readQueueMutex);

	for (mbase::list<async_io_context*>::iterator It = readContextList.begin(); It != readContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		iCtx->halt_context();
	}

	MBASE_TS_UNLOCK(readQueueMutex);
}

MBASE_INLINE GENERIC async_io_manager::set_allowed_write_context(U32 in_max_write_context) noexcept
{
	maximumAllowedWriteContext = in_max_write_context;
}

MBASE_INLINE GENERIC async_io_manager::set_allowed_read_context(U32 in_max_read_context) noexcept
{
	maximumAllowedReadContext = in_max_read_context;
}


MBASE_END

#endif // MBASE_ASYNC_IO_H