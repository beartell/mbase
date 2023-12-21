#ifndef MBASE_SAFE_BUFFER_H
#define MBASE_SAFE_BUFFER_H

#include <mbase/common.h>
#include <mbase/type_sequence.h>

MBASE_STD_BEGIN

struct safe_buffer {
	safe_buffer() noexcept {}

	safe_buffer(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
		bfSource = in_src;
		bfLength = in_length;
	}

	safe_buffer(const safe_buffer& in_rhs) noexcept {
		bfLength = in_rhs.bfLength;
		if(bfSource)
		{
			delete[] bfSource;
		}

		if(in_rhs.bfSource)
		{
			type_sequence<IBYTE>::copy(bfSource, in_rhs.bfSource, in_rhs.bfLength);
		}
	}

	safe_buffer(safe_buffer&& in_rhs) noexcept {
		if(bfSource)
		{
			delete[] bfSource;
		}

		bfSource = in_rhs.bfSource;
		bfLength = in_rhs.bfLength;
	}

	~safe_buffer() noexcept {
		if(bfSource)
		{
			delete[] bfSource;
		}
	}

	safe_buffer& operator=(const safe_buffer& in_rhs) noexcept {
		bfLength = in_rhs.bfLength;
		if (bfSource)
		{
			delete[] bfSource;
		}

		if (in_rhs.bfSource)
		{
			type_sequence<IBYTE>::copy(bfSource, in_rhs.bfSource, in_rhs.bfLength);
		}

		return *this;
	}

	safe_buffer& operator=(safe_buffer&& in_rhs) noexcept {
		if (bfSource)
		{
			delete[] bfSource;
		}

		bfSource = in_rhs.bfSource;
		bfLength = in_rhs.bfLength;
	}

	GENERIC clear() noexcept {
		delete[] bfSource;
		bfLength = 0;
		bfSource = nullptr;
	}

	IBYTEBUFFER bfSource = nullptr;
	SIZE_T bfLength = 0;
};

MBASE_STD_END

#endif //