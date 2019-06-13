#include "linker.hpp"
#include "win32exception.hpp"

using namespace rrl;

Linker::Linker(LinkageKind kind, symbol_resolver resolver)
    : linkage_kind_(kind)
    , symbol_resolver_(resolver)
{}

Linker::~Linker() {}

void Linker::set_unresolved_symbol_resolver(symbol_resolver resolver) {
    symbol_resolver_ = std::move(resolver);
}

void Linker::remove_unresolved_symbol_resolver() {
    symbol_resolver_ = nullptr;
}

uintptr_t Linker::resolve_internal_symbol(std::string const &symbol_library, std::string const &symbol_name) const {
    if (auto it = libraries_.find(symbol_library); it != libraries_.end()) {
        auto const &export_library = it->second;
        return export_library.get_symbol_address(symbol_name);
    }
    return 0;
}

uintptr_t Linker::resolve_unresolved_symbol(Library&, std::string const &symbol_library, std::string const &symbol_name) const {
    if (symbol_resolver_ == nullptr)
        return 0;
    return symbol_resolver_(symbol_library, symbol_name);
}

void Linker::dependency_bind(Library &dependant, std::string const &dependency) {
    auto &library_dependency = libraries_.at(dependency);
    library_dependency.add_dependent_library(dependant);
    dependant.add_library_dependency(library_dependency);
}

uintptr_t Linker::reserve_memory(Library &library, size_t size) const {
    uintptr_t address = reinterpret_cast<uintptr_t>(
        VirtualAllocEx(library.process, NULL, size, MEM_RESERVE, PAGE_NOACCESS)
        );
    if (!address) {
        throw win::Win32Exception(GetLastError());
    }
    library.add_memory_space(reinterpret_cast<LPVOID>(address), size);
    return address;
}

void Linker::commit_memory(Library &library, uintptr_t address, std::vector<std::byte> const &memory, uint32_t protection) const {
    SIZE_T nbBytesWritten;
    DWORD old_prot;
    LPVOID ptr = reinterpret_cast<LPVOID>(address);
    LPVOID new_ptr = VirtualAllocEx(library.process, ptr, memory.size(), MEM_COMMIT, PAGE_READWRITE);
    if (new_ptr != ptr) {
        throw win::Win32Exception(GetLastError());
    }
    if (!WriteProcessMemory(library.process, ptr, memory.data(), memory.size(), &nbBytesWritten)) {
        throw win::Win32Exception(GetLastError());
    }
    if (memory.size() != nbBytesWritten) {
        throw std::logic_error("commit_memory failed to write all buffer bytes");
    }
    if (!VirtualProtectEx(library.process, ptr, memory.size(), protection, &old_prot)) {
        throw win::Win32Exception(GetLastError());
    }
}

void Linker::create_thread(Library &library, uintptr_t address) const {
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

HMODULE Linker::get_module_handle(Library&, std::string const &module) {
    if (auto it = module_handles_.find(module); it != module_handles_.end()) {
        return it->second;
    }
    HMODULE handle = LoadLibraryA(module.c_str());
    module_handles_[module] = handle;
    return handle;
}

void Linker::register_library(Library &library) {
    libraries_.emplace(library.name, library);
}

void Linker::unlink(Library &library) {
    library.unlink();
}

void Linker::unregister_library(Library &library) {
    libraries_.erase(library.name);
}
