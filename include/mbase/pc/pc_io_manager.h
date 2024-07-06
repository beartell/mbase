#ifndef MBASE_IO_MANAGER
#define MBASE_IO_MANAGER

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/io_base.h>
#include <mbase/list.h>
#include <mbase/vector.h>
#include <mbase/pc/pc_stream_manager.h>
#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mbase/framework/handler_base.h>
#include <mbase/framework/async_io.h>
#include <mbase/framework/io_context.h>

MBASE_BEGIN

#define MBASE_IOMNG_RETURN_UNINITIALIZED \
if(!is_initialized())\
{\
	return flags::IO_MNG_ERR_NOT_INITIALIZED;\
}

#define MBASE_IOMNG_USUAL_CHECK \
if(!is_initialized())\
{\
	return flags::IO_MNG_ERR_NOT_INITIALIZED;\
}\
if(!in_handler.is_registered())\
{\
	return flags::IO_MNG_ERR_NOT_REGISTERED;\
}\
if (in_handler.get_io_context()->is_registered())\
{\
	return flags::IO_MNG_ERR_HANDLE_IS_ACTIVE;\
}

static const U32 gIoManagerMaxReadsDefault = 32;
static const U32 gIoManagerMaxWritesDefault = 32;

class PcIoHandler;
class PcIoManager;

class PcIoManager : public mbase::singleton<PcIoManager> {
public:
	using size_type = SIZE_T;
	using io_participants = mbase::list<PcIoHandler*>;

	enum class flags : U8 {
		IO_MNG_SUCCESS,
		IO_MNG_ERR_MISSING_DATA,
		IO_MNG_ERR_INVALID_SIZE,
		IO_MNG_ERR_STREAMS_ARE_FULL,
		IO_MNG_ERR_NOT_INITIALIZED,
		IO_MNG_ERR_ALREADY_REGISTERED,
		IO_MNG_ERR_NOT_REGISTERED,
		IO_MNG_ERR_HANDLE_IS_ACTIVE,
		IO_MNG_ERR_INVALID_DIRECTION,
		IO_MNG_DIRECTION_READ,
		IO_MNG_DIRECTION_WRITE
	};

	PcIoManager() = default;
	~PcIoManager() = default;

	const mbase::async_io_manager* get_aio_mng() const;
	const io_participants* get_io_participants() const;
	bool is_initialized() const;

	bool initialize(U32 in_max_write_count = gIoManagerMaxWritesDefault, U32 in_max_read_count = gIoManagerMaxReadsDefault);
	flags register_io_base(PcIoHandler& out_io_handler);
	flags unregister_io_handler(PcIoHandler& in_io_handler);
	flags io_mng_write(PcIoHandler& in_handler);
	flags io_mng_write(PcIoHandler& in_handler, mbase::char_stream& in_data);
	flags io_mng_read(PcIoHandler& in_handler);
	flags io_mng_read(PcIoHandler& in_handler, size_type in_size);
	flags update();
	flags halt();
	flags update_writes();
	flags update_reads();

private:
	io_participants mIoParticipants;
	async_io_manager mAioMng;
	PcStreamManager mReaderStreams;
	U32 mMaxWriterCount;
	U32 mMaxReaderCount;
	U32 mActiveWriterCount;
	U32 mActiveReaderCount;
	bool mIsInitialized = false;
};

class PcIoHandler : public handler_base {
public:
	using io_handle = io_base*;
	using size_type = SIZE_T;

	enum class flags : U8 {
		IO_HANDLER_DIRECTION_INPUT,
		IO_HANDLER_DIRECTION_OUTPUT
	};

	PcIoHandler(io_base& in_io_base, async_io_context::flags io_direction) : mIoContext(in_io_base, io_direction), mSelfIter(NULL), mIsRegistered(false), mStreamHandle(-1) {}

	bool is_registered() const { return mIsRegistered; }
	const async_io_context* get_io_context() const { return &mIoContext; }

	virtual GENERIC on_write(char_stream& out_data, size_type out_size) {}
	virtual GENERIC on_read(const char_stream& out_data, size_type out_size) {}
	virtual GENERIC on_finish(char_stream& out_data, size_type out_total_size) {}

private:

	friend class PcIoManager;
	using io_handler_element = typename PcIoManager::io_participants::iterator;

	async_io_context mIoContext;
	PcStreamManager::stream_handle mStreamHandle; // -1 IF NOT EXISTS
	io_handler_element mSelfIter;
	bool mIsRegistered;
};

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
	if(is_initialized())
	{
		return true;
	}

	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Initializing io manager.");

	mMaxWriterCount = in_max_write_count;
	mMaxReaderCount = in_max_read_count;

	if(!mMaxWriterCount)
	{
		mMaxWriterCount = gIoManagerMaxWritesDefault;
	}

	if(!mMaxReaderCount)
	{
		mMaxReaderCount = gIoManagerMaxReadsDefault;
	}

	mAioMng.set_allowed_write_context(mMaxWriterCount);
	mAioMng.set_allowed_read_context(mMaxReaderCount);

	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Allowed max write context count set: %d.", mMaxWriterCount);
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Allowed max read context count set: %d.", mMaxReaderCount);

	mReaderStreams.initialize(mMaxReaderCount);
	pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Reader streams for Io manager is initialized.", mMaxReaderCount);
	
	return true;
}

