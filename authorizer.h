#pragma once

#include "connection.h"

namespace rrl {

    class Authorizer {
        public:
            void authorize(Connection &connection);
    };

}

