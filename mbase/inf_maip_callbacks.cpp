#include <mbase/inference/inf_maip_callbacks.h>

MBASE_BEGIN

bool access_request_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    mbase::string accessToken = in_request.get_kval<mbase::string>("ACCTOK");

    mbase::string outToken;

    InfProgram::maip_err_code errCode = in_program.inf_access_request(username, accessToken, in_peer, outToken);
    if(errCode == InfProgram::maip_err_code::INF_SUCCESS)
    {
        out_packet.set_kval("STOK", outToken);
    }
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool destroy_session_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    InfProgram::maip_err_code errCode = in_program.inf_destroy_session(in_session_id);
    out_packet.set_response_message((U16)errCode);
    return true;
}

bool get_accessible_models_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::vector<mbase::string> modelsVector;
    InfProgram::maip_err_code errCode = in_program.inf_get_accessible_models(in_session_id, modelsVector);
    out_packet.set_response_message((U16)errCode);

    for(mbase::vector<mbase::string>::iterator It = modelsVector.begin(); It != modelsVector.end(); ++It)
    {
        out_packet.set_kval("MODEL", *It);
    }

    return true;
}

bool get_context_ids_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::vector<U64> ctxIds;
    InfProgram::maip_err_code errCode = in_program.inf_get_context_ids(in_session_id, ctxIds);
    out_packet.set_response_message((U16)errCode);

    for(mbase::vector<U64>::iterator It = ctxIds.begin(); It != ctxIds.end(); ++It)
    {
        out_packet.set_kval("CTXID", *It);
    }

    return true;
}

bool create_context_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string myModel = in_request.get_kval<mbase::string>("MODEL");
    U32 mCtxSize = in_request.get_kval<U32>("CTXSIZE");

    InfProgram::maip_err_code errCode = in_program.inf_create_context(in_session_id, in_peer, myModel, mCtxSize);
    if(errCode == InfProgram::maip_err_code::INF_SUCCESS)
    {
        // If the operation is successful, program will own the client pointer
        // THIS METHOD OWNS THE CLIENT POINTER
        return false;
    }

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool clear_context_history_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    U32 ctxId = in_request.get_kval<U32>("CTXID");
    InfProgram::maip_err_code errCode = in_program.inf_clear_context_history(in_session_id, ctxId);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool get_context_status_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    U32 ctxId = in_request.get_kval<U32>("CTXID");
    InfProgram::maip_err_code errCode = in_program.inf_get_context_status(in_session_id, ctxId);
    out_packet.set_response_message((U16)errCode);
    return true;
}

bool destroy_context_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    U32 ctxId = in_request.get_kval<U32>("CTXID");
    InfProgram::maip_err_code errCode = in_program.inf_destroy_context(in_session_id, ctxId);
    out_packet.set_response_message((U16)errCode);
    return true;
}

bool get_program_models_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::vector<mbase::string> programModels;
    InfProgram::maip_err_code errCode = in_program.inf_get_program_models(in_session_id, programModels);
    out_packet.set_response_message((U16)errCode);
    for(mbase::vector<mbase::string>::iterator It = programModels.begin(); It != programModels.end(); ++It)
    {
        out_packet.set_kval("MODEL", *It);
    }
    return true;
}

bool load_model_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string modelName = in_request.get_kval<mbase::string>("MODEL");
    U32 ctxSize = in_request.get_kval<U32>("CTXSIZE");

    InfProgram::maip_err_code errCode = in_program.inf_load_model(in_session_id, modelName, ctxSize);
    if(errCode == InfProgram::maip_err_code::INF_SUCCESS)
    {
        // TODO: Make the program own the pointer so that message will be sent on model load
        return false;
    }

    out_packet.set_response_message((U16)errCode);
    return true;
}

bool unload_model_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string modelName = in_request.get_kval<mbase::string>("MODEL");
    InfProgram::maip_err_code errCode = in_program.inf_unload_model(in_session_id, modelName);
    out_packet.set_response_message((U16)errCode);
    return true;
}

