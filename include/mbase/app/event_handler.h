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
		EVENT_ONCE = 1,
		EVENT_ON = 2
	};

	event_handler() : handler_base(), eventType(flags::EVENT_ON) {};
	std::string GetEventName() {
		return eventName;
	}

	flags GetEventType() {
		return eventType;
	}

	friend class event_manager;
private:
	using event_element = mbase::list<event_handler*>::iterator;
	
	event_element selfIter;
	std::string eventName;
	flags eventType;
};

MBASE_END

#endif // !MBASE_EVENT_HANDLER_H
