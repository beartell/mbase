#include <mbase/inference/inf_t2t_proc_diagnostics.h>

MBASE_BEGIN

InfProcT2TDiagnostics::InfProcT2TDiagnostics():
    loadTimeInMilliseconds(0),
    ppTokensPerSecond(0),
    evalTokensPerSecond(0)
{
}

MBASE_END