PcIoManager::flags PcIoManager::register_io_base(PcIoHandler& out_io_handler)
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;
	PcDiagnostics& pcDiag = PcDiagnostics::get_instance();

	if(out_io_handler.is_registered())
	{
		pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_ERROR, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Failed to register Io handler(code: %d). ", flags::IO_MNG_ERR_ALREADY_REGISTERED);
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

	if(!in_io_handler.is_registered())
	{
		pcDiag.log(mbase::PcDiagnostics::flags::LOGTYPE_INFO, mbase::PcDiagnostics::flags::LOGIMPORTANCE_MID, "Attempted to unregister an already unregistered io handle.");
		return flags::IO_MNG_SUCCESS;
	}

	if(in_io_handler.get_io_context()->is_registered())
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

	if(!in_data)
	{
		return flags::IO_MNG_ERR_MISSING_DATA;
	}

	if(!in_data->buffer_length())
	{
		return flags::IO_MNG_ERR_INVALID_SIZE;
	}

	SIZE_T bytesOnEachUpdate = in_data->buffer_length();
	if (bytesOnEachUpdate >= gDefaultStreamSize)
	{
		bytesOnEachUpdate = gDefaultStreamSize;
	}

	if(mAioMng.enqueue_write_context(in_handler.mIoContext, bytesOnEachUpdate) != async_io_manager::flags::AIO_MNG_SUCCESS)
	{
		return flags::IO_MNG_ERR_STREAMS_ARE_FULL;
	}
	in_handler.mIoContext.get_character_stream()->set_cursor_front();
	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::io_mng_write(PcIoHandler& in_handler, mbase::char_stream& in_data)
{
	MBASE_IOMNG_USUAL_CHECK;

	if(!in_data.buffer_length())
	{
		return flags::IO_MNG_ERR_MISSING_DATA;
	}

	if(in_handler.get_io_context()->get_io_direction() == mbase::async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
	{
		return flags::IO_MNG_ERR_INVALID_DIRECTION;
	}

	char_stream* oldOutStream = in_handler.mIoContext.get_io_handle()->get_os();

	in_handler.mIoContext.get_io_handle()->associate_os(in_data);
	SIZE_T bytesOnEachUpdate = in_data.buffer_length();
	if(bytesOnEachUpdate >= gDefaultStreamSize)
	{
		bytesOnEachUpdate = gDefaultStreamSize;
	}

	if(mAioMng.enqueue_write_context(in_handler.mIoContext, bytesOnEachUpdate) != mbase::async_io_manager::flags::AIO_MNG_SUCCESS)
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

	if(!in_data->buffer_length())
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
		if(in_size < mReaderStreams.get_stream_size())
		{
			
			if(mReaderStreams.acquire_stream(tempStreamHandle) != PcStreamManager::flags::STREAM_MNG_SUCCESS)
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
				if(mAioMng.enqueue_read_context(in_handler.mIoContext, in_size, in_size) != async_io_manager::flags::AIO_MNG_SUCCESS)
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
			if(mAioMng.enqueue_read_context(in_handler.mIoContext, in_size, mReaderStreams.get_stream_size()) != async_io_manager::flags::AIO_MNG_SUCCESS)
			{
				return flags::IO_MNG_ERR_STREAMS_ARE_FULL;
			}
		}
	}

	else
	{
		SIZE_T bytesOnEachUpdate = in_data->buffer_length();
		if(bytesOnEachUpdate < in_size)
		{
			return flags::IO_MNG_ERR_INVALID_SIZE;
		}

		if(bytesOnEachUpdate >= gDefaultStreamSize)
		{
			bytesOnEachUpdate = gDefaultStreamSize;
		}

		if(mAioMng.enqueue_read_context(in_handler.mIoContext, bytesOnEachUpdate) != async_io_manager::flags::AIO_MNG_SUCCESS)
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

	for(io_participants::iterator It = mIoParticipants.begin(); It != mIoParticipants.end(); It++)
	{
		PcIoHandler* tmpHandler = *It;
		async_io_context& tmpContext = tmpHandler->mIoContext;
		
		if(!tmpContext.is_active())
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
			if(tmpHandler->mStreamHandle != -1)
			{
				// MEANS WE ARE USING READER STREAMS
				mReaderStreams.release_stream(tmpHandler->mStreamHandle);
				tmpHandler->mStreamHandle = -1;
				tmpHandler->mIoContext.set_stream(nullptr);
			}
		}
	}

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

		if(tmpContext.get_io_direction() == async_io_context::flags::ASYNC_CTX_DIRECTION_INPUT)
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

#endif // !MBASE_IO_MANAGER
