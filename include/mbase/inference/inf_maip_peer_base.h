#ifndef MBASE_MAIP_PEER_BASE_H
#define MBASE_MAIP_PEER_BASE_H

#include <mbase/inference/inf_common.h>
#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_maip_user.h>
#include <mbase/pc/pc_net_manager.h>
#include <unordered_map>

MBASE_BEGIN

class MBASE_API InfMaipPeerBase {
public:
	using registered_processor_map = std::unordered_map<InfProcessorBase*, U64>;
	using indexed_processor_map = std::unordered_map<U64, InfProcessorBase*>;

	InfMaipPeerBase(std::shared_ptr<mbase::PcNetPeerClient> in_peer, const mbase::string& in_maip_user, const inf_model_category& in_peer_category);
	virtual ~InfMaipPeerBase();

	std::shared_ptr<mbase::PcNetPeerClient> get_maip_peer() const;
	const mbase::string& get_maip_username() const;
	const inf_model_category& get_peer_category() const;
	InfProcessorBase* get_processor_by_id(const U64& in_id);
	mbase::vector<U64> get_processor_ids() const;

	GENERIC add_processor(InfProcessorBase* in_address, U64& out_context_id);
	GENERIC set_network_peer(std::shared_ptr<mbase::PcNetPeerClient> in_peer);
	GENERIC set_maip_username(const mbase::string& in_username);
	GENERIC remove_processor_by_address(InfProcessorBase* in_address);
	GENERIC remove_processor_by_id(const U64& in_id);

protected:
	GENERIC mandatory_processor_cleanup(); // Must be called on destructor of the derived
	registered_processor_map mRegisteredProcMap;
	indexed_processor_map mIndexedProcMap;
	std::shared_ptr<mbase::PcNetPeerClient> mPeer = NULL;
	mbase::string mMaipUsername;
	inf_model_category mPeerCategory;
	U64 mContextCounter = 1;
};

MBASE_END

#endif // !MBASE_MAIP_PEER_BASE_H
