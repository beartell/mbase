#ifndef MBASE_BEHAVIORS_H
#define MBASE_BEHAVIORS_H

#include <mbase/common.h>

MBASE_STD_BEGIN

class non_copyable {
public:
	non_copyable() = default;
	non_copyable(const non_copyable&) = delete;
	non_copyable& operator=(const non_copyable&) = delete;
};

class non_movable {
public:
	non_movable() = default;
	non_movable(non_movable&&) = delete;
	non_movable& operator=(non_movable&&) = delete;
};

class non_copymovable {
public:
	non_copymovable() = default;
	non_copymovable(const non_copymovable&) = delete;
	non_copymovable(non_copymovable&&) = delete;
	non_copymovable& operator=(const non_copymovable&) = delete;
	non_copymovable& operator=(non_copymovable&&) = delete;
};

MBASE_STD_END

#endif // !MBASE_BEHAVIORS_H
