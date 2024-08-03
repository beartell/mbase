#include <mbase/pc/pc_net_manager.h>
#include <mbase/pc/pc_program.h>
#include <sys/types.h>

MBASE_BEGIN

PcNetReconnectTimerHandle::PcNetReconnectTimerHandle(PcNetClient& in_client) : mClient(&in_client), mAttemptCount(0)
{

}

MBASE_ND(MBASE_OBS_IGNORE) U32 PcNetReconnectTimerHandle::get_max_attempt_count() const noexcept
{
	return this->get_tick_count();
}

MBASE_ND(MBASE_OBS_IGNORE) U32 PcNetReconnectTimerHandle::get_attempt_count() const noexcept
{
	return mAttemptCount;
}

GENERIC PcNetReconnectTimerHandle::set_max_attempt_count(U32 in_rcc_attempt) noexcept
{
	this->set_tick_limit(in_rcc_attempt);
}

GENERIC PcNetReconnectTimerHandle::on_call(user_data in_data)
{
	PcNetPeerClient& tempPeer = mClient->get_peer_client();
	mbase::timer_loop* tempTl = MBASE_PROGRAM_TIMER();
	if(tempPeer.is_connected())
	{
		mAttemptCount = 0;
		tempTl->unregister_timer(*this);
	}
	else
	{
		if(tempPeer.is_awaiting_connection())
		{
			// ALREADY TRYING TO ESTABLISH CONNECTION
			return;
		}
		
		PcNetManager* mNetMng = MBASE_PROGRAM_NET_MANAGER();
		if(mNetMng->create_connection(tempPeer.get_peer_addr(), tempPeer.get_peer_port(), *mClient) != mbase::PcNetManager::flags::NET_MNG_SUCCESS)
		{
			mAttemptCount++;
		}
	}
}

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

	return *this;
}

PcNetPacket& PcNetPacket::operator=(PcNetPacket&& in_rhs) noexcept
{
	mPacketSize = in_rhs.mPacketSize;
	mPacketContent = std::move(in_rhs.mPacketContent);

	in_rhs.mPacketSize = 0;
	return *this;
}

PcNetPeerClient::PcNetPeerClient() :
	mPeerSocket(INVALID_SOCKET), 
	mIsProcessed(false), 
	mIsReadReady(false),
	mIsConnected(false), 
	mIsConnecting(false),
	mNetPacket(), 
	mPeerAddr(), 
	mPeerPort(0)
{
}

PcNetPeerClient::PcNetPeerClient(PcNetPeerClient&& in_rhs) noexcept
{
	mPeerSocket = in_rhs.mPeerSocket;
	mIsProcessed = in_rhs.mIsProcessed;
	mIsReadReady = in_rhs.mIsReadReady;
	mIsConnected = in_rhs.mIsConnected;
	mIsConnecting = in_rhs.mIsConnecting;
	mPeerAddr = in_rhs.mPeerAddr;
	mPeerPort = in_rhs.mPeerPort;
	mNetPacket = std::move(in_rhs.mNetPacket);

	in_rhs.mPeerSocket = INVALID_SOCKET;
	in_rhs.mIsProcessed = false;
	in_rhs.mIsReadReady = false;
	in_rhs.mIsConnected = false;
	in_rhs.mIsConnecting = false;
	in_rhs.mPeerPort = 0;
}

