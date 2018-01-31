#include "library.h"

using namespace rrl;

Library::Library(std::string const &name)
    : name(name)
{}

uintptr_t Library::get_symbol_address(std::string const &symbol) {
    if (auto it = symbols_.find(symbol); it != symbols_.end())
        return it->second.address;
    return 0;
}

Library::Symbol const& Library::operator[](std::string const & symbol) {
    return symbols_[symbol];
}
