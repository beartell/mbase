#ifndef MBASE_SAFE_BUFFER_H
#define MBASE_SAFE_BUFFER_H

#include <mbase/common.h>
#include <mbase/type_sequence.h>

MBASE_STD_BEGIN

struct safe_buffer {
	safe_buffer() noexcept : bfSource(nullptr), bfLength(0) {}

	safe_buffer(IBYTEBUFFER in_src, SIZE_T in_length) noexcept : bfSource(in_src), bfLength(in_length) {}
	safe_buffer(safe_buffer&& in_rhs) noexcept {
		bfSource = in_rhs.bfSource;
		bfLength = in_rhs.bfLength;

		in_rhs.bfSource = nullptr;
		in_rhs.bfLength = 0;
	}
	~safe_buffer() noexcept {
		clear();
	}

	safe_buffer& operator=(safe_buffer&& in_rhs) noexcept {
		clear();

		bfSource = in_rhs.bfSource;
		bfLength = in_rhs.bfLength;

		in_rhs.bfSource = nullptr;
		in_rhs.bfLength = 0;
	}

	GENERIC clear() noexcept {
		if(bfSource)
		{
			free(bfSource);
		}
		bfLength = 0;
	}

	SIZE_T bfLength;
	IBYTEBUFFER bfSource;
};

#define MB_SET_SAFE_BUFFER(in_sfb, in_size)\
in_sfb.bfLength = in_size;\
in_sfb.bfSource = reinterpret_cast<IBYTEBUFFER>(malloc(in_sfb.bfLength));


MBASE_STD_END

#endif //