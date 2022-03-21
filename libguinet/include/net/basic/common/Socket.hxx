#pragma once

#include "guinet.hxx"

namespace net
{
    class Connection;
    class Server;

    class Socket final
    {
        friend class net::Connection;
        friend class net::Server;
    public:
        Socket();
        ~Socket();

        int recieve(uint8_t* buffer, size_t length, uint32_t flags = 0);
        int trasmit(uint8_t* buffer, size_t length, uint32_t flags = 0);

    private:
        int _listen(AddrInfo addr, SockInfo sock);
        int _connect(AddrInfo addr, SockInfo sock);
        int _close();

    private:
        fdtype m_fdSocket;
        AddrInfo m_addr;
        SockInfo m_sock;

        int             m_state;
        ESocketError    m_error;
    };
}