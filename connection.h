#pragma once

#include <iostream>

#include "address.h"

#include <system_error>

namespace rrl {

    class Connection {
    public:
        Connection();

        virtual void connect(Address const &address) = 0;
        virtual void disconnect() = 0;
        virtual void send(std::byte const *data, size_t length) = 0;
        virtual void recv(std::byte *data, size_t length) = 0;

        template<typename T>
        Connection& operator<<(T const &value) {
            std::cout << "<< " << value << '\n';
            send(reinterpret_cast<std::byte const*>(&value), sizeof(T));
            return *this;
        }

        template<typename T>
        Connection& operator<<(T &&value) {
            std::cout << "<< " << (uint64_t)value << '\n';
            send(reinterpret_cast<std::byte const*>(&value), sizeof(T));
            return *this;
        }

        template<typename T>
        Connection& operator>>(T const &value) {
            recv(reinterpret_cast<std::byte*>(&value), sizeof(T));
            return *this;
        }

    protected:
        int socket_;
    };

}

