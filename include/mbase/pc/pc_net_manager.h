#ifndef MBASE_PC_NET_MANAGER_H
#define MBASE_PC_NET_MANAGER_H

#include <mbase/common.h>
#include <mbase/char_stream.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/list.h>
#include <mbase/vector.h>

#include <mbase/wsa_init.h>

MBASE_BEGIN

static const U16 gNetDefaultPacketSize = 32768; // 32KB

class PcNetClient;
class PcNetServer;
class PcNetPeerClient;

struct PcNetPacket {
	PcNetPacket(U16 in_min_packet_size = 32768) noexcept;
	PcNetPacket(const PcNetPacket& in_rhs) noexcept;
	PcNetPacket(PcNetPacket&& in_rhs) noexcept;

	PcNetPacket& operator=(const PcNetPacket& in_rhs) noexcept;
	PcNetPacket& operator=(PcNetPacket&& in_rhs) noexcept;

	~PcNetPacket() = default;

	U16 mPacketSize;
	mbase::deep_char_stream mPacketContent;
};

class PcNetPeerClient {
public:
	using size_type = SIZE_T;
	using socket_handle = SOCKET;

	friend class PcNetServer;

	enum class flags : U8 {
		NET_PEER_SUCCCES,
		NET_PEER_ERR_NOT_READY,
		NET_PERR_ERR_INSUFFICENT_BUFFER_SIZE,
		NET_PEER_ERR_UNAVAILABLE,
		NET_PEER_ERR_DISCONNECTED
	};

	PcNetPeerClient();
	PcNetPeerClient(PcNetPeerClient&& in_rhs) noexcept;

	bool is_ready() noexcept;
	bool is_processed() noexcept;
	bool is_connected() noexcept;

	flags write_data(IBYTEBUFFER in_data, size_type in_size);
	flags finish();

private:
	socket_handle mPeerSocket;
	bool mIsProcessed;
	bool mIsReady;
	bool mIsConnected;
	PcNetPacket mNetPacket;
};

class PcNetClient {
public:
	using size_type = SIZE_T;
	using socket_handle = SOCKET;

	friend class PcNetManager;

	PcNetPeerClient& get_peer_client();

	virtual GENERIC on_connect(PcNetPeerClient& out_client);
	virtual GENERIC on_send(PcNetPeerClient& out_client, char_stream& out_stream, size_type out_size);
	virtual GENERIC on_receive(PcNetPeerClient& out_client, char_stream& out_stream, size_type out_size);
	virtual GENERIC on_disconnect();

private:
	socket_handle mRawSocket;
	PcNetPeerClient mConnectedClient;
};

class PcNetServer {
public:
	using size_type = SIZE_T;
	using socket_handle = SOCKET;
	using client_list = mbase::list<PcNetPeerClient>;

	friend class PcNetManager;

	enum class flags : U8 {
		NET_SERVER_SUCCESS,
		NET_SERVER_WARN_ALREADY_LISTENING,
	};

	PcNetServer();
	~PcNetServer();

	virtual GENERIC on_listen();
	virtual GENERIC on_accept(PcNetPeerClient& out_peer);
	virtual GENERIC on_data(PcNetPeerClient& out_peer, char_stream& out_data, size_type out_size);
	virtual GENERIC on_disconnect(PcNetPeerClient& out_peer);
	virtual GENERIC on_stop();

	bool is_listening() const noexcept;
	mbase::string get_address() const noexcept;
	I32 get_port() const noexcept;

	flags listen() noexcept;
	flags stop() noexcept;
	flags broadcast_message();

	GENERIC accept(mbase::vector<PcNetPeerClient*>& out_clients);
	GENERIC update();

private:
	bool mIsListening;
	socket_handle mRawSocket;
	client_list mConnectedClients;
	mbase::string mAddr;
	I32 mPort;
};

class PcNetManager : public mbase::singleton<PcNetManager> {
public:
	enum class flags : U8 {
		NET_MNG_SUCCESS,
		NET_MNG_ERR_ADDR_IN_USE,
		NET_MNG_ERR_HOST_NOT_FOUND,
		NET_MNG_ERR_UNKNOWN
	};

	PcNetManager() {}
	GENERIC create_connection(const mbase::string& in_addr, I32 in_port, PcNetClient& out_client);
	flags create_server(const mbase::string& in_addr, I32 in_port, PcNetServer& out_server);

private:
	mbase::list<PcNetServer*> mServers;
};

MBASE_END

#endif // !MBASE_PC_NET_MANAGER_H
