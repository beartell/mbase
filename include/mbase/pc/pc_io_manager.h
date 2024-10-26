#ifndef MBASE_IO_MANAGER
#define MBASE_IO_MANAGER

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/io_file.h>
#include <mbase/list.h>
#include <mbase/vector.h>
#include <mbase/synchronization.h>
#include <mbase/framework/handler_base.h>
#include <mbase/pc/pc_stream_manager.h>

MBASE_BEGIN

#define MBASE_IOMNG_RETURN_UNINITIALIZED \
if(!is_initialized())\
{\
	return flags::IO_MNG_ERR_NOT_INITIALIZED;\
}

static const U32 gIoManagerMaxReadsDefault = 32;
static const U32 gIoManagerMaxWritesDefault = 32;

class PcIoHandler;
class PcIoManager;

class MBASE_API PcIoManager {
public:
	using size_type = SIZE_T;
	using io_participants = mbase::vector<PcIoHandler*>;
	using registered_handlers = mbase::list<PcIoHandler*>;
	using _sync_handle = HANDLE; // iocp handle

	enum class flags : U8 {
		IO_MNG_SUCCESS,
		IO_MNG_ERR_UNABLE_OPEN_FILE,
		IO_MNG_ERR_HANDLE_IS_BEING_PROCESSED,
		IO_MNG_ERR_NOT_INITIALIZED,
		IO_MNG_ERR_UNREGISTERED_HANDLER,
		IO_MNG_ERR_FILE_IS_NOT_OPEN,
		IO_MNG_ERR_INVALID_RAW_HANDLE,
		IO_MNG_ERR_ALREADY_REGISTERED,
		IO_MNG_WARN_FAILED_TO_ASSIGN_STREAM
	};

	PcIoManager() = default;
	~PcIoManager();

	const io_participants* get_io_participants() const;
	bool is_initialized() const;
	PcStreamManager* get_stream_manager();
	_sync_handle _get_sync_handle();

	flags initialize(U32 in_max_write_count = gIoManagerMaxWritesDefault, U32 in_max_read_count = gIoManagerMaxReadsDefault);
	flags register_handler(const mbase::wstring& in_filename, PcIoHandler& out_handler, bool in_stream_polled = true);
	flags unregister_handler(PcIoHandler& in_handler);
	flags add_handler(PcIoHandler& in_handler);
	flags update(); // SHOULD BE CALLED ON LOGIC LOOP
	flags update_t(); // SHOULD BE CALLED ON FILE IO LOOP

private:
	PcStreamManager mStreamManager;
	io_participants mIoParticipants;
	registered_handlers mRegisteredHandlers;
	mbase::mutex mIoMutex;
	mbase::mutex mRegistryMutex;
	bool mIsInitialized = false;
	_sync_handle mSyncHandle;
};

class MBASE_API PcIoHandler : public handler_base {
public:
	using io_handle_base = io_file;
	using size_type = SIZE_T;

	friend class PcIoManager;

	enum class direction : U8 {
		IO_HANDLER_DIRECTION_INPUT,
		IO_HANDLER_DIRECTION_OUTPUT
	};

	enum class flags : U8 {
		IO_HANDLER_SUCCESS,
		IO_HANDLER_ERR_INVALID_FLAG,
		IO_HANDLER_ERR_MISSING_DATA,
		IO_HANDLER_ERR_INVALID_DIRECTION,
		IO_HANDLER_ERR_IOMNG_PROCESSING_STREAM,
		IO_HANDLER_ERR_UNREGISTERED_HANDLER,
		IO_HANDLER_ERR_BUFFER_OVERFLOW
	};

	PcIoHandler();
	~PcIoHandler();

	bool is_open() const noexcept;
	bool is_registered() const noexcept;
	bool is_processing() const noexcept;
	direction get_io_direction() const noexcept;
	io_handle_base* get_io_handle() noexcept;

	flags set_io_direction(direction in_direction, bool is_sync = false); // SELF-NOTE: THIS SETS THE CURSOR ON THE FRONT
	flags set_stream(mbase::char_stream& in_stream, bool is_sync = false);
	flags write_buffer(CBYTEBUFFER in_data, size_type in_size, bool is_sync = false);
	flags read_buffer(size_type in_size, bool is_sync = false);
	flags flush_stream(bool is_sync = false);
	flags finish(bool is_sync = false);
	flags clear_file();

	virtual GENERIC on_registered();
	virtual GENERIC on_unregistered();
	virtual GENERIC on_write(CBYTEBUFFER out_data, size_type out_size);
	virtual GENERIC on_read(CBYTEBUFFER out_data, size_type out_size);
private:

	GENERIC _clear_handler();

	friend class PcIoManager;
	bool mIsRegistered;
	bool mIsProcessing;
	mbase::char_stream* mProcessorStream;
	io_handle_base mIoBase;
	direction mIoDirection;
	OVERLAPPED mOvp;
	ULONG_PTR mIocpKey;
	PcStreamManager::stream_handle mPolledStreamHandle;
	PcIoManager::registered_handlers::iterator mSelfIter;
};

MBASE_END

#endif // !MBASE_IO_MANAGER
