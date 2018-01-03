#include "locallinker.h"
#include "win32exception.h"

#include <Windows.h>

using namespace rrl;

uint64_t LocalLinker::resolve_symbol(std::string const &library, std::string const &symbol) const {
    // Check if library is local
    HMODULE hModule = GetModuleHandleA(library.c_str());
    if (hModule)
        return reinterpret_cast<uint64_t>(GetProcAddress(hModule, symbol.c_str()));
    else
        return resolve_internal_symbol(library, symbol);
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
    CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), NULL, 0, NULL);
}
