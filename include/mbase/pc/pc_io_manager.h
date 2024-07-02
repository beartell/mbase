#ifndef MBASE_IO_MANAGER
#define MBASE_IO_MANAGER

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/io_base.h>
#include <mbase/list.h>
#include <mbase/framework/handler_base.h>
#include <mbase/framework/async_io.h>
#include <mbase/framework/io_context.h>

MBASE_BEGIN

class PcIoHandler : public mbase::handler_base {
public:
	using io_handle = mbase::io_base*;
	using size_type = SIZE_T;

	const mbase::async_io_context* get_io_context() const;

	virtual GENERIC on_open(){}
	virtual GENERIC on_write(char_stream& out_data, size_type out_size){}
	virtual GENERIC on_read(const char_stream& out_data){}
	virtual GENERIC on_finish(){}
	virtual GENERIC on_close(){}
private:
	mbase::async_io_context mIoHandle;
};

class PcIoManager : public mbase::singleton<PcIoManager> {
public:
	using size_type = SIZE_T;
	using io_participants = mbase::list<PcIoHandler*>;

	enum class flags : U8 {
		IO_MNG_SUCCESS,
		IO_MNG_ERR_MISSING_DATA,
		IO_MNG_ERR_INVALID_SIZE,
		IO_MNG_OP_REGISTER,
		IO_MNG_OP_WRITE,
		IO_MNG_OP_READ,
		IO_MNG_OP_UNREGISTER
	};

	const mbase::async_io_manager* get_aio_mng() const;
	const io_participants* get_io_participants() const;

	bool initialize();
	flags register_io_base(mbase::io_base& in_handle, PcIoHandler& out_io_handler);
	flags unregister_io_handler(PcIoHandler& in_io_handler);
	flags io_mng_write(PcIoHandler& in_handler, const mbase::char_stream& in_data);
	flags io_mng_write(PcIoHandler& in_handler, IBYTEBUFFER in_data, size_type in_size);
	flags io_mng_read(PcIoHandler& in_handler, size_type in_size);
	flags update();
	flags update_writes();
	flags update_reads();

private:
	io_participants mIoParticipants;
	mbase::async_io_manager mAioMng;
};

MBASE_END

#endif // !MBASE_IO_MANAGER
