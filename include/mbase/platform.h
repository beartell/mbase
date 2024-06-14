#ifndef MBASE_PLATFORM_H
#define MBASE_PLATFORM_H

#include <mbase/common.h>

MBASE_STD_BEGIN

#ifdef _WIN32

/* SOCKET PROCS */
/* IO PROCS */
/* THREAD PROCS */

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class wsa_initializer {
public:
    wsa_initializer() noexcept 
    {
        wsaInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
    }

    ~wsa_initializer() noexcept 
    {
        WSACleanup();
    }

    MBASE_ND(MBASE_RESULT_IGNORE) I32 get_init_result() const noexcept 
    {
        return wsaInit;
    }

private:
    WSADATA wsaData;
    int wsaInit;
};

static wsa_initializer wsaInitializer;

#endif

MBASE_STD_END

#endif // !MBASE_PLATFORM_H
