#pragma once

#include "net.hxx"

class Socket;

namespace net
{
    class Connection final
    {
    public:
        Connection();
        ~Connection();

        ConnInfo getConnectionInfo();
    private:
        Socket* m_socket;
    };
}