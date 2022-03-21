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

        int recieve(uint8_t* buffer, int length);
        int trasmit(uint8_t* buffer, int length);

    private:
        int open(ESockOpenAction action, AddrInfo addr);
        int close();

    private:
        fdtype m_socket;
        AddrInfo m_addr;

        int             m_state;
        ESocketError    m_error;
    };
}