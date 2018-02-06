#include "locallinker.h"
#include "win32exception.h"

using namespace rrl;

uint64_t LocalLinker::resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) {
    uint64_t proc;
    HMODULE hModule = get_module_handle(library, symbol_library);
    // Try Win32 API first
    if ((proc = reinterpret_cast<uint64_t>(GetProcAddress(hModule, symbol_name.c_str())))) {
        return proc;
    }
    // Try local libraries
    if ((proc = resolve_internal_symbol(library, symbol_library, symbol_name))) {
        return proc;
    }
    // Try custom resolver
    return resolve_unresolved_symbol(library, symbol_library, symbol_name);
}

void LocalLinker::add_export(Library &library, std::string const &symbol, uint64_t address) {
    library.set_symbol_address(symbol, address);
}

void LocalLinker::unlink(Library &library) {
}
