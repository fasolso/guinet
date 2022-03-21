#include "Socket.hxx"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cerrno>

#if defined(__linux) or defined(__unix)
#include <unistd.h>
#else
#include <stdlib.h>
#endif

using net::Socket;

Socket::Socket()
{
    m_addr = AddrInfo();
    m_sock = SockInfo();
    m_error = net::ESocketError::NONE;
    m_fdSocket = 0;
    m_state = 0;
}

Socket::~Socket()
{
    _close();
}

int Socket::recieve(uint8_t* buffer, size_t length, uint32_t flags)
{
    if (!m_state)
    {
        m_error = net::ESocketError::INVALID;

        return -1;
    }

    int nRecv = 0;
    uint32_t sockType = m_sock.type;

    switch (sockType)
    {
    case SOCK_STREAM | SOCK_NONBLOCK:
    case SOCK_STREAM:
    {
        recv(m_fdSocket, reinterpret_cast<void*>(buffer), length, flags);
    }break;
    case SOCK_DGRAM:
    {
        sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);

        addr.sin_family = m_sock.addrFamily;
        addr.sin_addr.s_addr = m_addr.addr.va;
        addr.sin_port = htons(m_addr.port);

        nRecv = recvfrom(m_fdSocket, reinterpret_cast<void*>(buffer), length, flags, reinterpret_cast<sockaddr*>(&addr), &addrlen);
    }break;
    default:
    {
        m_error = ESocketError::NOTSUPPORTED;
    }
    }

    if (nRecv < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            m_error = net::ESocketError::EWB_EAGAIN;
        }
        else
            m_error = net::ESocketError::ERRIO;
    }
    else
    {
        if (length <= nRecv)
        {
            //potential buffer overflow, some data might be lost
        }
    }
    //return recieved bytes count
    return nRecv;
}

int Socket::trasmit(uint8_t* buffer, size_t length, uint32_t flags)
{
    if (!m_state)
    {
        m_error = net::ESocketError::INVALID;

        return -1;
    }

    int nRecv = 0;
    uint32_t sockType = m_sock.type;

    switch (sockType)
    {
    case SOCK_STREAM | SOCK_NONBLOCK:
    case SOCK_STREAM:
    {
        send(m_fdSocket, reinterpret_cast<void*>(buffer), length, flags);
    }break;
    case SOCK_DGRAM:
    {
        sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);

        addr.sin_family = m_sock.addrFamily;
        addr.sin_addr.s_addr = m_addr.addr.va;
        addr.sin_port = htons(m_addr.port);

        nRecv = sendto(m_fdSocket, reinterpret_cast<void*>(buffer), length, flags, reinterpret_cast<sockaddr*>(&addr), addrlen);
    }break;
    default:
    {
        m_error = ESocketError::NOTSUPPORTED;
    }
    }

    if (nRecv < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            m_error = net::ESocketError::EWB_EAGAIN;
        }
        else
            m_error = net::ESocketError::ERRIO;
    }
    else
    {
        if (length <= nRecv)
        {
            //potential buffer overflow, some data might be lost
        }
    }
    //return recieved bytes count
    return nRecv;
}

int Socket::_listen(AddrInfo addrInfo, SockInfo sockInfo)
{
    if (m_state)
    {
        m_error = ESocketError::BADFD;
        return -1;
    }

    int ret = 0;

    m_fdSocket = socket(sockInfo.addrFamily, sockInfo.type | SOCK_NONBLOCK, sockInfo.lowerProto);

    if (m_fdSocket < 0)
    {
        m_error = ESocketError::BADFD;

        return -1;
    }

    switch (sockInfo.type)
    {
    case SOCK_STREAM:
    case SOCK_STREAM | SOCK_NONBLOCK:
    {
        sockaddr_in addr;
        socklen_t addrlen = 0;

        addr.sin_addr.s_addr = addrInfo.addr.va;
        addr.sin_port = htons(addrInfo.port);
        addr.sin_family = sockInfo.addrFamily;

        ret = bind(m_fdSocket, reinterpret_cast<sockaddr*>(&addr), addrlen);

        if (ret < 0)
        {
            //address already in use
            if (errno == EADDRINUSE || EINVAL)
                m_error = ESocketError::ADDRINUSE;

            //wrong address, or not available
            if (errno == EADDRNOTAVAIL)
                m_error = ESocketError::ADDRNOTAVAIL;

            //connection locally blocked(by firewall or etc.)
            if (errno == EACCES || errno == EPERM)
                m_error = ESocketError::OTHER;

            //bad socket file descriptor
            if (errno == EBADF || errno == EBADFD || errno == ENOTSOCK)
                m_error = ESocketError::BADFD;

            return -1;
        }

        ret = listen(m_fdSocket, sockInfo.maxConnections);

        if (ret < 0)
        {
            //address already in use
            if (errno == EADDRINUSE || EINVAL)
                m_error = ESocketError::ADDRINUSE;

            //wrong address, or not available
            if (errno == EADDRNOTAVAIL)
                m_error = ESocketError::ADDRNOTAVAIL;

            //connection locally blocked(by firewall or etc.)
            if (errno == EACCES || errno == EPERM)
                m_error = ESocketError::OTHER;

            //bad socket file descriptor
            if (errno == EBADF || errno == EBADFD || errno == ENOTSOCK)
                m_error = ESocketError::BADFD;

            return -1;
        }

    }break;
    case SOCK_DGRAM:
    {
        //actually, ready
    }break;
    }

    m_sock = sockInfo;
    m_addr = addrInfo;
    m_state = sockInfo.type;

    return ret;
}

int Socket::_connect(AddrInfo addrInfo, SockInfo sockInfo)
{
    if (m_state)
    {
        m_error = ESocketError::BADFD;
        return -1;
    }

    int ret = 0;

    m_fdSocket = socket(sockInfo.addrFamily, sockInfo.type | SOCK_NONBLOCK, sockInfo.lowerProto);

    if (m_fdSocket < 0)
    {
        m_error = ESocketError::BADFD;

        return -1;
    }

    switch (sockInfo.type)
    {
    case SOCK_STREAM:
    case SOCK_STREAM | SOCK_NONBLOCK:
    {
        sockaddr_in addr;
        socklen_t addrlen = 0;

        addr.sin_addr.s_addr = addrInfo.addr.va;
        addr.sin_port = htons(addrInfo.port);
        addr.sin_family = sockInfo.addrFamily;

        ret = connect(m_fdSocket, reinterpret_cast<sockaddr*>(&addr), addrlen);

        if (ret < 0)
        {
            //address already in use
            if (errno == EADDRINUSE)
                m_error = ESocketError::ADDRINUSE;

            //wrong address, or not available
            if (errno == EADDRNOTAVAIL)
                m_error = ESocketError::ADDRNOTAVAIL;

            //connection locally blocked(by firewall or etc.)
            if (errno == EACCES || errno == EPERM)
                m_error = ESocketError::OTHER;

            //connection refused by host
            if (errno == ECONNREFUSED)
                m_error = ESocketError::REFUSED;

            //bad socket file descriptor
            if (errno == EBADF || errno == EBADFD || errno == ENOTSOCK)
                m_error = ESocketError::BADFD;

            //connection timed out
            if (errno == ETIMEDOUT)
                m_error = ESocketError::TIMEOUT;

            return -1;
        }
    }break;
    case SOCK_DGRAM:
    {
        //actually, ready
    }break;
    }

    m_sock = sockInfo;
    m_addr = addrInfo;
    m_state = sockInfo.type;

    return ret;
}

int Socket::_close()
{
    return close(m_fdSocket);
}