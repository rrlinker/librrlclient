#include <librlcom/connection.hpp>

#include <WinSock2.h>
#include <Ws2ipdef.h>
#include <ws2tcpip.h>
#include <Mstcpip.h>

namespace rrl::win {

    class WSConnection : public Connection {
    public:
        static int startup();
        static int cleanup();

        static SOCKADDR_INET make_sockaddr(Address const &address);

        WSConnection(bool _auto = false);
        virtual ~WSConnection() noexcept(false);

        virtual void connect(Address const &address) override;
        virtual void disconnect() override;
        virtual void send(std::byte const *data, uint64_t length) override;
        virtual void recv(std::byte *data, uint64_t length) override;

        const WSADATA& WSAData() const { return wsaData_; }

    private:
        bool auto_;
        static WSADATA wsaData_;
    };

}
