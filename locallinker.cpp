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

HMODULE LocalLinker::get_module_handle(Library &library, std::string const &module) {
    library.add_module_dependency(module);
    if (auto it = module_handles_.find(module); it != module_handles_.end()) {
        return it->second;
    }
    HMODULE handle = LoadLibraryA(module.c_str());
    module_handles_[module] = handle;
    return handle;
}

void LocalLinker::add_export(Library &library, std::string const &symbol, uint64_t address) {
    library.set_symbol_address(symbol, address);
}

uint64_t LocalLinker::reserve_memory(uint64_t address, size_t size) const {
    return reinterpret_cast<uint64_t>(
        VirtualAlloc(reinterpret_cast<LPVOID>(address), size, MEM_RESERVE, PAGE_READWRITE)
        );
}

void LocalLinker::commit_memory(uint64_t address, std::vector<std::byte> const &memory, uint32_t protection) const {
    DWORD old_prot = 0;
    LPVOID ptr = reinterpret_cast<void*>(address);
    LPVOID new_ptr = VirtualAlloc(ptr, memory.size(), MEM_COMMIT, PAGE_READWRITE);
    if (ptr != new_ptr) {
        throw 1;
    }
    memcpy(ptr, memory.data(), memory.size());
    if (!VirtualProtect(ptr, memory.size(), protection, &old_prot)) {
        throw rrl::win::Win32Exception(GetLastError());
    }
}

void LocalLinker::create_thread(Library &library, uint64_t address) const {
    HANDLE hThread = CreateThread(
        NULL,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(address),
        reinterpret_cast<LPVOID>(&library),
        0,
        NULL
    );
    library.add_thread(hThread);
}
