#ifndef MBASE_CONTEXT_LINE_H
#define MBASE_CONTEXT_LINE_H

#include <mbase/common.h>
#include <mbase/string.h>

MBASE_BEGIN

enum class context_role {
	SYSTEM,
	ASSISTANT,
	USER,
	NONE
};

struct context_line {
	context_role mRole = context_role::NONE;
	mbase::string mMessage = "";
	U32 mMessageIndex;
};

MBASE_END

#endif // !MBASE_CONTEXT_LINE_H
