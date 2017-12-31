#include <iostream>

#include "ws_connection.h"
#include "win32exception.h"

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
    memcpy(saddr.Ipv6.sin6_addr.s6_addr, address.addr(), sizeof(saddr.Ipv6.sin6_addr.s6_addr));
    saddr.Ipv6.sin6_port = htons(address.port());
    return *reinterpret_cast<SOCKADDR_INET*>(&saddr);
}


WSConnection::WSConnection()
    : Connection()
{}

WSConnection::~WSConnection() {
    disconnect();
}

void WSConnection::connect(const Address &address) {
    int err;

    socket_ = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET)
        throw Win32Exception(WSAGetLastError());

    const DWORD zero = 0;
    err = setsockopt(socket_, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&zero), sizeof(zero));
    if (err != 0)
        throw Win32Exception(WSAGetLastError());

    auto saddr = make_sockaddr(address);
    err = ::connect(socket_, reinterpret_cast<const sockaddr*>(&saddr), sizeof(saddr));
    if (err != 0)
        throw Win32Exception(WSAGetLastError());
}

void WSConnection::disconnect() {
    if (socket_ != INVALID_SOCKET) {
        int err = closesocket(socket_);
        if (err != 0)
            throw Win32Exception(WSAGetLastError());
        socket_ = INVALID_SOCKET;
    }
}

void WSConnection::send(const std::byte *data, uint64_t length) {
    int res;
    do {
        res = ::send(socket_, reinterpret_cast<const char*>(data), length, 0);
        if (res == SOCKET_ERROR)
            throw Win32Exception(WSAGetLastError());
        data += res;
        length -= res;
    } while (length > 0);
}

void WSConnection::recv(std::byte *data, uint64_t length) {
    int res;
    res = ::recv(socket_, reinterpret_cast<char*>(data), length, MSG_WAITALL);
    if (res == SOCKET_ERROR || res == 0)
        throw Win32Exception(WSAGetLastError());
}

