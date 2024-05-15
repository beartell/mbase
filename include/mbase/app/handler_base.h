#ifndef MBASE_HANDLER_BASE_H
#define MBASE_HANDLER_BASE_H

#include <mbase/common.h>

MBASE_BEGIN

class handler_base {
public:
	using user_data = PTRGENERIC;
	handler_base() noexcept : handlerId(0), threadIndex(-1), suppliedData(nullptr) {}

	virtual GENERIC on_call(user_data in_data) = 0;

	GENERIC SetUserData(user_data in_data) noexcept {
		suppliedData = in_data;
	}

	MBASE_ND("base handler observation being ignored") user_data GetUserData() const noexcept {
		return suppliedData;
	}

	MBASE_ND("base handler observation being ignored") U32 GetHandlerId() const noexcept {
		return handlerId;
	}

	// if -1, it is the main thread
	// should not be understood as thread id, that is a different thing
	MBASE_ND("base handler observation being ignored") I32 GetThreadIndex() const noexcept {
		return threadIndex;
	}

	// Internal call, do not manually call it
	GENERIC _SetThreadIndex(I32 in_index) noexcept {
		threadIndex = in_index;
	}

protected:
	U32 handlerId;
	I32 threadIndex;
	user_data suppliedData;
};

MBASE_END

#endif // MBASE_HANDLER_BASE_H