bool create_new_user_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    mbase::string accTok = in_request.get_kval<mbase::string>("ACCTOK");
    U32 accessLimit = in_request.get_kval<U32>("ACCLIMIT");
    U32 ctxLength = in_request.get_kval<U32>("CTXLENGTH");
    U32 batchSet = in_request.get_kval<U32>("BATCH_SET");
    U32 maxProcThreads = in_request.get_kval<U32>("MAX_PROC_THREADS");
    U32 procThreads = in_request.get_kval<U32>("PROC_THREADS");
    mbase::vector<mbase::string> authFlags = in_request.get_kval<mbase::vector<mbase::string>>("AUTHFLAGS");
    bool isSuper = in_request.get_kval<bool>("ISSUPER");
    bool isStatic = in_request.get_kval<bool>("ISSTATIC");
    const char_stream& systemPrompt = in_request.get_data();

    mbase::string systemPromptString;
    if(systemPrompt.buffer_length())
    {
        systemPromptString = std::move(mbase::string(systemPrompt.data(), systemPrompt.buffer_length()));
    }

    mbase::string outAccessToken;

    InfProgram::maip_err_code errCode = in_program.inf_create_new_user(
        in_session_id,
        username,
        accTok,
        systemPromptString,
        accessLimit,
        ctxLength,
        batchSet,
        maxProcThreads,
        procThreads,
        isSuper,
        isStatic, 
        authFlags,
        outAccessToken
    );

    if(errCode == InfProgram::maip_err_code::INF_SUCCESS)
    {
        out_packet.set_kval("ACCTOK", outAccessToken);
    }
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool delete_user_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    mbase::string accTok = in_request.get_kval<mbase::string>("ACCTOK");
    
    InfProgram::maip_err_code errCode = in_program.inf_delete_user(in_session_id, username, accTok);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_model_access_limit_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    U32 accessLimit = in_request.get_kval<U32>("ACCLIMIT");
    
    InfProgram::maip_err_code errCode = in_program.inf_modify_user_model_access_limit(in_session_id, username, accessLimit);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_maximum_context_length_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    U32 ctxLength = in_request.get_kval<U32>("CTXLENGTH");

    InfProgram::maip_err_code errCode = in_program.inf_modify_user_maximum_context_length(in_session_id, username, ctxLength);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_batch_size_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    U32 batchSet = in_request.get_kval<U32>("BATCH_SET");

    InfProgram::maip_err_code errCode = in_program.inf_modify_user_batch_size(in_session_id, username, batchSet);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_processor_thread_count_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    U32 procThreads = in_request.get_kval<U32>("PROC_THREADS");

    InfProgram::maip_err_code errCode = in_program.inf_modify_user_processor_thread_count(in_session_id, username, procThreads);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_max_processor_thread_count_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    U32 maxProcThreads = in_request.get_kval<U32>("MAX_PROC_THREADS");

    InfProgram::maip_err_code errCode = in_program.inf_modify_user_max_processor_thread_count(in_session_id, username, maxProcThreads);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_system_prompt_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    mbase::string systemPromptString;
    const char_stream& dataStream = in_request.get_data();

    if(dataStream.buffer_length())
    {
        systemPromptString = std::move(mbase::string(dataStream.data(), dataStream.buffer_length()));
    }

    InfProgram::maip_err_code errCode = in_program.inf_modify_user_system_prompt(in_session_id, username, systemPromptString);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_make_superuser_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    mbase::string accessToken = in_request.get_kval<mbase::string>("ACCTOK");

    InfProgram::maip_err_code errCode = in_program.inf_modify_user_make_superuser(in_session_id, username, accessToken);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_unmake_superuser_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    mbase::string accessToken = in_request.get_kval<mbase::string>("ACCTOK");

    InfProgram::maip_err_code errCode = in_program.inf_modify_user_unmake_superuser(in_session_id, username, accessToken);
    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_accept_models_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    mbase::vector<mbase::string> models = in_request.get_kval<mbase::vector<mbase::string>>("MODEL");

    std::set<mbase::string> modelSet(models.begin(), models.end());

    mbase::vector<mbase::string> missingModels;

    InfProgram::maip_err_code errCode = in_program.inf_modify_user_accept_models(
        in_session_id,
        username,
        modelSet,
        missingModels
    );

    for(mbase::vector<mbase::string>::iterator It = missingModels.begin(); It != missingModels.end(); ++It)
    {
        out_packet.set_kval("MODEL", *It);
    }

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_user_set_authority_flags_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string username = in_request.get_kval<mbase::string>("USERNAME");
    mbase::vector<mbase::string> authFlags = in_request.get_kval<mbase::vector<mbase::string>>("AUTHFLAGS");

    InfProgram::maip_err_code errCode = in_program.inf_modify_user_set_authority_flags(
        in_session_id,
        username,
        authFlags
    );

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool create_model_description_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string originalName = in_request.get_kval<mbase::string>("ORIGINAL");
    mbase::string customName = in_request.get_kval<mbase::string>("CUSTOM");
    mbase::string description = in_request.get_kval<mbase::string>("DESC");
    mbase::string modelFile = in_request.get_kval<mbase::string>("FILE");
    mbase::vector<mbase::string> tags = in_request.get_kval<mbase::vector<mbase::string>>("TAGS");
    mbase::string category = in_request.get_kval<mbase::string>("CATEGORY");
    bool isEmbedding = in_request.get_kval<bool>("IS_EMBEDDING");
    bool isForcePrompt = in_request.get_kval<bool>("IS_FORCE_PROMPT");
    U32 ctxLength = in_request.get_kval<U32>("CTXLENGTH");
    
    const char_stream& systemPromptStream = in_request.get_data();
    mbase::string systemPromptString;

    if(systemPromptStream.buffer_length())
    {
        systemPromptString = std::move(mbase::string(systemPromptStream.data(), systemPromptStream.buffer_length()));
    }

    InfProgram::maip_err_code errCode = in_program.inf_create_model_description(
        in_session_id,
        originalName,
        customName,
        description,
        systemPromptString,
        modelFile,
        tags,
        category,
        isEmbedding,
        isForcePrompt,
        ctxLength
    );

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_model_original_model_name_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string modelName = in_request.get_kval<mbase::string>("MODEL");
    mbase::string originalName = in_request.get_kval<mbase::string>("ORIGINAL");

    InfProgram::maip_err_code errCode = in_program.inf_modify_original_model_name(
        in_session_id,
        modelName,
        originalName
    );

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_model_custom_model_name_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string modelName = in_request.get_kval<mbase::string>("MODEL");
    mbase::string customName = in_request.get_kval<mbase::string>("CUSTOM");

    InfProgram::maip_err_code errCode = in_program.inf_modify_custom_model_name(
        in_session_id,
        modelName,
        customName
    );

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_model_description_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string modelName = in_request.get_kval<mbase::string>("MODEL");
    mbase::string modelDescription = in_request.get_kval<mbase::string>("DESC");

    InfProgram::maip_err_code errCode = in_program.inf_modify_model_description(
        in_session_id,
        modelName,
        modelDescription
    );

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_model_system_prompt_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string modelName = in_request.get_kval<mbase::string>("MODEL");
    const char_stream& systemPromptStream = in_request.get_data();
    mbase::string systemPrompt;

    if(systemPromptStream.buffer_length())
    {
        systemPrompt = std::move(mbase::string(systemPromptStream.data(), systemPromptStream.buffer_length()));
    }

    InfProgram::maip_err_code errCode = in_program.inf_modify_model_system_prompt(
        in_session_id,
        modelName,
        systemPrompt
    );

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_model_model_file_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string modelName = in_request.get_kval<mbase::string>("MODEL");
    mbase::string category = in_request.get_kval<mbase::string>("CATEGORY");
    mbase::string modelFile = in_request.get_kval<mbase::string>("FILE");
    bool isEmbedding = in_request.get_kval<bool>("IS_EMBEDDING");

    InfProgram::maip_err_code errCode = in_program.inf_modify_model_model_file(
        in_session_id,
        modelName,
        modelFile,
        isEmbedding,
        category
    );

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_model_tags_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string modelName = in_request.get_kval<mbase::string>("MODEL");
    mbase::vector<mbase::string> tags = in_request.get_kval<mbase::vector<mbase::string>>("TAGS");
    
    InfProgram::maip_err_code errCode = in_program.inf_modify_model_tags(
        in_session_id,
        modelName,
        tags
    );

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool modify_model_context_length_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    mbase::string modelName = in_request.get_kval<mbase::string>("MODEL");
    U32 ctxLength = in_request.get_kval<U32>("CTXLENGTH");

    InfProgram::maip_err_code errCode = in_program.inf_modify_model_context_length(
        in_session_id,
        modelName,
        ctxLength
    );

    out_packet.set_response_message((U16)errCode);

    return true;
}

bool execution_set_input_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool execution_execute_input_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool execution_next_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

MBASE_END