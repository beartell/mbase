#ifndef MBASE_PC_NET_MANAGER_H
#define MBASE_PC_NET_MANAGER_H

#include <mbase/common.h>
#include <mbase/char_stream.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/list.h>
#include <mbase/vector.h>
#include <mbase/synchronization.h>
#include <mbase/framework/timers.h>
#include <mbase/framework/logical_processing.h>
#include <memory>

MBASE_BEGIN

static const U16 gNetDefaultPacketSize = 32768; // 32KB

class PcNetClient;
class PcNetServer;
class PcNetPeerClient;

struct MBASE_API PcNetPacket {
	PcNetPacket(U16 in_min_packet_size = gNetDefaultPacketSize) noexcept;
	PcNetPacket(const PcNetPacket& in_rhs) noexcept;
	PcNetPacket(PcNetPacket&& in_rhs) noexcept;

	PcNetPacket& operator=(const PcNetPacket& in_rhs) noexcept;
	PcNetPacket& operator=(PcNetPacket&& in_rhs) noexcept;

	~PcNetPacket() = default;

	mbase::deep_char_stream mPacketContent;
	mbase::string mWriteBuffer;
};

class MBASE_API PcNetPeerClient : public non_copyable {
public:
	using size_type = SIZE_T;
	#ifdef MBASE_PLATFORM_WINDOWS
	using socket_handle = SOCKET;
	#endif

	#ifdef MBASE_PLATFORM_UNIX
	using socket_handle = I32;
	#endif
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

	PcNetPeerClient(socket_handle in_socket);
	PcNetPeerClient(PcNetPeerClient&& in_rhs) noexcept;
	~PcNetPeerClient();

	PcNetPeerClient& operator=(PcNetPeerClient&& in_rhs);

	MBASE_ND(MBASE_OBS_IGNORE) bool signal_read() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_read_state() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_write() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_write_state() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_disconnect() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_disconnect_state() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_connected() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_peer_addr() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) I32 get_peer_port() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) socket_handle get_raw_socket() const noexcept;

	flags write_data(CBYTEBUFFER in_data, size_type in_size); // TODO: CHECK NET PACKET OVERFLOW
	flags send_read_signal();
	flags send_write_signal();
	flags disconnect();

	bool operator==(const PcNetPeerClient& in_rhs);
	bool operator!=(const PcNetPeerClient& in_rhs);

private:
	GENERIC _destroy_peer() noexcept;
	GENERIC _set_new_socket_handle(socket_handle in_socket) noexcept;

	socket_handle mPeerSocket;
	processor_signal mReadSignal;
	processor_signal mWriteSignal;
	processor_signal mDisconnectSignal;
	mbase::string mPeerAddr;
	I32 mPeerPort;
	PcNetPacket mNetPacket;
};

class MBASE_API PcNetServer : public non_copymovable {
public:
	using size_type = SIZE_T;
	#ifdef MBASE_PLATFORM_WINDOWS
	using socket_handle = SOCKET;
	#endif
	
	#ifdef MBASE_PLATFORM_UNIX
	using socket_handle = I32;
	#endif

	friend class PcNetManager;

	enum class flags : U8 {
		NET_SERVER_SUCCESS,
		NET_SERVER_ERR_SOCKET_NOT_SET,
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
	using accept_clients = mbase::list<std::shared_ptr<PcNetPeerClient>>;

	MBASE_ND(MBASE_OBS_IGNORE) const client_list& get_connected_peers() const noexcept;
	bool signal_accepting();
	bool signal_state_accepting();
	bool signal_processing_data();
	bool signal_state_processing_data();

	virtual GENERIC on_accept(std::shared_ptr<PcNetPeerClient> out_peer) = 0;
	virtual GENERIC on_data(std::shared_ptr<PcNetPeerClient> out_peer, CBYTEBUFFER out_data, size_type out_size) = 0;
	virtual GENERIC on_disconnect(std::shared_ptr<PcNetPeerClient> out_peer) = 0;

	GENERIC accept();
	GENERIC update() override;
	GENERIC update_t() override;

private:
	accept_clients mAcceptClients;
	client_list mConnectedClients;
	client_list mConnectedClientsProcessLoop;
	mbase::mutex mAcceptMutex;

	processor_signal mConnectionAccept;
	processor_signal mDataProcess;
};


class MBASE_API PcNetManager : public logical_processor {
public:
	using servers_list = mbase::list<PcNetServer*>;

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
	// flags create_connection(const mbase::string& in_addr, I32 in_port, PcNetClient& out_client);
	flags create_server(const mbase::string& in_addr, I32 in_port, PcNetServer& out_server);

	GENERIC update() override;
	GENERIC update_t() override;

private:
	servers_list mServers;
};

MBASE_END
#endif // !MBASE_PC_NET_MANAGER_H
