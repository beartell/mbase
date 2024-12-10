#ifndef MBASE_INF_T2T_PROC_DIAGNOSTICS
#define MBASE_INF_T2T_PROC_DIAGNOSTICS

#include <mbase/inference/inf_common.h>
#include <mbase/pc/pc_diagnostics.h>

MBASE_BEGIN

class MBASE_API InfProcT2TDiagnostics : public mbase::PcDiagnostics {
public:
    InfProcT2TDiagnostics();

    U32 loadTimeInMilliseconds;
    F32 ppTokensPerSecond;
    F32 evalTokensPerSecond;
};

MBASE_END

#endif // MBASE_INF_T2T_PROC_DIAGNOSTICS