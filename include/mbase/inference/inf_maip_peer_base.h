#ifndef MBASE_MAIP_PEER_BASE_H
#define MBASE_MAIP_PEER_BASE_H

#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_maip_user.h>
#include <mbase/pc/pc_net_manager.h>
#include <unordered_map>

MBASE_BEGIN

class MBASE_API InfMaipPeerBase {
public:
protected:
	std::shared_ptr<mbase::PcNetPeerClient> mPeer;
	InfMaipUser mMaipUser;
	U64 mContextCounter = 0;
};

MBASE_END

#endif // !MBASE_MAIP_PEER_BASE_H
