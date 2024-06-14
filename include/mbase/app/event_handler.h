#ifndef MBASE_EVENT_HANDLER_H
#define MBASE_EVENT_HANDLER_H

#include <mbase/common.h>
#include <mbase/list.h>
#include <mbase/string.h>
#include <mbase/app/handler_base.h>

MBASE_BEGIN

class event_handler : public handler_base {
public:
	enum class flags : U32 {
		EVENT_HANDLER_SUCCESS = 0,
		EVENT_ONCE = MBASE_EVENT_HANDLER_FLAGS_CONTROL_START,
		EVENT_ON,
		EVENT_ERR_UNKNOWN = MBASE_EVENT_HANDLER_FLAGS_MAX
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE event_handler();
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE mbase::string get_event_name() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE flags get_event_type() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 get_manager_id() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	friend class event_manager;
private:
	using event_element = mbase::list<event_handler*>::iterator;
	
	event_element mSelfIter;
	mbase::string mEventName;
	flags mEventType;
	I32 mManagerId;
};

MBASE_INLINE event_handler::event_handler() : handler_base(), mEventType(flags::EVENT_ON), mSelfIter(nullptr), mEventName(""), mManagerId(-1)
{
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE mbase::string event_handler::get_event_name() const noexcept
{
	return mEventName;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE event_handler::flags event_handler::get_event_type() const noexcept
{
	return mEventType;
}

MBASE_ND(MBASE_OBS_IGNORE) MBASE_INLINE I32 event_handler::get_manager_id() const noexcept
{
	return mManagerId;
}

MBASE_END

#endif // !MBASE_EVENT_HANDLER_H
