#include "remotelibrary.hpp"

#include <system_error>

using namespace rrl;

RemoteLibrary::RemoteLibrary(HANDLE process, std::string const &name)
    : Library(LinkageKind::Remote, process, name)
{}

RemoteLibrary::RemoteSymbol RemoteLibrary::operator[](std::string const &symbol) const {
    if (auto it = symbols_.find(symbol); it != symbols_.end())
        return RemoteSymbol(it->second, process);
    else
        throw std::logic_error("symbol not found");
}

DWORD RemoteLibrary::RemoteSymbol::stdcall(LPVOID arg, DWORD timeout) const {
    DWORD exitCode;
    HANDLE hThread = CreateRemoteThreadEx(
        process,
        NULL,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(address),
        arg,
        0,
        NULL,
        NULL
    );
    if (!hThread) {
        throw std::system_error(GetLastError(), std::generic_category());
    }
    DWORD result = WaitForSingleObject(hThread, timeout);
    switch (result) {
    case WAIT_OBJECT_0:
        break;
    case WAIT_TIMEOUT:
        throw std::runtime_error("waiting for stdcall result timed out");
    case WAIT_FAILED:
    default:
        throw std::system_error(GetLastError(), std::generic_category());
    }
    if (!GetExitCodeThread(hThread, &exitCode)) {
        throw std::system_error(GetLastError(), std::generic_category());
    }
    return exitCode;
}
