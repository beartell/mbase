#ifndef MBASE_EVENT_MANAGER_H
#define MBASE_EVENT_MANAGER_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/app/event_handler.h>
#include <mbase/unordered_map.h>
#include <time.h>

MBASE_BEGIN

static const bool gDefaultEventGroupBlockState = true;
static const U32 gDefaultMaxListeners = 1024;

struct event_group {
	using size_type = SIZE_T;
	using handler_container = mbase::list<event_handler*>;

	bool isGroupBlocked = gDefaultEventGroupBlockState;
	size_type maxListeners = gDefaultMaxListeners;
	handler_container listeners;
};

class event_manager : public non_copymovable {
public:
	using user_data = PTRGENERIC;
	using event_map = mbase::unordered_map<mbase::string, event_group>;

	enum class flags : U32 {
		EVENT_MNG_SUCCESS = 0,
		EVENT_MNG_ERR_ID_MISMATCH = MBASE_EVENT_MANAGER_FLAGS_MIN,
		EVENT_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER,
		EVENT_MNG_ERR_ALREADY_REGISTERED,
		EVENT_MNG_ERR_GROUP_BLOCKED,
		EVENT_MNG_ERR_NOT_FOUND,
		EVENT_MNG_ERR_REACHED_MAX_LISTENERS,
		EVENT_MNG_ERR_UNKNOWN = MBASE_EVENT_MANAGER_FLAGS_MAX
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	MBASE_INLINE event_manager() noexcept;
	MBASE_INLINE ~event_manager() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	MBASE_INLINE flags dispatch_event(const mbase::string& in_event, user_data in_data = nullptr) noexcept;
	MBASE_INLINE flags add_event_listener(const mbase::string& in_event, event_handler& in_handler) noexcept;
	MBASE_INLINE flags remove_listener(event_handler& in_handler) noexcept;
	MBASE_INLINE flags remove_all_listeners(const mbase::string& in_event) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_INLINE I32 get_manager_id() const noexcept;
	MBASE_INLINE const event_map& get_event_map() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

private:
	I32 mManagerId = 0;
	event_map mEventMap;
};

MBASE_INLINE event_manager::event_manager() noexcept
{
	this->mManagerId = 1 + (rand() % 1000000);
	srand(time(0));
}

MBASE_INLINE event_manager::~event_manager() noexcept
{
	for(event_map::iterator It = mEventMap.begin(); It != mEventMap.end(); It++)
	{
		remove_all_listeners(It->first);
	}
}

MBASE_INLINE event_manager::flags event_manager::dispatch_event(const mbase::string& in_event, user_data in_data) noexcept 
{
	auto foundElement = mEventMap.find(in_event);
	if (foundElement != mEventMap.end())
	{
		event_group& eventGroup = foundElement->second;
		if (eventGroup.isGroupBlocked)
		{
			return flags::EVENT_MNG_ERR_GROUP_BLOCKED;
		}

		event_group::handler_container& eventList = eventGroup.listeners;
		event_group::handler_container::iterator It = eventList.begin();
		for (It; It != eventList.end(); It++)
		{
			event_handler* suppliedHandler = *It;
			suppliedHandler->on_call(in_data);
			if (suppliedHandler->mEventType == event_handler::flags::EVENT_ONCE)
			{
				suppliedHandler->on_unregister();
				suppliedHandler->mEventName.clear();
				suppliedHandler->mManagerId = -1;
				suppliedHandler->mStatus = event_handler::flags::EVENT_UNREGISTERED;
				It = eventList.erase(suppliedHandler->mSelfIter);
			}
		}
		return flags::EVENT_MNG_SUCCESS;
	}
	return flags::EVENT_MNG_ERR_NOT_FOUND;
}

MBASE_INLINE event_manager::flags event_manager::add_event_listener(const mbase::string& in_event, event_handler& in_handler) noexcept 
{
	if(in_handler.is_registered())
	{
		return flags::EVENT_MNG_ERR_ALREADY_REGISTERED;
	}

	auto foundElement = mEventMap.find(in_event);
	if (foundElement != mEventMap.end())
	{
		event_group& eventGroup = foundElement->second;
		if (eventGroup.isGroupBlocked)
		{
			return flags::EVENT_MNG_ERR_GROUP_BLOCKED;
		}

		if (eventGroup.listeners.size() >= eventGroup.maxListeners)
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

	in_handler.mEventName = in_event;
	in_handler.mManagerId = mManagerId;
	in_handler.mStatus = event_handler::flags::EVENT_REGISTERED;
	in_handler.on_register();

	return flags::EVENT_MNG_SUCCESS;
}

MBASE_INLINE event_manager::flags event_manager::remove_listener(event_handler& in_handler) noexcept
{
	if(!in_handler.is_registered())
	{
		return flags::EVENT_MNG_SUCCESS;
	}

	if (in_handler.mManagerId != this->mManagerId)
	{
		return flags::EVENT_MNG_ERR_BELONGS_TO_FOREIGN_MANAGER;
	}

	auto foundElement = mEventMap.find(in_handler.mEventName);
	if (foundElement != mEventMap.end())
	{
		event_group& eventGroup = foundElement->second;
		if (eventGroup.isGroupBlocked)
		{
			return flags::EVENT_MNG_ERR_GROUP_BLOCKED;
		}

		event_group::handler_container& eventList = eventGroup.listeners;
		eventList.erase(in_handler.mSelfIter);
	}

	in_handler.on_unregister();
	in_handler.mEventName.clear();
	in_handler.mManagerId = -1;
	in_handler.mStatus = event_handler::flags::EVENT_UNREGISTERED;

	return flags::EVENT_MNG_SUCCESS;
}

MBASE_INLINE event_manager::flags event_manager::remove_all_listeners(const mbase::string& in_event) noexcept
{
	auto foundElement = mEventMap.find(in_event);
	if (foundElement != mEventMap.end())
	{
		event_group& eventGroup = foundElement->second;
		event_group::handler_container& eventList = eventGroup.listeners;
		for(event_group::handler_container::iterator It = eventList.begin(); It != eventList.end(); ++It)
		{
			event_handler* evh = *It;
			evh->on_unregister();
			evh->mEventName.clear();
			evh->mManagerId = -1;
			evh->mStatus = event_handler::flags::EVENT_UNREGISTERED;
		}
		eventList.clear();
		mEventMap.erase(in_event);
		return flags::EVENT_MNG_SUCCESS;
	}
	return flags::EVENT_MNG_ERR_NOT_FOUND;
}

MBASE_INLINE I32 event_manager::get_manager_id() const noexcept
{
	return mManagerId;
}

MBASE_INLINE const event_manager::event_map& event_manager::get_event_map() const noexcept
{
	return mEventMap;
}

MBASE_END

#endif // !MBASE_EVENT_MANAGER_H
