#include "linker.h"
#include "win32exception.h"

using namespace rrl;

uint64_t Linker::resolve_internal_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) const {
    if (auto it = libraries_.find(symbol_library); it != libraries_.end()) {
        auto const &export_library = it->second;
        library.add_library_dependency(export_library);
        return export_library.get_symbol_address(symbol_name);
    }
    return 0;
}

uint64_t Linker::resolve_unresolved_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) const {
    return 0;
}

uint64_t Linker::reserve_memory(Library &library, uint64_t address, size_t size) const {
    uint64_t memory = reinterpret_cast<uint64_t>(
        VirtualAllocEx(library.process, reinterpret_cast<LPVOID>(address), size, MEM_RESERVE, PAGE_NOACCESS)
        );
    if (!memory) {
        throw win::Win32Exception(GetLastError());
    }
    return memory;
}

void Linker::commit_memory(Library &library, uint64_t address, std::vector<std::byte> const &memory, uint32_t protection) const {
    DWORD old_prot = 0;
    LPVOID ptr = reinterpret_cast<LPVOID>(address);
    LPVOID new_ptr = VirtualAllocEx(library.process, ptr, memory.size(), MEM_COMMIT, PAGE_READWRITE);
    if (new_ptr != ptr) {
        throw win::Win32Exception(GetLastError());
    }
    memcpy(ptr, memory.data(), memory.size());
    if (!VirtualProtectEx(library.process, ptr, memory.size(), protection, &old_prot)) {
        throw win::Win32Exception(GetLastError());
    }
}

void Linker::create_thread(Library &library, uint64_t address) const {
    LPVOID lpParam = &library;
    if (library.process != GetCurrentProcess())
        lpParam = NULL;
    HANDLE hThread = CreateRemoteThreadEx(
        library.process,
        NULL,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(address),
        lpParam,
        0,
        NULL,
        NULL
    );
    if (!hThread) {
        throw win::Win32Exception(GetLastError());
    }
    library.add_thread(hThread);
}

HMODULE Linker::get_module_handle(Library &library, std::string const &module) {
    library.add_module_dependency(module);
    if (auto it = module_handles_.find(module); it != module_handles_.end()) {
        return it->second;
    }
    HMODULE handle = LoadLibraryA(module.c_str());
    module_handles_[module] = handle;
    return handle;
}
