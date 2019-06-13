#pragma once

#include <stdexcept>

#include <Windows.h>

namespace rrl::win {

    class Win32Exception : public std::runtime_error
    {
    public:
        Win32Exception(DWORD error);

        DWORD error() const { return error_; }

    private:
        DWORD error_;
    };

}
