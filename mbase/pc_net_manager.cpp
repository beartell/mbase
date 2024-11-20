#include <mbase/pc/pc_net_manager.h>
#include <mbase/pc/pc_program.h>
#include <sys/types.h>

#ifdef MBASE_PLATFORM_UNIX
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#endif

MBASE_BEGIN

#ifdef MBASE_PLATFORM_WINDOWS
#define MBASE_INVALID_SOCKET INVALID_SOCKET
#define MBASE_SOCKET_ERROR SOCKET_ERROR
#endif

#ifdef MBASE_PLATFORM_UNIX
#define MBASE_INVALID_SOCKET -1
#define MBASE_SOCKET_ERROR -1
#endif


PcNetPacket::PcNetPacket(U16 in_min_packet_size) noexcept : mPacketContent(in_min_packet_size)
{
}

PcNetPacket::PcNetPacket(const PcNetPacket& in_rhs) noexcept
{
	mPacketContent = in_rhs.mPacketContent;
	mWriteBuffer = in_rhs.mWriteBuffer;
}

PcNetPacket::PcNetPacket(PcNetPacket&& in_rhs) noexcept
{
	mPacketContent = std::move(in_rhs.mPacketContent);
	mWriteBuffer = std::move(in_rhs.mWriteBuffer);
}

PcNetPacket& PcNetPacket::operator=(const PcNetPacket& in_rhs) noexcept
{
	mPacketContent = in_rhs.mPacketContent;
	mWriteBuffer = in_rhs.mWriteBuffer;
	return *this;
}

PcNetPacket& PcNetPacket::operator=(PcNetPacket&& in_rhs) noexcept
{
	mPacketContent = std::move(in_rhs.mPacketContent);
	mWriteBuffer = std::move(in_rhs.mWriteBuffer);
	return *this;
}

PcNetPeerClient::PcNetPeerClient(socket_handle in_socket) :
	mPeerSocket(in_socket), 
	mPeerAddr(), 
	mPeerPort(0),
	mNetPacket()
{
}

PcNetPeerClient::PcNetPeerClient(PcNetPeerClient&& in_rhs) noexcept
{
	mPeerSocket = in_rhs.mPeerSocket;
	mPeerPort = in_rhs.mPeerPort;

	in_rhs.mPeerSocket = MBASE_INVALID_SOCKET;
	in_rhs.mDisconnectSignal.reset_signal_with_state();
	in_rhs.mReadSignal.reset_signal_with_state();
	in_rhs.mWriteSignal.reset_signal_with_state();
	in_rhs.mNetPacket.mPacketContent.set_cursor_front();
	in_rhs.mNetPacket.mWriteBuffer.clear();
}

