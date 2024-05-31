#ifndef MBASE_EVENT_MANAGER_H
#define MBASE_EVENT_MANAGER_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/app/event_handler.h>
#include <string>
#include <unordered_map>
#include <time.h>

MBASE_BEGIN

struct event_group {
	using size_type = SIZE_T;

	bool isGroupBlocked = true;
	size_type maxListeners = 1024;
	mbase::list<event_handler*> listeners;
};

class event_manager : public non_copymovable {
public:
	using user_data = PTRGENERIC;

	enum class flags : U32 {
		EVENT_MNG_SUCCESS = 0,
		EVENT_MNG_ERR_ID_MISMATCH = MBASE_EVENT_MANAGER_FLAGS_MIN,
		EVENT_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER,
		EVENT_MNG_ERR_GROUP_BLOCKED,
		EVENT_MNG_ERR_NOT_FOUND,
		EVENT_MNG_ERR_REACHED_MAX_LISTENERS,
		EVENT_MNG_ERR_UNKNOWN = MBASE_EVENT_MANAGER_FLAGS_MAX
	};

	MBASE_INLINE event_manager() noexcept {
		this->managerId = 1 + (rand() % 1000000);
		srand(time(0));
	}

	MBASE_INLINE flags dispatch_event(const std::string& in_event, user_data in_data) {
		auto foundElement = eventMap.find(in_event);
		if (foundElement != eventMap.end())
		{
			event_group& eventGroup = foundElement->second;
			if(eventGroup.isGroupBlocked)
			{
				return flags::EVENT_MNG_ERR_GROUP_BLOCKED;
			}

			mbase::list<event_handler*>& eventList = eventGroup.listeners;
			mbase::list<event_handler*>::iterator It = eventList.begin();
			for(It; It != eventList.end(); It++)
			{
				event_handler* suppliedHandler = *It;
				suppliedHandler->on_call(in_data);
				if(suppliedHandler->eventType == event_handler::flags::EVENT_ONCE)
				{
					suppliedHandler->managerId = -1;
					It = eventList.erase(suppliedHandler->selfIter);
				}
			}
			return flags::EVENT_MNG_SUCCESS;
		}
		return flags::EVENT_MNG_ERR_NOT_FOUND;
	}

	MBASE_INLINE flags add_event_listener(const std::string& in_event, event_handler& in_handler) {
		if(in_handler.managerId != -1)
		{
			return flags::EVENT_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER;
		}

		auto foundElement = eventMap.find(in_event);
		if(foundElement != eventMap.end())
		{
			event_group& eventGroup = foundElement->second;
			if(eventGroup.isGroupBlocked)
			{
				return flags::EVENT_MNG_ERR_GROUP_BLOCKED;
			}

			if(eventGroup.listeners.size() >= eventGroup.maxListeners)
			{
				return flags::EVENT_MNG_ERR_REACHED_MAX_LISTENERS;
			}

			mbase::list<event_handler*>& eventList = eventGroup.listeners;
			in_handler.selfIter = eventList.insert(eventList.cend(), &in_handler);
		}
		else
		{
			event_group newGroup;
			newGroup.isGroupBlocked = false;
			newGroup.listeners = mbase::list<event_handler*>();

			eventMap[in_event] = newGroup;
			mbase::list<event_handler*>& eventList = eventMap[in_event].listeners;

			in_handler.selfIter = eventList.insert(eventList.cend(), &in_handler);
		}

		in_handler.managerId = this->managerId;

		return flags::EVENT_MNG_SUCCESS;
	}

	MBASE_INLINE flags remove_listener(event_handler& in_handler)
	{
		if(in_handler.managerId != this->managerId)
		{
			return flags::EVENT_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER;
		}

		auto foundElement = eventMap.find(in_handler.eventName);
		if(foundElement != eventMap.end())
		{
			event_group& eventGroup = foundElement->second;
			if(eventGroup.isGroupBlocked)
			{
				return flags::EVENT_MNG_ERR_GROUP_BLOCKED;
			}

			mbase::list<event_handler*>& eventList = eventGroup.listeners;
			eventList.erase(in_handler.selfIter);
		}

		return flags::EVENT_MNG_SUCCESS;
	}

	MBASE_INLINE flags remove_all_listeners(const std::string& in_event)
	{
		auto foundElement = eventMap.find(in_event);
		if(foundElement != eventMap.end())
		{
			event_group& eventGroup = foundElement->second;
			mbase::list<event_handler*> eventList = eventGroup.listeners;
			eventList.clear();
			eventMap.erase(in_event);
			return flags::EVENT_MNG_SUCCESS;
			// no need to erase
		}
		return flags::EVENT_MNG_ERR_NOT_FOUND;
	}

	MBASE_INLINE I32 get_manager_id() {
		return managerId;
	}
private:
	I32 managerId = 0;
	std::unordered_map<std::string, event_group> eventMap;
};

MBASE_END

#endif // !MBASE_EVENT_MANAGER_H
