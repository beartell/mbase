#ifndef MBASE_IOTCPCLIENT_H
#define MBASE_IOTCPCLIENT_H

#include <mbase/io_base.h>
#include <mbase/wsa_init.h>
#include <mbase/behaviors.h>

MBASE_STD_BEGIN

class io_tcp_client : public io_base, public non_copymovable {
public:
	io_tcp_client() noexcept : rawHandle(INVALID_SOCKET) {

	}
	io_tcp_client(const mbase::string& in_name, const mbase::string& in_port) noexcept {
		connect_target(in_name, in_port);
	}

	~io_tcp_client() noexcept {
		closesocket(rawHandle);
	}

	I32 connect_target(const mbase::string& in_name, const mbase::string& in_port) noexcept {
		rawHandle = INVALID_SOCKET;

		addrinfo* result = nullptr;
		addrinfo hints = { 0 };
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		
		I32 addrResult = getaddrinfo(in_name.c_str(), in_port.c_str(), &hints, &result);
		if (addrResult)
		{
			_set_last_error(addrResult);
			freeaddrinfo(result);
			return 1;
		}
		
		for (addrinfo* pt_addr = result; pt_addr != nullptr; pt_addr = pt_addr->ai_next)
		{
			rawHandle = socket(pt_addr->ai_family, pt_addr->ai_socktype, pt_addr->ai_protocol);
			if (rawHandle == INVALID_SOCKET)
			{
				_set_last_error(WSAGetLastError());
				freeaddrinfo(result);
				return 1;
			}

			addrResult = connect(rawHandle, pt_addr->ai_addr, pt_addr->ai_addrlen);
			if (addrResult == SOCKET_ERROR)
			{
				closesocket(rawHandle);
				rawHandle = INVALID_SOCKET;
				continue;
			}
			break;
		}
		
		freeaddrinfo(result);

		if (rawHandle == INVALID_SOCKET)
		{
			_set_last_error(WSAGetLastError());
			return 1;
		}

		socklen_t len = sizeof(sckAddr);
		getpeername(rawHandle, reinterpret_cast<sockaddr*>(&sckAddr), &len);

		return 0;
	}

	I32 disconnect() noexcept 
	{
		I32 dcResult = closesocket(rawHandle);
		if (dcResult == SOCKET_ERROR) 
		{
			_set_last_error(WSAGetLastError());
		}

		return dcResult;
	}

	USED_RETURN mbase::string get_remote_ipv4() const noexcept 
	{	
		IBYTE ipOut[INET_ADDRSTRLEN] = { 0 };
		inet_ntop(AF_INET, &sckAddr.sin_addr, ipOut, sizeof(ipOut));
		return mbase::string(ipOut);
	}

	USED_RETURN mbase::string get_remote_ipv6() const noexcept {
		IBYTE ipOut[INET6_ADDRSTRLEN] = { 0 };
		inet_ntop(AF_INET6, &sckAddr.sin_addr, ipOut, sizeof(ipOut));
		return mbase::string(ipOut);
	}

	size_type write_data(IBYTEBUFFER in_src) override 
	{
		DWORD dataWritten = 0;
		SIZE_T dataLength = type_sequence<IBYTE>::length(in_src);
		
		dataWritten = send(rawHandle, in_src, dataLength, 0);
		if(dataWritten == SOCKET_ERROR)
		{
			_set_last_error(WSAGetLastError());
		}
		return dataWritten;
	}
	size_type write_data(IBYTEBUFFER in_src, size_type in_length) override
	{
		DWORD dataWritten = 0;
		dataWritten = send(rawHandle, in_src, in_length, 0);
		if (dataWritten == SOCKET_ERROR)
		{
			_set_last_error(WSAGetLastError());
		}
		return dataWritten;
	}
	size_type write_data(const mbase::string& in_src) override
	{
		DWORD dataWritten = 0;
		dataWritten = send(rawHandle, in_src.c_str(), in_src.size(), 0);
		if (dataWritten == SOCKET_ERROR)
		{
			_set_last_error(WSAGetLastError());
		}
		return dataWritten;
	}
	size_type write_data(const char_stream& in_src) override
	{
		DWORD dataWritten = 0;
		PTRDIFF cursorPos = in_src.get_pos();
		IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
		dataWritten = send(rawHandle, tmpBuffer, in_src.buffer_length() - cursorPos, 0);
		if(dataWritten == SOCKET_ERROR)
		{
			_set_last_error(WSAGetLastError());
		}
		return dataWritten;
	}

	size_type read_data(IBYTEBUFFER in_src, size_type in_length) override
	{
		DWORD dataRead = 0;
		dataRead = recv(rawHandle, in_src, in_length, 0);
		if(dataRead == SOCKET_ERROR)
		{
			_set_last_error(WSAGetLastError());
		}
		return dataRead;
	}

	size_type read_data(const char_stream& in_src) override
	{
		DWORD dataRead = 0;
		PTRDIFF cursorPos = in_src.get_pos();
		IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
		dataRead = recv(rawHandle, tmpBuffer, in_src.buffer_length() - cursorPos, 0);
		if(dataRead == SOCKET_ERROR)
		{
			_set_last_error(WSAGetLastError());
		}
		return dataRead;
	}

private:
	SOCKET rawHandle = INVALID_SOCKET;
	sockaddr_in sckAddr = {0};
};

MBASE_STD_END

#endif // MBASE_IOTCPCLIENT_H
