#include <mbase/pc/pc_io_manager.h>
#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_diagnostics.h>

MBASE_BEGIN

const mbase::async_io_manager* PcIoManager::get_aio_mng() const
{
	return &mAioMng;
}

typename const PcIoManager::io_participants* PcIoManager::get_io_participants() const
{
	return &mIoParticipants;
}

bool PcIoManager::is_initialized() const
{
	return mIsInitialized;
}

bool PcIoManager::initialize(U32 in_max_write_count, U32 in_max_read_count)
{
	if (is_initialized())
	{
		return true;
	}

	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Initializing io manager.");

	mMaxWriterCount = in_max_write_count;
	mMaxReaderCount = in_max_read_count;

	if (!mMaxWriterCount)
	{
		mMaxWriterCount = gIoManagerMaxWritesDefault;
	}

	if (!mMaxReaderCount)
	{
		mMaxReaderCount = gIoManagerMaxReadsDefault;
	}

	mAioMng.set_allowed_write_context(mMaxWriterCount);
	mAioMng.set_allowed_read_context(mMaxReaderCount);

	//pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Allowed max write context count set: %d.", mMaxWriterCount);
	//pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Allowed max read context count set: %d.", mMaxReaderCount);

	mReaderStreams.initialize(mMaxReaderCount);
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Reader streams for Io manager is initialized.");

	return true;
}

