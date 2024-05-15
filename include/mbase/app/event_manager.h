#ifndef MBASE_EVENT_MANAGER_H
#define MBASE_EVENT_MANAGER_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/app/event_handler.h>
#include <string>
#include <unordered_map>

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
		EVENT_MNG_ERR_ID_MISMATCH = 1,
		EVENT_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER = 2,
		EVENT_MNG_ERR_GROUP_BLOCKED = 3,
		EVENT_MNG_ERR_NOT_FOUND = 4,
		EVENT_MNG_ERR_REACHED_MAX_LISTENERS = 5
	};

	flags DispatchEvent(const std::string& in_event, user_data in_data) {
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
					It = eventList.erase(suppliedHandler->selfIter);
				}
			}
			return flags::EVENT_MNG_SUCCESS;
		}
		return flags::EVENT_MNG_ERR_NOT_FOUND;
	}

	flags AddEventListener(const std::string& in_event, event_handler& in_handler) {
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

		return flags::EVENT_MNG_SUCCESS;
	}

	flags RemoveListener(event_handler& in_handler)
	{
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

	flags RemoveAllListeners(const std::string& in_event)
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
private:
	I32 managerId;
	std::unordered_map<std::string, event_group> eventMap;
};

MBASE_END

#endif // !MBASE_EVENT_MANAGER_H
