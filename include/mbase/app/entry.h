#ifndef MBASE_ENTRY_H
#define MBASE_ENTRY_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/app/timer_loop.h>
#include <mbase/char_stream.h>
#include <mbase/io_file.h>
#include <mbase/app/key_input.h>
#include <mbase/app/timers.h>
#include <iostream>

MBASE_BEGIN

class app {
public:
	app() {
		GetStartupInfo(&winInfo);
	}

    ~app() {
        
    }

	GENERIC run() {
        eventLoop.RunTimerOnly();
	}

    timer_loop* GetApplicationLoop() {
        return &eventLoop;
    }

private:
    mbase::wstring applicationName;
	STARTUPINFO winInfo;
	timer_loop eventLoop;
};

MBASE_END

#endif // MBASE_ENTRY_H