#include "library.h"

using namespace rrl;

Library::Library(HANDLE process, std::string const &name)
    : process(process)
    , name(name)
{}

void Library::add_module_dependency(std::string const &module) {
    module_dependencies_.emplace(module);
}

void Library::add_library_dependency(Library const &library) {
    library_dependencies_.emplace(library.name);
}

void Library::add_thread(HANDLE hThread) {
    threads_.emplace(hThread);
}

void Library::set_symbol_address(std::string symbol, uintptr_t address) {
    symbols_[symbol] = Symbol(address);
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
