#include "win32_error.hpp"

#include <vector>

#include <Windows.h>

using namespace rrl::win;

Win32Error::Win32Error(uint32_t error)
    : std::runtime_error(error_to_message(error))
    , error(error)
{}

std::string Win32Error::error_to_message(uint32_t error) {
    std::vector<char> buffer(4096);
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr,
        error,
        LANG_NEUTRAL,
        buffer.data(),
        buffer.size(),
        nullptr
    );
    return buffer.data();
}
