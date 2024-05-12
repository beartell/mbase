#ifndef MBASE_BEHAVIORS_H
#define MBASE_BEHAVIORS_H

#include <mbase/common.h>

MBASE_STD_BEGIN
/*

	--- CLASS INFORMATION ---
Identification: S0C4-STR-NA-ST

Name: non_copyable

Parent: None

Behaviour List:
- Default Constructible
- Move Constructible
- Move Assignable
- Stateless

Description:
By inheriting this class, it makes the derived
class non copyable.

*/

class non_copyable {
public:
	/* ===== BUILDER METHODS BEGIN ===== */
	non_copyable() = default;
	non_copyable(const non_copyable&) = delete;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	non_copyable& operator=(const non_copyable&) = delete;
	/* ===== OPERATOR BUILDER METHODS END ===== */
};

/*

	--- CLASS INFORMATION ---
Identification: S0C5-STR-NA-ST

Name: non_movable

Parent: None

Behaviour List:
- Default Constructible
- Copy Constructible
- Copy Assignable
- Stateless

Description:
By inheriting this class, it makes the derived
class non movable.

*/

class non_movable {
public:
	/* ===== BUILDER METHODS BEGIN ===== */
	non_movable() = default;
	non_movable(non_movable&&) = delete;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	non_movable& operator=(non_movable&&) = delete;
	/* ===== OPERATOR BUILDER METHODS END ===== */
};

/*

	--- CLASS INFORMATION ---
Identification: S0C6-STR-NA-ST

Name: non_copymovable

Parent: None

Behaviour List:
- Default Constructible
- Stateless

Description:
By inheriting this class, it makes the derived
both non copyable and non movable.

*/

class non_copymovable {
public:
	/* ===== BUILDER METHODS BEGIN ===== */
	non_copymovable() = default;
	non_copymovable(const non_copymovable&) = delete;
	non_copymovable(non_copymovable&&) = delete;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	non_copymovable& operator=(const non_copymovable&) = delete;
	non_copymovable& operator=(non_copymovable&&) = delete;
	/* ===== OPERATOR BUILDER METHODS END ===== */
};

MBASE_STD_END

#endif // !MBASE_BEHAVIORS_H
