#pragma once

#include "address.h"

#include <system_error>

namespace rrl {

    class Connection {
        public:

            virtual std::error_code connect(const Address &address) = 0;
            virtual void disconnect() = 0;

    };

}

