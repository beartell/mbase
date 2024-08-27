#ifndef MBASE_PC_NET_MANAGER_H
#define MBASE_PC_NET_MANAGER_H

#include <mbase/common.h>
#include <mbase/char_stream.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/list.h>
#include <mbase/vector.h>
#include <mbase/wsa_init.h>
#include <mbase/synchronization.h>
#include <mbase/framework/timers.h>
#include <memory>

MBASE_BEGIN

static const U16 gNetDefaultPacketSize = 32768; // 32KB

class PcNetClient;
class PcNetServer;
class PcNetPeerClient;

class MBASE_API PcNetReconnectTimerHandle : public mbase::time_interval {
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

struct MBASE_API PcNetPacket {
	PcNetPacket(U16 in_min_packet_size = gNetDefaultPacketSize) noexcept;
	PcNetPacket(const PcNetPacket& in_rhs) noexcept;
	PcNetPacket(PcNetPacket&& in_rhs) noexcept;

	PcNetPacket& operator=(const PcNetPacket& in_rhs) noexcept;
	PcNetPacket& operator=(PcNetPacket&& in_rhs) noexcept;

	~PcNetPacket() = default;

	U16 mPacketSize;
	mbase::deep_char_stream mPacketContent;
};

class MBASE_API PcNetPeerClient : public non_copyable {
public:
	using size_type = SIZE_T;
	using socket_handle = SOCKET;

	friend class PcNetServer;
	friend class PcNetManager;
	friend class PcNetClient;
	friend class PcNetTcpServer;

	enum class flags : U8 {
		NET_PEER_SUCCCES,
		NET_PEER_ERR_AWAITING_DATA,
		NET_PERR_ERR_INSUFFICENT_BUFFER_SIZE,
		NET_PEER_ERR_UNAVAILABLE,
		NET_PEER_ERR_DISCONNECTED,
		NET_PEER_ERR_ALREADY_PROCESSED,
		NET_PEER_ERR_DATA_IS_NOT_AVAILABLE,
		NET_PEER_ERR_INVALID_SIZE
	};

	PcNetPeerClient();
	PcNetPeerClient(PcNetPeerClient&& in_rhs) noexcept;
	~PcNetPeerClient();

	//PcNetPeerClient& operator=(PcNetPeerClient&& in_rhs);

	MBASE_ND(MBASE_OBS_IGNORE) bool is_read_ready() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_processed() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_connected() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_available() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_awaiting_connection() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_peer_addr() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) I32 get_peer_port() const noexcept;

	flags write_data(CBYTEBUFFER in_data, size_type in_size); // TODO: CHECK NET PACKET OVERFLOW
	flags read_data(IBYTEBUFFER& out_data, size_type& out_size);
	flags finish();
	flags finish_and_ready();
	flags disconnect();

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

class MBASE_API PcNetPeerUdpClient : public non_copyable { // UNFINISHED IMPLEMENTATION, UNSAFE TO USE
public:
	using size_type = SIZE_T;

	enum class flags : U8 {
		NET_PEER_UDP_SUCCCES,
		NET_PEER_UDP_ERR_AWAITING_DATA,
		NET_PERR_UDP_ERR_INSUFFICENT_BUFFER_SIZE,
		NET_PEER_UDP_ERR_UNAVAILABLE,
		NET_PEER_UDP_ERR_DISCONNECTED,
		NET_PEER_UDP_ERR_ALREADY_PROCESSED,
		NET_PEER_UDP_ERR_DATA_IS_NOT_AVAILABLE,
		NET_PEER_UDP_ERR_INVALID_SIZE
	};

	PcNetPeerUdpClient();
	PcNetPeerUdpClient(PcNetPeerUdpClient&& in_rhs) noexcept;
	~PcNetPeerUdpClient();

	MBASE_ND(MBASE_OBS_IGNORE) bool is_read_ready() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_processed() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_available() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_peer_addr() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) I32 get_peer_port() const noexcept;

	flags write_data(CBYTEBUFFER in_data, size_type in_size); // TODO: CHECK NET PACKET OVERFLOW
	flags read_data(IBYTEBUFFER& out_data, size_type& out_size);
	flags finish();
	flags finish_and_ready();

