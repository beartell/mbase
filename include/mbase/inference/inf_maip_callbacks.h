#ifndef MBASE_MAIP_CALLBACKS_H
#define MBASE_MAIP_CALLBACKS_H

#include <mbase/common.h>
#include <mbase/maip_parser.h>
#include <mbase/pc/pc_net_manager.h>
#include <mbase/inference/inf_program.h>
#include <memory>

MBASE_BEGIN

bool access_request_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool destroy_session_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool get_accessible_models_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool get_context_ids_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool create_context_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool clear_context_history_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool get_context_status_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool destroy_context_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool get_program_models_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool load_model_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool unload_model_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool create_new_user_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool delete_user_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_model_access_limit_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_maximum_context_length_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_batch_size_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_processor_thread_count_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_max_processor_thread_count_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_system_prompt_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_make_superuser_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_unmake_superuser_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_accept_models_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_user_set_authority_flags_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool create_model_description_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_model_original_model_name_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_model_custom_model_name_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_model_description_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_model_system_prompt_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_model_model_file_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_model_tags_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool modify_model_context_length_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool execution_set_input_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool execution_execute_input_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);
bool execution_next_cb(InfProgram& in_program, std::shared_ptr<PcNetPeerClient> in_peer, const maip_peer_request& in_request, const mbase::string& in_session_id, maip_packet_builder& out_packet);

MBASE_END

#endif // MBASE_MAIP_CALLBACKS_H