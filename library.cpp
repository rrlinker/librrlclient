#include "library.h"

using namespace rrl;

Library::Library(std::string const &name)
    : name(name)
{}

uintptr_t Library::get_symbol_address(std::string const &symbol) {
    if (auto it = symbols_.find(symbol); it != symbols_.end())
        return it->second;
    return 0;
}

uintptr_t Library::operator[](std::string const & symbol) {
    return get_symbol_address(symbol);
}
