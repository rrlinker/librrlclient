#pragma once

#include <courier.h>
#include <token.h>

#include <array>

namespace rrl {

    class Authorizer {
    public:
        Authorizer(Token t);

        void authorize(Courier &courier) const;

        const Token& token() const { return token_; }

    private:
        Token token_;
    };

}
