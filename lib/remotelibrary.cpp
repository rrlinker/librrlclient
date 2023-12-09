#include <rrlinker/client/remotelibrary.hpp>

#include <rrlinker/client/win32_error.hpp>

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
        throw win::Win32Error(GetLastError());
    }
    DWORD result = WaitForSingleObject(hThread, timeout);
    switch (result) {
    case WAIT_OBJECT_0:
        break;
    case WAIT_TIMEOUT:
        throw std::runtime_error("waiting for stdcall result timed out");
    case WAIT_FAILED:
    default:
        throw win::Win32Error(GetLastError());
    }
    if (!GetExitCodeThread(hThread, &exitCode)) {
        throw win::Win32Error(GetLastError());
    }
    return exitCode;
}