PcNetPeerClient::~PcNetPeerClient()
{
	_destroy_peer();
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::is_read_ready() const noexcept
{
	return mIsReadReady;
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::is_processed() const noexcept
{
	return mIsProcessed;
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::is_connected() const noexcept
{
	return mIsConnected;
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::is_available() const noexcept
{
	if(mNetPacket.mPacketContent.get_pos())
	{
		return false; // means we are writing to buffer
	}

	if((!mIsProcessed && !mIsReadReady) && mNetPacket.mPacketSize)
	{
		return true;
	}
	return false;
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::is_awaiting_connection() const noexcept
{
	return mIsConnecting;
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string PcNetPeerClient::get_peer_addr() const noexcept
{
	return mPeerAddr;
}

MBASE_ND(MBASE_OBS_IGNORE) I32 PcNetPeerClient::get_peer_port() const noexcept
{
	return mPeerPort;
}

PcNetPeerClient::flags PcNetPeerClient::write_data(CBYTEBUFFER in_data, size_type in_size)
{
	if(!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}

	if(!in_size)
	{
		return flags::NET_PEER_ERR_INVALID_SIZE;
	}

	if (is_processed())
	{
		return flags::NET_PEER_ERR_ALREADY_PROCESSED;
	}

	if(is_read_ready())
	{
		return flags::NET_PEER_ERR_AWAITING_DATA;
	}

	/*if(mNetPacket.mPacketSize + in_size > gNetDefaultPacketSize)
	{
		return flags::NET_PERR_ERR_INSUFFICENT_BUFFER_SIZE;
	}*/

	mNetPacket.mPacketContent.put_buffern(in_data, in_size);
	return flags::NET_PEER_SUCCCES;
}

PcNetPeerClient::flags PcNetPeerClient::read_data(IBYTEBUFFER& out_data, size_type& out_size)
{
	if (!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}

	if(!is_available())
	{
		return flags::NET_PEER_ERR_DATA_IS_NOT_AVAILABLE;
	}

	out_data = mNetPacket.mPacketContent.get_buffer();
	out_size = mNetPacket.mPacketSize;

	return flags::NET_PEER_SUCCCES;
}

PcNetPeerClient::flags PcNetPeerClient::finish()
{
	if (!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}

	if (is_read_ready())
	{
		return flags::NET_PEER_ERR_AWAITING_DATA;
	}

	mIsProcessed = true;
	return flags::NET_PEER_SUCCCES;
}

PcNetPeerClient::flags PcNetPeerClient::finish_and_ready()
{
	if (!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}

	if (is_read_ready())
	{
		return flags::NET_PEER_ERR_AWAITING_DATA;
	}

	mIsProcessed = true;
	mIsReadReady = true;
	return flags::NET_PEER_SUCCCES;
}

PcNetPeerClient::flags PcNetPeerClient::disconnect()
{
	if (!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}
	_destroy_peer();
	return flags::NET_PEER_SUCCCES;
}

bool PcNetPeerClient::operator==(const PcNetPeerClient& in_rhs)
{
	return mPeerSocket == in_rhs.mPeerSocket;
}

bool PcNetPeerClient::operator!=(const PcNetPeerClient& in_rhs)
{
	return mPeerSocket != in_rhs.mPeerSocket;
}

GENERIC PcNetPeerClient::_destroy_peer() noexcept
{
	if(mPeerSocket != INVALID_SOCKET)
	{
		closesocket(mPeerSocket);
		mPeerSocket = INVALID_SOCKET;
	}

	mPeerSocket = INVALID_SOCKET;
	mIsProcessed = false;
	mIsReadReady = false;
	mIsConnected = false;
	mNetPacket.mPacketSize = 0;
	mNetPacket.mPacketContent.set_cursor_front();
}

GENERIC PcNetPeerClient::_set_new_socket_handle(socket_handle in_socket) noexcept
{
	_destroy_peer();
	mPeerSocket = in_socket;
	mIsConnected = true;
	mIsReadReady = false;
	mIsProcessed = false;
}

PcNetClient::PcNetClient() : mReconnector(*this), mIsReconnect(false) 
{
}

PcNetPeerUdpClient::PcNetPeerUdpClient() :
	mIsProcessed(false),
	mIsReadReady(false),
	mNetPacket(),
	mPeerAddr(),
	mPeerPort(0)
{
	struct sockaddr_in tmpAddr = { 0 };
	mSockAddr = tmpAddr;
}

PcNetPeerUdpClient::PcNetPeerUdpClient(PcNetPeerUdpClient&& in_rhs) noexcept
{
	mIsProcessed = in_rhs.mIsProcessed;
	mIsReadReady = in_rhs.mIsReadReady;
	mPeerAddr = in_rhs.mPeerAddr;
	mPeerPort = in_rhs.mPeerPort;
	mNetPacket = std::move(in_rhs.mNetPacket);
	mSockAddr = in_rhs.mSockAddr;

	in_rhs.mIsProcessed = false;
	in_rhs.mIsReadReady = false;
	in_rhs.mPeerPort = 0;
	in_rhs.mSockAddr = { 0 };
}

PcNetPeerUdpClient::~PcNetPeerUdpClient()
{
	// _destroy_peer()
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerUdpClient::is_read_ready() const noexcept
{
	return mIsReadReady;
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerUdpClient::is_processed() const noexcept
{
	return mIsProcessed;
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerUdpClient::is_available() const noexcept
{
	if (mNetPacket.mPacketContent.get_pos())
	{
		return false; // means we are writing to buffer
	}

	if ((!mIsProcessed && !mIsReadReady) && mNetPacket.mPacketSize)
	{
		return true;
	}
	return false;
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string PcNetPeerUdpClient::get_peer_addr() const noexcept
{
	return mPeerAddr;
}

MBASE_ND(MBASE_OBS_IGNORE) I32 PcNetPeerUdpClient::get_peer_port() const noexcept
{
	return mPeerPort;
}

PcNetPeerUdpClient::flags PcNetPeerUdpClient::write_data(CBYTEBUFFER in_data, size_type in_size)
{
	if (!in_size)
	{
		return flags::NET_PEER_UDP_ERR_INVALID_SIZE;
	}

	if (is_processed())
	{
		return flags::NET_PEER_UDP_ERR_ALREADY_PROCESSED;
	}

	if (is_read_ready())
	{
		return flags::NET_PEER_UDP_ERR_AWAITING_DATA;
	}

	mNetPacket.mPacketContent.put_buffern(in_data, in_size);
	return flags::NET_PEER_UDP_SUCCCES;
}

PcNetPeerUdpClient::flags PcNetPeerUdpClient::read_data(IBYTEBUFFER& out_data, size_type& out_size)
{
	if (!is_available())
	{
		return flags::NET_PEER_UDP_ERR_DATA_IS_NOT_AVAILABLE;
	}

	out_data = mNetPacket.mPacketContent.get_buffer();
	out_size = mNetPacket.mPacketSize;

	return flags::NET_PEER_UDP_SUCCCES;
}

PcNetPeerUdpClient::flags PcNetPeerUdpClient::finish()
{
	if (is_read_ready())
	{
		return flags::NET_PEER_UDP_ERR_AWAITING_DATA;
	}

	mIsProcessed = true;
	return flags::NET_PEER_UDP_SUCCCES;
}

PcNetPeerUdpClient::flags PcNetPeerUdpClient::finish_and_ready()
{
	if (is_read_ready())
	{
		return flags::NET_PEER_UDP_ERR_AWAITING_DATA;
	}

	mIsProcessed = true;
	mIsReadReady = true;
	return flags::NET_PEER_UDP_SUCCCES;
}

bool PcNetPeerUdpClient::operator==(const PcNetPeerUdpClient& in_rhs)
{
	// TODO, IMPLEMENT
	return true;
}

bool PcNetPeerUdpClient::operator!=(const PcNetPeerUdpClient& in_rhs)
{
	// TODO, IMPLEMENT
	return true;
}

MBASE_ND(MBASE_OBS_IGNORE) PcNetPeerClient& PcNetClient::get_peer_client()
{
	return mConnectedClient;
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetClient::is_reconnect_set() const noexcept
{
	return mIsReconnect;
}

GENERIC PcNetClient::update()
{
	if(mConnectedClient.is_connected())
	{
		if(mConnectedClient.is_processed())
		{
			CBYTEBUFFER bytesToSend = mConnectedClient.mNetPacket.mPacketContent.get_buffer();
			SIZE_T bytesLength = mConnectedClient.mNetPacket.mPacketContent.get_pos();

			if (bytesLength)
			{
				I32 sResult = send(mConnectedClient.mPeerSocket, bytesToSend, bytesLength, 0);
				if (sResult == SOCKET_ERROR)
				{
					I32 socketLastError = WSAGetLastError();
					if (socketLastError == WSAEWOULDBLOCK || socketLastError == WSAEINPROGRESS)
					{
						return;
					}
					else
					{
						// DO THIS OPERATION ON THE LOGIC LOOP
						mConnectedClient._destroy_peer();
						on_disconnect();
						if(is_reconnect_set())
						{
							mbase::timer_loop* tempTl = MBASE_PROGRAM_TIMER();
							tempTl->register_timer(mReconnector);
						}
						return;
					}
				}
				else
				{
					// DO THIS OPERATION ON THE LOGIC LOOP
					on_send(mConnectedClient, mConnectedClient.mNetPacket.mPacketContent.get_buffer(), sResult);
					mConnectedClient.mIsProcessed = false;
					mConnectedClient.mNetPacket.mPacketContent.set_cursor_front();
				}
			}
		}

		if (mConnectedClient.is_read_ready())
		{
			IBYTEBUFFER bytesToRead = mConnectedClient.mNetPacket.mPacketContent.data();
			I32 rResult = recv(mConnectedClient.mPeerSocket, bytesToRead, gNetDefaultPacketSize, 0);
			if (rResult == SOCKET_ERROR)
			{
				I32 socketLastError = WSAGetLastError();
				if (socketLastError == WSAEWOULDBLOCK || socketLastError == WSAEINPROGRESS)
				{
					return;
				}
				else
				{
					mConnectedClient._destroy_peer();
					on_disconnect();
					if (is_reconnect_set())
					{
						mbase::timer_loop* tempTl = MBASE_PROGRAM_TIMER();
						tempTl->register_timer(mReconnector);
					}
					return;
				}
			}
			else
			{
				// DATA RECEIVED
				mConnectedClient.mIsReadReady = false;
				mConnectedClient.mIsProcessed = false;
				mConnectedClient.mNetPacket.mPacketSize = rResult;
				on_receive(mConnectedClient, mConnectedClient.mNetPacket.mPacketContent.get_buffer(), rResult);
			}
		}
	}
}

GENERIC PcNetClient::set_reconnect_params(U32 in_reconnect_attempt, U32 in_interval_ms)
{
	if(is_reconnect_set())
	{
		mReconnector.reset_tick_counter();
	}

	mIsReconnect = true;
	mReconnector.set_tick_limit(in_reconnect_attempt);
	mReconnector.set_target_time(in_interval_ms, mbase::timer_base::flags::TIMER_POLICY_ASYNC);
}

PcNetServer::PcNetServer() : 
	mIsListening(false), 
	mRawSocket(INVALID_SOCKET), 
	mAddr(""), 
	mPort(0)
{
}

PcNetServer::~PcNetServer()
{
	
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetServer::is_listening() const noexcept
{
	return mIsListening;
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string PcNetServer::get_address() const noexcept
{
	return mAddr;
}

MBASE_ND(MBASE_OBS_IGNORE) I32 PcNetServer::get_port() const noexcept
{
	return mPort;
}

PcNetServer::flags PcNetServer::listen() noexcept
{
	if(mRawSocket == INVALID_SOCKET || mRawSocket == SOCKET_ERROR)
	{
		return flags::NER_SERVER_ERR_SOCKET_NOT_SET;
	}

	if(mIsListening)
	{
		return flags::NET_SERVER_WARN_ALREADY_LISTENING;
	}

	mIsListening = true;
	on_listen();
	return flags::NET_SERVER_SUCCESS;
}

PcNetServer::flags PcNetServer::stop() noexcept
{
	if (mRawSocket == INVALID_SOCKET || mRawSocket == SOCKET_ERROR)
	{
		return flags::NER_SERVER_ERR_SOCKET_NOT_SET;
	}

	mIsListening = false;
	on_stop();
	return flags::NET_SERVER_SUCCESS;
}

MBASE_ND(MBASE_OBS_IGNORE) typename const PcNetTcpServer::client_list& PcNetTcpServer::get_connected_peers() const noexcept
{
	return mConnectedClients;
}

GENERIC PcNetTcpServer::accept()
{
	if(is_listening())
	{
		SOCKET resultClient = ::accept(mRawSocket, NULL, NULL);
		if (resultClient == INVALID_SOCKET)
		{
			I32 wsaError = WSAGetLastError();
			if(wsaError == WSAEWOULDBLOCK || wsaError == WSAEINPROGRESS)
			{
				
			}
			else
			{
				// TODO: DESTROY THE ENTIRE SERVER
			}
		}
		else
		{
			u_long ctlMode = 1;
			ioctlsocket(resultClient, FIONBIO, &ctlMode);

			PcNetPeerClient newClient;
			newClient.mPeerSocket = resultClient;
			newClient.mIsConnected = true;
			newClient.mIsProcessed = false;
			newClient.mIsReadReady = false;
			
			mConnectedClients.push_back(std::make_shared<PcNetPeerClient>(std::move(newClient)));
			//mbase::lock_guard accMut(mAcceptMutex);
			//mAcceptClients.push_back(mConnectedClients.back());
			on_accept(mConnectedClients.back());
		}
	}
}

GENERIC PcNetTcpServer::update()
{
	mAcceptMutex.acquire();
	for(accept_clients::iterator It = mAcceptClients.begin(); It != mAcceptClients.end(); ++It)
	{

	}
	mAcceptMutex.release();
}

GENERIC PcNetTcpServer::update_t()
{
	for(client_list::iterator It = mConnectedClients.begin(); It != mConnectedClients.end();)
	{
		PcNetPeerClient& currentClient = *It->get();
		if(currentClient.is_processed())
		{
			CBYTEBUFFER bytesToSend = currentClient.mNetPacket.mPacketContent.get_buffer();
			U16 bytesLength = currentClient.mNetPacket.mPacketContent.get_pos();
			if(bytesLength)
			{
				I32 sResult = send(currentClient.mPeerSocket, bytesToSend, bytesLength, 0);
				if(sResult == SOCKET_ERROR)
				{
					I32 socketLastError = WSAGetLastError();
					if(socketLastError == WSAEWOULDBLOCK || socketLastError == WSAEINPROGRESS)
					{
						++It;
						continue;
					}
					currentClient._destroy_peer();
					on_disconnect(*It);
					It = mConnectedClients.erase(It);
					continue;
				}

				currentClient.mNetPacket.mPacketContent.set_cursor_front();
				currentClient.mNetPacket.mPacketSize = 0;
				currentClient.mIsProcessed = false;
				++It;
				continue;
			}
		}
		
		if(currentClient.is_read_ready())
		{
			IBYTEBUFFER bytesToReceive = currentClient.mNetPacket.mPacketContent.data();
			I32 rResult = recv(currentClient.mPeerSocket, bytesToReceive, gNetDefaultPacketSize, 0);
			if (rResult == SOCKET_ERROR || rResult == 0)
			{
				I32 socketLastError = WSAGetLastError();
				if (socketLastError == WSAEWOULDBLOCK)
				{
					++It;
					continue;
				}
				currentClient._destroy_peer();
				on_disconnect(*It);
				It = mConnectedClients.erase(It);
				continue;
			}
			else
			{
				currentClient.mIsReadReady = false;
				currentClient.mIsProcessed = false;
				currentClient.mNetPacket.mPacketSize = rResult;
				on_data(*It, currentClient.mNetPacket.mPacketContent.get_buffer(), rResult);
			}
		}
		++It;
	}
}

PcNetUdpServer::PcNetUdpServer() : PcNetServer(), mReaderStream(gNetDefaultPacketSize)
{
}

PcNetUdpServer::~PcNetUdpServer()
{

}

GENERIC PcNetUdpServer::update()
{

}

GENERIC PcNetUdpServer::update_t()
{
	if(is_listening())
	{
		struct sockaddr sckAddr;
		I32 addrLen = sizeof(sckAddr);
		I32 sResult = recvfrom(mRawSocket, mReaderStream.get_buffer(), gNetDefaultPacketSize, 0, &sckAddr, &addrLen);
		if (sResult == SOCKET_ERROR)
		{
			I32 socketLastError = WSAGetLastError();
			if (socketLastError == WSAEWOULDBLOCK || socketLastError == WSAEINPROGRESS)
			{
				
			}
		}
	}
}

PcNetManager::flags PcNetManager::create_connection(const mbase::string& in_addr, I32 in_port, PcNetClient& out_client)
{
	if(out_client.mConnectedClient.is_awaiting_connection())
	{
		return flags::NET_MNG_ERR_AWAITING_PREVIOUS_CONNECTION;
	}

	if(out_client.mConnectedClient.is_connected())
	{
		out_client.on_disconnect();
		out_client.mConnectedClient._destroy_peer();
	}

	SOCKET clientSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	out_client.mConnectedClient.mPeerAddr = in_addr;
	out_client.mConnectedClient.mPeerPort = in_port;
	out_client.mConnectedClient.mIsConnecting = true;

	getaddrinfo(in_addr.data(), mbase::string::from_format("%d", in_port).data(), &hints, &result);
	for(ptr = result; ptr != NULL; ptr=ptr->ai_next)
	{
		clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if(clientSocket == INVALID_SOCKET)
		{
			out_client.mConnectedClient.mIsConnecting = false;
			return flags::NET_MNG_ERR_HOST_NOT_FOUND;
		}

		int cnnResult = connect(clientSocket, ptr->ai_addr, ptr->ai_addrlen);
		if(cnnResult == SOCKET_ERROR)
		{
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	if(clientSocket == INVALID_SOCKET)
	{
		out_client.mConnectedClient.mIsConnecting = false;
		return flags::NET_MNG_ERR_HOST_NOT_FOUND;
	}

	out_client.mConnectedClient.mIsConnecting = false;
	u_long ctlMode = 1;
	ioctlsocket(clientSocket, FIONBIO, &ctlMode);
	
	out_client.mConnectedClient._set_new_socket_handle(clientSocket);
	out_client.on_connect(out_client.mConnectedClient);

	return flags::NET_MNG_SUCCESS;
}

PcNetManager::flags PcNetManager::create_server(const mbase::string& in_addr, I32 in_port, PcNetServer& out_server)
{
	SOCKET serverSocket = INVALID_SOCKET;
	
	struct addrinfo* result = NULL;
	struct addrinfo hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;

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

		printf("maxl isten %d", WSAGetLastError());
		// TODO: ERROR CHECKING WILL BE IMPLEMENTED
		closesocket(serverSocket);
		return flags::NET_MNG_ERR_UNKNOWN;
	}
	u_long ctlMode = 1;
	ioctlsocket(serverSocket, FIONBIO, &ctlMode);

	out_server.mRawSocket = serverSocket;
	out_server.mAddr = in_addr;
	out_server.mPort = in_port;

	return flags::NET_MNG_SUCCESS;
}

MBASE_END
