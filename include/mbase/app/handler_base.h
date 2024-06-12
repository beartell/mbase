#ifndef MBASE_HANDLER_BASE_H
#define MBASE_HANDLER_BASE_H

#include <mbase/common.h>

MBASE_BEGIN

class handler_base {
public:
	using user_data = PTRGENERIC;

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE handler_base() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE user_data get_user_data() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 get_handler_id() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 get_thread_index() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE GENERIC set_user_data(user_data in_data) noexcept;
	MBASE_INLINE GENERIC _set_thread_index(I32 in_index) noexcept;
	virtual GENERIC on_call(user_data in_data) = 0;
	/* ===== STATE-MODIFIER METHODS END ===== */

protected:
	U32 mHandlerId;
	I32 mThreadIndex;
	user_data mSuppliedData;
};

MBASE_INLINE handler_base::handler_base() noexcept : mHandlerId(0), mThreadIndex(-1), mSuppliedData(nullptr) 
{
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE typename handler_base::user_data handler_base::get_user_data() const noexcept 
{
	return mSuppliedData;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE U32 handler_base::get_handler_id() const noexcept 
{
	return mHandlerId;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 handler_base::get_thread_index() const noexcept 
{
	return mThreadIndex;
}

MBASE_INLINE GENERIC handler_base::set_user_data(user_data in_data) noexcept
{
	mSuppliedData = in_data;
}

MBASE_INLINE GENERIC handler_base::_set_thread_index(I32 in_index) noexcept 
{
	mThreadIndex = in_index;
}

MBASE_END

#endif // MBASE_HANDLER_BASE_H