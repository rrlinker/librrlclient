#pragma once

#include "connection.h"
#include "token.h"

#include <vector>
#include <optional>
#include <cstdint>

#pragma pack(push, 1)

namespace rrl {

    enum class MessageType : uint64_t {
        Version = 0x01ULL,
        Authorization = 0x0ALL,

        OK = 0x0CULL << 56,

        RequestAuthorize = (1ULL << 63) | 0x0ALL,
    };

    struct MessageHeader {
        size_t size;
        MessageType type;

        MessageHeader(size_t size, MessageType type) noexcept
            : size(size)
            , type(type)
        {}
    };

    template<typename Body>
    struct MessageWrapper : private Body::value_type {
        MessageHeader header;
        typename Body::value_type& body() { return *this; }
        typename Body::value_type const& body() const { return *this; }

        MessageWrapper(size_t size, MessageType type) noexcept
            : header(size, type)
        {}

        MessageWrapper(size_t size, MessageType type, typename Body::value_type const &value)
            : header(size, type)
            , Body::value_type(value)
        {}

        MessageWrapper(size_t size, MessageType type, typename Body::value_type &&value)
            : header(size, type)
            , Body::value_type(std::forward<typename Body::value_type>(value))
        {}

        void write(Connection &conn) {
            conn << header.size << header.type;
            Body::write(conn, body());
        }

        void read(Connection &conn) {
            conn >> header.size >> header.type;
            Body::read(conn, header, body());
        }
    };

    struct UnknownMessageBody {
        using value_type = std::vector<std::byte>;
        static void write(Connection &conn, value_type const &value) {
            conn.send(reinterpret_cast<std::byte const*>(value.data()), value.size());
        }
        static void read(Connection &conn, MessageHeader const &header, value_type &value) {
            value.resize(header.size);
            conn.recv(reinterpret_cast<std::byte*>(value.data()), value.size());
        }
    };

    struct UnknownMessage : public MessageWrapper<UnknownMessageBody> {
        template<typename T>
        T const& cast() const { return *reinterpret_cast<T*>(body().data()); }
    };


    namespace msg {

        struct EmptyBody {
            struct value_type {};
            static void write(Connection&, value_type const&) {}
        };

        template<typename T>
        struct ValueBody {
            struct value_type { T value; };
            static void write(Connection &conn, value_type const &value) {
                conn.send(reinterpret_cast<std::byte const*>(&value.value), sizeof(value.value));
            }
        };

        struct TokenBody {
            using value_type = Token;
            static void write(Connection &conn, value_type const &value) {
                conn.send(value.data(), value.size());
            }
        };

        struct LinkLibrary {};

        using Empty = MessageWrapper<EmptyBody>;
        using RequestAuthorize = Empty;

#define BEGIN_DEFINE_MESSAGE(TYPE, BASE) \
struct TYPE : MessageWrapper<BASE> { \
TYPE() : MessageWrapper(sizeof(TYPE), MessageType::TYPE) {}
#define END_DEFINE_MESSAGE() };

#define DEFINE_MESSAGE(TYPE, BASE) \
BEGIN_DEFINE_MESSAGE(TYPE, BASE) \
END_DEFINE_MESSAGE()

        DEFINE_MESSAGE(Version, ValueBody<uint64_t>);
        DEFINE_MESSAGE(OK, EmptyBody);
        DEFINE_MESSAGE(Authorization, TokenBody);

#undef DEFINE_MESSAGE
#undef END_DEFINE_MESSAGE
#undef BEGIN_DEFINE_MESSAGE
    }

}

#pragma pack(pop)
