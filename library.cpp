#include "library.h"

using namespace rrl;

Library::Library(std::string const &name)
    : name(name)
{}

void Library::add_module_dependency(std::string const &module) {
    module_dependencies_.emplace(module);
}

void Library::add_library_dependency(Library const &library) {
    library_dependencies_.emplace(library.name);
}

uintptr_t Library::get_symbol_address(std::string const &symbol) const {
    if (auto it = symbols_.find(symbol); it != symbols_.end())
        return it->second.address;
    return 0;
}

Library::Symbol const& Library::operator[](std::string const &symbol) const {
    if (auto it = symbols_.find(symbol); it != symbols_.end())
        return it->second;
    else
        throw std::logic_error("symbol not found");
}
