#include "locallinker.h"

#include <Windows.h>

using namespace rrl;

uintptr_t LocalLinker::resolve_symbol(std::string const &library, std::string const &symbol) const {
    // Check if library is local
    HMODULE hModule = GetModuleHandleA(library.c_str());
    if (hModule)
        return reinterpret_cast<uintptr_t>(GetProcAddress(hModule, symbol.c_str()));
    else
        return resolve_internal_symbol(library, symbol);
}

uintptr_t LocalLinker::reserve_memory(uintptr_t address, size_t size) const {
    return reinterpret_cast<uintptr_t>(
        VirtualAlloc(reinterpret_cast<LPVOID>(address), size, MEM_RESERVE, PAGE_READWRITE)
        );
}

std::byte* rrl::LocalLinker::get_buffer(uintptr_t address, size_t size) const {
    return reinterpret_cast<std::byte*>(address);
}

void rrl::LocalLinker::commit_buffer(std::byte *buffer, uintptr_t address, size_t size, uint32_t protection) const {
    if (buffer != reinterpret_cast<std::byte*>(address)) {
    }
    DWORD old_prot = 0;
    if (!VirtualProtect(buffer, size, protection, &old_prot)) {
    }
}
