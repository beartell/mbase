#include <mbase/inference/inf_maip_callbacks.h>

MBASE_BEGIN

bool access_request_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool destroy_session_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool get_accessible_models_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool get_context_ids_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool create_context_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool clear_context_history_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool get_context_status_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool destroy_context_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool get_program_models_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool load_model_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool unload_model_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool create_new_user_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool delete_user_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_model_access_limit_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_maximum_context_length_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_batch_size_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_processor_thread_count_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_max_processor_thread_count_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_system_prompt_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_make_superuser_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_unmake_superuser_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_accept_models_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_user_set_authority_flags_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool create_model_description_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_model_original_model_name_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_model_custom_model_name_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_model_description_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_model_system_prompt_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_model_model_file_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_model_tags_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool modify_model_context_length_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool execution_set_input_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool execution_execute_input_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

bool execution_next_cb(std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet)
{
    return true;
}

MBASE_END