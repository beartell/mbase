#ifndef MBASE_IOTCPCLIENT_H
#define MBASE_IOTCPCLIENT_H

#include <mbase/io_base.h>
#include <mbase/wsa_init.h> // static wsa socket initializiation through mbase::wsa_initializer
#include <mbase/behaviors.h> // mbase::non_copymovable

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
	io_tcp_client() noexcept;
	io_tcp_client(const mbase::string& in_name, const mbase::string& in_port) noexcept;
	~io_tcp_client() noexcept;

	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_remote_ipv4() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) mbase::string get_remote_ipv6() const noexcept;

	I32 connect_target(const mbase::string& in_name, const mbase::string& in_port) noexcept;
	I32 disconnect() noexcept;
	size_type write_data(IBYTEBUFFER in_src) override;
	size_type write_data(IBYTEBUFFER in_src, size_type in_length) override;
	size_type write_data(const mbase::string& in_src) override;
	size_type write_data(const char_stream& in_src) override;
	size_type read_data(IBYTEBUFFER in_src, size_type in_length) override;
	size_type read_data(const char_stream& in_src) override;

	template<typename SerializableObject>
	size_type write_data(SerializableObject& in_src) {
		safe_buffer mBuffer;
		in_src.serialize(&mBuffer);
		return write_data(mBuffer.bfSource, mBuffer.bfLength);
	}

	template<typename SerializableObject>
	size_type read_data(SerializableObject& in_target, IBYTEBUFFER in_src, size_type in_length) {
		size_type readLength = read_data(in_src, in_length);
		in_target.deserialize(in_src, in_length);
		return readLength;
	}

private:
	SOCKET rawHandle = INVALID_SOCKET;
	sockaddr_in sckAddr = {0};
};

io_tcp_client::io_tcp_client() noexcept : rawHandle(INVALID_SOCKET) 
{

}

io_tcp_client::io_tcp_client(const mbase::string& in_name, const mbase::string& in_port) noexcept 
{
	if(!connect_target(in_name, in_port))
	{
		operateReady = true;
	}
}

io_tcp_client::~io_tcp_client() noexcept {
	disconnect();
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string io_tcp_client::get_remote_ipv4() const noexcept
{
	IBYTE ipOut[INET_ADDRSTRLEN] = { 0 };
	inet_ntop(AF_INET, &sckAddr.sin_addr, ipOut, sizeof(ipOut));
	return mbase::string(ipOut);
}

MBASE_ND(MBASE_OBS_IGNORE) mbase::string io_tcp_client::get_remote_ipv6() const noexcept 
{
	IBYTE ipOut[INET6_ADDRSTRLEN] = { 0 };
	inet_ntop(AF_INET6, &sckAddr.sin_addr, ipOut, sizeof(ipOut));
	return mbase::string(ipOut);
}

I32 io_tcp_client::connect_target(const mbase::string& in_name, const mbase::string& in_port) noexcept 
{
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

I32 io_tcp_client::disconnect() noexcept
{
	operateReady = false;
	I32 dcResult = closesocket(rawHandle);
	if (dcResult == SOCKET_ERROR)
	{
		_set_last_error(WSAGetLastError());
	}

	return dcResult;
}

typename io_tcp_client::size_type io_tcp_client::write_data(IBYTEBUFFER in_src) override
{
	DWORD dataWritten = 0;
	SIZE_T dataLength = type_sequence<IBYTE>::length_bytes(in_src);

	dataWritten = send(rawHandle, in_src, dataLength, 0);
	if (dataWritten == SOCKET_ERROR)
	{
		_set_last_error(WSAGetLastError());
	}
	return dataWritten;
}

typename io_tcp_client::size_type io_tcp_client::write_data(IBYTEBUFFER in_src, size_type in_length) override
{
	DWORD dataWritten = 0;
	dataWritten = send(rawHandle, in_src, in_length, 0);
	if (dataWritten == SOCKET_ERROR)
	{
		_set_last_error(WSAGetLastError());
	}
	return dataWritten;
}

typename io_tcp_client::size_type io_tcp_client::write_data(const mbase::string& in_src) override
{
	DWORD dataWritten = 0;
	dataWritten = send(rawHandle, in_src.c_str(), in_src.size(), 0);
	if (dataWritten == SOCKET_ERROR)
	{
		_set_last_error(WSAGetLastError());
	}
	return dataWritten;
}

typename io_tcp_client::size_type io_tcp_client::write_data(const char_stream& in_src) override
{
	DWORD dataWritten = 0;
	PTRDIFF cursorPos = in_src.get_pos();
	IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
	dataWritten = send(rawHandle, tmpBuffer, in_src.buffer_length() - cursorPos, 0);
	if (dataWritten == SOCKET_ERROR)
	{
		_set_last_error(WSAGetLastError());
	}
	return dataWritten;
}

typename io_tcp_client::size_type io_tcp_client::read_data(IBYTEBUFFER in_src, size_type in_length) override
{
	DWORD dataRead = 0;
	dataRead = recv(rawHandle, in_src, in_length, 0);
	if (dataRead == SOCKET_ERROR)
	{
		_set_last_error(WSAGetLastError());
	}
	return dataRead;
}

typename io_tcp_client::size_type io_tcp_client::read_data(const char_stream& in_src) override
{
	DWORD dataRead = 0;
	PTRDIFF cursorPos = in_src.get_pos();
	IBYTEBUFFER tmpBuffer = in_src.get_bufferc();
	dataRead = recv(rawHandle, tmpBuffer, in_src.buffer_length() - cursorPos, 0);
	if (dataRead == SOCKET_ERROR)
	{
		_set_last_error(WSAGetLastError());
	}
	return dataRead;
}

MBASE_STD_END

#endif // MBASE_IOTCPCLIENT_H
