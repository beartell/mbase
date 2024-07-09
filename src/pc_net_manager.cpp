#include <mbase/pc/pc_net_manager.h>
#include <sys/types.h>

MBASE_BEGIN

PcNetPacket::PcNetPacket(U16 in_min_packet_size) noexcept : mPacketSize(in_min_packet_size), mPacketContent(in_min_packet_size)
{
}

PcNetPacket::PcNetPacket(const PcNetPacket& in_rhs) noexcept
{
	mPacketSize = in_rhs.mPacketSize;
	mPacketContent = in_rhs.mPacketContent;
}

PcNetPacket::PcNetPacket(PcNetPacket&& in_rhs) noexcept
{
	mPacketSize = in_rhs.mPacketSize;
	mPacketContent = std::move(in_rhs.mPacketContent);

	in_rhs.mPacketSize = 0;
}

PcNetPacket& PcNetPacket::operator=(const PcNetPacket& in_rhs) noexcept
{
	mPacketSize = in_rhs.mPacketSize;
	mPacketContent = in_rhs.mPacketContent;
}

PcNetPacket& PcNetPacket::operator=(PcNetPacket&& in_rhs) noexcept
{
	mPacketSize = in_rhs.mPacketSize;
	mPacketContent = std::move(in_rhs.mPacketContent);

	in_rhs.mPacketSize = 0;
}

PcNetPeerClient::PcNetPeerClient() : mPeerSocket(INVALID_SOCKET), mIsProcessed(false), mIsReady(false), mIsConnected(false), mNetPacket()
{
}

PcNetPeerClient::PcNetPeerClient(PcNetPeerClient&& in_rhs) noexcept
{
	mPeerSocket = in_rhs.mPeerSocket;
	mIsProcessed = in_rhs.mIsProcessed;
	mIsReady = in_rhs.mIsReady;
	mIsConnected = in_rhs.mIsConnected;
	mNetPacket = std::move(in_rhs.mNetPacket);

	in_rhs.mPeerSocket = INVALID_SOCKET;
	in_rhs.mIsProcessed = false;
	in_rhs.mIsReady = false;
	in_rhs.mIsConnected = false;
}

bool PcNetPeerClient::is_ready() noexcept
{
	return mIsReady;
}

bool PcNetPeerClient::is_processed() noexcept
{
	return mIsProcessed;
}

bool PcNetPeerClient::is_connected() noexcept
{
	return mIsConnected;
}

PcNetPeerClient::flags PcNetPeerClient::write_data(IBYTEBUFFER in_data, size_type in_size)
{
	if(!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}

	if(!is_ready())
	{
		return flags::NET_PEER_ERR_NOT_READY;
	}

	if(mNetPacket.mPacketSize + in_size > gNetDefaultPacketSize)
	{
		return flags::NET_PERR_ERR_INSUFFICENT_BUFFER_SIZE;
	}

	mNetPacket.mPacketContent.put_buffern(in_data, in_size);
	mNetPacket.mPacketSize += in_size;
	return flags::NET_PEER_SUCCCES;
}

PcNetPeerClient::flags PcNetPeerClient::finish()
{
	if (!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}

	if (!is_ready())
	{
		return flags::NET_PEER_ERR_NOT_READY;
	}

	mIsProcessed = true;
	mIsReady = false;
	return flags::NET_PEER_SUCCCES;
}

PcNetPeerClient& PcNetClient::get_peer_client()
{
	return mConnectedClient;
}

GENERIC PcNetClient::on_connect(PcNetPeerClient& out_client)
{

}

GENERIC PcNetClient::on_send(PcNetPeerClient& out_client, char_stream& out_stream, size_type out_size)
{

}

GENERIC PcNetClient::on_receive(PcNetPeerClient& out_client, char_stream& out_stream, size_type out_size)
{

}

GENERIC PcNetClient::on_disconnect()
{

}

PcNetServer::PcNetServer() : mIsListening(false), mRawSocket(INVALID_SOCKET), mConnectedClients(), mAddr(""), mPort(0)
{
}

PcNetServer::~PcNetServer()
{
	
}

GENERIC PcNetServer::on_listen() 
{
}

GENERIC PcNetServer::on_accept(PcNetPeerClient& out_peer) 
{
}