PcIoManager::flags PcIoManager::register_io_base(PcIoHandler& out_io_handler)
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;
	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();

	if (out_io_handler.is_registered())
	{
		//pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_ERROR, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Failed to register Io handler(code: %d). ", flags::IO_MNG_ERR_ALREADY_REGISTERED);
		return flags::IO_MNG_ERR_ALREADY_REGISTERED;
	}

	mIoParticipants.push_back(&out_io_handler);
	out_io_handler.mSelfIter = mIoParticipants.end_node();
	out_io_handler.mIsRegistered = true;

	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Registered new Io handler.");

	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::unregister_io_handler(PcIoHandler& in_io_handler)
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;
	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();

	if (!in_io_handler.is_registered())
	{
		pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Attempted to unregister an already unregistered io handle.");
		return flags::IO_MNG_SUCCESS;
	}

	if (in_io_handler.get_io_context()->is_registered())
	{
		// MEANS, WE ARE ABANDONING AN IO OPERATION
		// TODO: NOTIFY THE USER
	}

	in_io_handler.mIsRegistered = false;
	mIoParticipants.erase(in_io_handler.mSelfIter);
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Io handle is unregistered.");

	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::io_mng_write(PcIoHandler& in_handler)
{
	MBASE_IOMNG_USUAL_CHECK;

	mbase::char_stream* in_data = in_handler.mIoContext.get_io_handle()->get_os();
	if (in_handler.get_io_context()->get_io_direction() == mbase::async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::IO_MNG_ERR_INVALID_DIRECTION;
	}

	if (!in_data)
	{
		return flags::IO_MNG_ERR_MISSING_DATA;
	}

	if (!in_data->buffer_length())
	{
		return flags::IO_MNG_ERR_INVALID_SIZE;
	}

	SIZE_T bytesOnEachUpdate = in_data->buffer_length();
	if (bytesOnEachUpdate >= gDefaultStreamSize)
	{
		bytesOnEachUpdate = gDefaultStreamSize;
	}

	if (mAioMng.enqueue_write_context(in_handler.mIoContext, bytesOnEachUpdate) != async_io_manager::flags::AIO_MNG_SUCCESS)
	{
		return flags::IO_MNG_ERR_STREAMS_ARE_FULL;
	}
	in_handler.mIoContext.get_character_stream()->set_cursor_front();
	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::io_mng_write(PcIoHandler& in_handler, mbase::char_stream& in_data)
{
	MBASE_IOMNG_USUAL_CHECK;

	if (!in_data.buffer_length())
	{
		return flags::IO_MNG_ERR_MISSING_DATA;
	}

	if (in_handler.get_io_context()->get_io_direction() == mbase::async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::IO_MNG_ERR_INVALID_DIRECTION;
	}

	char_stream* oldOutStream = in_handler.mIoContext.get_io_handle()->get_os();

	in_handler.mIoContext.get_io_handle()->associate_os(in_data);
	SIZE_T bytesOnEachUpdate = in_data.buffer_length();
	if (bytesOnEachUpdate >= gDefaultStreamSize)
	{
		bytesOnEachUpdate = gDefaultStreamSize;
	}

	if (mAioMng.enqueue_write_context(in_handler.mIoContext, bytesOnEachUpdate) != mbase::async_io_manager::flags::AIO_MNG_SUCCESS)
	{
		in_handler.mIoContext.get_io_handle()->associate_os(*oldOutStream);
		return flags::IO_MNG_ERR_STREAMS_ARE_FULL;
	}
	in_handler.mIoContext.get_character_stream()->set_cursor_front();
	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::io_mng_read(PcIoHandler& in_handler)
{
	MBASE_IOMNG_USUAL_CHECK;

	mbase::char_stream* in_data = in_handler.mIoContext.get_io_handle()->get_is();
	if (in_handler.get_io_context()->get_io_direction() == mbase::async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		return flags::IO_MNG_ERR_INVALID_DIRECTION;
	}

	if (!in_data)
	{
		return flags::IO_MNG_ERR_MISSING_DATA;
	}

	if (!in_data->buffer_length())
	{
		return flags::IO_MNG_ERR_INVALID_SIZE;
	}

	SIZE_T bytesOnEachUpdate = in_data->buffer_length();
	if (bytesOnEachUpdate >= gDefaultStreamSize)
	{
		bytesOnEachUpdate = gDefaultStreamSize;
	}

	if (mAioMng.enqueue_read_context(in_handler.mIoContext, bytesOnEachUpdate) != async_io_manager::flags::AIO_MNG_SUCCESS)
	{
		return flags::IO_MNG_ERR_STREAMS_ARE_FULL;
	}
	in_handler.mIoContext.get_character_stream()->set_cursor_front();
	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::io_mng_read(PcIoHandler& in_handler, size_type in_size)
{
	MBASE_IOMNG_USUAL_CHECK;

	mbase::char_stream* in_data = in_handler.mIoContext.get_io_handle()->get_is();
	PcStreamManager::stream_handle tempStreamHandle;
	if (in_handler.get_io_context()->get_io_direction() == mbase::async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
	{
		return flags::IO_MNG_ERR_INVALID_DIRECTION;
	}
	if (!in_data)
	{
		// BRINGING A READER STREAM
		if (in_size < mReaderStreams.get_stream_size())
		{

			if (mReaderStreams.acquire_stream(tempStreamHandle) != PcStreamManager::flags::STREAM_MNG_SUCCESS)
			{
				if (mAioMng.enqueue_read_context(in_handler.mIoContext, in_size, in_size) != async_io_manager::flags::AIO_MNG_SUCCESS)
				{
					return flags::IO_MNG_ERR_STREAMS_ARE_FULL;
				}
			}
			else
			{
				mbase::char_stream* outStream = nullptr;
				mReaderStreams.get_stream_by_handle(tempStreamHandle, outStream); // 100% success
				in_handler.mStreamHandle = tempStreamHandle;
				in_handler.mIoContext.set_stream(outStream);
				if (mAioMng.enqueue_read_context(in_handler.mIoContext, in_size, in_size) != async_io_manager::flags::AIO_MNG_SUCCESS)
				{
					mReaderStreams.release_stream(in_handler.mStreamHandle);
					in_handler.mStreamHandle = -1;
					in_handler.mIoContext.set_stream(nullptr);
					return flags::IO_MNG_ERR_STREAMS_ARE_FULL;
				}
			}
		}
		else
		{
			if (mAioMng.enqueue_read_context(in_handler.mIoContext, in_size, mReaderStreams.get_stream_size()) != async_io_manager::flags::AIO_MNG_SUCCESS)
			{
				return flags::IO_MNG_ERR_STREAMS_ARE_FULL;
			}
		}
	}

	else
	{
		SIZE_T bytesOnEachUpdate = in_data->buffer_length();
		if (bytesOnEachUpdate < in_size)
		{
			return flags::IO_MNG_ERR_INVALID_SIZE;
		}

		if (bytesOnEachUpdate >= gDefaultStreamSize)
		{
			bytesOnEachUpdate = gDefaultStreamSize;
		}

		if (mAioMng.enqueue_read_context(in_handler.mIoContext, bytesOnEachUpdate) != async_io_manager::flags::AIO_MNG_SUCCESS)
		{
			return flags::IO_MNG_ERR_STREAMS_ARE_FULL;
		}
	}
	in_handler.mIoContext.get_character_stream()->set_cursor_front();
	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::update()
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;

	mAioMng.run_both_contexts();

	for (io_participants::iterator It = mIoParticipants.begin(); It != mIoParticipants.end(); It++)
	{
		PcIoHandler* tmpHandler = *It;
		async_io_context& tmpContext = tmpHandler->mIoContext;

		if (!tmpContext.is_active())
		{
			continue;
		}

		switch (tmpContext.get_io_direction())
		{
		case async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT:
			tmpHandler->on_read(*tmpContext.get_character_stream(), tmpContext.get_bytes_transferred_last_iteration());
			break;
		case async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT:
			tmpHandler->on_write(*tmpContext.get_character_stream(), tmpContext.get_bytes_transferred_last_iteration());
			break;
		}

		if (tmpContext.get_io_status() == async_io_context::flags::ASYNC_CTX_STAT_FINISHED || tmpContext.get_io_status() == async_io_context::flags::ASYNC_CTX_STAT_ABANDONED)
		{
			tmpHandler->on_finish(*tmpContext.get_character_stream(), tmpContext.get_total_transferred_bytes());
			if (tmpHandler->mStreamHandle != -1)
			{
				// MEANS WE ARE USING READER STREAMS
				mReaderStreams.release_stream(tmpHandler->mStreamHandle);
				tmpHandler->mStreamHandle = -1;
				tmpHandler->mIoContext.set_stream(nullptr);
			}
		}
	}

	//mbase::PcProgram::get_instance().get_event_manager()->dispatch_event("root_path_updated", nullptr);


	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::update_writes()
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;

	mAioMng.run_write_contexts();
	for (io_participants::iterator It = mIoParticipants.begin(); It != mIoParticipants.end(); It++)
	{
		PcIoHandler* tmpHandler = *It;
		async_io_context& tmpContext = tmpHandler->mIoContext;

		if (!tmpContext.is_active())
		{
			continue;
		}

		if (tmpContext.get_io_direction() == async_io_context::flags::ASYNC_CTX_DIRECTION_OUTPUT)
		{
			tmpHandler->on_write(*tmpContext.get_character_stream(), tmpContext.get_bytes_transferred_last_iteration());
			if (tmpContext.get_io_status() == async_io_context::flags::ASYNC_CTX_STAT_FINISHED || tmpContext.get_io_status() == async_io_context::flags::ASYNC_CTX_STAT_ABANDONED)
			{
				tmpHandler->on_finish(*tmpContext.get_character_stream(), tmpContext.get_total_transferred_bytes());
			}
		}
	}

	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::update_reads()
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;

	mAioMng.run_read_contexts();

	for (io_participants::iterator It = mIoParticipants.begin(); It != mIoParticipants.end(); It++)
	{
		PcIoHandler* tmpHandler = *It;
		async_io_context& tmpContext = tmpHandler->mIoContext;

		if (!tmpContext.is_active())
		{
			continue;
		}

		if (tmpContext.get_io_direction() == async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
		{
			tmpHandler->on_read(*tmpContext.get_character_stream(), tmpContext.get_bytes_transferred_last_iteration());
			if (tmpContext.get_io_status() == async_io_context::flags::ASYNC_CTX_STAT_FINISHED || tmpContext.get_io_status() == async_io_context::flags::ASYNC_CTX_STAT_ABANDONED)
			{
				tmpHandler->on_finish(*tmpContext.get_character_stream(), tmpContext.get_total_transferred_bytes());
				if (tmpHandler->mStreamHandle != -1)
				{
					// MEANS WE ARE USING READER STREAMS
					mReaderStreams.release_stream(tmpHandler->mStreamHandle);
					tmpHandler->mStreamHandle = -1;
					tmpHandler->mIoContext.set_stream(nullptr);
				}
			}
		}
	}

	return flags::IO_MNG_SUCCESS;
}



MBASE_END