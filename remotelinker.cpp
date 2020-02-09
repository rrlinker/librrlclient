#include "remotelinker.hpp"

#include "win32_error.hpp"

#include <Windows.h>
#include <TlHelp32.h>

using namespace rrl;

DWORD const RemoteLinker::REMOTE_LOAD_LIBRARY_TIMEOUT = 16384;
DWORD const RemoteLinker::REMOTE_FREE_LIBRARY_TIMEOUT = 16384;

RemoteLinker::RemoteLinker(symbol_resolver resolver)
    : Linker(LinkageKind::Remote, resolver)
{}

uintptr_t RemoteLinker::resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) {
    uintptr_t proc;
    // if symbol_library is set
    if (!symbol_library.empty()) {
        // Try Win32 API first
        HMODULE hLocalHandle = get_module_handle(library, symbol_library);
        HMODULE hRemoteHandle = get_remote_module_handle(library, symbol_library);
        if ((proc = reinterpret_cast<uintptr_t>(GetProcAddress(hLocalHandle, symbol_name.c_str()))) != NULL) {
            library.add_module_dependency(symbol_library, hLocalHandle);
            return reinterpret_cast<uintptr_t>(hRemoteHandle)
                + (proc - reinterpret_cast<uintptr_t>(hLocalHandle));
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

void RemoteLinker::add_export(Library &library, std::string const &symbol, uintptr_t address) {
    library.set_symbol_address(symbol, address);
}

HMODULE RemoteLinker::get_remote_module_handle(Library &library, std::string const &module) {
    if (remote_module_handles_.find(module) == remote_module_handles_.end()) {
        remote_load_module(library.process, module);
        remote_module_handles_[module] = find_remote_module_handle(library.process, module);
    }
    return remote_module_handles_[module];
}

void RemoteLinker::remote_load_module(HANDLE hProcess, std::string const &module) {
    SIZE_T nbBytesWritten;
    LPVOID rszModule = VirtualAllocEx(hProcess, NULL, module.length() + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!rszModule) {
        throw win::Win32Error(GetLastError());
    }
    if (!WriteProcessMemory(hProcess, rszModule, module.c_str(), module.length() + 1, &nbBytesWritten)) {
        throw win::Win32Error(GetLastError());
    }
    if (nbBytesWritten != module.length() + 1) {
        throw std::logic_error("number of bytes written to the remote process doesn't equal to module string size");
    }
    HANDLE hLoadLibThread = CreateRemoteThreadEx(
        hProcess,
        NULL,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA),
        rszModule,
        0,
        NULL,
        NULL
    );
    if (!hLoadLibThread) {
        throw win::Win32Error(GetLastError());
    }
    DWORD result = WaitForSingleObject(hLoadLibThread, REMOTE_LOAD_LIBRARY_TIMEOUT);
    switch (result) {
    case WAIT_OBJECT_0:
        break;
    case WAIT_TIMEOUT:
        throw std::logic_error("waiting time for remote library get loaded has timed out.");
        break;
    case WAIT_FAILED:
        throw win::Win32Error(GetLastError());
        break;
    default:
        throw std::logic_error("WaitForSingleObject returned unexpected value");
    }
    if (!VirtualFreeEx(hProcess, rszModule, 0, MEM_RELEASE)) {
        throw win::Win32Error(GetLastError());
    }
}

void RemoteLinker::remote_free_module(HANDLE hProcess, HMODULE hModule) {
    HANDLE hFreeLibThread = CreateRemoteThreadEx(
        hProcess,
        NULL,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(FreeLibrary),
        hModule,
        0,
        NULL,
        NULL
    );
    if (!hFreeLibThread) {
        throw win::Win32Error(GetLastError());
    }
    DWORD result = WaitForSingleObject(hFreeLibThread, REMOTE_FREE_LIBRARY_TIMEOUT);
    switch (result) {
    case WAIT_OBJECT_0:
        break;
    case WAIT_TIMEOUT:
        throw std::logic_error("waiting time for remote library get freed has timed out.");
        break;
    case WAIT_FAILED:
        throw win::Win32Error(GetLastError());
        break;
    default:
        throw std::logic_error("WaitForSingleObject returned unexpected value");
    }
}

HMODULE RemoteLinker::find_remote_module_handle(HANDLE hProcess, std::string const &module) {
    HANDLE hModuleSnapshot = NULL;
    bool retry = false;
    DWORD errcode = 0;
    do {
        hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(hProcess));
        if (hModuleSnapshot == INVALID_HANDLE_VALUE)
            errcode = GetLastError();
    } while (!retry && errcode == ERROR_BAD_LENGTH);
    if (!hModuleSnapshot) {
        throw win::Win32Error(GetLastError());
    }
    
    bool found = false;
    MODULEENTRY32 me;
    Module32First(hModuleSnapshot, &me);
    do {
        if (_stricmp(module.c_str(), me.szModule) == 0) {
            found = true;
            break;
        }
    } while (Module32Next(hModuleSnapshot, &me));
    CloseHandle(hModuleSnapshot);
    if (!found) {
        if (GetLastError() != ERROR_NO_MORE_FILES) {
            throw win::Win32Error(GetLastError());
        } else {
            throw std::logic_error("failed to find just-loaded remote library");
        }
    }
    return reinterpret_cast<HMODULE>(me.modBaseAddr);
}

void RemoteLinker::unlink(Library &library) {
    Linker::unlink(library);
    DWORD exitCode;
    if (!GetExitCodeProcess(library.process, &exitCode)) {
        throw win::Win32Error(GetLastError());
    }
    if (exitCode == STILL_ACTIVE) {
        for (auto it = remote_module_handles_.begin(); it != remote_module_handles_.end(); ) {
            remote_free_module(library.process, it->second);
            it = remote_module_handles_.erase(it);
        }
    } else {
        remote_module_handles_.clear();
    }
}
