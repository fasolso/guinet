#pragma once

#include <stdint.h>
#include <time.h>

#define SOCKET_STATE_OK     0x00
#define SOCKET_STATE_FAIL   0xff

typedef int fdtype;

namespace net
{
    struct ConnInfo
    {
        time_t timeEstablished;
        AddrInfo addr;
        AddrInfo dest;
    };

    struct AddrInfo
    {
        union
        {
            struct
            {
                uint8_t a;
                uint8_t b;
                uint8_t c;
                uint8_t d;
            } un;
            uint32_t va;
        } addr;

        uint16_t port;
    private:
        fdtype m_fd;
    };

    enum class EPollType
    {
        AUTO = 0,
        LIBEVENT,
        KQUEUE,
#if defined(__unix) or defined(__linux) 
        EPOLL,
#endif
        POLL,
        SELECT,
        NONE,
    };

    enum class ESockOpenAction
    {
        BIND_LISTEN = 0,
        CONNECT,
        BIND_LISTEN_NON_BLOCK,
        CONNECT_NON_BLOCK,
        NONE,
    };

    enum class ESocketError
    {
        NONE = -1,
        BADFD = 0,
        ERRIO,
        NOCON,
        EAGAIN,
        OTHER,
    };

}