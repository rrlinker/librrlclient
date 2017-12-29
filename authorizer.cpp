#include "authorizer.h"

using namespace rrl;

Authorizer::Authorizer(Token t)
    : token_(t)
{}

void Authorizer::authorize(Courier &courier) const {
    msg::Authorization msg_auth;
    msg_auth.body() = token_;
    courier.send(msg_auth);
}