PcNetPeerClient::~PcNetPeerClient()
{
	_destroy_peer();
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::signal_read() const noexcept
{
	return mReadSignal.get_signal();
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::signal_read_state() const noexcept
{
	return mReadSignal.get_signal_state();
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::signal_write() const noexcept
{
	return mWriteSignal.get_signal();
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::signal_write_state() const noexcept
{
	return mWriteSignal.get_signal_state();
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::signal_disconnect() const noexcept
{
	return mDisconnectSignal.get_signal();
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::signal_disconnect_state() const noexcept 
{
	return mDisconnectSignal.get_signal_state();
}

MBASE_ND(MBASE_OBS_IGNORE) bool PcNetPeerClient::is_connected() const noexcept
{
	
	if(signal_disconnect() || mPeerSocket == MBASE_INVALID_SOCKET)
	{
		return false;
	}
	return true;
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string PcNetPeerClient::get_peer_addr() const noexcept
{
	return mPeerAddr;
}

MBASE_ND(MBASE_OBS_IGNORE) I32 PcNetPeerClient::get_peer_port() const noexcept
{
	return mPeerPort;
}

MBASE_ND(MBASE_OBS_IGNORE) typename PcNetPeerClient::socket_handle PcNetPeerClient::get_raw_socket() const noexcept
{
	return mPeerSocket;
}

PcNetPeerClient::flags PcNetPeerClient::write_data(CBYTEBUFFER in_data, size_type in_size)
{
	if(!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}

	if(!in_size || !in_data)
	{
		return flags::NET_PEER_ERR_INVALID_SIZE;
	}

	if (signal_write())
	{
		return flags::NET_PEER_ERR_ALREADY_PROCESSED;
	}

	mNetPacket.mWriteBuffer.clear();
	mNetPacket.mWriteBuffer.append(in_data, in_size);

	return flags::NET_PEER_SUCCCES;
}

PcNetPeerClient::flags PcNetPeerClient::send_write_signal()
{
	if (!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}

	if(!mNetPacket.mWriteBuffer.size())
	{
		return flags::NET_PEER_ERR_DATA_IS_NOT_AVAILABLE;
	}

	mWriteSignal.set_signal();
	return flags::NET_PEER_SUCCCES;
}

PcNetPeerClient::flags PcNetPeerClient::send_read_signal()
{
	if (!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}

	if(signal_read())
	{
		return flags::NET_PEER_SUCCCES;
	}

	mReadSignal.set_signal();
	return flags::NET_PEER_SUCCCES;
}

PcNetPeerClient::flags PcNetPeerClient::disconnect()
{
	if (!is_connected())
	{
		return flags::NET_PEER_ERR_DISCONNECTED;
	}
	mDisconnectSignal.set_signal();
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
	
	#ifdef MBASE_PLATFORM_WINDOWS
		if(mPeerSocket != INVALID_SOCKET)
		{
			closesocket(mPeerSocket);
			mPeerSocket = INVALID_SOCKET;
		}
		mPeerSocket = INVALID_SOCKET;
	#endif 
	#ifdef MBASE_PLATFORM_UNIX
		if(mPeerSocket != -1)
		{
			close(mPeerSocket);
			mPeerSocket = -1;
		}
		mPeerSocket = -1;
	#endif
	
	mDisconnectSignal.reset_signal_with_state();
	mReadSignal.reset_signal_with_state();
	mWriteSignal.reset_signal_with_state();
	mNetPacket.mPacketContent.set_cursor_front();
	mNetPacket.mWriteBuffer.clear();
}

GENERIC PcNetPeerClient::_set_new_socket_handle(socket_handle in_socket) noexcept
{
	_destroy_peer();
	mPeerSocket = in_socket;
}

PcNetServer::PcNetServer() : 
	mIsListening(false), 
	mRawSocket(MBASE_INVALID_SOCKET),
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
	#ifdef MBASE_PLATFORM_WINDOWS
	if(mRawSocket == INVALID_SOCKET || mRawSocket == SOCKET_ERROR)
	{
		return flags::NET_SERVER_ERR_SOCKET_NOT_SET;
	}
	#endif

	#ifdef MBASE_PLATFORM_UNIX
	if(mRawSocket == -1)
	{
		return flags::NET_SERVER_ERR_SOCKET_NOT_SET;
	}
	#endif

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
	#ifdef MBASE_PLATFORM_WINDOWS
	if(mRawSocket == INVALID_SOCKET || mRawSocket == SOCKET_ERROR)
	{
		return flags::NET_SERVER_ERR_SOCKET_NOT_SET;
	}
	#endif

	#ifdef MBASE_PLATFORM_UNIX
	if(mRawSocket == -1)
	{
		return flags::NET_SERVER_ERR_SOCKET_NOT_SET;
	}
	#endif

	mIsListening = false;
	on_stop();
	return flags::NET_SERVER_SUCCESS;
}

GENERIC PcNetServer::acquire_object_watcher(mbase::list_object_watcher<PcNetServer>* in_watcher)
{
	if(in_watcher)
	{
		mObjectWatcher = in_watcher;
		mObjectWatcher->mSubject = this;
	}
}

GENERIC PcNetServer::release_object_watcher()
{
	if(mObjectWatcher)
	{
		mObjectWatcher->mSubject = NULL;
	}
}

PcNetTcpServer::PcNetTcpServer()
{

}

PcNetTcpServer::~PcNetTcpServer()
{
	this->release_object_watcher();
}

MBASE_ND(MBASE_OBS_IGNORE) const typename PcNetTcpServer::client_list& PcNetTcpServer::get_connected_peers() const noexcept
{
	return mConnectedClients;
}

bool PcNetTcpServer::signal_accepting()
{
	return mConnectionAccept.get_signal();
}

bool PcNetTcpServer::signal_state_accepting()
{
	return mConnectionAccept.get_signal_state();
}

bool PcNetTcpServer::signal_processing_data()
{
	return mDataProcess.get_signal();
}

bool PcNetTcpServer::signal_state_processing_data()
{
	return mDataProcess.get_signal_state();
}

GENERIC PcNetTcpServer::accept()
{	
	socket_handle resultClient = ::accept(mRawSocket, NULL, NULL);
	if (resultClient == MBASE_INVALID_SOCKET)
	{
		#ifdef MBASE_PLATFORM_WINDOWS
		I32 wsaError = WSAGetLastError();
		if(wsaError == WSAEWOULDBLOCK || wsaError == WSAEINPROGRESS)
		{
			
		}
		else
		{
			// TODO: DESTROY THE ENTIRE SERVER
		}
		#endif

		#ifdef MBASE_PLATFORM_UNIX
		I32 socketError = errno;
		if(socketError == EWOULDBLOCK || socketError == EAGAIN)
		{
			
		}
		else
		{
			// TODO: DESTROY THE ENTIRE SERVER
		}
		#endif 	
	}
	else
	{
		u_long ctlMode = 1;
		
		#ifdef MBASE_PLATFORM_WINDOWS
		ioctlsocket(resultClient, FIONBIO, &ctlMode);
		#endif

		#ifdef MBASE_PLATFORM_UNIX
		ioctl(resultClient, FIONBIO, &ctlMode);
		#endif
		
		std::shared_ptr<PcNetPeerClient> connectedClient = std::make_shared<PcNetPeerClient>(PcNetPeerClient(resultClient));
		mConnectedClientsProcessLoop.push_back(connectedClient);
		mAcceptMutex.acquire();
		mAcceptClients.push_back(connectedClient);
		mAcceptMutex.release();
		mConnectionAccept.set_signal_with_state();
	}
}

GENERIC PcNetTcpServer::update()
{
	mAcceptMutex.acquire();
	for(accept_clients::iterator It = mAcceptClients.begin(); It != mAcceptClients.end();)
	{
		std::shared_ptr<PcNetPeerClient> netPeer = *It;
		mConnectedClients.push_back(netPeer);
		on_accept(netPeer);
		It = mAcceptClients.erase(It);
	}
	mAcceptMutex.release();

	for(client_list::iterator It = mConnectedClients.begin(); It != mConnectedClients.end();)
	{
		std::shared_ptr<PcNetPeerClient> netPeer = *It;
		if(!netPeer->is_connected())
		{
			on_disconnect(netPeer);
			It = mConnectedClients.erase(It);
			continue;
		}

		if(netPeer->signal_read_state())
		{
			CBYTEBUFFER inData = netPeer->mNetPacket.mPacketContent.get_buffer();
			size_type inDataLength = netPeer->mNetPacket.mPacketContent.buffer_length();
			netPeer->mNetPacket.mPacketContent.set_cursor_front();
			netPeer->mReadSignal.reset_signal_with_state();
			on_data(netPeer, inData, inDataLength);
		}

		++It;
	}
}

GENERIC PcNetTcpServer::update_t()
{
	if(this->is_listening())
	{
		this->accept();
	}

	for(client_list::iterator It = mConnectedClientsProcessLoop.begin(); It != mConnectedClientsProcessLoop.end();)
	{
		std::shared_ptr<PcNetPeerClient> netPeer = *It;
		if(!netPeer->is_connected() || netPeer->signal_disconnect())
		{
			netPeer->_destroy_peer();
			netPeer->mDisconnectSignal.reset_signal_with_state();
			It = mConnectedClientsProcessLoop.erase(It);
			continue;
		}

		if(netPeer->signal_read())
		{
			IBYTEBUFFER bytesToReceive = netPeer->mNetPacket.mPacketContent.data();
			I32 rResult = recv(netPeer->mPeerSocket, bytesToReceive, gNetDefaultPacketSize, 0);
			if(rResult == MBASE_SOCKET_ERROR)
			{
				#ifdef MBASE_PLATFORM_WINDOWS 
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// Something bad happened
					netPeer->_destroy_peer();
					It = mConnectedClientsProcessLoop.erase(It);
					continue;
				}
				#endif

				#ifdef MBASE_PLATFORM_UNIX
				if (errno != EWOULDBLOCK)
				{
					// Something bad happened
					netPeer->_destroy_peer();
					It = mConnectedClientsProcessLoop.erase(It);
					continue;
				}
				#endif
			}

			else if(!rResult)
			{
				netPeer->_destroy_peer();
				It = mConnectedClientsProcessLoop.erase(It);
				continue;
			}

			else
			{
				netPeer->mReadSignal.set_signal_state();
				netPeer->mReadSignal.reset_signal();
				netPeer->mNetPacket.mPacketContent.advance(rResult);
			}
		}

		if(netPeer->signal_write())
		{
			I32 sResult = send(netPeer->mPeerSocket, netPeer->mNetPacket.mWriteBuffer.c_str(), netPeer->mNetPacket.mWriteBuffer.size(), 0);
			if(sResult == MBASE_SOCKET_ERROR)
			{
				#ifdef MBASE_PLATFORM_WINDOWS 
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// Something bad happened
					netPeer->_destroy_peer();
					It = mConnectedClientsProcessLoop.erase(It);
					continue;
				}
				#endif

				#ifdef MBASE_PLATFORM_UNIX
				if (errno != EWOULDBLOCK)
				{
					// Something bad happened
					netPeer->_destroy_peer();
					It = mConnectedClientsProcessLoop.erase(It);
					continue;
				}
				#endif
			}
			
			else if(!sResult)
			{
				netPeer->_destroy_peer();
				It = mConnectedClientsProcessLoop.erase(It);
				continue;
			}

			else
			{
				netPeer->mWriteSignal.reset_signal_with_state();
				netPeer->mNetPacket.mWriteBuffer.clear();
			}
		}
		++It;
	}
}

//PcNetManager::flags PcNetManager::create_connection(const mbase::string& in_addr, I32 in_port, PcNetClient& out_client)
//{
//	if(out_client.mConnectedClient.is_awaiting_connection())
//	{
//		return flags::NET_MNG_ERR_AWAITING_PREVIOUS_CONNECTION;
//	}
//
//	if(out_client.mConnectedClient.is_connected())
//	{
//		out_client.on_disconnect();
//		out_client.mConnectedClient._destroy_peer();
//	}
//
//	SOCKET clientSocket = INVALID_SOCKET;
//	struct addrinfo* result = NULL;
//	struct addrinfo* ptr = NULL;
//	struct addrinfo hints = { 0 };
//
//	hints.ai_family = AF_INET;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
//
//	out_client.mConnectedClient.mPeerAddr = in_addr;
//	out_client.mConnectedClient.mPeerPort = in_port;
//	out_client.mConnectedClient.mIsConnecting = true;
//
//	getaddrinfo(in_addr.data(), mbase::string::from_format("%d", in_port).data(), &hints, &result);
//	for(ptr = result; ptr != NULL; ptr=ptr->ai_next)
//	{
//		clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
//		if(clientSocket == INVALID_SOCKET)
//		{
//			out_client.mConnectedClient.mIsConnecting = false;
//			return flags::NET_MNG_ERR_HOST_NOT_FOUND;
//		}
//
//		int cnnResult = connect(clientSocket, ptr->ai_addr, ptr->ai_addrlen);
//		if(cnnResult == SOCKET_ERROR)
//		{
//			closesocket(clientSocket);
//			clientSocket = INVALID_SOCKET;
//			continue;
//		}
//		break;
//	}
//
//	if(clientSocket == INVALID_SOCKET)
//	{
//		out_client.mConnectedClient.mIsConnecting = false;
//		return flags::NET_MNG_ERR_HOST_NOT_FOUND;
//	}
//
//	out_client.mConnectedClient.mIsConnecting = false;
//	u_long ctlMode = 1;
//	ioctlsocket(clientSocket, FIONBIO, &ctlMode);
//	
//	out_client.mConnectedClient._set_new_socket_handle(clientSocket);
//	out_client.on_connect(out_client.mConnectedClient);
//
//	return flags::NET_MNG_SUCCESS;
//}

PcNetManager::PcNetManager()
{
}

PcNetManager::~PcNetManager()
{
	stop_processor();
	for (servers_list::iterator It = mServers.begin(); It != mServers.end(); ++It)
	{
		if(It->mSubject)
		{
			It->mSubject->release_object_watcher();
		}
		
	}
}

PcNetManager::flags PcNetManager::create_server(const mbase::string& in_addr, I32 in_port, PcNetServer& out_server)
{
	#ifdef MBASE_PLATFORM_WINDOWS
	SOCKET serverSocket = MBASE_INVALID_SOCKET;
	#endif
	#ifdef MBASE_PLATFORM_UNIX
	I32 serverSocket = MBASE_INVALID_SOCKET;
	#endif

	struct addrinfo* result = NULL;
	struct addrinfo hints = {
		.ai_flags = 0,
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = IPPROTO_TCP,
		.ai_addrlen = 0,
		.ai_addr = NULL,
		.ai_canonname = NULL,
		.ai_next = NULL
	};

	mbase::string portString = mbase::string::from_format("%d", in_port);

	I32 iResult = getaddrinfo(in_addr.data(), portString.data(), &hints, &result);
	if(iResult)
	{
		// TODO: BETTER ERROR CHECKING WILL BE IMPLEMENTED
		return flags::NET_MNG_ERR_HOST_NOT_FOUND;
	}
	
	serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(serverSocket == MBASE_INVALID_SOCKET)
	{
		// TODO: ERROR CHECKING WILL BE IMPLEMENTED
		freeaddrinfo(result);
		return flags::NET_MNG_ERR_UNKNOWN;
	}

	iResult = bind(serverSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == MBASE_SOCKET_ERROR) 
	{
		// TODO: ERROR CHECKING WILL BE IMPLEMENTED
		freeaddrinfo(result);
		#ifdef MBASE_PLATFORM_WINDOWS
		closesocket(serverSocket);
		#endif
		#ifdef MBASE_PLATFORM_UNIX
		close(serverSocket);
		#endif
		return flags::NET_MNG_ERR_UNKNOWN;
	}

	freeaddrinfo(result);
	
	iResult = ::listen(serverSocket, SOMAXCONN);
	if(iResult == MBASE_SOCKET_ERROR)
	{
		// TODO: ERROR CHECKING WILL BE IMPLEMENTED
		#ifdef MBASE_PLATFORM_WINDOWS
		closesocket(serverSocket);
		#endif
		#ifdef MBASE_PLATFORM_UNIX
		close(serverSocket);
		#endif
		return flags::NET_MNG_ERR_UNKNOWN;
	}
	u_long ctlMode = 1;
	
	#ifdef MBASE_PLATFORM_WINDOWS
	ioctlsocket(serverSocket, FIONBIO, &ctlMode);
	#endif
	#ifdef MBASE_PLATFORM_UNIX
	ioctl(serverSocket, FIONBIO, &ctlMode);
	#endif

	out_server.mRawSocket = serverSocket;
	out_server.mAddr = in_addr;
	out_server.mPort = in_port;
	mServerReleaseLock.acquire();
	
	mServers.push_back(watcher_type());
	watcher_type& lastWatcher = mServers.back();
	lastWatcher.mItSelf = mServers.end_node();
	lastWatcher.mSubject = &out_server;
	out_server.acquire_object_watcher(&lastWatcher);

	mServerReleaseLock.release();
	start_processor();

	return flags::NET_MNG_SUCCESS;
}

GENERIC PcNetManager::update()
{
	for (servers_list::iterator It = mServers.begin(); It != mServers.end();)
	{
		if(!It->mSubject)
		{
			// Means the watcher is released
			mServerReleaseLock.acquire();
			It = mServers.erase(It->mItSelf);
			mServerReleaseLock.release();
		}
		++It;
	}
}

GENERIC PcNetManager::update_t()
{
	while(is_processor_running())
	{
		mServerReleaseLock.acquire();
		for (servers_list::iterator It = mServers.begin(); It != mServers.end(); ++It)
		{
			if(It->mSubject)
			{
				It->mSubject->update_t();
			}
			
		}
		mServerReleaseLock.release();
	}
}

MBASE_END
