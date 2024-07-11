#ifndef MBASE_PC_NET_MANAGER_H
#define MBASE_PC_NET_MANAGER_H

#include <mbase/common.h>
#include <mbase/char_stream.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/list.h>
#include <mbase/vector.h>
#include <mbase/wsa_init.h>
#include <mbase/framework/timers.h>

MBASE_BEGIN

static const U16 gNetDefaultPacketSize = 32768; // 32KB

class PcNetClient;
class PcNetServer;
class PcNetPeerClient;

class PcNetReconnectTimerHandle : public mbase::time_interval {
public:
	PcNetReconnectTimerHandle(PcNetClient& in_client);
	~PcNetReconnectTimerHandle() = default;

	MBASE_ND(MBASE_OBS_IGNORE) U32 get_max_attempt_count() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) U32 get_attempt_count() const noexcept;

	GENERIC set_max_attempt_count(U32 in_rcc_attempt) noexcept;
	GENERIC on_call(user_data in_data) override;

private:
	PcNetClient* mClient;
	U32 mAttemptCount;
};

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
	friend class PcNetManager;
	friend class PcNetClient;

	enum class flags : U8 {
		NET_PEER_SUCCCES,
		NET_PEER_ERR_AWAITING_DATA,
		NET_PERR_ERR_INSUFFICENT_BUFFER_SIZE,
		NET_PEER_ERR_UNAVAILABLE,
		NET_PEER_ERR_DISCONNECTED,
		NET_PEER_ERR_ALREADY_PROCESSED
	};

	PcNetPeerClient();
	PcNetPeerClient(PcNetPeerClient&& in_rhs) noexcept;
	~PcNetPeerClient();

	MBASE_ND(MBASE_OBS_IGNORE) bool is_read_ready() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_processed() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_connected() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_awaiting_connection() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_peer_addr() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) I32 get_peer_port() const noexcept;

	flags write_data(CBYTEBUFFER in_data, size_type in_size);
	flags finish();
	flags finish_and_ready();

	bool operator==(const PcNetPeerClient& in_rhs);
	bool operator!=(const PcNetPeerClient& in_rhs);

private:
	GENERIC _destroy_peer() noexcept;
	GENERIC _set_new_socket_handle(socket_handle in_socket) noexcept;

	socket_handle mPeerSocket;
	bool mIsProcessed;
	bool mIsReadReady;
	bool mIsConnected;
	bool mIsConnecting;
	mbase::string mPeerAddr;
	I32 mPeerPort;
	PcNetPacket mNetPacket;
};

class PcNetClient {
public:
	using size_type = SIZE_T;
	using socket_handle = SOCKET;

	friend class PcNetManager;

	PcNetClient();

	MBASE_ND(MBASE_OBS_IGNORE) PcNetPeerClient& get_peer_client();
	MBASE_ND(MBASE_OBS_IGNORE) bool is_reconnect_set() const noexcept;

	virtual GENERIC on_connect(PcNetPeerClient& out_client) = 0;
	virtual GENERIC on_send(PcNetPeerClient& out_client, CBYTEBUFFER out_data, size_type out_size) = 0;
	virtual GENERIC on_receive(PcNetPeerClient& out_client, CBYTEBUFFER out_data, size_type out_size) = 0;
	virtual GENERIC on_disconnect() = 0;

	GENERIC update();
	GENERIC set_reconnect_params(U32 in_reconnect_attempt, U32 in_interval_ms = 1000);

private:
	PcNetPeerClient mConnectedClient;
	PcNetReconnectTimerHandle mReconnector;
	bool mIsReconnect;
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

	virtual GENERIC on_listen() = 0;
	virtual GENERIC on_accept(PcNetPeerClient& out_peer) = 0;
	virtual GENERIC on_data(PcNetPeerClient& out_peer, CBYTEBUFFER out_data, size_type out_size) = 0;
	virtual GENERIC on_disconnect(PcNetPeerClient& out_peer) = 0;
	virtual GENERIC on_stop() = 0;

	MBASE_ND(MBASE_OBS_IGNORE) bool is_listening() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_address() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) I32 get_port() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) const client_list& get_connected_peers() const noexcept;

	flags listen() noexcept;
	flags stop() noexcept;
	flags broadcast_message();

	GENERIC accept();
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
		NET_MNG_ERR_AWAITING_PREVIOUS_CONNECTION,
		NET_MNG_ERR_UNKNOWN
	};

	PcNetManager() {}
	flags create_connection(const mbase::string& in_addr, I32 in_port, PcNetClient& out_client);
	flags create_server(const mbase::string& in_addr, I32 in_port, PcNetServer& out_server);

private:
	mbase::list<PcNetServer*> mServers;
};

MBASE_END

#endif // !MBASE_PC_NET_MANAGER_H
