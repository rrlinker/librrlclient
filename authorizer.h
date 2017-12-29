#pragma once

#include "connection.h"
#include "token.h"

#include <array>

namespace rrl {

    class Authorizer {
    public:
        Authorizer(Token t);

        void authorize(Connection &connection) const;

        const Token& token() const { return token_; }

    private:
        Token token_;
    };

}

