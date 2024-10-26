#include <mbase/pc/pc_io_manager.h>
#include <mbase/pc/pc_program.h>
#include <mbase/pc/pc_diagnostics.h>

MBASE_BEGIN

#define MBASE_IO_HANDLER_USUAL_CHECK() \
if(!this->is_registered())\
{\
	return flags::IO_HANDLER_ERR_UNREGISTERED_HANDLER;\
}\
if(is_sync)\
{\
	while(this->is_processing()){}\
}\
else\
{\
	if (this->is_processing())\
	{\
		return flags::IO_HANDLER_ERR_IOMNG_PROCESSING_STREAM;\
	}\
}

PcIoManager::~PcIoManager() 
{
	if(is_initialized())
	{
		mbase::lock_guard ioMutex(mIoMutex);
		mbase::lock_guard rgrMutex(mRegistryMutex);

		mIoParticipants.clear();
		for(registered_handlers::iterator It = mRegisteredHandlers.begin(); It != mRegisteredHandlers.end(); ++It)
		{
			PcIoHandler* mIoHandler = *It;
			mIoHandler->mIsRegistered = false;
			mIoHandler->mIsProcessing = false;
			mIoHandler->mOvp = { 0 };
			mIoHandler->mIocpKey = 0;
			
			if(mIoHandler->mPolledStreamHandle != MBASE_INVALID_STREAM_HANDLE)
			{
				mStreamManager.release_stream(mIoHandler->mPolledStreamHandle);
				mIoHandler->mPolledStreamHandle = MBASE_INVALID_STREAM_HANDLE;
				mIoHandler->mProcessorStream = NULL;
			}
			mIoHandler->mSelfIter = NULL;
			mIoHandler->on_unregistered();
		}
	}
}

const typename PcIoManager::io_participants* PcIoManager::get_io_participants() const
{
	return &mIoParticipants;
}

bool PcIoManager::is_initialized() const
{
	return mIsInitialized;
}

PcIoManager::flags PcIoManager::initialize(U32 in_max_write_count, U32 in_max_read_count)
{
	if(is_initialized())
	{
		return flags::IO_MNG_SUCCESS;
	}

	mSyncHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(mSyncHandle == NULL)
	{
		return flags::IO_MNG_ERR_NOT_INITIALIZED; // not the correct flags, return here later
	}
	mStreamManager.initialize(in_max_write_count + in_max_read_count);
	mIsInitialized = true;
	return flags::IO_MNG_SUCCESS;
}

PcStreamManager* PcIoManager::get_stream_manager()
{
	return &mStreamManager;
}

typename PcIoManager::_sync_handle PcIoManager::_get_sync_handle()
{
	return mSyncHandle;
}

