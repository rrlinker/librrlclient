#include "library.hpp"

#include <system_error>

using namespace rrl;

DWORD const Library::UNLINK_THREAD_EXIT_CODE = 0x0FF116C;

Library::Library(LinkageKind kind, HANDLE process, std::string const &name)
    : linkage_kind_(kind)
    , process(process)
    , name(name)
    , detached_(false)
{}

Library::~Library() {
    if (!detached_)
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
        throw std::system_error(GetLastError(), std::generic_category());
    }
    if (exitCode == STILL_ACTIVE) {
        // Terminate all threads
        for (auto it = threads_.begin(); it != threads_.end(); ) {
            if (!TerminateThread(*it, UNLINK_THREAD_EXIT_CODE)) {
                throw std::system_error(GetLastError(), std::generic_category());
            }
            CloseHandle(*it);
            it = threads_.erase(it);
        }
        // Free memory spaces
        for (auto it = memory_spaces_.begin(); it != memory_spaces_.end(); ) {
            if (!VirtualFreeEx(process, it->first, 0, MEM_RELEASE)) {
                throw std::system_error(GetLastError(), std::generic_category());
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
            throw std::system_error(GetLastError(), std::generic_category());
        }
        it = module_dependencies_.erase(it);
    }
    // Remove this library from dependent libraries of other libraries
    for (auto it = library_dependencies_.begin(); it != library_dependencies_.end(); ) {
        it->second.remove_dependent_library(*this);
        it = library_dependencies_.erase(it);
    }
}

void Library::detach() {
    detached_ = true;
}
