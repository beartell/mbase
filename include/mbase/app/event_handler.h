#ifndef MBASE_EVENT_HANDLER_H
#define MBASE_EVENT_HANDLER_H

#include <mbase/common.h>
#include <mbase/list.h>
#include <string>
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

	event_handler() : handler_base(), mEventType(flags::EVENT_ON), mSelfIter(nullptr), mEventName(""), mManagerId(-1) {};
	std::string get_event_name() {
		return mEventName;
	}

	flags get_event_type() {
		return mEventType;
	}

	friend class event_manager;
private:
	using event_element = mbase::list<event_handler*>::iterator;
	
	event_element mSelfIter;
	std::string mEventName;
	flags mEventType;
	I32 mManagerId;
};

MBASE_END

#endif // !MBASE_EVENT_HANDLER_H
