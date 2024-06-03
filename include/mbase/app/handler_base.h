#ifndef MBASE_HANDLER_BASE_H
#define MBASE_HANDLER_BASE_H

#include <mbase/common.h>

MBASE_BEGIN

class handler_base {
public:
	using user_data = PTRGENERIC;
	handler_base() noexcept : mHandlerId(0), mThreadIndex(-1), mSuppliedData(nullptr) {}

	virtual GENERIC on_call(user_data in_data) = 0;

	GENERIC set_user_data(user_data in_data) noexcept {
		mSuppliedData = in_data;
	}

	MBASE_ND("base handler observation being ignored") user_data get_user_data() const noexcept {
		return mSuppliedData;
	}

	MBASE_ND("base handler observation being ignored") U32 get_handler_id() const noexcept {
		return mHandlerId;
	}

	// if -1, it is the main thread
	// should not be understood as thread id, that is a different thing
	MBASE_ND("base handler observation being ignored") I32 get_thread_index() const noexcept {
		return mThreadIndex;
	}

	// Internal call, do not manually call it
	GENERIC _set_thread_index(I32 in_index) noexcept {
		mThreadIndex = in_index;
	}

protected:
	U32 mHandlerId;
	I32 mThreadIndex;
	user_data mSuppliedData;
};

MBASE_END

#endif // MBASE_HANDLER_BASE_H