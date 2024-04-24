#ifndef MBASE_WSAINIT_H
#define MBASE_WSAINIT_H

#include <mbase/common.h>

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

MBASE_STD_BEGIN

class wsa_initializer {
public:
    wsa_initializer() noexcept {
        wsaInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
    }

    ~wsa_initializer() noexcept {
        WSACleanup();
    }

    MBASE_ND("checking if wsa is initialized but ignoring the result") I32 get_init_result() const noexcept {
        return wsaInit;
    }

private:
    WSADATA wsaData;
    int wsaInit;
};

static wsa_initializer wsaInitializer;

MBASE_STD_END

#endif // !MBASE_WSAINIT_H