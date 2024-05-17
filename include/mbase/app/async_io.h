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
		AIO_MNG_ERR_MISSING_CONTEXT = 1,
		AIO_MNG_ERR_ALREADY_REGISTERED = 2,
		AIO_MNG_ERR_INVALID_PARAMS = 3,
		AIO_MNG_ERR_LIST_BLOATED = 4,
		AIO_MNG_ERR_MISSING_STREAM = 5,
		AIO_MNG_ERR_INVALID_IO_DIRECTION = 6,
		AIO_MNG_WARN_INTERNAL_STREAM_ALTERED = 7
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE async_io_manager(U32 in_max_write_context, U32 in_max_read_context) noexcept;
	MBASE_INLINE ~async_io_manager() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND("") MBASE_INLINE mbase::list<async_io_context*>* GetWriteContextList() noexcept;
	MBASE_ND("") MBASE_INLINE mbase::list<async_io_context*>* GetReadContextList() noexcept;
	MBASE_ND("") MBASE_INLINE U32 GetAllowedWriteContext() const noexcept;
	MBASE_ND("") MBASE_INLINE U32 GetAllowedReadContext() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE flags EnqueueWriteContext(async_io_context* in_ctx, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE flags EnqueueWriteContext(async_io_context* in_ctx, IBYTEBUFFER in_data, size_type in_bytes_to_write, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE flags EnqueueReadContext(async_io_context* in_ctx, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE flags EnqueueReadContext(async_io_context* in_ctx, size_type in_bytes_to_read, size_type in_bytes_on_each) noexcept;
	MBASE_INLINE GENERIC FlushReadContexts() noexcept;
	MBASE_INLINE GENERIC FlushWriteContexts() noexcept;
	MBASE_INLINE GENERIC ClearReadContexts() noexcept;
	MBASE_INLINE GENERIC ClearWriteContexts() noexcept;
	MBASE_INLINE GENERIC RunWriteContexts() noexcept;
	MBASE_INLINE GENERIC RunReadContexts() noexcept;
	MBASE_INLINE GENERIC RunBothContexts(bool in_writefirst = true) noexcept;
	MBASE_INLINE GENERIC HaltWriteContexts() noexcept;
	MBASE_INLINE GENERIC HaltReadContexts() noexcept;
	MBASE_INLINE GENERIC SetAllowedWriteContext(U32 in_max_write_context) noexcept;
	MBASE_INLINE GENERIC SetAllowedReadContext(U32 in_max_read_context) noexcept;
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
	ClearWriteContexts();
	ClearReadContexts();
}

MBASE_ND("") MBASE_INLINE mbase::list<async_io_context*>* async_io_manager::GetWriteContextList() noexcept
{
	return &writeContextList;
}

MBASE_ND("") MBASE_INLINE mbase::list<async_io_context*>* async_io_manager::GetReadContextList() noexcept
{
	return &readContextList;
}

MBASE_ND("") MBASE_INLINE U32 async_io_manager::GetAllowedWriteContext() const noexcept
{
	return maximumAllowedWriteContext;
}

MBASE_ND("") MBASE_INLINE U32 async_io_manager::GetAllowedReadContext() const noexcept
{
	return maximumAllowedReadContext;
}

MBASE_INLINE async_io_manager::flags async_io_manager::EnqueueWriteContext(async_io_context* in_ctx, size_type in_bytes_on_each) noexcept 
{
	MBASE_NULL_CHECK_RETURN_VAL(in_ctx, flags::AIO_MNG_ERR_MISSING_CONTEXT);
	if (in_ctx->GetIoDirection() != async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	char_stream* srcBuffer = in_ctx->GetCharacterStream();
	MBASE_NULL_CHECK_RETURN_VAL(srcBuffer, flags::AIO_MNG_ERR_MISSING_STREAM);

	if (writeContextList.size() > maximumAllowedWriteContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	if (in_ctx->ais == async_io_context::flags::ASYNC_CTX_STAT_IDLE || in_ctx->ais == async_io_context::flags::ASYNC_CTX_STAT_OPERATING)
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}

	in_ctx->ais = async_io_context::flags::ASYNC_CTX_STAT_IDLE;
	in_ctx->targetBytes = in_ctx->GetCharacterStream()->buffer_length();
	in_ctx->bytesOnEachIteration = in_bytes_on_each;
	in_ctx->calculatedHop = in_ctx->targetBytes / in_bytes_on_each;
	in_ctx->lastFraction = in_ctx->targetBytes % in_bytes_on_each;
	in_ctx->bytesTransferred = 0;

	MBASE_TS_LOCK(writeQueueMutex)
		writeContextList.push_back(in_ctx);
	MBASE_TS_UNLOCK(writeQueueMutex)

	return flags::AIO_MNG_SUCCESS;
}

MBASE_INLINE async_io_manager::flags async_io_manager::EnqueueWriteContext(async_io_context* in_ctx, IBYTEBUFFER in_data, size_type in_bytes_to_write, size_type in_bytes_on_each) noexcept 
{
	MBASE_NULL_CHECK_RETURN_VAL(in_ctx, flags::AIO_MNG_ERR_MISSING_CONTEXT);
	if (in_ctx->GetIoDirection() != async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (writeContextList.size() > maximumAllowedWriteContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	if (in_ctx->ais == async_io_context::flags::ASYNC_CTX_STAT_IDLE || in_ctx->ais == async_io_context::flags::ASYNC_CTX_STAT_OPERATING)
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
			//deep_char_stream* dcsTemp = static_cast<deep_char_stream*>(in_ctx->srcBuffer);
			delete in_ctx->srcBuffer;
			in_ctx->srcBuffer = nullptr;
			in_ctx->srcBuffer = new deep_char_stream(in_data, in_bytes_to_write);
		}
	}
	else
	{
		in_ctx->srcBuffer = new deep_char_stream(in_data, in_bytes_to_write);
	}

	in_ctx->isBufferInternal = false;

	in_ctx->ais = async_io_context::flags::ASYNC_CTX_STAT_IDLE;
	in_ctx->targetBytes = in_ctx->GetCharacterStream()->buffer_length();
	in_ctx->bytesOnEachIteration = in_bytes_on_each;
	in_ctx->calculatedHop = in_ctx->targetBytes / in_bytes_on_each;
	in_ctx->lastFraction = in_ctx->targetBytes % in_bytes_on_each;
	in_ctx->bytesTransferred = 0;

	MBASE_TS_LOCK(writeQueueMutex)
		writeContextList.push_back(in_ctx);
	MBASE_TS_UNLOCK(writeQueueMutex)

	return err;
}

MBASE_INLINE async_io_manager::flags async_io_manager::EnqueueReadContext(async_io_context* in_ctx, size_type in_bytes_on_each) noexcept 
{
	MBASE_NULL_CHECK_RETURN_VAL(in_ctx, flags::AIO_MNG_ERR_MISSING_CONTEXT);

	if (in_ctx->GetIoDirection() != async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (in_ctx->ais == async_io_context::flags::ASYNC_CTX_STAT_IDLE || in_ctx->ais == async_io_context::flags::ASYNC_CTX_STAT_OPERATING)
	{
		return flags::AIO_MNG_ERR_ALREADY_REGISTERED;
	}

	char_stream* srcBuffer = in_ctx->GetCharacterStream();
	MBASE_NULL_CHECK_RETURN_VAL(srcBuffer, flags::AIO_MNG_ERR_MISSING_STREAM);

	if (readContextList.size() > maximumAllowedReadContext)
	{
		return flags::AIO_MNG_ERR_LIST_BLOATED;
	}

	in_ctx->ais = async_io_context::flags::ASYNC_CTX_STAT_IDLE;
	in_ctx->targetBytes = in_ctx->GetCharacterStream()->buffer_length();
	in_ctx->bytesOnEachIteration = in_bytes_on_each;
	in_ctx->calculatedHop = in_ctx->targetBytes / in_bytes_on_each;
	in_ctx->lastFraction = in_ctx->targetBytes % in_bytes_on_each;
	in_ctx->bytesTransferred = 0;

	MBASE_TS_LOCK(readQueueMutex)
		readContextList.push_back(in_ctx);
	MBASE_TS_UNLOCK(readQueueMutex);

	return flags::AIO_MNG_SUCCESS;
}

MBASE_INLINE async_io_manager::flags async_io_manager::EnqueueReadContext(async_io_context* in_ctx, size_type in_bytes_to_read, size_type in_bytes_on_each) noexcept 
{
	MBASE_NULL_CHECK_RETURN_VAL(in_ctx, flags::AIO_MNG_ERR_MISSING_CONTEXT);

	if (in_ctx->GetIoDirection() != async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::AIO_MNG_ERR_INVALID_IO_DIRECTION;
	}

	if (in_ctx->ais == async_io_context::flags::ASYNC_CTX_STAT_IDLE || in_ctx->ais == async_io_context::flags::ASYNC_CTX_STAT_OPERATING)
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
			//deep_char_stream* dcsTemp = static_cast<deep_char_stream*>(in_ctx->srcBuffer);
			delete in_ctx->srcBuffer;
			in_ctx->srcBuffer = nullptr;
			in_ctx->srcBuffer = new deep_char_stream(in_bytes_to_read);
		}
	}
	else
	{
		in_ctx->srcBuffer = new deep_char_stream(in_bytes_to_read);
	}

	in_ctx->isBufferInternal = false;

	in_ctx->ais = async_io_context::flags::ASYNC_CTX_STAT_IDLE;
	in_ctx->targetBytes = in_ctx->GetCharacterStream()->buffer_length();
	in_ctx->bytesOnEachIteration = in_bytes_on_each;
	in_ctx->calculatedHop = in_ctx->targetBytes / in_bytes_on_each;
	in_ctx->lastFraction = in_ctx->targetBytes % in_bytes_on_each;
	in_ctx->bytesTransferred = 0;

	MBASE_TS_LOCK(readQueueMutex);
		readContextList.push_back(in_ctx);
	MBASE_TS_UNLOCK(readQueueMutex);

	return err;
}

MBASE_INLINE GENERIC async_io_manager::FlushReadContexts() noexcept 
{
	MBASE_TS_LOCK(readQueueMutex);
	for (mbase::list<async_io_context*>::iterator It = readContextList.begin(); It != readContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		iCtx->FlushContext();
	}
	MBASE_TS_UNLOCK(readQueueMutex);
}

MBASE_INLINE GENERIC async_io_manager::FlushWriteContexts() noexcept 
{
	MBASE_TS_LOCK(writeQueueMutex)
		for (mbase::list<async_io_context*>::iterator It = writeContextList.begin(); It != writeContextList.end(); It++)
		{
			async_io_context* iCtx = *It;
			iCtx->FlushContext();
		}
	MBASE_TS_UNLOCK(writeQueueMutex)
}

MBASE_INLINE GENERIC async_io_manager::ClearReadContexts() noexcept 
{
	MBASE_TS_LOCK(readQueueMutex);
	for (mbase::list<async_io_context*>::iterator It = readContextList.begin(); It != readContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		async_io_context::flags st = iCtx->ais;
		iCtx->FlushContext();
		if (st != async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
		{
			iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_ABANDONED;
		}

		It = readContextList.erase(It);
	}
	MBASE_TS_UNLOCK(readQueueMutex);
}

MBASE_INLINE GENERIC async_io_manager::ClearWriteContexts() noexcept 
{
	MBASE_TS_LOCK(writeQueueMutex)
		for (mbase::list<async_io_context*>::iterator It = writeContextList.begin(); It != writeContextList.end(); It++)
		{
			async_io_context* iCtx = *It;
			async_io_context::flags st = iCtx->ais;
			iCtx->FlushContext();
			if (st != async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
			{
				iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_ABANDONED;
			}
			It = writeContextList.erase(It);
		}
	MBASE_TS_UNLOCK(writeQueueMutex)
}

MBASE_INLINE GENERIC async_io_manager::RunWriteContexts() noexcept 
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
			char_stream* mStream = iCtx->GetCharacterStream();
			size_type writtenBytes = rawHandle->write_data(*mStream, bytesToWrite);
			iCtx->bytesTransferred += writtenBytes;
			if (iCtx->ais == async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
			{
				iCtx->FlushContext();
				It = writeContextList.erase(It);
			}
			else
			{
				if (!writtenBytes)
				{
					// means something is wrong with the io handler
					// notify the user about it
					iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_FAILED;
					iCtx->HaltContext();
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

MBASE_INLINE GENERIC async_io_manager::RunReadContexts() noexcept 
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
		char_stream* mStream = iCtx->GetCharacterStream();
		size_type readBytes = rawHandle->read_data(*mStream, bytesToRead);
		iCtx->bytesTransferred += readBytes;
		if (iCtx->ais == async_io_context::flags::ASYNC_CTX_STAT_FINISHED)
		{
			iCtx->FlushContext();
			It = readContextList.erase(It);
		}
		else
		{
			if (!readBytes)
			{
				// something is wrong with the io handler
				// notify the user about it
				iCtx->ais = async_io_context::flags::ASYNC_CTX_STAT_FAILED;
				iCtx->HaltContext();
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

MBASE_INLINE GENERIC async_io_manager::RunBothContexts(bool in_writefirst) noexcept 
{
	if (in_writefirst)
	{
		RunWriteContexts();
		RunReadContexts();
	}
	else
	{
		RunReadContexts();
		RunWriteContexts();
	}
}

MBASE_INLINE GENERIC async_io_manager::HaltWriteContexts() noexcept 
{
	MBASE_TS_LOCK(writeQueueMutex)

		for (mbase::list<async_io_context*>::iterator It = writeContextList.begin(); It != writeContextList.end(); It++)
		{
			async_io_context* iCtx = *It;
			iCtx->HaltContext();
		}

	MBASE_TS_UNLOCK(writeQueueMutex)
}

MBASE_INLINE GENERIC async_io_manager::HaltReadContexts() noexcept 
{
	MBASE_TS_LOCK(readQueueMutex);

	for (mbase::list<async_io_context*>::iterator It = readContextList.begin(); It != readContextList.end(); It++)
	{
		async_io_context* iCtx = *It;
		iCtx->HaltContext();
	}

	MBASE_TS_UNLOCK(readQueueMutex);
}

MBASE_INLINE GENERIC async_io_manager::SetAllowedWriteContext(U32 in_max_write_context) noexcept
{
	maximumAllowedWriteContext = in_max_write_context;
}

MBASE_INLINE GENERIC async_io_manager::SetAllowedReadContext(U32 in_max_read_context) noexcept
{
	maximumAllowedReadContext = in_max_read_context;
}


MBASE_END

#endif // MBASE_ASYNC_IO_H