PcIoManager::flags PcIoManager::register_handler(const mbase::wstring& in_filename, PcIoHandler& out_handler, bool in_stream_polled)
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;

	if(out_handler.is_processing())
	{
		return flags::IO_MNG_ERR_HANDLE_IS_BEING_PROCESSED;
	}

	if(out_handler.is_registered())
	{
		return flags::IO_MNG_ERR_ALREADY_REGISTERED;
	}

	out_handler.mIoBase.open_file(in_filename, mbase::io_file::access_mode::RW_ACCESS, mbase::io_file::disposition::OPEN, true);
	  
	if(!out_handler.mIoBase.is_file_open())
	{
		return flags::IO_MNG_ERR_UNABLE_OPEN_FILE;
	}

	out_handler.mIsRegistered = true;
	out_handler.mIsProcessing = false;

	if(in_stream_polled)
	{
		PcStreamManager::stream_handle streamHandle = MBASE_INVALID_STREAM_HANDLE;
		if(mStreamManager.acquire_stream(streamHandle, out_handler.mProcessorStream) != PcStreamManager::flags::STREAM_MNG_SUCCESS)
		{
			return flags::IO_MNG_WARN_FAILED_TO_ASSIGN_STREAM;
		}
		out_handler.mPolledStreamHandle = streamHandle;
	}

	// SELF-NOTE: The reason I don't use lockguard here is because
	// I don't want the on_registered function to block the IO LOOP

	mRegistryMutex.acquire();
	mRegisteredHandlers.push_back(&out_handler);
	out_handler.mSelfIter = mRegisteredHandlers.end_node();
	mRegistryMutex.release();

	out_handler.on_registered();

	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::unregister_handler(PcIoHandler& in_handler)
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;

	if(in_handler.is_processing())
	{
		return flags::IO_MNG_ERR_HANDLE_IS_BEING_PROCESSED;
	}

	if(!in_handler.is_registered())
	{
		return flags::IO_MNG_SUCCESS;
	}

	mRegistryMutex.acquire();

	if (in_handler.mPolledStreamHandle != MBASE_INVALID_STREAM_HANDLE)
	{
		mStreamManager.release_stream(in_handler.mPolledStreamHandle);
		in_handler.mPolledStreamHandle = MBASE_INVALID_STREAM_HANDLE;
		in_handler.mProcessorStream = NULL;
	}

	mRegisteredHandlers.erase(in_handler.mSelfIter);
	in_handler.mSelfIter = NULL;
	in_handler.mIsRegistered = false;
	in_handler.mIsProcessing = false;
	in_handler.mOvp = { 0 };
	in_handler.mIocpKey = 0;

	mRegistryMutex.release();

	in_handler.on_unregistered();

	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::add_handler(PcIoHandler& in_handler)
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;

	if(!in_handler.is_registered())
	{
		return flags::IO_MNG_ERR_UNREGISTERED_HANDLER;
	}

	if(!in_handler.is_open())
	{
		return flags::IO_MNG_ERR_FILE_IS_NOT_OPEN;
	}

	if(in_handler.is_processing())
	{
		return flags::IO_MNG_ERR_HANDLE_IS_BEING_PROCESSED;
	}

	if(in_handler.mIocpKey)
	{
		
	}
	else
	{
		mbase::io_file::os_file_handle mHandle = in_handler.get_io_handle()->get_raw_context().raw_handle;
		ULONG_PTR mCompKey = (ULONG_PTR)mHandle;

		if (CreateIoCompletionPort(mHandle, mSyncHandle, mCompKey, 0) == NULL)
		{
			return flags::IO_MNG_ERR_INVALID_RAW_HANDLE;
		}

		in_handler.mIocpKey = mCompKey;
	}
	mbase::lock_guard lg(mIoMutex);
	mIoParticipants.push_back(&in_handler);

	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::update()
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;
	DWORD bytesTransferred = 0;
	ULONG_PTR compKey = 0;
	LPOVERLAPPED ovpStruct = NULL;
	if(GetQueuedCompletionStatus(mSyncHandle, &bytesTransferred, &compKey, &ovpStruct, 0))
	{
		mbase::lock_guard rgrMutex(mRegistryMutex);
		ovpStruct->Offset += bytesTransferred;
		for(registered_handlers::iterator It = mRegisteredHandlers.begin(); It != mRegisteredHandlers.end(); ++It)
		{
			PcIoHandler* tmpHandler = *It;
			if(tmpHandler->mIocpKey == compKey)
			{
				tmpHandler->mProcessorStream->set_cursor_front();
				tmpHandler->mProcessorStream->advance(bytesTransferred);
				if(tmpHandler->get_io_direction() == PcIoHandler::direction::IO_HANDLER_DIRECTION_OUTPUT)
				{
					tmpHandler->on_write(tmpHandler->mProcessorStream->get_buffer(), bytesTransferred);
				}
				else
				{
					tmpHandler->on_read(tmpHandler->mProcessorStream->get_buffer(), bytesTransferred);
				}
			}
		}
	}
	return flags::IO_MNG_SUCCESS;
}

PcIoManager::flags PcIoManager::update_t()
{
	MBASE_IOMNG_RETURN_UNINITIALIZED;
	mbase::lock_guard lg(mIoMutex);
	for (io_participants::iterator It = mIoParticipants.begin(); It != mIoParticipants.end(); ++It)
	{
		PcIoHandler* ioh = *It;
		IBYTEBUFFER bytesToOperate = ioh->mProcessorStream->get_buffer();
		U32 bytesLength = ioh->mProcessorStream->get_pos();
		if (bytesLength)
		{
			if (ioh->get_io_direction() == PcIoHandler::direction::IO_HANDLER_DIRECTION_OUTPUT)
			{
				WriteFile(ioh->get_io_handle()->get_raw_context().raw_handle, bytesToOperate, bytesLength, NULL, &ioh->mOvp);
			}
			else
			{
				ReadFile(ioh->get_io_handle()->get_raw_context().raw_handle, bytesToOperate, bytesLength, NULL, &ioh->mOvp);
			}

			DWORD lastError = GetLastError();
			if (lastError != ERROR_IO_PENDING)
			{
				// THERE IS A PROBLEM HERE, handle this
			}
		}

		ioh->mIsProcessing = false;
	}
	mIoParticipants.clear();

	return flags::IO_MNG_SUCCESS;
}

