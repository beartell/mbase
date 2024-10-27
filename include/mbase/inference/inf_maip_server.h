#ifndef MBASE_INF_MAIP_SERVER_H
#define MBASE_INF_MAIP_SERVER_H

#include <mbase/common.h>
#include <mbase/maip_parser.h>
#include <mbase/pc/pc_net_manager.h>
#include <mbase/inference/inf_program.h>
#include <unordered_map>

MBASE_BEGIN

#define MBASE_MAIP_INF_OP_TYPE "INF"
#define MBASE_MAIP_EXEC_OP_TYPE "EXEC"

struct MBASE_API InfMaipPacketAccumulator { // THIS IS NECESSARY IF THE GIVEN DATA IS BIGGER THEN 32KB
	U64 mDataToRead;
	mbase::string mSessionToken;
	mbase::string mAccumulatedData;
	maip_peer_request mPeerRequest;
};

class MBASE_API InfMaipServerBase : public mbase::PcNetTcpServer {
public:
	using accumulation_map = std::unordered_map<PcNetPeerClient::socket_handle, InfMaipPacketAccumulator>;

	GENERIC on_accept(std::shared_ptr<PcNetPeerClient> out_peer) override;
	GENERIC on_data(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size) override;
	GENERIC on_disconnect(std::shared_ptr<PcNetPeerClient> out_peer) override;

	virtual GENERIC on_informatic_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer) = 0;
	virtual GENERIC on_execution_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer) = 0;
	virtual GENERIC on_custom_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer) = 0;
private:

	GENERIC accumulated_processing(std::shared_ptr<PcNetPeerClient> out_peer, accumulation_map::iterator in_accum_iterator, CBYTEBUFFER out_data, size_type out_size);
	GENERIC simple_processing(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size);
	accumulation_map mAccumulationMap;
};

class MBASE_API InfMaipDefaultServer : public mbase::InfMaipServerBase {
public:
	InfMaipDefaultServer(InfProgram& in_program);
	~InfMaipDefaultServer() = default;

	GENERIC on_informatic_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer) override;
	GENERIC on_execution_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer) override;
	GENERIC on_custom_request(const maip_peer_request& out_request, std::shared_ptr<PcNetPeerClient> out_peer) override;

	GENERIC on_listen() override;
	GENERIC on_stop() override;
private:
	InfProgram* mHostProgram;
};

MBASE_END

#endif // !MBASE_INF_MAIP_SERVER_H