GENERIC PcNetServer::on_data(PcNetPeerClient& out_peer, char_stream& out_data, size_type out_size) 
{
}

GENERIC PcNetServer::on_disconnect(PcNetPeerClient& out_peer) 
{
}

GENERIC PcNetServer::on_stop() 
{
}

bool PcNetServer::is_listening() const noexcept
{
	return mIsListening;
}

mbase::string PcNetServer::get_address() const noexcept
{
	return mAddr;
}

I32 PcNetServer::get_port() const noexcept
{
	return mPort;
}

PcNetServer::flags PcNetServer::listen() noexcept
{
	if(mIsListening)
	{
		return flags::NET_SERVER_WARN_ALREADY_LISTENING;
	}

	mIsListening = true;
	return flags::NET_SERVER_SUCCESS;
}

PcNetServer::flags PcNetServer::stop() noexcept
{
	mIsListening = false;
	return flags::NET_SERVER_SUCCESS;
}

PcNetServer::flags PcNetServer::broadcast_message() 
{
	return flags::NET_SERVER_SUCCESS;
}

GENERIC PcNetServer::accept(mbase::vector<PcNetPeerClient*>& out_clients)
{

	if(is_listening())
	{
		SOCKET resultClient = ::accept(mRawSocket, NULL, NULL);
		if (resultClient == INVALID_SOCKET)
		{
			I32 wsaError = WSAGetLastError();
			if(wsaError == WSAEWOULDBLOCK)
			{
				
			}
			else
			{
				// TODO: DESTROY THE ENTIRE SERVER
			}
		}
		else
		{
			PcNetPeerClient newClient;
			newClient.mPeerSocket = resultClient;
			newClient.mIsConnected = true;
			newClient.mIsProcessed = false;
			newClient.mIsReady = true;

			mConnectedClients.push_back(std::move(newClient));
			PcNetPeerClient& lastClient = *mConnectedClients.end_node();
			on_accept(lastClient);
			out_clients.push_back(&lastClient);
		}
	}
}

GENERIC PcNetServer::update()
{
	for(client_list::iterator It = mConnectedClients.begin(); It != mConnectedClients.end(); It++)
	{
		PcNetPeerClient& currentClient = *It;
		if(currentClient.is_processed())
		{
			for(;;)
			{

			}
		}
	}
}

GENERIC PcNetManager::create_connection(const mbase::string& in_addr, I32 in_port, PcNetClient& out_client)
{

}

PcNetManager::flags PcNetManager::create_server(const mbase::string& in_addr, I32 in_port, PcNetServer& out_server)
{
	SOCKET serverSocket = INVALID_SOCKET;
	
	struct addrinfo* result = NULL;
	struct addrinfo hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	mbase::string portString = mbase::string::from_format("%d", in_port);

	I32 iResult = getaddrinfo(in_addr.data(), portString.data(), &hints, &result);
	if(iResult)
	{
		// TODO: BETTER ERROR CHECKING WILL BE IMPLEMENTED
		return flags::NET_MNG_ERR_HOST_NOT_FOUND;
	}
	
	serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(serverSocket == INVALID_SOCKET)
	{
		// TODO: ERROR CHECKING WILL BE IMPLEMENTED
		freeaddrinfo(result);
		return flags::NET_MNG_ERR_UNKNOWN;
	}

	iResult = bind(serverSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		// TODO: ERROR CHECKING WILL BE IMPLEMENTED
		freeaddrinfo(result);
		closesocket(serverSocket);
		return flags::NET_MNG_ERR_UNKNOWN;
	}

	freeaddrinfo(result);

	iResult = listen(serverSocket, SOMAXCONN);
	if(iResult == SOCKET_ERROR)
	{
		// TODO: ERROR CHECKING WILL BE IMPLEMENTED
		closesocket(serverSocket);
		return flags::NET_MNG_ERR_UNKNOWN;
	}
	u_long ctlMode = 1;
	ioctlsocket(serverSocket, FIONBIO, &ctlMode);

	out_server.mRawSocket = serverSocket;
	out_server.mAddr = in_addr;
	out_server.mPort = in_port;
	out_server.mIsListening = true;

	return flags::NET_MNG_SUCCESS;

}

MBASE_END
