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
	using handler_container = mbase::list<event_handler*>;

	bool isGroupBlocked = true;
	size_type maxListeners = 1024;
	handler_container listeners;
};

class event_manager : public non_copymovable {
public:
	using user_data = PTRGENERIC;
	using event_map = std::unordered_map<std::string, event_group>;

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
		this->mManagerId = 1 + (rand() % 1000000);
		srand(time(0));
	}

	MBASE_INLINE flags dispatch_event(const std::string& in_event, user_data in_data) noexcept {
		auto foundElement = mEventMap.find(in_event);
		if (foundElement != mEventMap.end())
		{
			event_group& eventGroup = foundElement->second;
			if(eventGroup.isGroupBlocked)
			{
				return flags::EVENT_MNG_ERR_GROUP_BLOCKED;
			}

			event_group::handler_container& eventList = eventGroup.listeners;
			event_group::handler_container::iterator It = eventList.begin();
			for(It; It != eventList.end(); It++)
			{
				event_handler* suppliedHandler = *It;
				suppliedHandler->on_call(in_data);
				if(suppliedHandler->eventType == event_handler::flags::EVENT_ONCE)
				{
					suppliedHandler->mManagerId = -1;
					It = eventList.erase(suppliedHandler->mSelfIter);
				}
			}
			return flags::EVENT_MNG_SUCCESS;
		}
		return flags::EVENT_MNG_ERR_NOT_FOUND;
	}

	MBASE_INLINE flags add_event_listener(const std::string& in_event, event_handler& in_handler) noexcept {
		if(in_handler.mManagerId != -1)
		{
			return flags::EVENT_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER;
		}

		auto foundElement = mEventMap.find(in_event);
		if(foundElement != mEventMap.end())
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

			event_group::handler_container& eventList = eventGroup.listeners;
			in_handler.mSelfIter = eventList.insert(eventList.cend(), &in_handler);
		}
		else
		{
			event_group newGroup;
			newGroup.isGroupBlocked = false;
			newGroup.listeners = event_group::handler_container();

			mEventMap[in_event] = newGroup;
			event_group::handler_container& eventList = mEventMap[in_event].listeners;

			in_handler.mSelfIter = eventList.insert(eventList.cend(), &in_handler);
		}

		in_handler.mManagerId = this->mManagerId;

		return flags::EVENT_MNG_SUCCESS;
	}

	MBASE_INLINE flags remove_listener(event_handler& in_handler) noexcept
	{
		if(in_handler.mManagerId != this->mManagerId)
		{
			return flags::EVENT_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER;
		}

		auto foundElement = mEventMap.find(in_handler.mEventName);
		if(foundElement != mEventMap.end())
		{
			event_group& eventGroup = foundElement->second;
			if(eventGroup.isGroupBlocked)
			{
				return flags::EVENT_MNG_ERR_GROUP_BLOCKED;
			}

			event_group::handler_container& eventList = eventGroup.listeners;
			eventList.erase(in_handler.mSelfIter);
		}

		return flags::EVENT_MNG_SUCCESS;
	}

	MBASE_INLINE flags remove_all_listeners(const std::string& in_event) noexcept
	{
		auto foundElement = mEventMap.find(in_event);
		if(foundElement != mEventMap.end())
		{
			event_group& eventGroup = foundElement->second;
			event_group::handler_container& eventList = eventGroup.listeners;
			eventList.clear();
			mEventMap.erase(in_event);
			return flags::EVENT_MNG_SUCCESS;
		}
		return flags::EVENT_MNG_ERR_NOT_FOUND;
	}

	MBASE_INLINE I32 get_manager_id() const noexcept 
	{
		return mManagerId;
	}

	MBASE_INLINE event_map* get_event_map() const noexcept 
	{
		return mEventMap;
	}

private:
	I32 mManagerId = 0;
	event_map mEventMap;
};

MBASE_END

#endif // !MBASE_EVENT_MANAGER_H
