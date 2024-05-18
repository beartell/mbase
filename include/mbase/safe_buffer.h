#ifndef MBASE_SAFE_BUFFER_H
#define MBASE_SAFE_BUFFER_H

#include <mbase/common.h>
#include <mbase/type_sequence.h>

MBASE_STD_BEGIN

/*

	--- CLASS INFORMATION ---
Identification: S0C33-OBJ-UD-ST

Name: safe_buffer

Parent: None

Behaviour List:
- Default Constructible
- Destructible
- Move Constructible
- Move Assignable

Description:

*/


struct safe_buffer {
	/* ===== BUILDER METHODS BEGIN ===== */
	safe_buffer() noexcept;
	safe_buffer(IBYTEBUFFER in_src, SIZE_T in_length) noexcept;
	safe_buffer(safe_buffer&& in_rhs) noexcept;
	~safe_buffer() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OPERATOR BUILDER METHODS BEGIN ===== */
	safe_buffer& operator=(safe_buffer&& in_rhs) noexcept;
	/* ===== OPERATOR BUILDER METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	GENERIC clear() noexcept;
	GENERIC swap(safe_buffer& sfb) noexcept;
	/* ===== STATE-MODIFIER METHODS END ===== */

	SIZE_T bfLength;
	IBYTEBUFFER bfSource;
};

safe_buffer::safe_buffer() noexcept : bfSource(nullptr), bfLength(0) 
{
}

safe_buffer::safe_buffer(IBYTEBUFFER in_src, SIZE_T in_length) noexcept : bfSource(in_src), bfLength(in_length)
{
}

safe_buffer::safe_buffer(safe_buffer&& in_rhs) noexcept
{
	bfSource = in_rhs.bfSource;
	bfLength = in_rhs.bfLength;

	in_rhs.bfSource = nullptr;
	in_rhs.bfLength = 0;
}

safe_buffer::~safe_buffer() noexcept
{
	clear();
}

safe_buffer& safe_buffer::operator=(safe_buffer&& in_rhs) noexcept 
{
	clear();

	bfSource = in_rhs.bfSource;
	bfLength = in_rhs.bfLength;

	in_rhs.bfSource = nullptr;
	in_rhs.bfLength = 0;

	return *this;
}

GENERIC safe_buffer::clear() noexcept 
{
	if (bfSource)
	{
		free(bfSource);
	}
	bfLength = 0;
}

GENERIC safe_buffer::swap(safe_buffer& sfb) noexcept
{
	std::swap(bfLength, sfb.bfLength);
	std::swap(bfSource, sfb.bfSource);
}

#define MB_SET_SAFE_BUFFER(in_sfb, in_size)\
in_sfb.bfLength = in_size;\
in_sfb.bfSource = reinterpret_cast<IBYTEBUFFER>(malloc(in_sfb.bfLength));


MBASE_STD_END

#endif //