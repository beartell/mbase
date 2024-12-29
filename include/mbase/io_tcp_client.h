#ifndef MBASE_IOTCPCLIENT_H
#define MBASE_IOTCPCLIENT_H

#include <mbase/io_base.h>
#include <mbase/string.h> // mbase::string
#include <mbase/behaviors.h> // mbase::non_copymovable

#ifdef MBASE_PLATFORM_WINDOWS
#include <mbase/wsa_init.h> // static wsa socket initializiation through mbase::wsa_initializer
#endif

#ifdef MBASE_PLATFORM_UNIX
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

MBASE_STD_BEGIN

/*

	--- CLASS INFORMATION ---
Identification: S0C18-OBJ-UD-ST

Name: io_tcp_client

Parent: S0C16-SAB-UD-ST, S0C6-STR-NA-ST

Behaviour List:
- Default Constructible
- Destructible

Description:
io_tcp_client is a class that implements the io_base abstract class.
It is providing tcp client operations through it's corresponding methods such as:
connect_target, disconnect, get_remote_ipv4 etc.

The read/write operation which are through read_data/write_data methods are synchronized,
blocking operations. Attempting to write to an invalid socket handle will result in a OS dependant behavior.
io_tcp_client does not make further control on read/write operations whether the socket handle is valid or not.

To achieve async io behavior, refer to the section Async I/O in MBASE.

*/

class io_tcp_client : public io_base, public non_copymovable {
public:
	/* ===== BUILDER METHODS BEGIN ===== */
	io_tcp_client() noexcept;
	io_tcp_client(const mbase::string& in_name, const mbase::string& in_port) noexcept;
	~io_tcp_client() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_remote_ipv4() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_remote_ipv6() const noexcept;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	I32 connect_target(const mbase::string& in_name, const mbase::string& in_port) noexcept;
	I32 disconnect() noexcept;
	size_type write_data(CBYTEBUFFER in_src) override;
	size_type write_data(CBYTEBUFFER in_src, size_type in_length) override;
	size_type write_data(const mbase::string& in_src) override;
	size_type write_data(char_stream& in_src) override;
	size_type write_data(char_stream& in_src, size_type in_length) override { return 0; }
	size_type read_data(IBYTEBUFFER in_src, size_type in_length) override;
	size_type read_data(char_stream& in_src) override;
	size_type read_data(char_stream& in_src, size_type in_length) override { return 0; }
	/* ===== STATE-MODIFIER METHODS END ===== */

private:
	SOCKET mRawHandle = INVALID_SOCKET;
	sockaddr_in mSocketAddr = {0};
};

io_tcp_client::io_tcp_client() noexcept : mRawHandle(INVALID_SOCKET) 
{
}

io_tcp_client::io_tcp_client(const mbase::string& in_name, const mbase::string& in_port) noexcept 
{
	if(!connect_target(in_name, in_port))
	{
		mOperateReady = true;
	}
}