	bool operator==(const PcNetPeerUdpClient& in_rhs);
	bool operator!=(const PcNetPeerUdpClient& in_rhs);

private:
	struct sockaddr_in mSockAddr;
	bool mIsProcessed;
	bool mIsReadReady;
	mbase::string mPeerAddr;
	I32 mPeerPort;
	PcNetPacket mNetPacket;
};

class MBASE_API PcNetClient {
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

class MBASE_API PcNetServer : public non_copymovable {
public:
	using size_type = SIZE_T;
	using socket_handle = SOCKET;

	friend class PcNetManager;

	enum class flags : U8 {
		NET_SERVER_SUCCESS,
		NER_SERVER_ERR_SOCKET_NOT_SET,
		NET_SERVER_WARN_ALREADY_LISTENING
	};

	PcNetServer();
	~PcNetServer();

	virtual GENERIC on_listen() = 0;
	virtual GENERIC on_stop() = 0;

	MBASE_ND(MBASE_OBS_IGNORE) bool is_listening() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_address() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) I32 get_port() const noexcept;

	flags listen() noexcept;
	flags stop() noexcept;

	virtual GENERIC update() = 0;
	virtual GENERIC update_t() = 0;

protected:
	bool mIsListening;
	socket_handle mRawSocket;
	mbase::string mAddr;
	I32 mPort;
};

class MBASE_API PcNetTcpServer : public PcNetServer {
public:
	using client_list = mbase::list<std::shared_ptr<PcNetPeerClient>>;
	using accept_clients = mbase::vector<std::shared_ptr<PcNetPeerClient>>;
	using data_clients = mbase::vector<std::shared_ptr<PcNetPeerClient>>;

	MBASE_ND(MBASE_OBS_IGNORE) const client_list& get_connected_peers() const noexcept;

	virtual GENERIC on_accept(std::shared_ptr<PcNetPeerClient> out_peer) = 0;
	virtual GENERIC on_data(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size) = 0;
	virtual GENERIC on_disconnect(std::shared_ptr<PcNetPeerClient> out_peer) = 0;

	GENERIC accept();
	GENERIC update() override;
	GENERIC update_t() override;

private:
	client_list mConnectedClients;
	accept_clients mAcceptClients;
	data_clients mDataClients;
	mbase::mutex mAcceptMutex;
};

class MBASE_API PcNetUdpServer : public PcNetServer {
public:
	using client_list = mbase::vector<std::shared_ptr<PcNetPeerUdpClient>>;
	using client_write_list = mbase::list<std::shared_ptr<PcNetPeerUdpClient>>;
	using client_read_list = mbase::list<std::shared_ptr<PcNetPeerUdpClient>>;

	PcNetUdpServer();
	~PcNetUdpServer();

	virtual GENERIC on_data(std::shared_ptr<PcNetPeerUdpClient> out_peer, CBYTEBUFFER out_data, size_type out_size) = 0;
	GENERIC update() override;
	GENERIC update_t() override;

private:
	client_list mActiveClients;
	client_write_list mWriteClients;
	client_read_list mReadClients;
	mbase::deep_char_stream mReaderStream;
};

class MBASE_API PcNetManager : public mbase::singleton<PcNetManager> {
public:
	enum class flags : U8 {
		NET_MNG_SUCCESS,
		NET_MNG_ERR_ADDR_IN_USE,
		NET_MNG_ERR_HOST_NOT_FOUND,
		NET_MNG_ERR_AWAITING_PREVIOUS_CONNECTION,
		NET_MNG_ERR_UNKNOWN
	};

	enum class protocol : U8 {
		NET_MNG_PROTOCOL_TCP,
		NET_MNG_PROTOCOL_UDP
	};

	PcNetManager() {}
	flags create_connection(const mbase::string& in_addr, I32 in_port, PcNetClient& out_client);
	flags create_server(const mbase::string& in_addr, I32 in_port, PcNetServer& out_server);

private:
	mbase::list<PcNetServer*> mServers;
};

MBASE_END

#endif // !MBASE_PC_NET_MANAGER_H