PcIoHandler::PcIoHandler() :
	mIsRegistered(false),
	mIsProcessing(false),
	mProcessorStream(NULL),
	mIoBase(),
	mIoDirection(direction::IO_HANDLER_DIRECTION_OUTPUT),
	mOvp({0}),
	mIocpKey(0),
	mPolledStreamHandle(MBASE_INVALID_STREAM_HANDLE),
	mSelfIter(NULL)
{
}

PcIoHandler::~PcIoHandler()
{
	PcIoManager* ioMng = NULL;
	if(is_registered())
	{
		ioMng->unregister_handler(*this);
	}
}

bool PcIoHandler::is_open() const noexcept
{
	return mIoBase.is_file_open();
}

bool PcIoHandler::is_registered() const noexcept
{
	return mIsRegistered;
}

bool PcIoHandler::is_processing() const noexcept
{
	return mIsProcessing;
}

PcIoHandler::direction PcIoHandler::get_io_direction() const noexcept
{
	return mIoDirection;
}

typename PcIoHandler::io_handle_base* PcIoHandler::get_io_handle() noexcept
{
	return &mIoBase;
}

PcIoHandler::flags PcIoHandler::set_io_direction(direction in_direction, bool is_sync) // SELF-NOTE: THIS SETS THE CURSOR ON THE FRONT
{
	MBASE_IO_HANDLER_USUAL_CHECK();

	if(in_direction == mIoDirection)
	{
		return flags::IO_HANDLER_SUCCESS;
	}

	mIoDirection = in_direction;
	mProcessorStream->set_cursor_front();

	return flags::IO_HANDLER_SUCCESS;
} 

PcIoHandler::flags PcIoHandler::set_stream(mbase::char_stream& in_stream, bool is_sync)
{
	MBASE_IO_HANDLER_USUAL_CHECK();
	PcIoManager* ioMng = NULL;
	ioMng->get_stream_manager()->release_stream(mPolledStreamHandle);
	mPolledStreamHandle = MBASE_INVALID_STREAM_HANDLE;
	mProcessorStream = &in_stream;
	return flags::IO_HANDLER_SUCCESS;
}

PcIoHandler::flags PcIoHandler::write_buffer(CBYTEBUFFER in_data, size_type in_size, bool is_sync)
{
	MBASE_IO_HANDLER_USUAL_CHECK();
	if(mIoDirection != direction::IO_HANDLER_DIRECTION_OUTPUT)
	{
		return flags::IO_HANDLER_ERR_INVALID_DIRECTION;
	}
	mProcessorStream->put_buffern(in_data, in_size);
	return flags::IO_HANDLER_SUCCESS;
}

PcIoHandler::flags PcIoHandler::read_buffer(size_type in_size, bool is_sync)
{
	MBASE_IO_HANDLER_USUAL_CHECK();
	if(mIoDirection != direction::IO_HANDLER_DIRECTION_INPUT)
	{
		return flags::IO_HANDLER_ERR_INVALID_DIRECTION;
	}

	mProcessorStream->advance(in_size);
	return flags::IO_HANDLER_SUCCESS;
}

PcIoHandler::flags PcIoHandler::flush_stream(bool is_sync)
{
	MBASE_IO_HANDLER_USUAL_CHECK();
	mProcessorStream->set_cursor_front();
	return flags::IO_HANDLER_SUCCESS;
}

PcIoHandler::flags PcIoHandler::finish(bool is_sync)
{
	MBASE_IO_HANDLER_USUAL_CHECK();
	PcIoManager* ioMng = NULL;
	
	ioMng->add_handler(*this);
	mIsProcessing = true;
	return flags::IO_HANDLER_SUCCESS;
}

PcIoHandler::flags PcIoHandler::clear_file()
{
	bool is_sync = true;
	MBASE_IO_HANDLER_USUAL_CHECK();

	mOvp.Offset = 0;
	mIoBase.clear_file();

	return flags::IO_HANDLER_SUCCESS;
}

GENERIC PcIoHandler::on_registered()
{

}

GENERIC PcIoHandler::on_unregistered()
{

}

GENERIC PcIoHandler::on_write(CBYTEBUFFER out_data, size_type out_size)
{

}

GENERIC PcIoHandler::on_read(CBYTEBUFFER out_data, size_type out_size)
{

}

GENERIC PcIoHandler::_clear_handler()
{

}

MBASE_END