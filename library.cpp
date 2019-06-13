#include "library.hpp"
#include "win32exception.hpp"

using namespace rrl;

DWORD const Library::UNLINK_THREAD_EXIT_CODE = 0x0FF116C;

Library::Library(LinkageKind kind, HANDLE process, std::string const &name)
    : linkage_kind_(kind)
    , process(process)
    , name(name)
{}

Library::~Library() {
    unlink();
}

void Library::add_module_dependency(std::string const &module, HMODULE handle) {
    module_dependencies_.emplace(module, handle);
}

void Library::add_library_dependency(Library &library) {
    library_dependencies_.emplace(library.name, library);
}

void Library::remove_library_dependency(Library &library) {
    library_dependencies_.erase(library.name);
}

void Library::add_dependent_library(Library &library) {
    dependent_libraries_.emplace(library.name, library);
}

void Library::remove_dependent_library(Library &library) {
    dependent_libraries_.erase(library.name);
}

void Library::add_thread(HANDLE hThread) {
    threads_.emplace(hThread);
}

void Library::add_memory_space(LPVOID address, SIZE_T size) {
    memory_spaces_.emplace(address, size);
}

void Library::set_symbol_address(std::string symbol, uintptr_t address) {
    symbols_[symbol] = Symbol(address);
}

uintptr_t Library::get_symbol_address(std::string const &symbol) const {
    if (auto it = symbols_.find(symbol); it != symbols_.end())
        return it->second.address;
    return 0;
}

void Library::unlink() {
    if (dependent_libraries_.size() > 0) {
        throw std::logic_error("cannot unlink library with dependent libraries");
    }
    DWORD exitCode;
    if (!GetExitCodeProcess(process, &exitCode)) {
        throw win::Win32Exception(GetLastError());
    }
    if (exitCode == STILL_ACTIVE) {
        // Terminate all threads
        for (auto it = threads_.begin(); it != threads_.end(); ) {
            if (!TerminateThread(*it, UNLINK_THREAD_EXIT_CODE)) {
                throw win::Win32Exception(GetLastError());
            }
            CloseHandle(*it);
            it = threads_.erase(it);
        }
        // Free memory spaces
        for (auto it = memory_spaces_.begin(); it != memory_spaces_.end(); ) {
            if (!VirtualFreeEx(process, it->first, 0, MEM_RELEASE)) {
                throw win::Win32Exception(GetLastError());
            }
            it = memory_spaces_.erase(it);
        }
    } else {
        for (auto thread : threads_) {
            CloseHandle(thread);
        }
        threads_.clear();
        memory_spaces_.clear();
    }
    // Free modules
    for (auto it = module_dependencies_.begin(); it != module_dependencies_.end(); ) {
        if (!FreeLibrary(it->second)) {
            throw win::Win32Exception(GetLastError());
        }
        it = module_dependencies_.erase(it);
    }
    // Remove this library from dependent libraries of other libraries
    for (auto it = library_dependencies_.begin(); it != library_dependencies_.end(); ) {
        it->second.remove_dependent_library(*this);
        it = library_dependencies_.erase(it);
    }
}
