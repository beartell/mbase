#ifndef MBASE_INF_PROGRAM_H
#define MBASE_INF_PROGRAM_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/string.h>
#include <mbase/list.h>

MBASE_BEGIN

class MBASE_API InfAcceptedClient {
public:

private:
	mbase::string mToModel;
};

class MBASE_API InfProgram {
public:

	enum class flags : U16 {
		SUCCESS = 2000,
		MAXIMUM_CLIENTS,
		MISSING_CLIENT,
		UNAUTHORIZED_ACCESS,
		MISSING_MODEL,
		CLIENT_TOKEN_MISMATCH,
		MISSING_CONTEXT,
		CONTEXT_LIMIT_REACHED
	};

	flags accept_client(const mbase::string& in_model_name);

private:
	
};

MBASE_END

#endif // !MBASE_INF_PROGRAM_H
