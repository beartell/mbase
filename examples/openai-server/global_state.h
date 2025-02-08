#ifndef MBASE_OPENAI_GLOBAL_STATE_H
#define MBASE_OPENAI_GLOBAL_STATE_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>
#include <mbase/json/json.h>
#include <mbase/pc/pc_diagnostics.h>
#include "model.h"

MBASE_BEGIN

struct mbase_openai_program_data {
    // Program Arguments
    mbase::string apiKey;
    mbase::string hostName = "127.0.0.1"; // --hostname || -h;
    int listenPort = 8080; // --port || -p
    mbase::Json jsonDescription; // -jsdesc

    // Program data
    mbase::vector<mbase::OpenaiModel*> programModels;
    bool serverListening = true;
    mbase::PcDiagnostics diagnostic;
};

MBASE_END

inline mbase::mbase_openai_program_data gProgramData;

#endif // MBASE_OPENAI_GLOBAL_STATE_H