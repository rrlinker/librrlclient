#include "authorizer.h"

using namespace rrl;

Authorizer::Authorizer(Token t)
    : token_(t)
{}

void Authorizer::authorize(Connection &connection) const {
    connection.send(token_.data(), token_.size());
}
