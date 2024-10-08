#ifndef MBASE_CLIENT_SECURITY_PARAMS
#define MBASE_CLIENT_SECURITY_PARAMS

#include <mbase/common.h>
#include <mbase/string.h>

MBASE_BEGIN

static const U8 INF_CLIENT_INITIATIVE_LOAD_AUTHORITY = 0x1;
static const U8 INF_CLIENT_INITIATIVE_UNLOAD_AUTHORITY = 0x2;

static const U16 INF_CLIENT_BEHAVIORAL_CTX_MODIFICATION = 0x1;
static const U16 INF_CLIENT_BEHAVIORAL_MAX_LISTENER_MODIFICATION = 0x2;

struct MBASE_API InfClientSecurityStructure {
	U8 mInitiativeAuthorityParams;
	U16 mBehavioralAuthorityParams;
};


MBASE_END

#endif // !MBASE_CLIENT_SECURITY_PARAMS
