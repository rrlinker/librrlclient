#include <iostream>

#include <rrlinker/client/ws_connection.hpp>
#include <rrlinker/client/win32_error.hpp>

using namespace rrl;
using namespace rrl::win;

WSADATA WSConnection::wsaData_;

int WSConnection::startup() {
    return WSAStartup(WINSOCK_VERSION, &wsaData_);
}

int WSConnection::cleanup() {
    return WSACleanup();
}

SOCKADDR_INET WSConnection::make_sockaddr(const Address &address) {
    SOCKADDR_INET saddr = {};
    saddr.si_family = AF_INET6;
    memcpy(saddr.Ipv6.sin6_addr.s6_addr, address.address().data(), sizeof(saddr.Ipv6.sin6_addr.s6_addr));
    saddr.Ipv6.sin6_port = htons(address.port());
    return *reinterpret_cast<SOCKADDR_INET*>(&saddr);
}


WSConnection::WSConnection(bool _auto)
    : Connection()
    , auto_(_auto)
{
    if (auto_) {
        if (startup()) {
            throw win::Win32Error(WSAGetLastError());
        }
    }
}

WSConnection::~WSConnection() noexcept(false) {
    disconnect();
    if (auto_) {
        if (cleanup()) {
            throw win::Win32Error(WSAGetLastError());
        }
    }
}

void WSConnection::connect(const Address &address) {
    int err;

    socket_ = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET)
        throw win::Win32Error(WSAGetLastError());

    const DWORD zero = 0;
    err = setsockopt(socket_, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&zero), sizeof(zero));
    if (err != 0)
        throw win::Win32Error(WSAGetLastError());

    auto saddr = make_sockaddr(address);
    err = ::connect(socket_, reinterpret_cast<const sockaddr*>(&saddr), sizeof(saddr));
    if (err != 0)
        throw win::Win32Error(WSAGetLastError());
}

void WSConnection::disconnect() {
    if (socket_ != INVALID_SOCKET) {
        int err = closesocket(socket_);
        if (err != 0)
            throw win::Win32Error(WSAGetLastError());
        socket_ = INVALID_SOCKET;
    }
}

void WSConnection::send(const std::byte *data, uint64_t length) {
    int res;
    do {
        res = ::send(socket_, reinterpret_cast<const char*>(data), static_cast<int>(length), 0);
        if (res == SOCKET_ERROR)
            throw win::Win32Error(WSAGetLastError());
        data += res;
        length -= res;
    } while (length > 0);
}

void WSConnection::recv(std::byte *data, uint64_t length) {
    while (length > 0) {
        int res = ::recv(socket_, reinterpret_cast<char*>(data), static_cast<int>(length), 0);
        if (res == SOCKET_ERROR || res == 0) {
            throw win::Win32Error(WSAGetLastError());
        }
        data += res;
        length -= res;
    }
}

