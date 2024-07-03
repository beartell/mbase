#ifndef MBASE_IO_MANAGER
#define MBASE_IO_MANAGER

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/io_base.h>
#include <mbase/list.h>
#include <mbase/vector.h>
#include <mbase/index_assigner.h>
#include <mbase/framework/handler_base.h>
#include <mbase/framework/async_io.h>
#include <mbase/framework/io_context.h>

MBASE_BEGIN

static const U32 gIoManagerMaxReads = 8;
static const U32 gIoManagerMaxWrites = 8;
static const U32 gIoManagerStreamCount = 8;
static const U64 gIoManagerStreamSize = 0xfffff; // 1MB

class PcIoHandler : public mbase::handler_base {
public:
	using io_handle = mbase::io_base*;
	using size_type = SIZE_T;

	const mbase::async_io_context* get_io_context() const { return &mIoContext; }

	virtual GENERIC on_open(){}
	virtual GENERIC on_write(char_stream& out_data, size_type out_size){}
	virtual GENERIC on_read(const char_stream& out_data, size_type out_read){}
	virtual GENERIC on_finish(char_stream& out_data, size_type out_total_size){}
	virtual GENERIC on_close(){}
private:
	mbase::async_io_context mIoContext;
};

class PcIoManager : public mbase::singleton<PcIoManager> {
public:
	using size_type = SIZE_T;
	using io_participants = mbase::list<PcIoHandler*>;

	enum class flags : U8 {
		IO_MNG_SUCCESS,
		IO_MNG_ERR_MISSING_DATA,
		IO_MNG_ERR_INVALID_SIZE,
		IO_MNG_ERR_STREAMS_ARE_FULL,
		IO_MNG_OP_REGISTER,
		IO_MNG_OP_WRITE,
		IO_MNG_OP_READ,
		IO_MNG_OP_UNREGISTER
	};

	PcIoManager();
	~PcIoManager();

	const mbase::async_io_manager* get_aio_mng() const;
	const io_participants* get_io_participants() const;

	bool initialize(U32 in_stream_count = gIoManagerStreamCount, U64 in_stream_size = gIoManagerStreamSize);
	flags register_io_base(mbase::io_base& in_handle, PcIoHandler& out_io_handler);
	flags unregister_io_handler(PcIoHandler& in_io_handler);
	flags io_mng_write(PcIoHandler& in_handler, const mbase::char_stream& in_data);
	flags io_mng_write(PcIoHandler& in_handler, IBYTEBUFFER in_data, size_type in_size);
	flags io_mng_read(PcIoHandler& in_handler, size_type in_size);
	flags update();
	flags halt();
	flags update_writes();
	flags update_reads();

private:
	mbase::vector<mbase::char_stream*> mReaderStreams;
	io_participants mIoParticipants;
	mbase::async_io_manager mAioMng;
	U64 mStreamSize;
};

PcIoManager::PcIoManager() : mAioMng(512, 512)
{

}

PcIoManager::~PcIoManager()
{

}

const mbase::async_io_manager* PcIoManager::get_aio_mng() const
{
	return &mAioMng;
}

typename const PcIoManager::io_participants* PcIoManager::get_io_participants() const
{
	return &mIoParticipants;
}

bool PcIoManager::initialize(U32 in_stream_count = gIoManagerStreamCount, U64 in_stream_size = gIoManagerStreamSize)
{

}

PcIoManager::flags PcIoManager::register_io_base(mbase::io_base& in_handle, PcIoHandler& out_io_handler)
{

}

PcIoManager::flags PcIoManager::unregister_io_handler(PcIoHandler& in_io_handler)
{

}

PcIoManager::flags PcIoManager::io_mng_write(PcIoHandler& in_handler, const mbase::char_stream& in_data)
{

}

PcIoManager::flags PcIoManager::io_mng_write(PcIoHandler& in_handler, IBYTEBUFFER in_data, size_type in_size)
{

}

PcIoManager::flags PcIoManager::io_mng_read(PcIoHandler& in_handler, size_type in_size)
{

}

PcIoManager::flags PcIoManager::update()
{

}

PcIoManager::flags PcIoManager::halt()
{

}

PcIoManager::flags PcIoManager::update_writes()
{

}

PcIoManager::flags PcIoManager::update_reads()
{

}


MBASE_END

#endif // !MBASE_IO_MANAGER
