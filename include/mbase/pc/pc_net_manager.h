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

class PcNetClient;
class PcNetServer;

class PcNetPacket {
	U16 mPacketSize;
	mbase::deep_char_stream mPacketContent;
};

class PcNetClient {
public:

	using size_type = SIZE_T;
	using socket_handle = SOCKET;

	virtual GENERIC on_connect() {};
	virtual GENERIC on_send() {};
	virtual GENERIC on_sendtotal() {};
	virtual GENERIC on_receive() {};
	virtual GENERIC on_disconnect() {};

	size_type send_data();

private:
	socket_handle mRawSocket;
	mbase::vector<PcNetPacket> mWriterPackets;
	mbase::vector<PcNetPacket> mReaderPackets;
};

class PcNetServer {
public:
	friend class PcNetClient;

	using size_type = SIZE_T;
	using socket_handle = SOCKET;
	using client_list = mbase::list<PcNetClient*>;

	enum class flags : U8 {
		NET_SERVER_SUCCESS,
		NET_SERVER_WARN_ALREADY_LISTENING,
	};

	virtual GENERIC on_listen() {};
	virtual GENERIC on_accept() {};
	virtual GENERIC on_data() {};
	virtual GENERIC on_datatotal() {};
	virtual GENERIC on_stop() {};

	bool is_listening() const noexcept;
	mbase::string get_address() const noexcept;
	I32 get_port() const noexcept;

	flags listen() noexcept;
	flags stop() noexcept;
	flags broadcast_message();

	GENERIC update_server();
private:
	bool mIsListening;
	socket_handle mRawSocket;
	client_list mConnectedClients;
};

class PcNetManager : public mbase::singleton<PcNetManager> {
public:
	PcNetManager() {}

	GENERIC create_connection(mbase::string in_addr, int in_port) {
		
	}

private:
};

MBASE_END

#endif // !MBASE_PC_NET_MANAGER_H
