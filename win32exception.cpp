#include "win32exception.hpp"

#include <Windows.h>

#include <vector>

using namespace rrl;
using namespace rrl::win;

std::string FormatErrorMessage(DWORD error) {
    std::vector<char> buf(4096);
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM,
        0,
        error,
        0,
        buf.data(),
        static_cast<DWORD>(buf.size() - 1),
        0
    );
    return std::string(buf.data());
}

Win32Exception::Win32Exception(DWORD error)
    : std::runtime_error(FormatErrorMessage(error))
    , error_(error)
{}
