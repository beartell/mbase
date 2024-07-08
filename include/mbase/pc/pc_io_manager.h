#ifndef MBASE_IO_MANAGER
#define MBASE_IO_MANAGER

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/io_base.h>
#include <mbase/list.h>
#include <mbase/vector.h>
#include <mbase/framework/handler_base.h>
#include <mbase/framework/async_io.h>
#include <mbase/framework/io_context.h>
#include <mbase/pc/pc_stream_manager.h>

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


MBASE_END

#endif // !MBASE_IO_MANAGER
