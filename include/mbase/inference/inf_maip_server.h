#ifndef MBASE_INF_MAIP_SERVER_H
#define MBASE_INF_MAIP_SERVER_H

#include <mbase/common.h>
#include <mbase/maip_parser.h>
#include <mbase/pc/pc_net_manager.h>

MBASE_BEGIN

#define MBASE_MAIP_INF_OP_TYPE "INF"
#define MBASE_MAIP_EXEC_OP_TYPE "EXEC"

class MBASE_API InfMaipServerBase : public mbase::PcNetTcpServer {
public:
	GENERIC on_accept(std::shared_ptr<PcNetPeerClient> out_peer) override;
	GENERIC on_data(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size) override;
	GENERIC on_disconnect(std::shared_ptr<PcNetPeerClient> out_peer) override;

	virtual GENERIC on_informatic_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer) = 0;
	virtual GENERIC on_execution_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer) = 0;
	virtual GENERIC on_custom_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer) = 0;
private:
	maip_peer_request mMaipPeerRequest;
};

MBASE_END

#endif // !MBASE_INF_MAIP_SERVER_H