io_tcp_client::~io_tcp_client() noexcept {
	disconnect();
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string io_tcp_client::get_remote_ipv4() const noexcept
{
	IBYTE ipOut[INET_ADDRSTRLEN] = { 0 };
	inet_ntop(AF_INET, &mSocketAddr.sin_addr, ipOut, sizeof(ipOut));
	return mbase::string(ipOut);
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string io_tcp_client::get_remote_ipv6() const noexcept 
{
	IBYTE ipOut[INET6_ADDRSTRLEN] = { 0 };
	inet_ntop(AF_INET6, &mSocketAddr.sin_addr, ipOut, sizeof(ipOut));
	return mbase::string(ipOut);
}

I32 io_tcp_client::connect_target(const mbase::string& in_name, const mbase::string& in_port) noexcept 
{
	disconnect();

	mRawHandle = INVALID_SOCKET;

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
		mRawHandle = socket(pt_addr->ai_family, pt_addr->ai_socktype, pt_addr->ai_protocol);
		if (mRawHandle == INVALID_SOCKET)
		{
#ifdef MBASE_PLATFORM_WINDOWS
			_set_last_error(WSAGetLastError());
#endif
			freeaddrinfo(result);
			return 1;
		}

		addrResult = connect(mRawHandle, pt_addr->ai_addr, pt_addr->ai_addrlen);
		if (addrResult == SOCKET_ERROR)
		{
			closesocket(mRawHandle);
			mRawHandle = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (mRawHandle == INVALID_SOCKET)
	{
#ifdef MBASE_PLATFORM_WINDOWS
		_set_last_error(WSAGetLastError());
#endif
		return 1;
	}

	socklen_t len = sizeof(mSocketAddr);
	getpeername(mRawHandle, reinterpret_cast<sockaddr*>(&mSocketAddr), &len);

	return 0;
}

I32 io_tcp_client::disconnect() noexcept
{
	mOperateReady = false;
	I32 dcResult = closesocket(mRawHandle);
	if (dcResult == SOCKET_ERROR)
	{
#ifdef MBASE_PLATFORM_WINDOWS
		_set_last_error(WSAGetLastError());
#endif
	}

	return dcResult;
}

typename io_tcp_client::size_type io_tcp_client::write_data(CBYTEBUFFER in_src)
{
	DWORD dataWritten = 0;
	SIZE_T dataLength = type_sequence<IBYTE>::length_bytes(in_src);

	dataWritten = send(mRawHandle, in_src, dataLength, 0);
	if (dataWritten == SOCKET_ERROR)
	{
		disconnect();
#ifdef MBASE_PLATFORM_WINDOWS
		_set_last_error(WSAGetLastError());
#endif
	}
	return dataWritten;
}

typename io_tcp_client::size_type io_tcp_client::write_data(CBYTEBUFFER in_src, size_type in_length)
{
	DWORD dataWritten = 0;
	dataWritten = send(mRawHandle, in_src, in_length, 0);
	if (dataWritten == SOCKET_ERROR)
	{
		disconnect();
#ifdef MBASE_PLATFORM_WINDOWS
		_set_last_error(WSAGetLastError());
#endif
		return 0;
	}
	return dataWritten;
}

typename io_tcp_client::size_type io_tcp_client::write_data(const mbase::string& in_src)
{
	DWORD dataWritten = 0;
	dataWritten = send(mRawHandle, in_src.c_str(), in_src.size(), 0);
	if (dataWritten == SOCKET_ERROR)
	{
		disconnect();
#ifdef MBASE_PLATFORM_WINDOWS
		_set_last_error(WSAGetLastError());
#endif
	}
	return dataWritten;
}

typename io_tcp_client::size_type io_tcp_client::write_data(char_stream& in_src)
{
	DWORD dataWritten = 0;
	PTRDIFF cursorPos = in_src.get_pos();
	IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
	dataWritten = send(mRawHandle, tmpBuffer, in_src.buffer_length() - cursorPos, 0);
	if (dataWritten == SOCKET_ERROR)
	{
		disconnect();
#ifdef MBASE_PLATFORM_WINDOWS
		_set_last_error(WSAGetLastError());
#endif
	}
	return dataWritten;
}

typename io_tcp_client::size_type io_tcp_client::read_data(IBYTEBUFFER in_src, size_type in_length)
{
	DWORD dataRead = 0;
	dataRead = recv(mRawHandle, in_src, in_length, 0);
	if (dataRead == SOCKET_ERROR)
	{
		disconnect();
#ifdef MBASE_PLATFORM_WINDOWS
		_set_last_error(WSAGetLastError());
#endif
	}
	return dataRead;
}

typename io_tcp_client::size_type io_tcp_client::read_data(char_stream& in_src)
{
	DWORD dataRead = 0;
	PTRDIFF cursorPos = in_src.get_pos();
	IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
	dataRead = recv(mRawHandle, tmpBuffer, in_src.buffer_length() - cursorPos, 0);
	if (dataRead == SOCKET_ERROR)
	{
		disconnect();
#ifdef MBASE_PLATFORM_WINDOWS
		_set_last_error(WSAGetLastError());
#endif

	}
	return dataRead;
}

MBASE_STD_END

#endif // MBASE_IOTCPCLIENT_H
