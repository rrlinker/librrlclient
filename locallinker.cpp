#include "locallinker.hpp"
#include "win32exception.hpp"

using namespace rrl;

LocalLinker::LocalLinker(symbol_resolver resolver)
    : Linker(LinkageKind::Local, resolver)
{}

uintptr_t LocalLinker::resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) {
    uintptr_t proc;
    // if symbol_library is set
    if (!symbol_library.empty()) {
        HMODULE hModule = get_module_handle(library, symbol_library);
        // Try Win32 API first
        if ((proc = reinterpret_cast<uintptr_t>(GetProcAddress(hModule, symbol_name.c_str()))) != NULL) {
            library.add_module_dependency(symbol_library, hModule);
            return proc;
        }
        // Try local libraries
        if ((proc = resolve_internal_symbol(symbol_library, symbol_name)) != 0) {
            dependency_bind(library, symbol_library);
            return proc;
        }
    }
    // Try custom resolver
    return resolve_unresolved_symbol(library, symbol_library, symbol_name);
}

void LocalLinker::add_export(Library &library, std::string const &symbol, uintptr_t address) {
    library.set_symbol_address(symbol, address);
}
