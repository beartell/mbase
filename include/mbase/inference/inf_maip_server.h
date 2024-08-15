#ifndef MBASE_INF_MAIP_SERVER_H
#define MBASE_INF_MAIP_SERVER_H

#include <mbase/common.h>
#include <mbase/pc/pc_net_manager.h>

MBASE_BEGIN

class MBASE_API InfMaipServerBase : public mbase::PcNetTcpServer {
public:
	GENERIC on_accept(std::shared_ptr<PcNetPeerClient> out_peer) override;
	GENERIC on_data(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size) override;
	GENERIC on_disconnect(std::shared_ptr<PcNetPeerClient> out_peer) override;
private:
};

MBASE_END

#endif // !MBASE_INF_MAIP_SERVER_H
