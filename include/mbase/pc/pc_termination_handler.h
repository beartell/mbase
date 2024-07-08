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

class PcTerminationBasic : public PcTerminationHandler {
public:

	GENERIC on_termination() override 
	{

	}

	GENERIC on_segfault() override 
	{

	}

	GENERIC on_interrupt() override 
	{

	}

	GENERIC on_invalidation() override 
	{

	}

	GENERIC on_abort() override 
	{

	}
};

MBASE_END

#endif // !MBASE_TERMINATION_HANDLER_H
