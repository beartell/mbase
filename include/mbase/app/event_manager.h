#ifndef MBASE_EVENT_MANAGER_H
#define MBASE_EVENT_MANAGER_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <string>
#include <unordered_map>

MBASE_BEGIN

class event_manager : public non_copymovable {
public:

	GENERIC DispatchEvent();
	GENERIC AddEventListener();
	GENERIC RemoveListener();
	GENERIC RemoveAllListeners();

private:
	std::unordered_map<std::string, int> a;
};

MBASE_END

#endif // !MBASE_EVENT_MANAGER_H
