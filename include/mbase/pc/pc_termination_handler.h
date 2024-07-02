#ifndef MBASE_TERMINATION_HANDLER_H
#define MBASE_TERMINATION_HANDLER_H

#include <mbase/common.h>

MBASE_BEGIN

class PcTerminationHandler {
public:
	virtual GENERIC on_termination() = 0;
	virtual GENERIC on_segfault() = 0;
	virtual GENERIC on_interrupt() = 0;
	virtual GENERIC on_invalidation() = 0;
	virtual GENERIC on_abort() = 0;
};

MBASE_END

#endif // !MBASE_TERMINATION_HANDLER_H
