#include "locallinker.h"
#include "win32exception.h"

using namespace rrl;

uint64_t LocalLinker::resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) {
    if (symbol_library.length()) {
        // Win32 Library
        HMODULE hModule = get_module_handle(library, symbol_library);
        return reinterpret_cast<uint64_t>(GetProcAddress(hModule, symbol_name.c_str()));
    } else {
        // Local library
        return resolve_internal_symbol(library, symbol_library, symbol_name);
    }
}

void LocalLinker::add_export(Library &library, std::string const &symbol, uint64_t address) {
    library.set_symbol_address(symbol, address);
}

